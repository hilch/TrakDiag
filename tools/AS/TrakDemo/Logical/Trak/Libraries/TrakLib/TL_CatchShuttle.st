FUNCTION_BLOCK TL_CatchShuttle
	
	IF NOT(Enable) THEN
		step := 0;
	END_IF
		
	CASE step OF
		(* ------------------------------------------------------------------------------
			wait for Enable
		   ------------------------------------------------------------------------------ *)

		0: 
		IF Enable THEN (* activate process points as trigger points *)
			Busy := TRUE;
			fbTrgPointEnableEntry.ProcessPoint := ADR(ProcessPoint);
			fbTrgPointEnableEntry.Enable := TRUE;
			step := 100;
		ELSE
			fbShReadInfo( Enable := FALSE );
			fbTrgPointEnableEntry( Enable := FALSE );
			fbTrgPointGetInfo( Execute := FALSE );
			Valid := FALSE;
			Busy := FALSE;
			Error := FALSE;
			ErrorID := 0;
		END_IF		

		(* ------------------------------------------------------------------------------
			wait for arriving shuttle at entry trigger point
		   ------------------------------------------------------------------------------ *)

		100: 
		fbTrgPointEnableEntry();
		IF fbTrgPointEnableEntry.Valid THEN 
			Valid := TRUE;
			IF fbTrgPointEnableEntry.EventCount <> 0 THEN
				fbTrgPointGetInfo( ProcessPoint := fbTrgPointEnableEntry.ProcessPoint, Execute := TRUE );
				step := 110;
			END_IF
		ELSIF fbTrgPointEnableEntry.Error THEN
			Valid := FALSE;
			ErrorID := 101;
			Error := TRUE;
			step := 9000;
		END_IF
		
		(* ------------------------------------------------------------------------------
			shuttle passed trigger point - switch on reading shuttle info
		   ------------------------------------------------------------------------------ *)

		110: 
		IF fbTrgPointGetInfo.Done THEN
			Axis := fbTrgPointGetInfo.TrgPointInfo.Axis;
			IF fbTrgPointGetInfo.TrgPointInfo.Event = mcACPTRAK_TP_PASSED_POSITIVE THEN (* only consider shuttles in positive direction *)
				(* switch on reading shuttle info *)
				fbShReadInfo( Axis := ADR(Axis), Enable := TRUE ); 
				step := 120;
			ELSE
				step := 100;
			END_IF
			fbTrgPointGetInfo( Execute := FALSE );

		ELSIF fbTrgPointGetInfo.Error THEN
			Valid := FALSE;
			ErrorID := 111;
			Error := TRUE;
			step := 9000;
		ELSE
			fbTrgPointGetInfo();
		END_IF

		(* ------------------------------------------------------------------------------
			read shuttles's info
		   ------------------------------------------------------------------------------ *)

		120:
		IF fbShReadInfo.Valid THEN
			ShuttleInfo := fbShReadInfo.ShuttleInfo;
			fbShReadInfo( Enable := FALSE );
			(* switch on reading shuttle's user data *)
			IF DataAddress <> 0 AND DataSize <> 0 THEN (* shall user data be read, too ? *)
				fbCopyUserData( Axis := ADR(Axis), Mode := mcACPTRAK_USERDATA_GET, DataAddress := DataAddress, DataSize := DataSize, Execute := TRUE );
				step := 130;
			ELSE (* we don't need the user data, so we are ready *)
				ShuttleCounter := ShuttleCounter + 1; (* shuttle data valid *)
				step := 100;
			END_IF
		ELSIF fbShReadInfo.Error THEN
			IF fbShReadInfo.ErrorID = -1067319295 THEN (* ignore 'invalid axis' *)
				fbShReadInfo( Enable := FALSE );
				step := 100;
			ELSE
				Valid := FALSE;
				ErrorID := 121;
				Error := TRUE;
				fbShReadInfo( Enable := FALSE );
				step := 9000;
			END_IF
		ELSE
			fbShReadInfo();
		END_IF


		(* ------------------------------------------------------------------------------
			read shuttles's user data
		   ------------------------------------------------------------------------------ *)
			
		130: 
		IF fbCopyUserData.Done THEN
			fbCopyUserData( Execute := FALSE );
			ShuttleCounter := ShuttleCounter + 1; (* shuttle data valid *)
			step := 100;
		ELSIF fbCopyUserData.Error THEN
			IF fbCopyUserData.ErrorID = -1067319295 THEN (* ignore 'invalid axis' *)
				fbCopyUserData( Execute := FALSE );					
				step := 100;
			ELSE
				Valid := FALSE;
				ErrorID := 131;
				Error := TRUE;
				fbCopyUserData( Execute := FALSE );
				step := 9000;
			END_IF
		ELSE
			fbCopyUserData();
		END_IF


		9000: (* Error *)
		Busy := FALSE;
		

	END_CASE
	


END_FUNCTION_BLOCK
