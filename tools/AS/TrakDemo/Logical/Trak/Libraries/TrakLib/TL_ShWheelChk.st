
(* Shuttle Wheel Check *)
FUNCTION_BLOCK TL_ShWheelChk
	
	IF NOT(Enable) THEN
		step := 0;
	END_IF
		
	CASE step OF
		(* ------------------------------------------------------------------------------
			wait for Enable
		   ------------------------------------------------------------------------------ *)

		0: 
		IF Enable THEN (* activate process points as trigger points *)
			fbTrgPointEnableEntry( ProcessPoint := ADR(TrgPointEntry), Enable := TRUE );
			IF CycleTime < 1200e-6 THEN
				CycleTime := 1200e-6;
			END_IF
			IF fbTrgPointEnableEntry.Valid THEN
				fbTrgPointGetInfo( Execute := FALSE );
				fbShSelCtrlParSet( Execute := FALSE );
				Data.AverageLagError := 0.0;
				Data.PeakLagError := 0.0;
				step := 100;
			ELSIF fbTrgPointEnableEntry.Error THEN
				ErrorID := 1;
				Error := TRUE;
				step := 9000;
			END_IF
		ELSE
			fbShReadInfo( Enable := FALSE );
			fbShReadSectorPos( Enable := FALSE );
			fbShSelCtrlParSet( Execute := FALSE );
			fbTrgPointEnableEntry( Enable := FALSE );
			fbTrgPointGetInfo( Execute := FALSE );
			Done := FALSE;
			Busy := FALSE;
			Error := FALSE;
			ErrorID := 0;
		END_IF		

		(* ------------------------------------------------------------------------------
			wait for arriving shuttle at entry trigger point
		   ------------------------------------------------------------------------------ *)

		100: 
		fbTrgPointEnableEntry();
		IF fbTrgPointEnableEntry.EventCount <> 0 THEN
			fbTrgPointGetInfo( ProcessPoint := fbTrgPointEnableEntry.ProcessPoint, Execute := TRUE );
			tonTimeout( IN:= FALSE, PT := T#20s ); (* abort timer *)
			tonTimeout.IN := TRUE; (* start timeout *)
			peakLagError := 0.0;
			lagErrorSum := 0.0;
			currentLagError := 0.0;
			Done := FALSE;
			Busy := FALSE;
			step := 110;
		ELSIF fbTrgPointEnableEntry.Error THEN
			ErrorID := 101;
			Error := TRUE;
			step := 9000;
		END_IF
		
		(* ------------------------------------------------------------------------------
			shuttle passed trigger point - switch on reading shuttle info and reduce 
			the controller settings to make the lag error more visible
		   ------------------------------------------------------------------------------ *)

		110: 
		IF fbTrgPointGetInfo.Done THEN
			axis := fbTrgPointGetInfo.TrgPointInfo.Axis;
			IF fbTrgPointGetInfo.TrgPointInfo.Event = mcACPTRAK_TP_PASSED_POSITIVE THEN (* only consider shuttles in positive direction *)
				(* switch on reading shuttle info (position) *)
				fbShReadInfo( Axis := ADR(axis), Enable := TRUE ); 
				(* switch on reading position of sector where triggerpoint is located *)
				fbShReadSectorPos( Axis := ADR(axis), Sector := ADR(Sector), Enable := TRUE );
				(* switch on reading segment info where shuttle is located *)
				fbShReadSegmentInfo( Axis := ADR(axis), Enable := TRUE );
				(* weak position controller to make the lag error more visible *)
				fbShSelCtrlParSet( Axis := ADR(axis), Mode := mcACPTRAK_SIDE_BOTH, ParameterSetIndex := CtrlParIndex, Execute := TRUE ); 
				step := 120;
			ELSE
				step := 100;
			END_IF
			fbTrgPointGetInfo( Execute := FALSE );
		ELSIF fbTrgPointGetInfo.Busy THEN
			fbTrgPointGetInfo();
		ELSIF fbTrgPointGetInfo.Error THEN
			ErrorID := 111;
			Error := TRUE;
			step := 9000;
		END_IF

		(* ------------------------------------------------------------------------------
			read shuttles's position and lag error (set position - actual position )
		   ------------------------------------------------------------------------------ *)
			
		120: 
		IF fbShReadInfo.Valid AND fbShReadSectorPos.Valid AND fbShReadSegmentInfo.Valid  AND fbShSelCtrlParSet.Done THEN
			cycleCount := 0;
			start_position := fbShReadSectorPos.Info.Position;
			end_position := start_position + MeasuringDistance;
			fbShSelCtrlParSet( Execute := FALSE );
			step := 130;
		ELSIF fbShReadInfo.Error THEN
			ErrorID := 121;
			Error := TRUE;
			step := 9000;
		ELSIF fbShReadSectorPos.Error THEN
			ErrorID := 122;
			Error := TRUE;
			step := 9000;
		ELSIF fbShReadSegmentInfo.Error THEN
			ErrorID := 123;
			Error := TRUE;
			step := 9000;
		ELSIF fbShSelCtrlParSet.Error THEN
			ErrorID := 124;
			Error := TRUE;
			step := 9000;
		ELSE
			fbShReadInfo();
			fbShReadSectorPos();
			fbShSelCtrlParSet();
			fbShReadSegmentInfo();
		END_IF

				
		(* ------------------------------------------------------------------------------
			read shuttles's position and lag error (set position - actual position )
		   ------------------------------------------------------------------------------ *)

		130:
		(* calculate lag error when fifo is already filled and bumper position is reached *)
		IF  (cycleCount >= CYCLES_PLK) AND (fbShReadSectorPos.Info.Position >= start_position) THEN 
			Busy := TRUE;
			l := setPosDelay[0] - fbShReadInfo.ShuttleInfo.CurrentValues.ActualPosition;
			IF l < 0.0001 THEN (* < epsilon or negative *)
				currentLagError := 0.0;
			ELSIF l > 0.008 THEN (* plausibility control at sector end *)
				currentLagError := currentLagError;
			ELSE
				currentLagError := l;
			END_IF
			lagErrorSum := lagErrorSum + currentLagError;
			peakLagError := MAX( peakLagError, currentLagError );
		END_IF
		FOR n := 0 TO (CYCLES_PLK-2) DO (* shift fifo *)
			setPosDelay[n] := setPosDelay[n+1];
		END_FOR	
		setPosDelay[CYCLES_PLK-1] := fbShReadInfo.ShuttleInfo.CurrentValues.Position;  (* set position that must be delayed *)

		fbShReadInfo();
		fbShReadSectorPos();
		fbShReadSegmentInfo();

		(* check the exit point to see if the shuttle passed it. *)
		IF (fbShReadSectorPos.Info.Position >= end_position) OR tonTimeout.Q THEN (* shuttle passes exit point *)
			Busy := FALSE;
			(* switch off reading the shuttle position *)
			fbShReadInfo( Enable := FALSE );
			fbShReadSectorPos( Enable := FALSE );
			fbShReadSegmentInfo( Enable := FALSE );
			(* set controller to default again ! *)
			fbShSelCtrlParSet( Axis := ADR(axis), Mode := mcACPTRAK_SIDE_BOTH, ParameterSetIndex := 0, Execute := TRUE ); 
			IF NOT(tonTimeout.Q) THEN
				Done := TRUE;
				ShuttleCounter := ShuttleCounter + 1; (* a shuttle was measured *)
				Data.ShuttleAxis := axis;
				Data.PeakLagError := peakLagError;
				IF Data.PeakLagError >= ABS(Threshold) THEN
					Data.Good := FALSE;
					Data.Bad := TRUE;
				ELSE
					Data.Good := TRUE;
					Data.Bad := FALSE;
				END_IF
				IF cycleCount <> 0 THEN (* prevent division by zero error *)
					Data.AverageLagError := lagErrorSum / UDINT_TO_LREAL(cycleCount);
				END_IF
			ELSE (* timeout *)
				timeouts := timeouts + 1;  (* just a counter for debugging *)
			END_IF
			step := 140;				
		END_IF
		(* count cycles *)
		cycleCount := cycleCount + 1;
		tonTimeout();


		(* ------------------------------------------------------------------------------
			wait until controller settings are on defaults again
		   ------------------------------------------------------------------------------ *)

		140: 
		IF 	fbShSelCtrlParSet.Done THEN
			fbShSelCtrlParSet.Execute := FALSE;
			step := 100;
		END_IF
		fbShSelCtrlParSet();


		9000: (* Error *)
		

	END_CASE
	


END_FUNCTION_BLOCK
