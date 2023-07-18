
TYPE
	AssemblyStepEnum : 
		(
		ASM_INIT,
		ASM_GET_INFO,
		ASM_W_READY_FOR_POWER_ON,
		ASM_POWER_IS_OFF,
		ASM_W_POWER_ON,
		ASM_ADD_SIM_SHUTTLES,
		ASM_POWER_IS_ON := 100,
		ASM_START_MOVEMENT,
		ASM_W_POWER_OFF,
		ASM_SIMULATE_ERROR,
		ASM_WAIT_FOR_DEFECT_SHUTTLE,
		ASM_ERR_GET_INFO := 9000,
		ASM_ERR_POWERING_ON,
		ASM_ERR_POWERING_OFF,
		ASM_ERR_START_MOVEMENT,
		ASM_ERR_ADDING_SHUTTLE,
		ASM_ERR_SIMULATE_ERROR,
		ASM_ERR_GENERAL
		);
	AssemblyResetStepEnum : 
		(
		ASMR_NO_ERROR,
		ASMR_W_ACKNOWLEDGE,
		ASMR_RESET,
		ASMR_INTERNAL_ERROR
		);
	AssemblyStatusType : 	STRUCT 
		Step : AssemblyStepEnum;
		ResetStep : AssemblyResetStepEnum;
		AllShuttlesMoving : BOOL;
		Disabled : BOOL;
		Ready : BOOL;
		Homing : BOOL;
		Stopping : BOOL;
		ErrorStop : BOOL;
		SimulationOnPlcMode : McAcpTrakSimulationOnPlcEnum;
		Info : McAcpTrakAsmInfoType;
	END_STRUCT;
	AssemblyParaType : 	STRUCT 
		NumberOfSimulatedShuttles : USINT;
	END_STRUCT;
	AssemblyCmdType : 	STRUCT 
		Reset : BOOL;
		PowerOn : BOOL;
		PowerOff : BOOL;
		Start : BOOL;
		SimEncoderError : BOOL;
		SimChannelError : BOOL;
		SimSegmentError : BOOL;
	END_STRUCT;
	AssemblyType : 	STRUCT 
		SizeOf : UDINT; (*size of this struct*)
		Status : AssemblyStatusType;
		Para : AssemblyParaType;
		Cmd : AssemblyCmdType;
	END_STRUCT;
END_TYPE
