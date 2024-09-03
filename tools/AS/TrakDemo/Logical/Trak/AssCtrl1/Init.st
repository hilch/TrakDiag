

PROGRAM _INIT
	gSegA1;gSegA2;gSegA3;gSegA4;gSegA5;gSegA6;gSegA7;gSegA8;gSegA9;gSegA10;gSegA11;gSegA12;gSegA13;gSegA14;
	gSegB1;gSegB2;gSegB3;gSegB4;gSegB5;gSegB6;gSegB7;gSegB8;gSegB9;gSegB10;gSegB11;gSegB12;gSegB13;gSegB14;gSegB15;gSegB16;gSegB17;gSegB18;
	gSecOvalA;
	gSecOvalB;
	gSecB7;

	fbReadStatus.Assembly := ADR(gAssembly_1);
	fbReadStatus.Enable := TRUE;

	fbReadInfo.Assembly := fbReadStatus.Assembly;
	fbReadInfo.Enable := TRUE; 

	fbGetInfo.Assembly := fbReadStatus.Assembly;

	fbPowerOn.Assembly := fbReadStatus.Assembly;

	fbPowerOff.Assembly := fbReadStatus.Assembly; 
	
	fbStartShuttles.Sector := gSecCompleteLoop1;
	fbStartShuttles.TargetSector := gSecCompleteLoop1;
	fbStartShuttles.Acceleration := 4.0;
	fbStartShuttles.Deceleration := 4.0;
	fbStartShuttles.Velocity := 2.0;
	fbStartShuttles.Jerk := 0.0;
	fbStartShuttles.Direction := mcDIR_POSITIVE;
	
	fbAddSimShuttle.Sector := ADR(gSecCompleteLoop1);
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
	fbRecorder.FileNamePrefix := 'Demo_';
	fbRecorder.MaxNumberOfRecordings := 10;
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

	fbLogStep( Enable := FALSE, LogName := 'MyTrak', SignalName := 'AssCtrl1 Step', EventID := TRAK_BASIC_INFO_ID );
	fbLogStep.Enable := TRUE;

	fbLogResetStep( Enable := FALSE, LogName := 'MyTrak', SignalName := 'AssCtrl1 ResetStep', EventID := TRAK_BASIC_INFO_ID );
	fbLogResetStep.Enable := TRUE;
END_PROGRAM