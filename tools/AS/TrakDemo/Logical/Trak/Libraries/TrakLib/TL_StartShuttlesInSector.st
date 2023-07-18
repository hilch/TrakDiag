
(* 
	starts all shuttles in a given sector 

	first of all we get a list of shuttles in a given sector.
	afterwards we execute a elastic movement on all shuttles (optionally stop at the given position)

*)
FUNCTION_BLOCK TL_StartShuttlesInSector

	IF Execute THEN		
		
		CASE step OF

		(* -------------------------------------------------------------------------------------------
		start
		---------------------------------------------------------------------------------------------*)	
		0:  
		Busy := TRUE;
		brsmemset(ADR(list), 0, SIZEOF(list));
		maxShuttles := SIZEOF(list)/SIZEOF(list[0]);
		fbSecGetShuttle.Sector := ADR(Sector);
		fbSecGetShuttle.AdvancedParameters.SelectionMode := mcACPTRAK_GET_SH_ALL;
		fbSecGetShuttle.AdvancedParameters.VirtualSelectionMode := mcACPTRAK_GET_SH_VIRT_NONVIRTUAL;
		fbSecGetShuttle.AdvancedParameters.StartPosition := StartPosition;
		fbSecGetShuttle.AdvancedParameters.EndPosition := EndPosition;
		fbSecGetShuttle.Next := FALSE;
		fbSecGetShuttle.Enable := TRUE;

		IF Direction = mcDIR_POSITIVE THEN
			fbSecGetShuttle( Mode := mcACPTRAK_SEARCH_BACKWARD );
			fbRoutedMoveVel.AdvancedParameters.ShuttleOrientation := mcDIR_UNDEFINED;
			fbRoutedMoveVel.AdvancedParameters.StartDirection := mcDIR_UNDEFINED;
			fbRoutedMoveVel.AdvancedParameters.EndDirection := mcDIR_POSITIVE;
			fbRoutedMoveAbs.AdvancedParameters.ShuttleOrientation := mcDIR_UNDEFINED;
			fbRoutedMoveAbs.AdvancedParameters.StartDirection := mcDIR_UNDEFINED;
			fbRoutedMoveAbs.AdvancedParameters.EndDirection := mcDIR_POSITIVE;
		ELSE
			fbSecGetShuttle( Mode := mcACPTRAK_SEARCH_FORWARD );
			fbRoutedMoveVel.AdvancedParameters.ShuttleOrientation := mcDIR_UNDEFINED;
			fbRoutedMoveVel.AdvancedParameters.StartDirection := mcDIR_UNDEFINED;
			fbRoutedMoveVel.AdvancedParameters.EndDirection := mcDIR_NEGATIVE;
			fbRoutedMoveAbs.AdvancedParameters.ShuttleOrientation := mcDIR_UNDEFINED;
			fbRoutedMoveAbs.AdvancedParameters.StartDirection := mcDIR_UNDEFINED;
			fbRoutedMoveAbs.AdvancedParameters.EndDirection := mcDIR_NEGATIVE;
		END_IF

		IF fbSecGetShuttle.Valid THEN
			NumberOfShuttles := UINT_TO_USINT(fbSecGetShuttle.TotalCount);
			IF NumberOfShuttles > 0 THEN
				
				list[0] := fbSecGetShuttle.Axis;
				
				IF fbSecGetShuttle.RemainingCount > 0 THEN (* more then one shuttle *)
					n := 1;
					fbSecGetShuttle( Next := TRUE );
					step := 10;
				ELSE (* only one shuttle *)
					fbSecGetShuttle( Enable	:= FALSE );
					n := 0;
					step := 20;		
				END_IF
			ELSE (* no shuttles to start *)
				fbSecGetShuttle( Enable	:= FALSE );
				step := 100;
			END_IF
		ELSIF fbSecGetShuttle.Error THEN
			Error := TRUE;
			ErrorID := step;
			step := 9001; (* error *)
		ELSE
			fbSecGetShuttle();
		END_IF;
	

		(* -------------------------------------------------------------------------------------------
		get the list of shuttles in sector
		---------------------------------------------------------------------------------------------*)				
		10:
		IF 	fbSecGetShuttle.Axis.controlif <> list[n-1].controlif THEN
			IF fbSecGetShuttle.Error THEN
				Error := TRUE;
				ErrorID := step;
				fbSecGetShuttle( Enable := FALSE );
				step := 9002; (* error *)
			ELSE
				fbSecGetShuttle( Next := FALSE );
				list[n] := fbSecGetShuttle.Axis;					
				n := n + 1;
				IF (fbSecGetShuttle.RemainingCount > 0) AND (n < maxShuttles) THEN
						fbSecGetShuttle( Next := TRUE );
				ELSE (* all shuttles found *)
					n := 0;
					fbSecGetShuttle( Enable := FALSE );
					step := 20;
				END_IF
			END_IF
		ELSE
			fbSecGetShuttle();
		END_IF
	

		(* -------------------------------------------------------------------------------------------
		reset shuttle
		---------------------------------------------------------------------------------------------*)	
	    20: 
		fbReset( Axis := ADR(list[n]), Execute := TRUE );
		IF fbReset.Done THEN
			fbReset( Execute := FALSE );
			n := n + 1;
			IF n < maxShuttles THEN
				IF list[n].controlif = 0 THEN (* all shuttles reset *)
					fbSecReadInfo( Sector := ADR(Sector), Enable := FALSE ); (* reset fb *)
					ton( PT := T#1s, IN := FALSE ); (* reset fb *)					
					step := 30; 
				END_IF	
			ELSE (* end of list *)
				fbSecReadInfo( Sector := ADR(Sector), Enable := FALSE ); (* reset fb *)
				ton( PT := T#1s, IN := FALSE ); (* reset fb *)					
				step := 30; 			
			END_IF
		ELSIF fbReset.Error THEN
			Error := TRUE;	
			ErrorID := step;									
			step := 9003; (* error *)
		ELSE
			fbReset();
		END_IF
		
		(* -------------------------------------------------------------------------------------------
		wait until no shuttle is in 'Disabled' state (or timeout has expired )
		---------------------------------------------------------------------------------------------*)
		30:
		fbSecReadInfo( Enable := TRUE );
		ton( IN := TRUE );
		IF ton.Q THEN
			fbSecReadInfo( Sector := ADR(Sector), Enable := FALSE ); (* reset fb *)
			ton( PT := T#1s, IN := FALSE ); (* reset fb *)					
			n := 0;
			step := 40; 
		ELSIF fbSecReadInfo.Valid THEN
			IF fbSecReadInfo.SectorInfo.ShuttlesInDisabledCount = 0 THEN
				fbSecReadInfo( Sector := ADR(Sector), Enable := FALSE ); (* reset fb *)
				ton( PT := T#1s, IN := FALSE ); (* reset fb *)					
				n := 0;
				step := 40; 
			END_IF
		ELSIF fbSecReadInfo.Error THEN
			fbSecReadInfo( Enable := FALSE ); (* reset fb *)
			Error := TRUE;
			ErrorID := step;
			step := 9006;
		END_IF

		(* -------------------------------------------------------------------------------------------
		start Shuttle
		---------------------------------------------------------------------------------------------*)	
		40: 
		IF Absolute THEN
			fbRoutedMoveAbs( Axis:= ADR(list[n]), Sector := ADR(TargetSector), Position := Position, 
							Velocity := Velocity, Acceleration := Acceleration, 
							Deceleration := Deceleration, Jerk := Jerk, BufferMode := mcABORTING, Execute := TRUE );
			step := 70;			
		ELSE
			fbRoutedMoveVel( Axis:= ADR(list[n]), Sector := ADR(TargetSector), Position := Position, 
							Velocity := Velocity, RouteVelocity := Velocity, Acceleration := Acceleration, 
							Deceleration := Deceleration, Jerk := Jerk, BufferMode := mcABORTING, Execute := TRUE );
			step := 50;
		END_IF


		(* -------------------------------------------------------------------------------------------
		wait until movement is started
		---------------------------------------------------------------------------------------------*)								
		50: 
		IF fbRoutedMoveVel.Active THEN
			fbRoutedMoveVel( Execute := FALSE );
			n := n + 1;
			step := 100; (* assume end of list *)
			IF n < maxShuttles THEN
				IF list[n].controlif <> 0 THEN
					step := 40; (* no, we still need to start a shuttle *)
				END_IF	
			END_IF

		ELSIF fbRoutedMoveVel.Error OR fbRoutedMoveVel.CommandAborted THEN
			Error := TRUE;	
			ErrorID := step;									
			step := 9004; (* error *)
		ELSE
			fbRoutedMoveVel();
		END_IF;


		(* -------------------------------------------------------------------------------------------
		wait until (absolute) movement is started
		---------------------------------------------------------------------------------------------*)	
		70: 
		IF fbRoutedMoveAbs.Active THEN
			fbRoutedMoveAbs( Execute := FALSE );
			n := n + 1;
			step := 100; (* assume end of list *)
			IF n < maxShuttles THEN
				IF list[n].controlif <> 0 THEN
					step := 40; (* no, we still need to start a shuttle *)
				END_IF	
			END_IF
		ELSIF fbRoutedMoveAbs.Error OR fbRoutedMoveAbs.CommandAborted THEN
			Error := TRUE;	
			ErrorID := step;									
			step := 9005; (* error *)
		ELSE
			fbRoutedMoveAbs();
		END_IF;


		(* -------------------------------------------------------------------------------------------
		done
		---------------------------------------------------------------------------------------------*)	
		100: 
		Done := TRUE;
		Busy := FALSE;
		Error := FALSE;
		ErrorID := 0; 	

			
		(* -------------------------------------------------------------------------------------------
		error
		---------------------------------------------------------------------------------------------*)	
		9000..9006: (* Error *)
		
		END_CASE;
	
	ELSE
		(* Reset all data if the FUB is disabled *)
		fbSecGetShuttle( Enable	:= FALSE );
		fbRoutedMoveVel( Execute := FALSE );
		fbRoutedMoveAbs( Execute := FALSE );
		fbReset( Execute := FALSE );
		Done					:= FALSE;
		Busy					:= FALSE;
		Error					:= FALSE;
		ErrorID					:= 0;
		NumberOfShuttles		:= 0;
		step		:= 0;
	END_IF

END_FUNCTION_BLOCK
