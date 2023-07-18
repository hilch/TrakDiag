
{REDUND_ERROR} {REDUND_UNREPLICABLE} FUNCTION_BLOCK TL_StartShuttlesInSector (*starts all shuttles in a given sector*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Execute : {REDUND_UNREPLICABLE} BOOL;
		Sector : {REDUND_UNREPLICABLE} McSectorType; (*shuttles on this sector should be moved*)
		StartPosition : {REDUND_UNREPLICABLE} LREAL; (*Start of the sector section where the shuttles to be output are located
The starting position must be between 0 and the length of the sector. If not set, value 0 is used.*)
		EndPosition : {REDUND_UNREPLICABLE} LREAL; (*End of the sector section on which the shuttles to be output are located
The end position must be between 0 and the length of the sector. If not set or set to 0, the length of the sector is used.*)
		TargetSector : {REDUND_UNREPLICABLE} McSectorType; (*target sector. If not given then only a velocity command is started*)
		Position : {REDUND_UNREPLICABLE} LREAL; (*position on target sector. Will be ignored if TargetSector is not given*)
		Absolute : {REDUND_UNREPLICABLE} BOOL; (*absolute movement if set*)
		Velocity : {REDUND_UNREPLICABLE} REAL;
		Direction : {REDUND_UNREPLICABLE} McDirectionEnum; (*move all shuttles forward (positive direction)*)
		Acceleration : {REDUND_UNREPLICABLE} REAL;
		Deceleration : {REDUND_UNREPLICABLE} REAL;
		Jerk : {REDUND_UNREPLICABLE} REAL;
	END_VAR
	VAR_OUTPUT
		NumberOfShuttles : {REDUND_UNREPLICABLE} USINT;
		Done : {REDUND_UNREPLICABLE} BOOL;
		Busy : {REDUND_UNREPLICABLE} BOOL;
		Error : {REDUND_UNREPLICABLE} BOOL;
		ErrorID : {REDUND_UNREPLICABLE} UDINT;
	END_VAR
	VAR
		n : {REDUND_UNREPLICABLE} USINT;
		list : {REDUND_UNREPLICABLE} ARRAY[0..255] OF McAxisType; (*the list of shuttles*)
		step : {REDUND_UNREPLICABLE} UINT;
		fbSecGetShuttle : {REDUND_UNREPLICABLE} MC_BR_SecGetShuttleSel_AcpTrak;
		fbRoutedMoveVel : {REDUND_UNREPLICABLE} MC_BR_RoutedMoveVel_AcpTrak;
		fbRoutedMoveAbs : {REDUND_UNREPLICABLE} MC_BR_RoutedMoveAbs_AcpTrak;
		fbReset : {REDUND_UNREPLICABLE} MC_Reset;
		fbSecReadInfo : {REDUND_UNREPLICABLE} MC_BR_SecReadInfo_AcpTrak;
		ton : {REDUND_UNREPLICABLE} TON;
		maxShuttles : {REDUND_UNREPLICABLE} UINT;
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR} {REDUND_UNREPLICABLE} FUNCTION_BLOCK TL_Barrier (*open/ close a barrier*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Enable : {REDUND_UNREPLICABLE} BOOL;
		Close : {REDUND_UNREPLICABLE} BOOL;
		ProcessPoint : {REDUND_UNREPLICABLE} McProcessPointType;
	END_VAR
	VAR_OUTPUT
		BarrierClosed : {REDUND_UNREPLICABLE} BOOL;
		BarrierOpen : {REDUND_UNREPLICABLE} BOOL;
		Error : {REDUND_UNREPLICABLE} BOOL;
		ErrorID : {REDUND_UNREPLICABLE} DINT;
	END_VAR
	VAR
		MC_BR_BarrierCommand : {REDUND_UNREPLICABLE} MC_BR_BarrierCommand_AcpTrak;
		MC_BR_BarrierReadInfo : {REDUND_UNREPLICABLE} MC_BR_BarrierReadInfo_AcpTrak;
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR} {REDUND_UNREPLICABLE} FUNCTION_BLOCK TL_CatchShuttle (*catches a shuttle at a process point*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Enable : {REDUND_UNREPLICABLE} BOOL;
		ProcessPoint : {REDUND_UNREPLICABLE} McProcessPointType; (*process point at entry*)
		DataAddress : {REDUND_UNREPLICABLE} UDINT; (*pointer to variable for shuttle's user data*)
		DataSize : {REDUND_UNREPLICABLE} UDINT; (*sizeof shuttle's user data*)
	END_VAR
	VAR_OUTPUT
		Busy : {REDUND_UNREPLICABLE} BOOL;
		Valid : {REDUND_UNREPLICABLE} BOOL;
		Error : {REDUND_UNREPLICABLE} BOOL;
		ErrorID : {REDUND_UNREPLICABLE} DINT;
		ShuttleCounter : {REDUND_UNREPLICABLE} DINT; (*shuttle counter at exit process point*)
		Axis : {REDUND_UNREPLICABLE} McAxisType;
		ShuttleInfo : {REDUND_UNREPLICABLE} McAcpTrakShInfoType;
	END_VAR
	VAR
		fbTrgPointEnableEntry : {REDUND_UNREPLICABLE} MC_BR_TrgPointEnable_AcpTrak;
		fbTrgPointGetInfo : {REDUND_UNREPLICABLE} MC_BR_TrgPointGetInfo_AcpTrak;
		fbShReadInfo : {REDUND_UNREPLICABLE} MC_BR_ShReadInfo_AcpTrak;
		fbCopyUserData : {REDUND_UNREPLICABLE} MC_BR_ShCopyUserData_AcpTrak;
		step : {REDUND_UNREPLICABLE} UINT;
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR} {REDUND_UNREPLICABLE} FUNCTION_BLOCK TL_ShWheelChk (*Shuttle Wheel Check*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Enable : {REDUND_UNREPLICABLE} BOOL;
		CycleTime : {REDUND_UNREPLICABLE} LREAL; (*cycle time [s]*)
		CtrlParIndex : {REDUND_UNREPLICABLE} USINT; (*controller settings parameter index for weaking while measurement ( 0 = default )*)
		MeasuringDistance : {REDUND_UNREPLICABLE} LREAL; (*measuring distance (= bumper width) [m]*)
		Threshold : {REDUND_UNREPLICABLE} LREAL; (*lag error threshold for worn shuttle [m]*)
		TrgPointEntry : {REDUND_UNREPLICABLE} McProcessPointType; (*process point at entry*)
		Sector : {REDUND_UNREPLICABLE} McSectorType;
	END_VAR
	VAR_OUTPUT
		Busy : {REDUND_UNREPLICABLE} BOOL;
		Done : {REDUND_UNREPLICABLE} BOOL;
		Error : {REDUND_UNREPLICABLE} BOOL;
		ErrorID : {REDUND_UNREPLICABLE} DINT;
		ShuttleCounter : {REDUND_UNREPLICABLE} DINT; (*shuttle counter at exit process point*)
		Data : {REDUND_UNREPLICABLE} TL_ShWheelChkData_Type;
	END_VAR
	VAR
		fbTrgPointEnableEntry : {REDUND_UNREPLICABLE} MC_BR_TrgPointEnable_AcpTrak;
		fbTrgPointGetInfo : {REDUND_UNREPLICABLE} MC_BR_TrgPointGetInfo_AcpTrak;
		fbShReadInfo : {REDUND_UNREPLICABLE} MC_BR_ShReadInfo_AcpTrak;
		fbShReadSegmentInfo : {REDUND_UNREPLICABLE} MC_BR_ShReadSegmentInfo_AcpTrak;
		fbShReadSectorPos : {REDUND_UNREPLICABLE} MC_BR_ShReadSectorPos_AcpTrak;
		fbShSelCtrlParSet : {REDUND_UNREPLICABLE} MC_BR_ShSelCtrlParSet_AcpTrak;
		currentLagError : {REDUND_UNREPLICABLE} LREAL; (*current lag error with cycle time compensation*)
		setPosDelay : {REDUND_UNREPLICABLE} ARRAY[0..5] OF LREAL; (*fifo for cycle time compensation*)
		n : {REDUND_UNREPLICABLE} UINT;
		l : {REDUND_UNREPLICABLE} LREAL; (*temporary variable*)
		cycleCount : {REDUND_UNREPLICABLE} UDINT; (*cycle counter, starts when a shuttle enters and stops when shuttle leaves*)
		step : {REDUND_UNREPLICABLE} UINT;
		tonTimeout : {REDUND_UNREPLICABLE} TON; (*timeout if shuttle enters but does not leave*)
		start_position : {REDUND_UNREPLICABLE} LREAL; (*sector position where measurement starts*)
		end_position : {REDUND_UNREPLICABLE} LREAL; (*sector position where measurement stops*)
		peakLagError : {REDUND_UNREPLICABLE} LREAL;
		lagErrorSum : {REDUND_UNREPLICABLE} LREAL;
	END_VAR
	VAR CONSTANT
		CYCLES_PLK : UINT := 6; (*total number of cycles for PLK delay*)
	END_VAR
	VAR
		axis : {REDUND_UNREPLICABLE} McAxisType;
		timeouts : {REDUND_UNREPLICABLE} DINT; (*counter for shuttle timeouts (just for debugging)*)
	END_VAR
END_FUNCTION_BLOCK
