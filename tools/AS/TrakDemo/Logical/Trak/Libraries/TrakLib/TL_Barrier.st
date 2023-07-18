
(* TODO: Add your comment here *)
FUNCTION_BLOCK TL_Barrier
	IF Enable THEN

		(* Barrier parameters *)
		MC_BR_BarrierCommand.ProcessPoint := ADR(ProcessPoint);
		MC_BR_BarrierReadInfo.ProcessPoint := ADR(ProcessPoint);
		MC_BR_BarrierReadInfo.Enable := TRUE;
		MC_BR_BarrierReadInfo();
		
		(* Waiting for a command *)
		IF Close AND (MC_BR_BarrierReadInfo.BarrierInfo.Status = mcACPTRAK_BARRIER_OPENED ) THEN
			MC_BR_BarrierCommand.Command := mcACPTRAK_BARRIER_CLOSE;
			MC_BR_BarrierCommand.Execute := TRUE;		
		ELSIF NOT Close AND (MC_BR_BarrierReadInfo.BarrierInfo.Status = mcACPTRAK_BARRIER_CLOSED) THEN
			MC_BR_BarrierCommand.Command := mcACPTRAK_BARRIER_OPEN;
			MC_BR_BarrierCommand.Execute := TRUE;
		END_IF
		
		(* When the tickets are added, reset the execute *)
		IF MC_BR_BarrierCommand.Done THEN	
			MC_BR_BarrierCommand.Execute := FALSE;
		END_IF
			
		(* Call the function block *)
		MC_BR_BarrierCommand(); 

			
		IF MC_BR_BarrierReadInfo.BarrierInfo.Status = mcACPTRAK_BARRIER_OPENED THEN
			BarrierClosed := FALSE;
			BarrierOpen := TRUE;
		ELSIF MC_BR_BarrierReadInfo.BarrierInfo.Status = mcACPTRAK_BARRIER_CLOSED THEN
			BarrierClosed := TRUE; 
			BarrierOpen := FALSE;
		END_IF;
		
		Error := MC_BR_BarrierCommand.Error;
		ErrorID := MC_BR_BarrierCommand.ErrorID;
	ELSE
		MC_BR_BarrierCommand( Execute := FALSE );
		MC_BR_BarrierReadInfo( Enable := FALSE );
		Error := FALSE;
		ErrorID := 0;
	END_IF
END_FUNCTION_BLOCK
