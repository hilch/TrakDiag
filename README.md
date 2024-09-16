[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Made For B&R](https://github.com/hilch/BandR-badges/blob/main/Made-For-BrAutomation.svg)](https://www.br-automation.com)

# TrakDiag
Library for ACOPOStrak diagnosis.
Included are some function blocks that can be used to easily make entries in application specific loggers. 
In the event of an error, internal states (state machines, internal statuses, shuttle user data) can thus be brought into connection with the 'Motion' logger.

As a further aid, a 'flight recorder' is available, which graphically displays the last seconds before an error.

Finally, a web-based diagnostic interface can be generated with this library, which continuously displays the shuttle positions.

## Dependencies / Preconditions

- ArEventLog
- astime
- DataObj
- McAcpTrak >= 5.21.1
- standard
- McBase
- sys_lib
- brsystem
- FileIO
- AsHttp
- AsArSdm

GCC >= V6.3.0



# Functions / function blocks

## Helper functions for generating strings with timestamps.

### TD_ascDT

Returns ASCII timestring for DATE_AND_TIME

usage:
```
TD_ascDT( entry.LocalTime, ADR(strAsctime), SIZEOF(strAsctime) );  (* convert time stamp to readable string *)
```
The output will be a string like `'Thu Aug 23  14:55:20'`.

### TD_filenameDT

Returns a valid filename with given prefix and IEC like timestamp included.
This function block is used for generating filenames within TD_Recorder and can be also useful for
generating filenames in combination with library AsArSdm.

usage:
```
TD_filenameDT( ADR('SystemDump_'), ADR(filename), SIZEOF(filename) );
```
The output will be a string like `'SystemDump_2023-01-15T14_55_20'`.


## Function blocks for logging

These are just convinience function around ArEventLog. If logger does not exist it will be created.
(Older Automation Runtime seem to have problems generating logbooks in tasks with short cycle times. 
This can be avoided if these functions are called once in _INIT.)
Set 'EventID' according to ArEventLog to decide if message is error, warning, info etc.
Every write command is executed within 1 cycle.

### TD_LogWatchBOOL

Write status of BOOL variable into logger on variable change.

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


### TD_LogWatch8Flags

Writes status of up to 8 bits ('flags') into logger on variable change.
Each bit is assigned a text via '.Flag[x]', where a '1' state is indicated with upper case letters and a '0' state with lower case letters. 
If the output text in logger is preceded by an asterisk '*', this means that the state of this bit has changed.

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


### TD_LogWatchDINT

Writes status of DINT variable into logger on variable change.

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


### TD_LogWatchSTRING

Writes contents of STRING variable into logger on variable change.

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


### TD_LogWrite

Writes into Logger. Generic function with 'Execute' input.


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

Simulates Segment Error.
Currently 'SEGMENT', 'CHANNEL' and 'ENCODER' errors are supported.

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

Read motion logger entries into array and provides some filtering options.
The purpose is to provide some more information about segment errors for an operator.
Overreads entries that refer to shuttle commands since these are hardly useful for operators.

(As the capabilities of mappMotion with respect to alarm handling continue to expand, the importance of this function block will diminish in future mappMotion versions.)

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

Records shuttle data cyclically and provides a video for playing back the last seconds before a failure happend as scene viewer would do.

If 'Trigger' is set a html file containing the 'SvgData' with shuttle movements and a system dump file is created.

Since MC_BR_AsmCopyShuttleData_AcpTrak is used internally and only one instance of this FB is allowed in
project one can use the shuttle data provided by 'DataAddress'/'DataSize' for own usage instead.

usage:
```
PROGRAM _INIT
	MC_BR_AsmReadInfo_AcpTrak_0.Assembly := ADR(gAssembly_1);
	MC_BR_AsmReadInfo_AcpTrak_0.Enable := TRUE;

	TD_Recorder_0.AssemblyName := 'gAssembly_1';
	TD_Recorder_0.UserDataSize := 0; (* no user data used *)
	TD_Recorder_0.FileDeviceName := 'Diagnosis';
	TD_Recorder_0.DataAddress := ADR(shuttleData);
	TD_Recorder_0.DataSize := SIZEOF(shuttleData);	(* type of McAcpTrakShuttleData[] if no shuttle user data is defined, see MC_BR_AsmCopyShuttleData_AcpTrak *)
END_PROGRAM

PROGRAM _CYCLIC
	MC_BR_AsmReadInfo_AcpTrak_0();

	TD_Recorder_0.Enable  := MC_BR_AsmReadInfo_AcpTrak_0.AssemblyInfo.PowerOn;
	TD_Recorder_0.Trigger := MC_BR_AsmReadInfo_AcpTrak_0.AssemblyInfo.ShuttlesInErrorStopCount <> 0;

	TD_Recorder_0();	 
END_PROGRAM
```

output:

![example_recorder](https://github.com/hilch/TrakDiag/blob/main/examples/screenshot_recorder.png)

[download this example and open it in your browser](https://github.com/hilch/TrakDiag/blob/main/examples/TD_Recorder_2023-07-06T13_40_34.html)


## TD_WebServices

(>= V2.1.x)
Provides some live information about Trak system via web based services.
Data is provided by TD_Recorder or MC_BR_AsmCopyShuttleData_AcpTrak.


usage:
```
PROGRAM _INIT
	...
	...
	
	TD_WebServices_0.AssemblyName := TD_Recorder_0.AssemblyName;
	TD_WebServices_0.DataAddress := TD_Recorder_0.DataAddress;
	TD_WebServices_0.DataSize := TD_Recorder_0.DataSize;
	TD_WebServices_0.UserDataSize := TD_Recorder_0.UserDataSize;
	TD_WebServices_0.Enable := TRUE;	
END_PROGRAM

PROGRAM _CYCLIC
	...
	TD_WebServices_0();	 
	...
END_PROGRAM
```


### /TrakWebApi/index.html

HTML page with assembly's SvgData and shuttle positions.
Simply embedd this path into mappView's WebViewer widget.

![screenshot_mappView.png](https://github.com/hilch/TrakDiag/blob/main/examples/screenshot_mappView.png)

### /TrakWebApi/info

JSON with Basic information

```
{"TrakDiag" : "2.02.0","McAcpTrak" : "V5.22.1"}
```

### /TrakWebApi/positions

JSON with shuttle information

we return an array of pairs of two UDINT:
- [0] Bit 0-31  : shuttle index
- [1] Bit 0-7   : PLCopen state
- [1] Bit 8     : active shuttle
- [1] Bit 9     : virtual shuttle
- [1] Bit 10-20 : segment index (11 Bit)
- [1] Bit 21-27 : segment position in % (7 Bit)

```
[[1,197149959],[2,102778119],[3,10503431],[4,121636103],[5,6291719],[6,98566407],[7,182454535],[8,90179847],[9,207621383],[10,113249543]]
```

### /TrakWebApi/assembly

assembly information (>= V2.2.x)

```
{"CommunicationReady" : true,"ReadyForPowerOn" : true,"PowerOn" : true,"SegmentsInDisabledCount" : 0,"SegmentsInStoppingCount" : 0,"SegmentsInErrorStopCount" : 0,"ShuttlesCount" : 10,"ShuttlesInErrorStopCount" : 0}
```

### /TrakWebApi/segments

Deliver JSON with list of assembly's segments.

```
[ { "name": "gSegB7", "ID": 31, "length": 0.450642056481534 },{ "name": "gSegB6", "ID": 30, "length": 0.450642056481534 },{ "name": "gSegB8", "ID": 29, "length": 0.450642056481534 },{ "name": "gSegB9", "ID": 28, "length": 0.450642056481534 },{ "name": "gSegB10", "ID": 27, "length": 0.450642056481534 },{ "name": "gSegB11", "ID": 26, "length": 0.240000000000000 },{ "name": "gSegB12", "ID": 25, "length": 0.240000000000000 },{ "name": "gSegB13", "ID": 24, "length": 0.240000000000000 },{ "name": "gSegB14", "ID": 23, "length": 0.450642056481534 },{ "name": "gSegB16", "ID": 22, "length": 0.450642056481534 },{ "name": "gSegB15", "ID": 21, "length": 0.450642056481534 },{ "name": "gSegB17", "ID": 20, "length": 0.450642056481534 },{ "name": "gSegB18", "ID": 19, "length": 0.450642056481534 },{ "name": "gSegB1", "ID": 18, "length": 0.450642056481534 },{ "name": "gSegB2", "ID": 17, "length": 0.240000000000000 },{ "name": "gSegB3", "ID": 16, "length": 0.240000000000000 },{ "name": "gSegB4", "ID": 15, "length": 0.240000000000000 },{ "name": "gSegB5", "ID": 14, "length": 0.450642056481534 },{ "name": "gSegA2", "ID": 13, "length": 0.660000000000000 },{ "name": "gSegA3", "ID": 12, "length": 0.450642056481534 },{ "name": "gSegA4", "ID": 11, "length": 0.240000000000000 },{ "name": "gSegA5", "ID": 10, "length": 0.240000000000000 },{ "name": "gSegA6", "ID": 9, "length": 0.240000000000000 },{ "name": "gSegA7", "ID": 8, "length": 0.450642056481534 },{ "name": "gSegA8", "ID": 7, "length": 0.660000000000000 },{ "name": "gSegA9", "ID": 6, "length": 0.660000000000000 },{ "name": "gSegA10", "ID": 5, "length": 0.450642056481534 },{ "name": "gSegA11", "ID": 4, "length": 0.240000000000000 },{ "name": "gSegA12", "ID": 3, "length": 0.240000000000000 },{ "name": "gSegA13", "ID": 2, "length": 0.240000000000000 },{ "name": "gSegA14", "ID": 1, "length": 0.450642056481534 },{ "name": "gSegA1", "ID": 0, "length": 0.660000000000000 }]
```

### /TrakWebApi/segment_status

returns segments' status (bit coded).

```
[ 7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7]
```

### /TrakWebApi/svgdata

Deliver assembly's generated SvgData (SVG).




