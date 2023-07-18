

PROGRAM _INIT
	gSecOvalA;
	gSecOvalB;
	gSecCompleteLoop;

	fbReadStatus.Assembly := ADR(gAssembly_1);
	fbReadStatus.Enable := TRUE;

	fbReadInfo.Assembly := fbReadStatus.Assembly;
	fbReadInfo.Enable := TRUE; 

	fbGetInfo.Assembly := fbReadStatus.Assembly;

	fbPowerOn.Assembly := fbReadStatus.Assembly;

	fbPowerOff.Assembly := fbReadStatus.Assembly; 
	
	fbStartShuttles.Sector := gSecCompleteLoop;
	fbStartShuttles.TargetSector := gSecCompleteLoop;
	fbStartShuttles.Acceleration := 4.0;
	fbStartShuttles.Deceleration := 4.0;
	fbStartShuttles.Velocity := 2.0;
	fbStartShuttles.Jerk := 0.0;
	fbStartShuttles.Direction := mcDIR_POSITIVE;
	
	fbAddSimShuttle.Sector := ADR(gSecCompleteLoop);
	fbAddSimShuttle.Position := 0.5;

	fbAddSimShuttle.AdvancedParameters.Velocity := 0.0;
	fbAddSimShuttle.AdvancedParameters.Orientation := mcDIR_POSITIVE;
	fbAddSimShuttle.AdvancedParameters.Deceleration := 6.0;
	fbAddSimShuttle.AdvancedParameters.Virtual := FALSE;


	fbCatchShuttle.ProcessPoint := PP_ErrorPosition1;
	fbCatchShuttle.Enable := TRUE;

	fbAsmReset.Assembly := ADR(gAssembly_1);

	fbRecorder.AssemblyName := 'gAssembly_1';
	fbRecorder.FileDeviceName := 'Diagnosis';
	fbRecorder.DataAddress := ADR(shuttleData);
	fbRecorder.DataSize := SIZEOF(shuttleData); (* type of McAcpTrakShuttleData[] if no shuttle user data is defined, see MC_BR_AsmCopyShuttleData_AcpTrak *)
	fbRecorder.UserDataSize := 0;
	fbRecorder.Enable := TRUE;	

	fbWebServices.AssemblyName := fbRecorder.AssemblyName;
	fbWebServices.DataAddress := fbRecorder.DataAddress;
	fbWebServices.DataSize := fbRecorder.DataSize;
	fbWebServices.UserDataSize := fbRecorder.UserDataSize;
	fbWebServices.Enable := TRUE;

	assemblyCtrl.Para.NumberOfSimulatedShuttles := 10;
	assemblyCtrl.SizeOf := SIZEOF(assemblyCtrl);
	
	step := ASM_INIT;

	fbLogStep( Enable := FALSE, LogName := 'MyTrak', SignalName := 'AssCtrl Step', EventID := TRAK_BASIC_INFO_ID );
	fbLogStep.Enable := TRUE;

	fbLogResetStep( Enable := FALSE, LogName := 'MyTrak', SignalName := 'AssCtrl ResetStep', EventID := TRAK_BASIC_INFO_ID );
	fbLogResetStep.Enable := TRUE;
END_PROGRAM