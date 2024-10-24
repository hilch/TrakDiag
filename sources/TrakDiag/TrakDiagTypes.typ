
TYPE
	TD_GetShuttlesShuttleInfoType : 	STRUCT 
		Axis : McAxisType; (*shuttle's axis*)
		ShuttleID : UDINT; (*shuttle's unique ID*)
	END_STRUCT;
	TD_HttpServiceType : 	STRUCT 
		fbHttpService : httpService;
		fbGetParamUrl : httpGetParamUrl;
		urlParamBuffer : STRING[256];
		tempDINT : DINT;
		serviceName : STRING[80];
		uri : STRING[256];
		uri_part : ARRAY[0..5]OF STRING[128];
		requestHeader : httpRequestHeader_t;
		requestData : STRING[2048];
		responseHeader : httpResponseHeader_t;
		responseData : STRING[65536];
		statistics : httpStatistics_t;
		lastError : UINT;
		shuttle : McAcpTrakShuttleData;
	END_STRUCT;
	TD_SegInfoType : 	STRUCT  (*segment information*)
		numberOfSegments : UINT;
		segment : ARRAY[0..TD_MAX_SUPPORTED_SEGMENTS_ASM]OF McSegmentType;
		segmentInfo : ARRAY[0..TD_MAX_SUPPORTED_SEGMENTS_ASM]OF McAcpTrakSegGetInfoType;
		segmentData : ARRAY[0..TD_MAX_SUPPORTED_SEGMENTS_ASM]OF McAcpTrakSegmentData;
	END_STRUCT;
	TD_ServicesShuttleType : 	STRUCT 
		index : UDINT; (*internal shuttle index*)
		flags : USINT; (*Bit0: shuttle is active
Bit1: shuttle is virtual*)
		plcOpenState : McAxisPLCopenStateEnum; (*PLCopen state*)
		segmentID : UINT; (*ID of segment where shuttle is located*)
		segmentPosition : USINT; (*position on segment [%] where shuttle is located*)
	END_STRUCT;
	TD_ServicesShuttlesType : 	STRUCT 
		numberOfEntries : USINT; (*number of *all* shuttles (deleted shuttles included)*)
		numberOfActiveShuttles : USINT; (*number of active shuttles*)
		shuttle : ARRAY[0..TD_MAX_SUPPORTED_SHUTTLES_ASM]OF TD_ServicesShuttleType;
	END_STRUCT;
	TD_SegErrorCommandEnum : 
		( (*error type for TD_SegCommandError*)
		TD_SEGERRCMD_SEGMENT, (*segment error*)
		TD_SEGERRCMD_CHANNEL, (*channel error*)
		TD_SEGERRCMD_ENCODER (*encoder error*)
		);
	TD_LoggerEntryType : 	STRUCT 
		RecordID : ArEventLogRecordIDType;
		OriginRecordID : ArEventLogRecordIDType;
		EventID : DINT;
		Severity : USINT;
		ObjectID : STRING[36];
		Description : STRING[80];
		TimeStamp : ArEventLogTimeStampType; (*Timestamp UTC + nanoseconds*)
		LocalTime : DATE_AND_TIME; (*local time*)
	END_STRUCT;
END_TYPE
