
{REDUND_ERROR} FUNCTION TD_ascDT : BOOL (*Returns ASCII timestring for DATE_AND_TIME*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		DT1 : DATE_AND_TIME;
		pString : UDINT; (*pointer to output string*)
		maxSize : USINT; (*max length of output string*)
	END_VAR
END_FUNCTION

{REDUND_ERROR} FUNCTION TD_filenameDT : UINT (*Returns a filename with timestamp included*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		prefix : UDINT; (*pointer to prefix string (input)*)
		pString : UDINT; (*pointer to output string*)
		maxSize : UDINT; (*max length of output string*)
	END_VAR
END_FUNCTION

{REDUND_ERROR} FUNCTION_BLOCK TD_LogWatchBOOL (*Write status of BOOL variable into logger*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Enable : BOOL;
		Signal : BOOL;
		LogName : STRING[10];
		SignalName : STRING[36];
		EventID : DINT;
	END_VAR
	VAR_OUTPUT
		Valid : BOOL;
		Busy : BOOL;
		Error : BOOL;
		StatusID : DINT;
	END_VAR
	VAR
		signalOld : BOOL;
		step : UINT;
		fbLogWrite : TD_LogWrite;
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR} FUNCTION_BLOCK TD_LogWatch8Flags (*Write status of 8 flags variable into logger*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Enable : BOOL;
		Signal : USINT;
		LogName : STRING[10];
		SignalName : STRING[36];
		EventID : DINT;
		Flag : ARRAY[0..7] OF STRING[8];
	END_VAR
	VAR_OUTPUT
		Valid : BOOL;
		Busy : BOOL;
		Error : BOOL;
		StatusID : DINT;
	END_VAR
	VAR
		signalOld : USINT;
		step : UINT;
		fbLogWrite : TD_LogWrite;
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR} FUNCTION_BLOCK TD_LogWatchDINT (*Write status of DINT variable into logger*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Enable : BOOL;
		Signal : DINT;
		LogName : STRING[10];
		SignalName : STRING[36];
		EventID : DINT;
	END_VAR
	VAR_OUTPUT
		Valid : BOOL;
		Busy : BOOL;
		Error : BOOL;
		StatusID : DINT;
	END_VAR
	VAR
		signalOld : DINT;
		step : UINT;
		fbLogWrite : TD_LogWrite;
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR} FUNCTION_BLOCK TD_LogWatchSTRING (*Write contents of STRING variable into logger*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Enable : BOOL;
		Signal : STRING[120];
		LogName : STRING[10];
		SignalName : STRING[36];
		EventID : DINT;
	END_VAR
	VAR_OUTPUT
		Valid : BOOL;
		Busy : BOOL;
		Error : BOOL;
		StatusID : DINT;
	END_VAR
	VAR
		signalOld : STRING[120];
		step : UINT;
		fbLogWrite : TD_LogWrite;
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR} FUNCTION_BLOCK TD_LogWrite (*Write into Logger*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Execute : BOOL;
		Name : STRING[10];
		EventID : DINT;
		ObjectID : STRING[36];
		Ascii : STRING[120];
	END_VAR
	VAR_OUTPUT
		Done : BOOL;
		Busy : BOOL;
		Error : BOOL;
		StatusID : DINT;
	END_VAR
	VAR
		fbLogCreate : ArEventLogCreate;
		fbLogGetIdent : ArEventLogGetIdent;
		fbLogWrite : ArEventLogWrite;
		step : UINT;
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR} FUNCTION_BLOCK TD_CrashReport (*Read motion logger*)
	VAR_INPUT
		Execute : BOOL;
		pLoggerEntries : {REDUND_UNREPLICABLE} UDINT; (*pointer to TD_LoggerEntryType[]*)
		SizeLoggerEntries : UDINT; (*sizeof TD_LoggerEntryType[]*)
		thrSeverity : USINT; (*severity threshold [0-3]*)
		objectStartsWith1 : STRING[5]; (*filter objects whose names begin like this*)
		objectStartsWith2 : STRING[5]; (*filter objects whose names begin like this*)
		observationPeriod : {REDUND_UNREPLICABLE} TIME;
	END_VAR
	VAR_OUTPUT
		MaxEntries : UINT; (*max number of entries possible in TD_LoggerEntryType[]*)
		NbrOfEntries : UINT; (*no of entries in TD_LoggerEntryType[]*)
		Done : BOOL;
		Busy : BOOL;
		Error : BOOL;
		StatusID : DINT;
	END_VAR
	VAR
		step : UINT;
		index : UINT; (*current index in list*)
		fbGetIdent : ArEventLogGetIdent;
		fbGetPreviousRecordID : ArEventLogGetPreviousRecordID;
		fbGetLatestRecordID : ArEventLogGetLatestRecordID;
		fbDTGetTime : DTGetTime;
		fbReadEntry : TD_LoggerReadEntry;
		tonDelayRead : TON;
		eventTimeMinimum : DATE_AND_TIME; (*ignore entries before this time*)
		counterEntries : UDINT;
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR} FUNCTION_BLOCK TD_SegCommandError (*Simulate Segment Error*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Segment : REFERENCE TO McSegmentType;
		Type : TD_SegErrorCommandEnum;
		Execute : BOOL;
	END_VAR
	VAR_OUTPUT
		Done : BOOL;
		Busy : BOOL;
		Error : BOOL;
		StatusID : DINT;
	END_VAR
	VAR
		fbSegCommandError : MC_BR_SegCommandError_AcpTrak;
		fbSegPowerOn : MC_BR_SegPowerOn_AcpTrak;
		step : UINT;
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR} FUNCTION_BLOCK TD_LoggerReadEntry (*(internal use)*)
	VAR_INPUT
		Execute : BOOL;
		Ident : ArEventLogIdentType;
		RecordID : ArEventLogRecordIDType;
	END_VAR
	VAR_OUTPUT
		Entry : TD_LoggerEntryType; (*TD_LoggerEntryType*)
		Done : BOOL;
		Error : BOOL;
		StatusID : DINT;
	END_VAR
	VAR
		fbRead : ArEventLogRead;
		fbReadDescription : ArEventLogReadDescription;
		fbReadObjectID : ArEventLogReadObjectID;
		fbUtcToLocalTime : UtcDT_TO_LocalDTStructure;
		UtcToLocalTime_Done : BOOL;
		LocalTimeDTStructure : DTStructure;
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR} FUNCTION_BLOCK TD_GetShuttles (*Returns the axis references of all shuttles of the assembly that corresponds to the selected mode*)
	VAR_INPUT
		Assembly : REFERENCE TO McAssemblyType;
		Execute : BOOL; (*Assembly reference*)
		SelectionMode : McAcpTrakGetShuttleModeEnum; (*Criteria according to which the shuttles to be output are selected*)
	END_VAR
	VAR_OUTPUT
		Shuttle : ARRAY[0..TD_MAX_SUPPORTED_SHUTTLES_ASM] OF TD_GetShuttlesShuttleInfoType;
		Count : UINT;
		Done : BOOL;
		Busy : BOOL;
		Error : BOOL;
		ErrorID : DINT;
	END_VAR
	VAR
		step : UINT;
		fbGetShuttle : MC_BR_AsmGetShuttleSel_AcpTrak;
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR} FUNCTION_BLOCK TD_ShuttleErrorTexts (*Read last error records of a shuttle given by ID or Axis reference*)
	VAR_INPUT
		Execute : BOOL;
		Assembly : REFERENCE TO McAssemblyType;
		Axis : REFERENCE TO McAxisType; (*Shuttle Axis reference*)
		Index : UDINT; (*Shuttles index (if Axis is 0)*)
		Language : McLanguageEnum;
	END_VAR
	VAR_OUTPUT
		Done : BOOL;
		Busy : BOOL;
		Error : BOOL;
		StatusID : DINT;
		NumberOfRecords : UINT;
		Records : McErrorRecordsType;
	END_VAR
	VAR
		step : UINT;
		axis : McAxisType;
		fbAsmGetShuttle : MC_BR_AsmGetShuttle_AcpTrak;
		fbReadErrorText : MC_BR_ReadErrorText;
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR} FUNCTION_BLOCK TD_SegmentsInfo (*Gets information about all segments in assembly*)
	VAR_INPUT
		Execute : BOOL;
		Assembly : REFERENCE TO McAssemblyType;
		MaxCount : UINT; (*max. permitted number of segments in Segments resp. SegmentsInfo*)
		Segments : REFERENCE TO McSegmentType; (*pointer to array of McSegmentType*)
		SegmentsInfo : REFERENCE TO McAcpTrakSegGetInfoType; (*pointer to array of McAcpTrakSegGetInfoType*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL;
		Busy : BOOL;
		Error : BOOL;
		StatusID : DINT;
		Count : UINT;
	END_VAR
	VAR
		step : UINT;
		fbGetSegment : MC_BR_AsmGetSegment_AcpTrak;
		fbSegGetInfo : MC_BR_SegGetInfo_AcpTrak;
		n : UINT;
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR} FUNCTION_BLOCK TD_LimitFileNumber (*(internal use)*)
	VAR_INPUT
		Execute : BOOL;
		FileDeviceName : STRING[32]; (*file device*)
		DirectoryName : STRING[32]; (*directory name*)
		FileNamePattern : STRING[128]; (*regular expression for file names*)
		MaxCount : UINT; (*limited number of files*)
	END_VAR
	VAR_OUTPUT
		Done : BOOL;
		Busy : BOOL;
		Error : BOOL;
		ErrorID : DINT;
		ErrorFileNamePattern : STRING[80]; (*extended error information*)
	END_VAR
	VAR
		step : UINT;
		fbDirOpen : DirOpen;
		fbDirRead : DirReadEx;
		fbDirClose : DirClose;
		fbFileDelete : FileDelete;
		dirEntry : fiDIR_READ_EX_DATA;
		fileCount : UINT;
		timeStamps : ARRAY[0..100] OF DATE_AND_TIME;
		pass : USINT;
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR} FUNCTION_BLOCK TD_Recorder (*Shuttle 'flight recorder'*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Enable : BOOL;
		AssemblyName : {REDUND_UNREPLICABLE} STRING[32]; (*name of the assembly*)
		DataAddress : UDINT; (*address of shuttle data array*)
		DataSize : UDINT; (*sizeo of shuttle data array. 
type of McAcpTrakShuttleData[] if no shuttle user data is defined, see MC_BR_AsmCopyShuttleData_AcpTrak *)
		UserDataSize : USINT; (*shuttle user data size in bytes*)
		DataObjectName : STRING[10]; (*optional: temporary data object for memory allocation*)
		FileDeviceName : STRING[32]; (*file device where recorder data should be stored*)
		FileNamePrefix : STRING[32]; (*filename prefix used for generated recorder file*)
		MaxNumberOfRecordings : {REDUND_UNREPLICABLE} USINT; (*Maximum number of recordings*)
		NumberOfCycles : USINT; (*Refresh rate in number of task cycles (min. 3)*)
		Trigger : BOOL; (*stops recording and save output file*)
	END_VAR
	VAR_OUTPUT
		Valid : BOOL;
		Busy : BOOL;
		Error : BOOL;
		Saved : BOOL; (*output file was saved*)
		ErrorID : DINT;
		SegInfo : TD_RecorderSegInfoType; (*segment information*)
		OutputFileName : STRING[255]; (*name of output file*)
		DumpFileName : STRING[255];
	END_VAR
	VAR
		svgFileName : STRING[32];
		step : UINT;
		currentRecord : INT;
		n : UINT;
		refreshCycles : UINT;
		cycleCounter : UINT;
		maxRecords : UINT;
		fbDatObjInfo : DatObjInfo;
		fbDatObjCreate : DatObjCreate;
		fbDatObjDelete : DatObjDelete;
		fbFileCreate : FileCreate;
		fbFileClose : FileClose;
		fbFileWrite : FileWriteEx;
		fbFileOpen : FileOpen;
		fbFileRead : FileReadEx;
		fbCopyShuttleData : MC_BR_AsmCopyShuttleData_AcpTrak;
		fbGetSegment : MC_BR_AsmGetSegment_AcpTrak;
		fbSegGetInfo : MC_BR_SegGetInfo_AcpTrak;
		fbSystemDump : SdmSystemDump;
		fbLimitFileNumber : TD_LimitFileNumber;
		pAssembly : REFERENCE TO McAssemblyType;
		tonTriggerDelay : TON;
		fbRtInfo : RTInfo;
		tempudint : UDINT;
		pDataObject : UDINT;
		pBuffer : UDINT;
		pTimestamps : UDINT;
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR} FUNCTION_BLOCK TD_WebServices (*Trak web services*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Enable : BOOL;
		AssemblyName : {REDUND_UNREPLICABLE} STRING[32]; (*name of the assembly*)
		DataAddress : UDINT; (*address of shuttle data array*)
		DataSize : UDINT; (*sizeo of shuttle data array*)
		UserDataSize : USINT; (*shuttle user data size in bytes*)
		DataObjectName : STRING[10]; (*optional: temporary data object for memory allocation*)
	END_VAR
	VAR_OUTPUT
		Valid : BOOL;
		Busy : BOOL;
		Error : BOOL;
		ErrorID : DINT;
		SegInfo : TD_ServicesSegInfoType; (*segment information*)
		ShuttleInfo : TD_ServicesShuttlesType; (*cyclic shuttle information*)
	END_VAR
	VAR
		svgFileName : STRING[32];
		infoSvgFile : fiFILE_INFO;
		acpTrakVersion : STRING[10];
		step : UINT;
		n : UINT;
		tempudint : UDINT;
		refreshTimer : UDINT;
		hashShuttleInfo : UDINT;
		webData : TD_HttpServiceType;
		pDataObject : UDINT;
		fbDatObjInfo : DatObjInfo;
		fbDatObjCreate : DatObjCreate;
		fbDatObjDelete : DatObjDelete;
		fbFileInfo : FileInfo;
		fbFileClose : FileClose;
		fbFileOpen : FileOpen;
		fbFileRead : FileReadEx;
		fbSegmentsInfo : TD_SegmentsInfo;
		fbAsmReadInfo : MC_BR_AsmReadInfo_AcpTrak;
		fbSegReadInfo : ARRAY[0..TD_MAX_SUPPORTED_SEGMENTS_ASM] OF MC_BR_SegReadInfo_AcpTrak;
		fbShuttleErrorTexts : TD_ShuttleErrorTexts;
		pAssembly : REFERENCE TO McAssemblyType;
		fbRtInfo : RTInfo;
		asmInfo : McAcpTrakAsmInfoType; (*assembly information*)
		stepTimeout : TON;
	END_VAR
END_FUNCTION_BLOCK
