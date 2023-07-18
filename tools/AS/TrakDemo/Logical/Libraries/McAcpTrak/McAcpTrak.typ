TYPE
	McInternalAssemblyIfType : STRUCT (*Partial interface type (C only)*)
		vtable : DWORD;
	END_STRUCT;

	McAssemblyType : STRUCT
		controlif : REFERENCE TO McInternalAssemblyIfType;
	END_STRUCT;

	McInternalSegmentIfType : STRUCT (*Partial interface type (C only)*)
		vtable : DWORD;
	END_STRUCT;

	McSegmentType : STRUCT
		controlif : REFERENCE TO McInternalSegmentIfType;
	END_STRUCT;

	McInternalSectorIfType : STRUCT (*Partial interface type (C only)*)
		vtable : DWORD;
	END_STRUCT;

	McSectorType : STRUCT
		controlif : REFERENCE TO McInternalSectorIfType;
	END_STRUCT;

	McInternalProcessPointIfType : STRUCT (*Partial interface type (C only)*)
		vtable : DWORD;
	END_STRUCT;

	McProcessPointType : STRUCT
		controlif : REFERENCE TO McInternalProcessPointIfType;
	END_STRUCT;

	McAcpTrakSegProcessParIDModeEnum :
	(
		mcACPTRAK_SEG_PARID_GET := 0, (*read ParID once from the segment*)
		mcACPTRAK_SEG_PARID_SET,	(*write ParID once to the segment*)
		mcACPTRAK_SEG_PARID_GET_NO_NCT (*read ParID once from the segment without entry into NCT*)
	);

	McAcpTrakSegDataTypeEnum :
	(
		mcACPTRAK_SEG_PARTYPE_BOOL := 1, (*data type BOOL*)
		mcACPTRAK_SEG_PARTYPE_SINT, (*data type SINT*)
		mcACPTRAK_SEG_PARTYPE_INT, (*data type INT*)
		mcACPTRAK_SEG_PARTYPE_DINT, (*data type DINT*)
		mcACPTRAK_SEG_PARTYPE_USINT, (*data type USINT*)
		mcACPTRAK_SEG_PARTYPE_UINT, (*data type UINT*)
		mcACPTRAK_SEG_PARTYPE_UDINT, (*data type UDINT*)
		mcACPTRAK_SEG_PARTYPE_REAL, (*data type REAL*)
		mcACPTRAK_SEG_PARTYPE_LREAL := 17, (*data type LREAL*)
		mcACPTRAK_SEG_PARTYPE_VOID := 65535  (*data type unknown*)
	);

	McAcpTrakSegProcessParIDType : STRUCT
		ParID : UINT; (*ParID which is read/written*)
		VariableAddress : UDINT; (*address the value of the ParID is written to/read from*)
		DataType : McAcpTrakSegDataTypeEnum; (*data type of the ParID value*)
	END_STRUCT;

	McAcpTrakTrgPointEventEnum :
	(
		mcACPTRAK_TP_PASSED_NEGATIVE, 	(*passed in negative direction*)
		mcACPTRAK_TP_PASSED_POSITIVE 	(*passed in positive direction*)
	);

	McAcpTrakSearchModeEnum :
	(
		mcACPTRAK_SEARCH_FORWARD, (*search forward*)
		mcACPTRAK_SEARCH_BACKWARD (*search backward*)
	);

	McAcpTrakCopyUserDataModeEnum :
	(
		mcACPTRAK_USERDATA_GET, (*get user data*)
		mcACPTRAK_USERDATA_SET 	(*set user data*)
	);

	McAcpTrakCopyShDataModeEnum :
	(
		mcACPTRAK_SH_DATA_ALL, 		(*copy all shuttle elements to PV*)
		mcACPTRAK_SH_DATA_SPECIFIC		(*copy specific shuttle element array*)
	);

	McAcpTrakCopyShDataTriggerEnum:
	(
		mcACPTRAK_TRIGGER_IMMEDIATELY		(*copying is startet immediately*)
	);

	McAcpTrakShManeuverTypeEnum :
	(
		mcACPTRAK_MANEUVER_NONE := 0, 		(*normal behaviour*)
		mcACPTRAK_MANEUVER_ADJUSTMENT := 1,	(*adjustment maneuver*)
		mcACPTRAK_MANEUVER_STANDSTILL := 2,	(*standstill maneuver*)
		mcACPTRAK_MANEUVER_EMERGENCY := 3 	(*emergency stop maneuver*)
	);

	McAcpTrakShManeuverReasonEnum:
	(
		mcACPTRAK_REASON_UNSPECIFIED := 0,		(*unspecified reason*)
		mcACPTRAK_REASON_SHUTTLE := 1,			(*maneuver due to another shuttle*)
		mcACPTRAK_REASON_LOCAL_VEL_LIMIT := 2,	(*maneuver due to an active local velocity limit*)
		mcACPTRAK_REASON_BARRIER := 3,			(*maneuver due to a closed barrier*)
		mcACPTRAK_REASON_SECTOREND := 4,		(*maneuver due to a closed sector end*)
		mcACPTRAK_REASON_SECTORSWITCH := 5,		(*maneuver due to an invalid sector switch*)
		mcACPTRAK_REASON_LOGISTICS := 6,		(*maneuver due to motion parameters of logistics*)
		mcACPTRAK_REASON_NONE := 7,				(*normal behaviour*)
		mcACPTRAK_REASON_LOCAL_ACC_LIMIT := 8,	(*maneuver due to an acceleration limit*)
		mcACPTRAK_REASON_UNCONTROLLED := 9,		(*maneuver due to transition from controlled to uncontrolled*)
		mcACPTRAK_REASON_RECONTROLLED := 10,	(*maneuver due to transition from uncontrolled to controlled*)
		mcACPTRAK_REASON_CONVOY := 11,			(*maneuver due to homogeneous behavior within convoy*)
		mcACPTRAK_REASON_CON_DIFF_SECTOR := 12,	(*maneuver due to being on a different sector than the convoy*)
		mcACPTRAK_REASON_CON_INV_REF := 13,		(*maneuver due to removal of relative master of a shuttle in a convoy*)
		mcACPTRAK_REASON_CON_SECSWITCH := 14,	(*maneuver due to not being able to switch sector when convoy is switching sector*)
		mcACPTRAK_REASON_CON_UNCONTR := 15,		(*maneuver due to at least one shuttle of a convoy being uncontrolled*)
		mcACPTRAK_REASON_CON_RIGID_MOVE := 16,	(*maneuver due to conflicting elastic flag of convoy for selected movement*)
		mcACPTRAK_REASON_CON_TCP := 17			(*maneuver due to not converging iterative method for target tcp distance*)
	);

	McAcpTrakBarrierTypeEnum:
	(
		mcACPTRAK_BARRIER_USER := 0,			(*user-defined barrier*)
		mcACPTRAK_BARRIER_CONFLICTZONE := 1,	(*internal conflictzone barrier*)
		mcACPTRAK_BARRIER_SEGMENT := 2			(*internal segment barrier*)
	);

	McAcpTrakSelectShSideEnum:
	(
		 mcACPTRAK_SIDE_BOTH := 0			(* Both sides of the shuttle are affected*)
	);

	McAcpTrakSecTypeEnum :
	(
		mcACPTRAK_SEC_USER_DEFINED,		(*user defined sector*)
		mcACPTRAK_SEC_SEGMENT,			(*segment sector*)
		mcACPTRAK_SEC_INTERNAL			(*route*)
	);

	 McAcpTrakRoutePosRelToEnum :
	(
		mcACPTRAK_SEC_START,				(*Target position is calculated in relation to the start of the sector*)
	 	mcACPTRAK_SEC_END					(*Target position is calculated in relation to the end of the sector*)
	);

	McAcpTrakShMovementTypeEnum :
	(
		mcACPTRAK_MOVEMENT_STANDSTILL,	(*no movement active*)
		mcACPTRAK_MOVEMENT_ELASTIC,		(*elastic movement*)
		mcACPTRAK_MOVEMENT_RIGID		(*rigid movement*)
	);

	McAcpTrakMoveCmdEnum :
	(
		mcACPTRAK_MOV_CMD_HALT,					(*halt*)
		mcACPTRAK_MOV_CMD_MOVEABS,				(*move absolute*)
		mcACPTRAK_MOV_CMD_ELASTICMOVEABS,		(*elastic move absolute*)
		mcACPTRAK_MOV_CMD_MOVEVEL,				(*move velocity*)
		mcACPTRAK_MOV_CMD_ELASTICMOVEVEL		(*elastic move velocity*)
	);

	McAcpTrakBarrierCmdEnum :
	(
		mcACPTRAK_BARRIER_OPEN,			(*open the barrier*)
		mcACPTRAK_BARRIER_CLOSE,		(*close the barrier*)
		mcACPTRAK_BARRIER_ADD_TICKETS	(*adds tickets to or subtracts tickets from the barrier*)
	);

	McAcpTrakBarrierStatusEnum :
	(
		mcACPTRAK_BARRIER_OPENED,		(*barrier is open*)
		mcACPTRAK_BARRIER_CLOSED,		(*barrier is closed*)
		mcACPTRAK_BARRIER_TICKETING		(*barrier assigns tickets to shuttles*)
	);

	McAcpTrakCouplingObjCmdEnum :
	(
		mcACPTRAK_COUPLE_OBJ_SET,		(*sets a coupling object for the shuttle*)
		mcACPTRAK_COUPLE_OBJ_REMOVE,	(*removes a coupling object from the shuttle*)
		mcACPTRAK_COUPLE_OBJ_RESET		(*sets a coupling object for the shuttle, independently of whether it currently has a coupling object*)
	);

	 McAcpTrakSimulationOnPlcEnum :
	 (
	  	mcACPTRAK_SIM_STATE_OFF,		(*simulation disabled*)
	  	mcACPTRAK_SIM_STATE_ON			(*simulation enabled*)
	 );

	 McAcpTrakFieldModeEnum :
	 (
	 	mcACPTRAK_FIELD_NORMAL,			(*magnetic field behaving normally*)
	 	mcACPTRAK_FIELD_WEAKENING,		(*magnetic field weakening*)
	 	mcACPTRAK_FIELD_STRENGTHENING	(*magnetic field strengthening *)
	 );

	 McAcpTrakShResizeCmdEnum :
	 (
	 	mcACPTRAK_RESIZE_DIMENSIONS		(*change dimension parameters immediately*)
	 );

	 McAcpTrakShResizeModeEnum :
	 (
	 	 mcACPTRAK_RESIZE_IMMEDIATE, 		(*resize immediately*)
	 	 mcACPTRAK_RESIZE_AVOID_ERRORSTOP	(*resize when the change won't trigger an ErrorStop*)
	 );

	 McAcpTrakAsmDeleteShCmdEnum :
	 (
	 	mcACPTRAK_SH_DELETE_SPECIFIC,	(* Delete only one specific shuttle which is given at the input "Shuttle" *)
	 	mcACPTRAK_SH_DELETE_ALL,		(* Delete all shuttles of the given assembly *)
		mcACPTRAK_SH_DELETE_VIRTUAL,	(* Delete all virtual shuttles of the given assembly *)
		mcACPTRAK_SH_DELETE_NONVIRTUAL	(* Delete all non-virtual shuttles of the given assembly *)
	 );

	McAcpTrakPLCopenStateEnum :
	(
		mcACPTRAK_DISABLED,
		mcACPTRAK_HOMING,
		mcACPTRAK_READY,
		mcACPTRAK_STOPPING,
		mcACPTRAK_ERRORSTOP,
		mcACPTRAK_STARTUP,
		mcACPTRAK_INVALID_CONFIGURATION
	);

	McAcpTrakShRelativeAlignmentEnum :
	(
		mcACPTRAK_ALIGNED_FRONT_TO_BACK,
		mcACPTRAK_ALIGNED_BACK_TO_FRONT,
		mcACPTRAK_ALIGNED_FRONT_TO_FRONT,
		mcACPTRAK_ALIGNED_BACK_TO_BACK
	);

	McAcpTrakShRelativeRefPointEnum :
	(
	 	mcACPTRAK_DIST_CENTER_TO_CENTER,
		mcACPTRAK_DIST_CENTER_TO_EXTENT,
		mcACPTRAK_DIST_EXTENT_TO_CENTER,
		mcACPTRAK_DIST_EXTENT_TO_EXTENT
	);

	McAcpTrakShLocalLimitCmdEnum :
	(
		mcACPTRAK_LL_ADD_SHUTTLE,
		mcACPTRAK_LL_REMOVE_SHUTTLE,
		mcACPTRAK_LL_ADD_CONVOY,
		mcACPTRAK_LL_REMOVE_CONVOY
	);

	McAcpTrakShInteractCmdEnum :
	(
		mcACPTRAK_SH_ACT_COLLAVOID,
		mcACPTRAK_SH_DEACT_COLLAVOID
	);

	McAcpTrakShErrorReasonEnum :
	(
		mcACPTRAK_SH_ERROR_NONE,
		mcACPTRAK_SH_ERROR_UNSPECIFIED,
		mcACPTRAK_SH_ERROR_MANEUVER,
		mcACPTRAK_SH_ERROR_ENCDIFF,
		mcACPTRAK_SH_ERROR_NOGRIPPER,
		mcACPTRAK_SH_ERROR_COMMAND,
		mcACPTRAK_SH_ERROR_SEGMENT,
		mcACPTRAK_SH_ERROR_ASSEMBLY,
		mcACPTRAK_SH_ERROR_INVALIDMOVE,
		mcACPTRAK_SH_ERROR_COUPLING,
		mcACPTRAK_SH_ERROR_CHANNEL
	);

	McAcpTrakSegTypeEnum :
	(
		mcACPTRAK_SEG_CIRC,
		mcACPTRAK_SEG_STR_630,
		mcACPTRAK_SEG_STR_90_TO_CIRC,
		mcACPTRAK_SEG_CIRC_TO_STR_90,
		mcACPTRAK_SEG_STR_660,
		mcACPTRAK_SEG_STR_105_TO_CIRC,
		mcACPTRAK_SEG_CIRC_TO_STR_105,
		mcACPTRAK_SEG_COMPACT_CURVE_180
	);

	McAcpTrakRouteTransitPointsType : STRUCT
		Sector : REFERENCE TO McSectorType; (*midpoint sector reference*)
		Position : LREAL; (*midpoint position on the sector*)
		TransitDirection : McDirectionEnum; (*direction in which the shuttle passes the transit point*)
	END_STRUCT;

	McAcpTrakAdvRouteParType : STRUCT
		StartDirection : McDirectionEnum; (*defines in which direction the shuttle is starting*)
		EndDirection : McDirectionEnum; (*defines in which direction the shuttle is starting*)
		ShuttleOrientation : McDirectionEnum; (*defines the orientation of the shuttle on end position*)
		TransitPoints : REFERENCE TO McAcpTrakRouteTransitPointsType; (*defines the midpoints for the shuttle to go through before reaching the end point *)
		NumberOfTransitPoints : UINT; (*number of used transit points*)
		PosRelativeTo :  McAcpTrakRoutePosRelToEnum; (*End position calculated relative to begin or end of a sector*)
	END_STRUCT;

	McAcpTrakShManeuverInfoType : STRUCT
		Type : McAcpTrakShManeuverTypeEnum := mcACPTRAK_MANEUVER_NONE; (*maneuver type*)
		Reason : McAcpTrakShManeuverReasonEnum := mcACPTRAK_REASON_NONE; (*maneuver reason*)
	END_STRUCT;

	McAcpTrakShCurrentValuesType : STRUCT
		Position : LREAL; 						(*set position on sector*)
		ActualPosition : LREAL; 				(*actual position on sector*)
		ModuloPosition : LREAL; 				(*set position on sector, modulo-reduced by sector length*)
		ActualModuloPosition : LREAL; 			(*actual position on sector, modulo-reduced by sector length*)
		Sector : McSectorType; 					(*sector reference*)
		SectorName : STRING[32]; 				(*name of the reference sector*)
		SectorType : McAcpTrakSecTypeEnum; 		(*kind of sector*)
		SectorLength : LREAL; 					(*length of the reference sector*)
		Orientation : McDirectionEnum; 			(*defines the orientation of the shuttle on reference sector*)
		Velocity : REAL; 						(*current velocity*)
		Acceleration : REAL; 						(*current acceleration*)
		SegmentPosition : McAcpTrakSegPositionType; (* Segment position of the shuttle *)
		MovementType : McAcpTrakShMovementTypeEnum; (*type of movement*)
		CtrlParSetLeft : USINT;					(*active controller parameter set of the left side of the shuttle*)
		CtrlParSetRight : USINT;				(*active controller parameter set of the right side of the shuttle*)
	END_STRUCT;

	McAcpTrakShRouteDestInfoType	: STRUCT
		Position : LREAL;						(*position on Sector*)
		Sector : McSectorType; 					(*sector reference*)
		SectorName : STRING[32]; 				(*name of the reference sector*)
		SectorType : McAcpTrakSecTypeEnum; 		(*kind of sector*)
		VelocityAtDestination : REAL; 			(*velocity at destination*)
		TotalDistance : LREAL; 					(*total distance*)
		RemainingDistance : LREAL; 				(*remaining distance to target*)
	END_STRUCT;

	McAcpTrakShLifeCycleInfoType : STRUCT
		AbsoluteMovementDistance : LREAL; (*distance traveled since the last shuttle identification was performed [measurement units]*)
	END_STRUCT;

	McAcpTrakShSegSetValuesType : STRUCT
		Valid : BOOL; (*values of "SegmentName", "Position", "Channel", "FieldMode" and "SegmentID" are valid*)
		SegmentName : STRING[32]; (*name of the segment*)
		Position : LREAL; (*position of the shuttle on the segment*)
		Channel : USINT; (*channel used to transmit the data*)
		FieldMode : McAcpTrakFieldModeEnum; (*active field mode*)
		SegmentID : UINT; (*ID of the segment*)
	END_STRUCT;

	McAcpTrakShSegCurrentValuesType : STRUCT
		Valid : BOOL; (*values of "SegmentName" and "Position" are valid*)
		SegmentName : STRING[32]; (*name of the segment*)
		Position : LREAL; (*position of the shuttle on the segment*)
	END_STRUCT;

	McAcpTrakObjectTypeEnum :
	(
		mcACPTRAK_OBJECT_SHUTTLE,
		mcACPTRAK_OBJECT_CONVOY
	);

	McAcpTrakShConvoyInfoType : STRUCT
		IsConvoyMaster : BOOL; (*true if the shuttle is its own convoy's master*)
		ConvoyMaster : McAxisType; (*the master of the shuttle's convoy, or that of the convoy's convoy if the shuttle is its own convoy master*)
		ReferenceShuttle : McAxisType; (*the shuttle's relative master in the convoy, or the convoy's relative master if the shuttle is its own convoy master*)
		ReferenceObjectType : McAcpTrakObjectTypeEnum; (*gives information on whether the reference shuttle is a single shuttle or the master of a convoy*)
		Distance : LREAL; (*the shuttle's (or convoy's, resp.) current distance to its relative master in the convoy*)
		CouplingTargetLag : LREAL; (*the difference of desired target and slave's position*)
	END_STRUCT;

	McAcpTrakShInfoType	: STRUCT
		CurrentValues : McAcpTrakShCurrentValuesType; (*info about current values*)
		RouteDestinationInfo : McAcpTrakShRouteDestInfoType; (*info about destination values*)
		ManeuverInfo : McAcpTrakShManeuverInfoType; (*info about current maneuver*)
		LifeCycleInfo : McAcpTrakShLifeCycleInfoType; (*life cycle info*)
		ConvoyInfo : McAcpTrakShConvoyInfoType; (*info about a convoy of the shuttle*)
		AdditionalInfo : McAcpTrakShAddInfoType; (*additional shuttle info*)
	END_STRUCT;

	McAcpTrakShAddInfoType : STRUCT
		ID : UINT; (*ID of the shuttle*)
		UserID : STRING[32]; (*User ID of the shuttle*)
		PLCopenState : McAxisPLCopenStateEnum; (*PLC Open state of the shuttle*)
		Controllable : BOOL; (*true if the shuttle is controllable*)
		ErrorReason : McAcpTrakShErrorReasonEnum;
	END_STRUCT;

	 McAcpTrakShSegmentInfoType	: STRUCT
		SegmentSetValues : ARRAY[0..3] OF McAcpTrakShSegSetValuesType; (*setpoints that are transferred to the segments*)
		SegmentCurrentValues : ARRAY[0..3] OF McAcpTrakShSegCurrentValuesType; (*actual values that are read from the segments*)
	END_STRUCT;

	McAcpTrakSegPositionType : STRUCT
		Segment : McSegmentType; (*a segment the shuttle was located on*)
		Name : STRING[32]; (*name of the segment the shuttle was located on*)
		Position : LREAL; (*position of the shuttle on the segment*)
	END_STRUCT;

	McAcpTrakSecPositionType : STRUCT
		Sector : McSectorType; (*a sector*)
		Name : STRING[32]; (*name of the sector*)
		Position : LREAL; (*position on the sector*)
	END_STRUCT;

	McAcpTrakShRecoveryInfoType	: STRUCT
		SectorPosition : McAcpTrakSecPositionType; (* Sector position where shuttle was controlled last time*)
		OnSector : BOOL; (* Indicates whether shuttle is still on the sector given in sector position*)
		Destination : McAcpTrakSecPositionType; (* Sector position which shuttle was heading for*)
		SegmentPosition : McAcpTrakSegPositionType; (* Segment position where shuttle was controlled last time *)
	END_STRUCT;

	McAcpTrakShErrorShuttleInfoType : STRUCT
		Shuttle : McAxisType; (*the other shuttle*)
		SegmentPosition : McAcpTrakSegPositionType; (*the segment position of the other shuttle*)
	END_STRUCT;

	McAcpTrakShErrorBarrierInfoType : STRUCT
	    BarrierType : McAcpTrakBarrierTypeEnum; (*type of the barrier*)
		ProcessPoint : McProcessPointType; (*the process point containing the barrier*)
		Name : STRING[32]; (*the name of the process point containing the barrier*)
		Status : McAcpTrakBarrierStatusEnum; (*the barrier's mode*)
	END_STRUCT;

	McAcpTrakShErrorLocLimitInfoType : STRUCT
		Name : STRING[32]; (*the local limit's name*)
		VelocityLimit : REAL; (*the limit's bound on the velocity*)
		AccelerationLimit : REAL; (*the limit's bound on the acceleration*)
		InRange : BOOL; (*true iff the error happened in range of the local limit*)
	END_STRUCT;

	McAcpTrakShErrorConvoyInfoType : STRUCT
		ConvoyMaster : McAxisType; (*the convoy master*)
		Shuttle : McAxisType; (*the shuttle causing the error*)
	END_STRUCT;

	McAcpTrakShErrorManeuverInfoType : STRUCT
		Reason : McAcpTrakShManeuverReasonEnum; (*the reason for the maneuver*)
		ShuttleManInfo : McAcpTrakShErrorShuttleInfoType; (*information about maneuver due to shuttle*)
		BarrierManInfo : McAcpTrakShErrorBarrierInfoType; (*information about maneuver due to barrier*)
		LocalLimitManInfo : McAcpTrakShErrorLocLimitInfoType; (*information about maneuver due to local limit*)
		ConvoyManInfo : McAcpTrakShErrorConvoyInfoType; (*information about maneuver due to convoy*)
	END_STRUCT;

	McAcpTrakShErrorEncdiffInfoType : STRUCT
		Difference : LREAL; (*the difference between the encoder positions*)
		LowPosition : McAcpTrakSegPositionType; (*the shuttle's segment position that implies, after translation to the shuttle's sector, to the lowest sector position*)
		HighPosition : McAcpTrakSegPositionType; (*the shuttle's segment position that implies, after translation to the shuttle's sector, to the highest sector position*)
	END_STRUCT;

	McAcpTrakShErrorGripperInfoType : STRUCT
		Segment : McSegmentType; (*the segment where the shuttle could not allocate a gripper*)
		SegmentName : STRING[32]; (*the name of the segment where the shuttle could not allocate a gripper*)
	END_STRUCT;

	McAcpTrakShErrorChannelInfoType : STRUCT
		Segment : McSegmentType; (*the segment whose error triggered the shuttle error*)
		SegmentName : STRING[32]; (*the name of the segment whose error triggered the shuttle error*)
	END_STRUCT;

	McAcpTrakShErrorSegmentInfoType : STRUCT
		Segment : McSegmentType; (*the segment whose error triggered the shuttle error*)
		SegmentName : STRING[32]; (*the name of the segment whose error triggered the shuttle error*)
	END_STRUCT;

	McAcpTrakShErrorInfoType : STRUCT
		SegmentPosition : McAcpTrakSegPositionType; (*a segment position of the shuttle when the error occured*)
		Velocity : REAL; (*the shuttle's absolute velocity*)
		Acceleration : REAL; (*the shuttle's acceleration; if negative, it was decelerating*)
		Reason : McAcpTrakShErrorReasonEnum; (*reason for the error*)
		ManeuverErrorInfo : McAcpTrakShErrorManeuverInfoType; (*information about a maneuver triggering an error*)
		EncdiffErrorInfo : McAcpTrakShErrorEncdiffInfoType; (*information about a encoder discrepancy error*)
		GripperErrorInfo : McAcpTrakShErrorGripperInfoType; (*information about an error where no gripper could be allocated*)
		ChannelErrorInfo : McAcpTrakShErrorChannelInfoType; (*information about an error attributed to a specific segment channel*)
		SegmentErrorInfo : McAcpTrakShErrorSegmentInfoType; (*information about a segment error*)
	END_STRUCT;

	McAcpTrakShResizeParamType : STRUCT
		Mode :  McAcpTrakShResizeModeEnum; (*resize mode selector*)
		ExtentToFront : LREAL; (*extent from the center point of the magnet plate to the front of the shuttle*)
		ExtentToBack : LREAL; (*extent from the center point of the magnet plate to the back of the shuttle*)
		Width : LREAL; (*symmetric width relative to the shuttle center point*)
	END_STRUCT;

	McAcpTrakShSizeInfoType	: STRUCT
		ExtentToFront : LREAL; (*extent from the center point of the magnet plate to the front of the shuttle*)
		ExtentToBack : LREAL; (*extent from the center point of the magnet plate to the back of the shuttle*)
		Width : LREAL; (*symmetric width relative to the shuttle center point*)
	END_STRUCT;

	McAcpTrakShuttleMonData : STRUCT (*Shuttles*)
		Available : BOOL;
		Position : McPosType;
		Orientation : McOrientType;
		ExtentToFront : LREAL;
		ExtentToBack : LREAL;
		Width : LREAL;
		UserData : UDINT;
		Index : UINT;
	END_STRUCT;

	McAcpTrakAssemblyMonData : STRUCT
		Shuttle : ARRAY[0..149] OF McAcpTrakShuttleMonData;
	END_STRUCT;

	McAcpTrakShuttleVisData : STRUCT
		Available : BOOL; (* the shuttle is displayed in the scene *)
		Reserved : ARRAY[0..2] OF USINT;
		PositionX : REAL; (* X position of the shuttle *)
		PositionY : REAL; (* Y position of the shuttle *)
		RotationZ : REAL; (* rotation of the shuttle around Z axis *)
		Text : STRING[31]; (* text to be displayed for the shuttle *)
		ColorRed : REAL; (* red component of the shuttle color *)
		ColorGreen : REAL; (* blue component of the shuttle color *)
		ColorBlue : REAL; (* green component of the shuttle color *)
		ExtentToFront : REAL; (* extent from shuttle center point to front *)
		ExtentToBack : REAL; (* extent from shuttle center point to back *)
		Width : REAL; (* symmetric width of the shuttle *)
	END_STRUCT;

	McAcpTrakAssemblyVisData  : STRUCT (*representation of visualization communication buffer*)
		Shuttle : ARRAY[0..149] OF McAcpTrakShuttleVisData; (* shuttles *)
	END_STRUCT;

	McAcpTrakShuttleData : STRUCT (*Shuttles*)
		Index : UINT;				(*Index of the shuttle, 0 if not used*)
		UserID : STRING[32];		(*User ID of the shuttle*)
		Active : BOOL;				(*Shuttle is available*)
		Virtual : BOOL;				(*Shuttle is virtual*)
		Controlled : BOOL;			(*Shuttle is controlled*)
		PLCopenState : McAxisPLCopenStateEnum; (*PLCopen state*)
		SegmentName : STRING[32];		(*Name of the segment*)
		SegmentPosition : LREAL;	(*Position on the segment*)
		Frame : McFrameType;
		SectorName : STRING[32]; 	(*Name of the user defined sector*)
		SectorPosition : LREAL;		(*Position on the sector*)
	END_STRUCT;

	McAcpTrakAdvCopyShDataType : STRUCT
		ShuttleIndex : UINT;		(*Must be != 0 to copy a specific shuttle to AsmShuttleData*)
		Trigger : McAcpTrakCopyShDataTriggerEnum; (*Trigger for saving shuttle data*)
		DataAddress : UDINT; 		(*Append array address of userdata*)
		DataSize : UDINT; 		(*Size of array of userdata*)
	END_STRUCT;

	McAcpTrakSegInfoType : STRUCT
		CommunicationReady : BOOL; 	(*Segment has a network connection*)
		ReadyForPowerOn : BOOL; 	(*Controller can be switched on*)
		PowerOn : BOOL; 			(*Controller is switched on*)
		StartupCount : UDINT; 		(*Number of times the segment was started up since the last PLC start*)
		CommunicationState : McCommunicationStateEnum; (*State of network communication*)
		SegmentEnable : BOOL; (*Segment hardware input "Enable" is active*)
	END_STRUCT;

	McAcpTrakDateTimeType : STRUCT
		Year : UINT;
		Month : UINT;
		Day : UINT;
		Hour : UINT;
		Minute : UINT;
		Second : UINT;
		Millisec : UINT;
	END_STRUCT;

	McAcpTrakCopyShDataInfoType : STRUCT
		TimeStamp : McAcpTrakDateTimeType;
	END_STRUCT;

	McAcpTrakSegErrorReasonEnum :
	(
		mcACPTRAK_SEG_ERROR_NONE,
		mcACPTRAK_SEG_ERROR_UNSPECIFIED,
		mcACPTRAK_SEG_ERROR_NOGRIPPER,
		mcACPTRAK_SEG_ERROR_COMMAND,
		mcACPTRAK_SEG_ERROR_SEGMENT,
		mcACPTRAK_SEG_ERROR_ASSEMBLY,
		mcACPTRAK_SEG_ERROR_CHANNEL
	);

	McAcpTrakSegErrorGripperInfoType : STRUCT
		Shuttle : McAxisType; (*the error shuttle*)
	END_STRUCT;

	McAcpTrakSegErrorChannelInfoType : STRUCT
		Shuttle : McAxisType; (*the error shuttle*)
	END_STRUCT;

	McAcpTrakSegErrorInfoType : STRUCT
		Reason : McAcpTrakSegErrorReasonEnum; (*reason for the error*)
		GripperErrorInfo : McAcpTrakSegErrorGripperInfoType; (*information about an error where no gripper could be allocated*)
		ChannelErrorInfo : McAcpTrakSegErrorChannelInfoType; (*information about an error attributed to a specific segment channel*)
	END_STRUCT;

	McAcpTrakProcPointGetInfoType : STRUCT
		SectorPosition : McAcpTrakSecPositionType; (*sector position where the process point was created*)
	END_STRUCT;

	McAcpTrakTrgPointGetInfoType : STRUCT
		Axis : McAxisType; (*determined axis reference*)
		Event : McAcpTrakTrgPointEventEnum; (*direction the shuttle passed*)
		ShuttleControlled : BOOL; (*shuttle status when passing - controlled/uncontrolled*)
		ConvoyMaster : McAxisType; (*convoy master of the convoy the shuttle is contained in, if it is contained in any*)
	END_STRUCT;

	McAcpTrakBarrierInfoType : STRUCT
		Status : McAcpTrakBarrierStatusEnum; (*barrier state*)
		RemainingTickets : UDINT; (*number of remaining tickets*)
	END_STRUCT;

	McAcpTrakAdvBarrierParType : STRUCT
		TicketCount : DINT; (*number of tickets to be added to or subtracted from the barrier*)
	END_STRUCT;

	McAcpTrakSegmentPosition : STRUCT
		Index : INT;
		Position : LREAL;
	END_STRUCT;

	 McAcpTrakShuttleEncoderPosition : STRUCT
		Available : BOOL;
		SegPos : ARRAY[0..3] OF  McAcpTrakSegmentPosition;
		ActPosition : LREAL;
		DesPosition : LREAL;
	END_STRUCT;

	 McAcpTrakShuttleEncoderMonitor : STRUCT
		ShuttleEncPos :  ARRAY[0..31] OF  McAcpTrakShuttleEncoderPosition;
	END_STRUCT;

	 McAcpTrakAsmGetInfoType : STRUCT
	 	SimulationOnPlcMode : McAcpTrakSimulationOnPlcEnum; (*information about whether simulation mode is used*)
	 END_STRUCT;

	 McAcpTrakSegGetInfoType : STRUCT
	 	Name : STRING[32]; (*name of the segment*)
	 	ID : UINT; (* Unique segment index on the assembly.*)
	 	Type : McAcpTrakSegTypeEnum; (*shape type of the segment*)
	 	Length : LREAL; (*length of the segment*)
	 	NodeNumber : UDINT; (*node number of the segment*)
	 	PlkInterface : STRING[32]; (*powerlink interface of the segment*)
	 	ChannelCount : USINT; (* number of available gripper channels *)
	 	StartFrame : McFrameType; (* coordinate frame at the origin of the segment *)
	 	EndFrame : McFrameType; (* coordinate frame at the end of the segment *)
	 END_STRUCT;

	 McAcpTrakSecGetInfoType : STRUCT
	 	Length : LREAL; (*length of the sector*)
	 	SectorClosed : BOOL; (*set to TRUE if the sector is closed*)
	 	Name : STRING[32]; (*name of the sector*)
	 	Type :  McAcpTrakSecTypeEnum; (*type of the sector*)
	 END_STRUCT;

	McAcpTrakAdvSecAddShWithMovType : STRUCT
		MoveCmd :  McAcpTrakMoveCmdEnum; (*Specifies the movement type for the shuttle*)
		Position : LREAL; 			(*target position for the motion*)
		Velocity : REAL; 			(*maximum velocity*)
		Acceleration : REAL; 		(*maximum acceleration*)
		Deceleration : REAL; 		(*maximum deceleration*)
		Jerk : REAL; 				(*maximum jerk*)
		Direction : McDirectionEnum; (*movement direction*)
		Virtual : BOOL; 			(*Create a virtual shuttle if true*)
	END_STRUCT;

	 McAcpTrakSecAddShWithMovInfoType : STRUCT
	 	PLCopenState : McAxisPLCopenStateEnum; (*PLCopen state*)
	END_STRUCT;

	 McAcpTrakAdvSecAddShParType  : STRUCT
		Velocity : REAL; 				(*velocity of a moving shuttle*)
		Orientation : McDirectionEnum; 	(*orientation of the shuttle in the sector*)
		Deceleration : REAL; 			(*maximum deceleration of a moving shuttle*)
		Virtual : BOOL; 				(*Create a virtual shuttle if true*)
	END_STRUCT;

	 McAcpTrakAsmInfoType : STRUCT
	 	CommunicationReady : BOOL; (*"Network" of all segments of the assembly is initialized and ready for communication*)
		ReadyForPowerOn : BOOL; (*the controllers of all segments of the assembly are ready to be switched on*)
		PowerOn : BOOL; (*the controllers of all segments of the assembly are switched on*)
		StartupCount : UDINT; (*number of completed switch-on operations of the assembly*)
	 	SegmentsInDisabledCount : UINT; (*number of segments in state Disabled*)
	 	SegmentsInStoppingCount : UINT; (*number of segments in state Stopping*)
	 	SegmentsInErrorStopCount : UINT; (*number of segments in state ErrorStop*)
	 	ShuttlesCount : UINT; (*number of shuttles on the assembly*)
	 	ShuttlesInStandstillCount : UINT; (*number of shuttles without active movement command*)
	 	ShuttlesInDisabledCount : UINT; (*number of shuttles in state Disabled*)
	 	ShuttlesInStoppingCount : UINT; (*number of shuttles in state Stopping*)
	 	ShuttlesInErrorStopCount : UINT; (*number of shuttles in state ErrorStop*)
	 	VirtualShuttlesCount : UINT; (*number of virtual shuttles*)
	 	ConvoysCount : UINT; (*number of convoys on the assembly*)
	 END_STRUCT;

	McAcpTrakAsmErrorReasonEnum :
	(
		mcACPTRAK_ASM_ERROR_NONE,
		mcACPTRAK_ASM_ERROR_UNSPECIFIED,
		mcACPTRAK_ASM_ERROR_ENCDIFF,
		mcACPTRAK_ASM_ERROR_NOGRIPPER,
		mcACPTRAK_ASM_ERROR_COMMAND,
		mcACPTRAK_ASM_ERROR_SEGMENT,
		mcACPTRAK_ASM_ERROR_UNOBSERVABLE
	);

	McAcpTrakAsmErrorEncdiffInfoType : STRUCT
		Difference : LREAL; (*the difference between the encoder positions*)
		LowPosition : McAcpTrakSegPositionType; (*the shuttle's segment position that implies, after translation to the shuttle's sector, to the lowest sector position*)
		HighPosition : McAcpTrakSegPositionType; (*the shuttle's segment position that implies, after translation to the shuttle's sector, to the highest sector position*)
	END_STRUCT;

	McAcpTrakAsmErrorGripperInfoType : STRUCT
		Shuttle : McAxisType; (*the error shuttle*)
		Segment : McSegmentType; (*the segment where the shuttle could not allocate a gripper*)
		SegmentName : STRING[32]; (*the name of the segment where the shuttle could not allocate a gripper*)
	END_STRUCT;

	McAcpTrakAsmErrorSegmentInfoType : STRUCT
		Segment : McSegmentType; (*the segment whose error triggered the shuttle error*)
		SegmentName : STRING[32]; (*the name of the segment whose error triggered the shuttle error*)
	END_STRUCT;

	McAcpTrakAsmErrorInfoType : STRUCT
		Reason : McAcpTrakAsmErrorReasonEnum; (*reason for the error*)
		EncdiffErrorInfo : McAcpTrakAsmErrorEncdiffInfoType; (*information about an encoder discrepancy error*)
		GripperErrorInfo : McAcpTrakAsmErrorGripperInfoType; (*information about an error where no gripper could be allocated*)
		SegmentErrorInfo : McAcpTrakAsmErrorSegmentInfoType; (*information about a segment error*)
	END_STRUCT;

	McAcpTrakGetShuttleModeEnum :
	(
		mcACPTRAK_GET_SH_ALL := 0, (*get all shuttles*)
		mcACPTRAK_GET_SH_STANDSTILL, (*get shuttles without active movement command*)
		mcACPTRAK_GET_SH_DISABLED, (*get shuttles in state disabled*)
		mcACPTRAK_GET_SH_STOPPING, (*get shuttles in state stopping*)
		mcACPTRAK_GET_SH_ERRORSTOP, (*get shuttles in state error stop*)
		mcACPTRAK_GET_SH_ERROR_MANEUVER, (*get shuttles in state error stop with reason standstill maneuver or emergency stop*)
		mcACPTRAK_GET_SH_ERROR_ENCDIFF, (*get shuttles in state error stop with reason encoder failure*)
		mcACPTRAK_GET_SH_ERROR_NOGRIPPER, (*get shuttles in state error stop with reason 'no gripper available'*)
		mcACPTRAK_GET_SH_ERROR_COMMAND, (*get shuttles in state error stop with reason user command*)
		mcACPTRAK_GET_SH_ERROR_SEGMENT, (*get shuttles in state error stop with reason segment failure*)
		mcACPTRAK_GET_SH_ERROR_ASSEMBLY, (*get shuttles in state error stop with reason assembly error stop*)
		mcACPTRAK_GET_SH_ERROR_INVMOV, (*get shuttles in state error stop with reason invalid move*)
		mcACPTRAK_GET_SH_ERROR_COUPLING, (*get shuttles in state error stop with reason coupling*)
		mcACPTRAK_GET_SH_CONVOYMASTER, (*get shuttles that are masters of convoys*)
		mcACPTRAK_GET_SH_ERROR_CHANNEL (*get shuttles in state error stop with reason channel error*)
	);

	McAcpTrakGetShuttleVirtModeEnum :
	(
		mcACPTRAK_GET_SH_VIRT_ALL := 0, 	(*get all shuttles*)
		mcACPTRAK_GET_SH_VIRT_VIRTUAL, 		(*get virtual shuttles*)
		mcACPTRAK_GET_SH_VIRT_NONVIRTUAL 	(*get nonvirtual shuttles*)
	);

	McAcpTrakGetSegmentModeEnum :
	(
		mcACPTRAK_GET_SEG_ALL := 0, (*get all segments*)
		mcACPTRAK_GET_SEG_DISABLED, (*get segments in state disabled*)
		mcACPTRAK_GET_SEG_STOPPING, (*get segments in state stopping*)
		mcACPTRAK_GET_SEG_ERRORSTOP (*get segments in state error stop*)
	);

	McAcpTrakGetSectorModeEnum :
	(
		mcACPTRAK_GET_SEC_ALL := 0, (*get all sectors*)
		mcACPTRAK_GET_SEC_CLOSED, (*get closed sectors*)
		mcACPTRAK_GET_SEC_NONCLOSED (*get nonclosed sectors*)
	);

	McAcpTrakAdvAsmGetShParType : STRUCT
		SelectionMode : McAcpTrakGetShuttleModeEnum; (*Selector for the shuttles to output*)
		VirtualSelectionMode : McAcpTrakGetShuttleVirtModeEnum; (*Additional selector for the shuttles to output considering virtual property*)
	END_STRUCT;

	McAcpTrakAsmGetShAddInfoType : STRUCT
		ShuttleID : UINT; (* Unique shuttle index on the assembly.*)
	END_STRUCT;

	McAcpTrakAdvAsmGetSegParType : STRUCT
		SelectionMode : McAcpTrakGetSegmentModeEnum; (*Selector for the segments to output*)
	END_STRUCT;

	McAcpTrakAdvAsmGetSecParType : STRUCT
		SelectionMode : McAcpTrakGetSectorModeEnum; (*Selector for the sectors to output*)
	END_STRUCT;

	McAcpTrakAdvSecGetShParType : STRUCT
		SelectionMode : McAcpTrakGetShuttleModeEnum; (*Selector for the shuttles to output*)
		VirtualSelectionMode : McAcpTrakGetShuttleVirtModeEnum; (*Additional selector for the shuttles to output considering virtual property*)
		StartPosition : LREAL; (*Start position of the sector position interval for search*)
		EndPosition : LREAL; (*Start position of the sector position interval for search*)
	END_STRUCT;

	McAcpTrakSecGetShAddInfoType : STRUCT
		ShuttleID : UINT; (* Unique shuttle index on the assembly.*)
		Position : LREAL; (* Position on sector*)
		Orientation : McDirectionEnum; (* Orientation of the shuttle on the sector*)
	END_STRUCT;

	McAcpTrakBarrierGetShModeEnum :
	(
		mcACPTRAK_BARR_GET_SH_ALL := 0, (*get all shuttles performing adjustment maneuvers*)
		mcACPTRAK_BARR_GET_SH_STANDSTILL, (*get shuttles performing adjustment maneuvers which are not moving*)
		mcACPTRAK_BARR_GET_SH_CLOSEST (*get closest shuttle performing adjustment maneuver*)
	);

	McAcpTrakAdvBarrierGetShParType : STRUCT
		SelectionMode : McAcpTrakBarrierGetShModeEnum; (*Selector for the shuttles to output*)
	END_STRUCT;

	McAcpTrakBarrierGetShInfoType : STRUCT
		ShuttleID : UINT; (* Unique shuttle index on the assembly.*)
		Distance : LREAL; (* Distance to the barrier*)
	END_STRUCT;

	 McAcpTrakSecInfoType : STRUCT
	 	ShuttlesCount : UINT; (*number of shuttles on the sector*)
	 	ShuttlesInStandstillCount : UINT; (*number of shuttles without active movement command*)
	 	ShuttlesInDisabledCount : UINT; (*number of shuttles in state Disabled*)
	 	ShuttlesInStoppingCount : UINT; (*number of shuttles in state Stopping*)
	 	ShuttlesInErrorStopCount : UINT; (*number of shuttles in state ErrorStop*)
	 	VirtualShuttlesCount : UINT; (*number of virtual shuttles*)
	 	ConvoysCount : UINT; (*number of convoys on the sector*)
	 END_STRUCT;

	McAcpTrakAdvGetMinShDistParType : STRUCT
	 	MeasurePointsMode : McAcpTrakShRelativeRefPointEnum; (*Measurement points selector*)
		Shuttle1 : REFERENCE TO McAxisType; (*Reference of the first shuttle in the comparison(optional)*)
		Shuttle2 : REFERENCE TO McAxisType; (*Reference of the second shuttle in the comparison(optional)*)
	END_STRUCT;

	McAcpTrakAsmGetMinShDistType : STRUCT
		Curve : LREAL; (* Minimum distance on a curved segment*)
		Junction : LREAL; (* Minimum distance in a junction*)
		Straight : LREAL; (* Minimum distance on a straight*)
	END_STRUCT;

	McAcpTrakConvoyParType : STRUCT
		PositiveOffset : LREAL;
		NegativeOffset : LREAL;
		Elastic : BOOL; (*Determines whether convoy master is elastic wrt. other shuttles in convoy*)
		Separable : BOOL; (*Allow shuttles outside of convoy to use the spaces between convoy members*)
		MasterVelocityAdjustment : BOOL; (*Allow master shuttle to adjust velocity to convoy members*)
		TargetDistDeceleration: REAL; (*Alternative deceleration for TCP coupling*)
	END_STRUCT;

	 McAcpTrakCouplingModeEnum :
	(
		mcACPTRAK_COUPL_DIST,	 (*try to keep target distance to the reference shuttle*)
		mcACPTRAK_COUPL_TCP_DIST (*try to keep target distance between TCPs*)
	);

	McAcpTrakShConvoyParType : STRUCT
		ReferenceShuttle : REFERENCE TO McAxisType; (*the shuttle whose position the shuttle's position depends on*)
		CouplingMode : McAcpTrakCouplingModeEnum; (*mode of coupling to the reference shuttle*)
		Elastic : BOOL; (*Determines whether shuttle is elastic wrt. other shuttles in convoy*)
	END_STRUCT;

	McAcpTrakAdvShConvoyParType : STRUCT
		ConvoyMaster : REFERENCE TO McAxisType; (*the shuttle which is the master of the convoy*)
		TargetDistance : LREAL; 				(*The target distance between the shuttle and its reference shuttle*)
		Velocity : REAL; 						(*maximum velocity*)
		Acceleration : REAL; 					(*maximum acceleration*)
		Deceleration : REAL; 					(*maximum deceleration*)
		Tcp : McPosType;						(*TCP of the shuttle*)
		ReferenceTcp : McPosType;				(*TCP of the reference shuttle*)
	END_STRUCT;

	McAcpTrakAdvShRemoveConParType : STRUCT
		Deceleration : REAL; (*maximum deceleration*)
	END_STRUCT;

	McAcpTrakConDeleteModeEnum :
	(
		mcACPTRAK_CON_DELETE_ANY_CASE, (*delete convoy in any case*)
		mcACPTRAK_CON_DELETE_EMPTY (*delete convoy if it does not contain other shuttles than the master*)
	);

	McAcpTrakAdvConDeleteParType : STRUCT
		Mode : McAcpTrakConDeleteModeEnum; (*deletion mode*)
	END_STRUCT;

	McAcpTrakConInfoType	: STRUCT
		MemberCount : UINT; (*number of direct members (shuttles and convoys) in the convoy*)
		ShuttleCount : UINT; (*number of shuttles in the convoy*)
		ActualPositiveOffset : LREAL; (*actual maximum position offset in positive direction*)
		ActualNegativeOffset : LREAL; (*actual maximum position offset in negative direction*)
	END_STRUCT;

	McAcpTrakConGetShuttleModeEnum :
	(
		mcACPTRAK_CON_GET_SH_SECPOS_ASC, (*ascending order by position*)
		mcACPTRAK_CON_GET_SH_SECPOS_DESC (*descending order by position*)
	);

	McAcpTrakConGetShSelectModeEnum :
	(
		mcACPTRAK_CON_GET_SH_MEMBERS, (*consider only direct members (shuttles and convoys' master shuttles contained in the convoy)*)
		mcACPTRAK_CON_GET_SH_SHUTTLES (*consider all shuttles contained in the convoy or a convoy it contains*)
	);

	McAcpTrakAdvConGetShParType : STRUCT
		Mode : McAcpTrakConGetShuttleModeEnum; (*Selector for order of the shuttles to output*)
		SelectionMode : McAcpTrakConGetShSelectModeEnum; (*Selector for which shuttles will be contained in the resulting list*)
	END_STRUCT;

	McAcpTrakConGetShAddInfoType : STRUCT
		ShuttleID : UINT; (* Unique shuttle index on the assembly.*)
		Position : LREAL; (* Position on the convoy's sector*)
		Orientation : McDirectionEnum; (* Orientation of the shuttle on the convoy's sector*)
	END_STRUCT;

	McAcpTrakShSwitchSecInfoType : STRUCT
		Position : LREAL; (*Shuttle position on the sector after switch*)
		Orientation : McDirectionEnum; (*Shuttle orientation on the sector after switch*)
	END_STRUCT;

	McAcpTrakAdvShSwitchSecParType : STRUCT
		Deceleration : REAL; (*maximum deceleration of a moving shuttle*)
		StartPosition : LREAL; (*Start position of the sector position interval to switch to*)
		EndPosition : LREAL; (*Start position of the sector position interval to switch to*)
	END_STRUCT;

	McAcpTrakShReadSecPosInfoType : STRUCT
		Position : LREAL; (*Shuttle position on the sector*)
		Orientation : McDirectionEnum; (*Shuttle orientation on the sector*)
	END_STRUCT;

	McAcpTrakAdvShReadSecPosParType : STRUCT
		StartPosition : LREAL; (*Start position of the sector position interval to search for position*)
		EndPosition : LREAL; (*Start position of the sector position interval to search for position*)
	END_STRUCT;

	McAcpTrakAdvShCamAutCmdParType : STRUCT
		Elastic : BOOL; (*true iff elastic movements are started*)
		Deceleration : REAL; (*maximum deceleration*)
	END_STRUCT;

	McAcpTrakShCamAutCmdCommandsType : STRUCT
		Start : BOOL; (*starts the cam automat*)
		Stop : BOOL; (*stops the movement on the slave axis*)
		Restart : BOOL; (*restarts a cam automat which is in stand by*)
		EndAutomat : BOOL; (*finishes the execution of the cam automat*)
		SetSignal1 : BOOL; (*sets signal 1 of the cam automat*)
		SetSignal2 : BOOL; (*sets signal 2 of the cam automat*)
		SetSignal3 : BOOL; (*sets signal 3 of the cam automat*)
		SetSignal4 : BOOL; (*sets signal 4 of the cam automat*)
		ResetSignal1 : BOOL; (*resets signal 1 of the cam automat*)
		ResetSignal2 : BOOL; (*resets signal 2 of the cam automat*)
		ResetSignal3 : BOOL; (*resets signal 3 of the cam automat*)
		ResetSignal4 : BOOL; (*resets signal 4 of the cam automat*)
	END_STRUCT;

	McAcpTrakShCamAutCmdInfoType : STRUCT
		Running : BOOL; (*automat is currently running*)
		StandBy : BOOL; (*automat is in standby and can be restarted*)
		ActualStateIndex : USINT; (*index of the actual cam automat state*)
		ActualStateCamIndex : UINT; (*index of the cam of the actual cam automat state*)
		InCam : BOOL; (*cam in the current cam automat state is active*)
		InCompensation : BOOL; (*compensation in the current cam automat state is active*)
		InPosition : BOOL; (*shuttle realized its current cyclic target position*)
	END_STRUCT;

	McAcpTrakAdvSecStopParType : STRUCT
		StopMode : McStopModeEnum; (*determines whether jerk limits are used*)
		StartPosition : LREAL; (*Start position of the sector position interval for search*)
		EndPosition : LREAL; (*Start position of the sector position interval for search*)
		Deceleration : REAL; (*maximum deceleration of a moving shuttle*)
	END_STRUCT;

	McAcpTrakDiverterInfoType : STRUCT
		SegmentPosition1 : McAcpTrakSegPositionType; (*first segment position s.t. the diverter diverges in direction of the segment*)
		SegmentPosition2 : McAcpTrakSegPositionType; (*first segment position s.t. the diverter converges in direction of the segment*)
	END_STRUCT;

	McAcpTrakSegLimitErrorScopeMode :
	(
		mcACPTRAK_UNOBSERV_SH_ACTIVATE, (*activate the limitation of the error reaction scope for unobservable shuttles on this segment*)
		mcACPTRAK_UNOBSERV_SH_DEACTIVATE (*deactivate the limitation of the error reaction scope for unobservable shuttles on this segment*)
	);

END_TYPE

