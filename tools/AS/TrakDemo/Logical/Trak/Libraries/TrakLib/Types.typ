
TYPE
	TL_ShWheelChkData_Type : 	STRUCT 
		ShuttleAxis : McAxisType;
		PeakLagError : LREAL; (*maximum lag error*)
		AverageLagError : LREAL; (*average lag error*)
		Good : BOOL; (*shuttle wheels in good condition*)
		Bad : BOOL; (*shuttle wheels in bad condition*)
	END_STRUCT;
END_TYPE
