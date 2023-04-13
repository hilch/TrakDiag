
TYPE
	TD_RecorderSegInfoType : 	STRUCT  (*segment information*)
		numberOfSegments : UINT;
		fill1 : USINT;
		fill2 : USINT;
		segment : ARRAY[0..TD_MAX_SUPPORTED_SEGMENTS]OF McSegmentType;
		segmentInfo : ARRAY[0..TD_MAX_SUPPORTED_SEGMENTS]OF McAcpTrakSegGetInfoType;
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
