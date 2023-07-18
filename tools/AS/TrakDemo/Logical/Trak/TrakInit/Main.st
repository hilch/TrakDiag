
PROGRAM _INIT
	(* create the loggers *)
	REPEAT
		fbLogWrite( Name := 'MyTrak', EventID := TRAK_BASIC_SUCCESS_ID, ObjectID := 'MyTrak', Ascii := 'startup', Execute := TRUE );
		UNTIL fbLogWrite.Done OR fbLogWrite.Error
	END_REPEAT
	fbLogWrite( Execute := FALSE );
	 
END_PROGRAM

PROGRAM _CYCLIC
	(* Insert code here *)
	 
END_PROGRAM

PROGRAM _EXIT
	(* Insert code here *)
	 
END_PROGRAM

