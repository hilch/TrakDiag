[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Made For B&R](https://github.com/hilch/BandR-badges/blob/main/Made-For-BrAutomation.svg)](https://www.br-automation.com)

# TrackDiag
Library for ACOPOStrak diagnosis

# Prerequisites

- mappMotion 5.21.2

# Functions / function blocks

# TD_ascDT

Returns ASCII timestring for DATE_AND_TIME


# TD_filenameDT

Returns a filename with given prefix and IEC like timestamp included.

usage:
```
	TD_filenameDT( ADR('TD_Recorder_'), ADR(filename), SIZEOF(filename) );
```


## TD_LogWatchBOOL

Write status of BOOL variable into logger.
If logger does not exist it will be created.
Set 'EventID' according to ArEventLog.

usage:
```
	PROGRAM _INIT
		TD_LogWatchBOOL_0( Enable := FALSE, LogName := 'mylog', SignalName := 'barrier status', EventID := BASIC_INFO_ID  );
		TD_LogWatchBOOL_0.Enable := TRUE;
	END_PROGRAM
	
	PROGRAM _CYCLIC
		(* wait for signal change *)
		TD_LogWatchBOOL_0 Signal := barrierStatus) );
	END_PROGRAM
```


## TD_LogWatch8Flags

Write status of 8 flags variable into logger.
If logger does not exist it will be created.
Set 'EventID' according to ArEventLog.


usage:
```
	PROGRAM _INIT
		TD_LogWatch8Flags_0( Enable := FALSE, LogName := 'mylog', SignalName := 'plc signals', EventID := BASIC_INFO_ID );
		TD_LogWatch8Flags_0.Flag[0] := 'fr.Power';
		TD_LogWatch8Flags_0.Flag[1] := 'fr.Auto';
		TD_LogWatch8Flags_0.Flag[2] := 'to.Power';
		TD_LogWatch8Flags_0.Flag[3] := 'to.Auto';
		TD_LogWatch8Flags_0.Flag[4] := 'to.Cover';
		TD_LogWatch8Flags_0.Flag[5] := 'to.Error';
		TD_LogWatch8Flags_0.Enable := TRUE;
	END_PROGRAM
	
	PROGRAM _CYCLIC
		(* collect data *)
		plcFlags.0 := fromPLC.PowerOn;
		plcFlags.1 := fromPLC.StartAutomatic;
		plcFlags.2 := toPLC.PowerIsOn;
		plcFlags.3 := toPLC.AutomaticActive;
		plcFlags.4 := toPLC.CoverOpenPermission;
		plcFlags.5 := toPLC.Error;
		(* wait for signal change *)
		TD_LogWatch8Flags_0( Signal := plcFlags );
	END_PROGRAM
```


## TD_LogWatchDINT

Write status of DINT variable into logger.
If logger does not exist it will be created.
Set 'EventID' according to ArEventLog.


usage:
```
	PROGRAM _INIT
		TD_LogWatchDINT_0( Enable := FALSE, LogName := 'mylog', SignalName := 'shuttle ID', EventID := BASIC_INFO_ID );
		TD_LogWatchDINT_0.Enable := TRUE;
	END_PROGRAM
	
	PROGRAM _CYCLIC
		(* wait for signal change *)
		TD_LogWatchDINT_0( Signal := shuttleID) );
	END_PROGRAM
```


## TD_LogWatchSTRING

Write contents of STRING variable into logger.
If logger does not exist it will be created.
Set 'EventID' according to ArEventLog.


usage:
```
	PROGRAM _INIT
		TD_LogWatchSTRING_0( Enable := FALSE, LogName := 'mylog', SignalName := 'step', EventID := BASIC_INFO_ID );
		TD_LogWatchSTRING_0.Enable := TRUE;
	END_PROGRAM
	
	PROGRAM _CYCLIC
		(* wait for signal change *)
		TD_LogWatchSTRING_0( Signal := stepStr );
	END_PROGRAM
```


## TD_LogWrite

Write into Logger. Generic function.
If logger does not exist it will be created.
Set 'EventID' according to ArEventLog.


usage:
```
	PROGRAM _INIT
		TD_LogWrite_0(Execute := FALSE, Name := 'mylog', ObjectID := 'shuttle info', EventID := BASIC_INFO_ID );
	END_PROGRAM
	
	PROGRAM _CYCLIC
		(* wait for an event *)
		IF shuttleArrived THEN
			shuttleArrived := FALSE;

			(* prepare storing data into logger *)
			TD_LogWrite_0.Ascii := 'cnt:';
			brsitoa( shuttleCounter, ADR(tempstring) );
			brsstrcat( ADR(TD_LogWrite_0.Ascii),  ADR(tempstring) );

			brsstrcat( ADR(TD_LogWrite_0.Ascii),  ADR( ' ID:') );
			brsitoa( shuttleID, ADR(tempstring) );
			brsstrcat( ADR(TD_LogWrite_0.Ascii),  ADR(tempstring) );
			
			TD_LogWrite_0( Execute := TRUE );
			TD_LogWrite_0( Execute := FALSE ); (* reset fb *)			
			
		END_IF
	END_PROGRAM
```


## TD_SegCommandError

Simulate Segment Error.

usage:
```
	PROGRAM _INIT
		TD_SegCommandError_0( Execute := FALSE, Segment := ADR(Seg11), Type := TD_SEGERRCMD_ENCODER );  (* reset fb *)
	END_PROGRAM
	
	PROGRAM _CYCLIC
		TD_SegCommandError_0( Execute := simulateError );
	END_PROGRAM
```


## TD_CrashReport

Read motion logger.

usage:
```
	PROGRAM _INIT
		MC_BR_AsmReadInfo_AcpTrak_0.Assembly := ADR(gAssembly_1);
		MC_BR_AsmReadInfo_AcpTrak_0.Enable := TRUE;
	
		TD_CrashReport_0( thrSeverity := arEVENTLOG_SEVERITY_ERROR,
				objectStartsWith1 := 'Seg_',
				observationPeriod := T#2s,
				pLoggerEntries := ADR(errorEntry), (* container for all logger entries *)
				SizeLoggerEntries := SIZEOF(errorEntry),
				Execute := FALSE ); (* reset fb *)
	END_PROGRAM
	
	PROGRAM _CYCLIC
		MC_BR_AsmReadInfo_AcpTrak_0();	
		TD_CrashReport_0( Execute := MC_BR_AsmReadInfo_AcpTrak_0.AssemblyInfo.SegmentsInErrorStopCount <> 0 );
	END_PROGRAM
```



## TD_LoggerReadEntry

Read logger entry (internal use).


## TD_Recorder

Scene Viewer like shuttle 'flight recorder'.
Records shuttle data cyclically. If 'Trigger' is set a html file containing shuttle movements is created.
Thus the period **before** the trigger event becomes visible.

Since MC_BR_AsmCopyShuttleData_AcpTrak is used internally and only one instance of this FB is allowed in
project one can use the shuttle data provided by 'DataAddress'/'DataSize' for own usage instead.

usage:
```
	PROGRAM _INIT
		MC_BR_AsmReadInfo_AcpTrak_0.Assembly := ADR(gAssembly_1);
		MC_BR_AsmReadInfo_AcpTrak_0.Enable := TRUE;

		TD_Recorder_0.AssemblyName := 'gAssembly_1';
		TD_Recorder_0.UserDataSize := SIZEOF(TSLComShuttleType);
		TD_Recorder_0.FileDeviceName := 'LogData';
		TD_Recorder_0.DataAddress := ADR(shuttleData);
		TD_Recorder_0.DataSize := SIZEOF(shuttleData);	
	END_PROGRAM

	PROGRAM _CYCLIC
		MC_BR_AsmReadInfo_AcpTrak_0();
		
		TD_Recorder_0.Enable  := MC_BR_AsmReadInfo_AcpTrak_0.AssemblyInfo.PowerOn;
		TD_Recorder_0.Trigger := MC_BR_AsmReadInfo_AcpTrak_0.AssemblyInfo.SegmentsInErrorStopCount <> 0;

		TD_Recorder_0();	 
	END_PROGRAM
```







