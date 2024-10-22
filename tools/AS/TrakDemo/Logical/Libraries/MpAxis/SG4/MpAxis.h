/* Automation Studio generated header file */
/* Do not edit ! */
/* MpAxis 5.27.1 */

#ifndef _MPAXIS_
#define _MPAXIS_
#ifdef __cplusplus
extern "C" 
{
#endif
#ifndef _MpAxis_VERSION
#define _MpAxis_VERSION 5.27.1
#endif

#include <bur/plctypes.h>

#ifndef _BUR_PUBLIC
#define _BUR_PUBLIC
#endif

#ifdef _SG4
#include <McAxis.h> 
#include <MpBase.h> 
#include <McBase.h>
#endif
 
#ifdef _SG3
#include <McAxis.h> 
#include <MpBase.h> 
#include <McBase.h>
#endif
 
#ifdef _SGC
#include <McAxis.h> 
#include <MpBase.h> 
#include <McBase.h>
#endif

/* Datatypes and datatypes of function blocks */
typedef enum MpAxisErrorEnum
{	mcAXIS_NO_ERROR = 0,
	mcAXIS_ERR_PLC_OPEN = -1067278080,
	mcAXIS_WRN_PLC_OPEN = -2141019903,
	mcAXIS_WRN_MULTIPLE_COMMAND = -2141019902,
	mcAXIS_ERR_RECOVERY_NOT_ALLOWED = -1067278072,
	mcAXIS_WRN_RESTOREPOS_INVALID = -2141019895,
	mcAXIS_ERR_ACTIVATION = -1064239103,
	mcAXIS_ERR_MPLINK_NULL = -1064239102,
	mcAXIS_ERR_MPLINK_INVALID = -1064239101,
	mcAXIS_ERR_MPLINK_CHANGED = -1064239100,
	mcAXIS_ERR_MPLINK_CORRUPT = -1064239099,
	mcAXIS_ERR_MPLINK_IN_USE = -1064239098,
	mcAXIS_ERR_PAR_NULL = -1064239097,
	mcAXIS_ERR_CONFIG_NULL = -1064239096,
	mcAXIS_ERR_CONFIG_CMD_FAILED = -1067278070
} MpAxisErrorEnum;

typedef enum MpAxisGetCamPositionModeEnum
{	mcAXIS_GET_CAM_POSITION_SLAVE,
	mcAXIS_GET_CAM_POSITION_MASTER,
	mcAXIS_MOVE_CAM_POSITION_SLAVE
} MpAxisGetCamPositionModeEnum;

typedef enum MpAxisSequenceSetModeEnum
{	mcAXIS_CAM_SEQ_SET_INACTIVE,
	mcAXIS_CAM_SEQ_SET_ON_UPDATE,
	mcAXIS_CAM_SEQ_SET_ON_START
} MpAxisSequenceSetModeEnum;

typedef enum MpAxisExecutingCmdEnum
{	mcAXIS_CMD_IDLE = 0,
	mcAXIS_CMD_HOMING,
	mcAXIS_CMD_STOP,
	mcAXIS_CMD_HALT,
	mcAXIS_CMD_MOVE_VELOCITY,
	mcAXIS_CMD_MOVE_ABSOLUTE,
	mcAXIS_CMD_MOVE_ADDITIVE,
	mcAXIS_CMD_JOG_POSITIVE,
	mcAXIS_CMD_JOG_NEGATIVE,
	mcAXIS_CMD_REMOTE_CONTROL,
	mcAXIS_CMD_ERROR_RESET,
	mcAXIS_CMD_GEAR,
	mcAXIS_CMD_CAM,
	mcAXIS_CMD_GEAR_IN_POS,
	mcAXIS_CMD_OFFSET_SHIFT,
	mcAXIS_CMD_PHASE_SHIFT,
	mcAXIS_CMD_GET_CAM_POSITION,
	mcAXIS_CMD_CAM_PREPARE,
	mcAXIS_CMD_CAM_RECOVERY,
	mcAXIS_CMD_CAM_SEQUENCE,
	mcAXIS_CMD_PARLOCK,
	mcAXIS_CMD_GET_SEQUENCE,
	mcAXIS_CMD_SET_SEQUENCE,
	mcAXIS_CMD_SIMULATION,
	mcAXIS_CMD_STOP_AT_POSITION,
	mcAXIS_CMD_POWER,
	mcAXIS_CMD_AUTOTUNE,
	mcAXIS_CMD_MOVE_CYCLIC_POSITION,
	mcAXIS_CMD_MOVE_CYCLIC_VELOCITY,
	mcAXIS_CMD_VELOCITY_CONTROL,
	mcAXIS_CMD_TORQUE_CONTROL,
	mcAXIS_CMD_TORQUE_FF
} MpAxisExecutingCmdEnum;

typedef enum MpAxisCamStartModeEnum
{	mcAXIS_CAM_START_ENTER_CAM,
	mcAXIS_CAM_START_RESTART
} MpAxisCamStartModeEnum;

typedef enum MpAxisAutoTuneModeEnum
{	mcAXIS_TUNE_AUTOMATIC,
	mcAXIS_TUNE_SPEED,
	mcAXIS_TUNE_POSITION,
	mcAXIS_TUNE_TEST,
	mcAXIS_TUNE_LOOP_FILTER,
	mcAXIS_TUNE_FEED_FORWARD
} MpAxisAutoTuneModeEnum;

typedef enum MpAxisAutoTuneLoopFilterModeEnum
{	mcAXIS_TUNE_LOOP_FILTER_F1,
	mcAXIS_TUNE_LOOP_FILTER_F1_F2,
	mcAXIS_TUNE_LOOP_FILTER_F1_F2_F3
} MpAxisAutoTuneLoopFilterModeEnum;

typedef enum MpAxisMoveCyclicVelocityModeEnum
{	mcAXIS_MCV_MODE_MOVE_CYC_VEL,
	mcAXIS_MCV_MODE_MOVE_VEL_CTRL
} MpAxisMoveCyclicVelocityModeEnum;

typedef enum MpAxisBasicConfigSectionEnum
{	mcAXB_CFG_SEC_ALL
} MpAxisBasicConfigSectionEnum;

typedef enum MpAxisBasicConfigCmdEnum
{	mcAXIS_BASIC_CFG_READ,
	mcAXIS_BASIC_CFG_WRITE,
	mcAXIS_BASIC_CFG_LOAD,
	mcAXIS_BASIC_CFG_SAVE,
	mcAXIS_BASIC_CFG_CREATE,
	mcAXIS_BASIC_CFG_DELETE
} MpAxisBasicConfigCmdEnum;

typedef enum MpAXBModuleChannelEnum
{	mcAXB_CFG_HW_CHANNEL1,
	mcAXB_CFG_HW_CHANNEL2,
	mcAXB_CFG_HW_CHANNEL3
} MpAXBModuleChannelEnum;

typedef enum MpAXBModuleAxTypeEnum
{	mcAXB_CFG_AX_REAL,
	mcAXB_CFG_AX_VIRT,
	mcAXB_CFG_AX_EXT_ENC,
	mcAXB_CFG_AX_PUREVAX,
	mcAXB_CFG_AX_PUREVAX_GPAI,
	mcAXB_CFG_AX_PUREVAX_EXT_ENC,
	mcAXB_CFG_AX_POWER_SUPPLY
} MpAXBModuleAxTypeEnum;

typedef enum MpAXBMotorDataTypeEnum
{	mcAXB_CFG_MOTOR_NOT_USE,
	mcAXB_CFG_MOTOR_SYNC_ANY = 10500,
	mcAXB_CFG_MOTOR_IND_ANY = 10501,
	mcAXB_CFG_MOTOR_STP = 13013
} MpAXBMotorDataTypeEnum;

typedef enum MpAXBAxBaseTypEnum
{	mcAXB_BASE_TYPE_LIN_BD = 0,
	mcAXB_BASE_TYPE_LIN = 1,
	mcAXB_BASE_TYPE_LIN_PER = 2,
	mcAXB_BASE_TYPE_ROT_BD = 10,
	mcAXB_BASE_TYPE_ROT = 11,
	mcAXB_BASE_TYPE_ROT_PER = 12
} MpAXBAxBaseTypEnum;

typedef enum MpAXBAxMeasUnitEnum
{	mcAXB_MEAS_UNIT_G_SET = 0,
	mcAXB_MEAS_UNIT_MILIMETERS = 5066068,
	mcAXB_MEAS_UNIT_M = 5067858,
	mcAXB_MEAS_UNIT_INCH = 4804168,
	mcAXB_MEAS_UNIT_DEG = 17476,
	mcAXB_MEAS_UNIT_GRAD = 4274481,
	mcAXB_MEAS_UNIT_REV = 5059636,
	mcAXB_MEAS_UNIT_GEN = -1
} MpAXBAxMeasUnitEnum;

typedef enum MpAXBAxCntDirEnum
{	mcAXB_COUNT_DIR_STD = 0,
	mcAXB_COUNT_DIR_INV = 1
} MpAXBAxCntDirEnum;

typedef enum MpAXBAxMoveLimTypEnum
{	mcAXB_MOV_LIM_STD = 0,
	mcAXB_MOV_LIM_PATH_CTRL_AX = 1,
	mcAXB_MOV_LIM_LIM_SET_REF = 2
} MpAXBAxMoveLimTypEnum;

typedef enum MpAXBDrvCtrlModEnum
{	mcAXB_CTRL_MODE_POS = 0,
	mcAXB_CTRL_MODE_POS_W_TORQ_FF = 1,
	mcAXB_CTRL_MODE_POS_MDL_BASED = 3,
	mcAXB_CTRL_MODE_V_FREQ = 2,
	mcAXB_CTRL_MODE_STP_CUR = 4,
	mcAXB_CTRL_MODE_STP_POS_AND_CUR = 5
} MpAXBDrvCtrlModEnum;

typedef enum MpAXBDrvCtrlFFwdModEnum
{	mcAXB_FF_MODE_STD = 0,
	mcAXB_FF_MODE_PRED_SPD = 1,
	mcAXB_FF_MODE_TWO_MASS_MDL = 2
} MpAXBDrvCtrlFFwdModEnum;

typedef enum MpAXBDrvCtrlFdbkModEnum
{	mcAXB_CTLR_FEED_STD = 0,
	mcAXB_CTLR_FEED_ONE_MASS_MDL = 1,
	mcAXB_CTLR_FEED_TWO_MASS_MDL = 2,
	mcAXB_CTLR_FEED_TWO_ENC_SPD = 3
} MpAXBDrvCtrlFdbkModEnum;

typedef enum MpAXBDrvCtrlVFreqCtrlAutCfgEnum
{	mcAXB_VF_AUTO_CFG_NOT_USE = 0,
	mcAXB_VF_AUTO_CFG_MOT_PAR_BASED = 1
} MpAXBDrvCtrlVFreqCtrlAutCfgEnum;

typedef enum MpAXBDrvCtrlLoopFltrTypEnum
{	mcAXB_LP_FLTR_NOT_USE = 0,
	mcAXB_LP_FLTR_LOW_PASS_2ND_ORD = 1,
	mcAXB_LP_FLTR_NOTCH = 2,
	mcAXB_LP_FLTR_BIQUAD = 3,
	mcAXB_LP_FLTR_DISC_TIME_TRAN_FUN = 4,
	mcAXB_LP_FLTR_LIM = 5,
	mcAXB_LP_FLTR_LIN_LIM = 6,
	mcAXB_LP_FLTR_RISE_TIME_LIM = 7,
	mcAXB_LP_FLTR_COMP = 8
} MpAXBDrvCtrlLoopFltrTypEnum;

typedef enum MpAXBLoopFltrLimLimTypEnum
{	mcAXB_LOOP_FLTR_LIM_TYPE_PARID = 0,
	mcAXB_LOOP_FLTR_LIM_TYPE_FIX_VAL = 1
} MpAXBLoopFltrLimLimTypEnum;

typedef enum MpAXBDrvCtrlCurModEnum
{	mcAXB_CUR_CTRL_MODE_STD = 0
} MpAXBDrvCtrlCurModEnum;

typedef enum MpAXBDrvStopReacQstopEnum
{	mcAXB_QSTOP_RCT_DEC_LIM = 0,
	mcAXB_QSTOP_RCT_DEC_LIM_W_JERK = 1,
	mcAXB_QSTOP_RCT_TORQ_LIM = 2,
	mcAXB_QSTOP_RCT_INDUCT_HALT = 3
} MpAXBDrvStopReacQstopEnum;

typedef enum MpAXBDrvStopReacDrvErrEnum
{	mcAXB_ERR_RCT_DEC_LIM = 0,
	mcAXB_ERR_RCT_INDUCT_HALT = 1,
	mcAXB_ERR_RCT_COAST_STANDSTILL = 2,
	mcAXB_ERR_RCT_CYC_DEC_AXESGROUP = 3
} MpAXBDrvStopReacDrvErrEnum;

typedef enum MpAXBDrvMovVelErrMonEnum
{	mcAXB_VEL_MON_AUT_1 = 0,
	mcAXB_VEL_MON_AUT_2 = 1,
	mcAXB_VEL_MON_USR_DEF = 2,
	mcAXB_VEL_MON_NOT_USE = 3
} MpAXBDrvMovVelErrMonEnum;

typedef enum MpAXBDrvJerkFilterTypEnum
{	mcAXB_JERK_FLTR_NOT_USE = 0,
	mcAXB_JERK_FLTR_TIME = 1,
	mcAXB_JERK_FLTR_LIM = 2
} MpAXBDrvJerkFilterTypEnum;

typedef enum MpAXBDrvDigInLevelEnum
{	mcAXB_DI_LEVEL_HIGH = 0,
	mcAXB_DI_LEVEL_LOW = 1
} MpAXBDrvDigInLevelEnum;

typedef enum MpAXBDrvDigInQstopInEnum
{	mcAXB_QSTOP_IN_TRG_2 = 0,
	mcAXB_QSTOP_IN_TRG_1 = 1,
	mcAXB_QSTOP_IN_POS_LIM_SW = 2,
	mcAXB_QSTOP_IN_NEG_LIM_SW = 3,
	mcAXB_QSTOP_IN_HOME_SW = 4,
	mcAXB_QSTOP_IN_NOT_USE = 5
} MpAXBDrvDigInQstopInEnum;

typedef struct MpAxisHomingAddTorqLimParType
{	float PositiveDirection;
	float NegativeDirection;
} MpAxisHomingAddTorqLimParType;

typedef struct MpAxisHomingOptionsType
{	float StartVelocity;
	float HomingVelocity;
	float Acceleration;
	enum McDirectionEnum SwitchEdge;
	enum McDirectionEnum StartDirection;
	enum McDirectionEnum HomingDirection;
	enum McSwitchEnum ReferencePulse;
	enum McSwitchEnum KeepDirection;
	float ReferencePulseBlockingDistance;
	float TorqueLimit;
	float BlockDetectionPositionError;
	float PositionErrorStopLimit;
	unsigned long RestorePositionVariableAddress;
	enum McSwitchEnum DriveSpecificHoming;
	signed char DriveSpecificHomingMode;
	double SensorOffset;
	enum McDirectionEnum SensorOffsetDirection;
	enum McSwitchEnum DisableRestorePositionOnEnable;
	struct MpAxisHomingAddTorqLimParType AdditionalTorqueLimit;
} MpAxisHomingOptionsType;

typedef struct MpAxisHomingType
{	enum McHomingModeEnum Mode;
	double Position;
	struct MpAxisHomingOptionsType Options;
} MpAxisHomingType;

typedef struct MpAxisJogLimitPositionType
{	double FirstPosition;
	double LastPosition;
} MpAxisJogLimitPositionType;

typedef struct MpAxisJogType
{	float Velocity;
	float Acceleration;
	float Deceleration;
	struct MpAxisJogLimitPositionType LimitPosition;
	float Jerk;
} MpAxisJogType;

typedef struct MpAxisStopAtPositionType
{	plcbit Activate;
	float Deceleration;
	double Position;
} MpAxisStopAtPositionType;

typedef struct MpAxisStopType
{	float Deceleration;
	float Jerk;
	struct MpAxisStopAtPositionType StopAtPosition;
} MpAxisStopType;

typedef struct MpAxisLimitLoadType
{	float Load;
	enum McDirectionEnum Direction;
} MpAxisLimitLoadType;

typedef struct MpAxisAutoTuneOptionsType
{	float MaxProportionalGain;
	enum McAcpAxLoopFilterModeEnum SpeedTuneLoopFilter1Mode;
	enum McAcpAxFilterTimeModeEnum SpeedTuneFilterTmeMode;
} MpAxisAutoTuneOptionsType;

typedef struct MpAxisAutoTuneFFOptionsType
{	enum McAcpAxAutoTuneFeedFwdModeEnum Mode;
	enum McDirectionEnum Direction;
	float MaxVelocityPercent;
	float Acceleration;
} MpAxisAutoTuneFFOptionsType;

typedef struct MpAxisAutoTuneType
{	enum MpAxisAutoTuneModeEnum Mode;
	enum McAcpAxAutoTuneOrientationEnum Orientation;
	float MaxCurrentPercent;
	double MaxDistance;
	double MaxPositionError;
	plcbit SaveControllerSettings;
	enum MpAxisAutoTuneLoopFilterModeEnum LoopFilterMode;
	struct MpAxisAutoTuneOptionsType Options;
	struct MpAxisAutoTuneFFOptionsType FeedForward;
} MpAxisAutoTuneType;

typedef struct MpAxisBasicParType
{	double Position;
	double Distance;
	float Velocity;
	float Acceleration;
	float Deceleration;
	enum McDirectionEnum Direction;
	struct MpAxisHomingType Homing;
	struct MpAxisJogType Jog;
	struct MpAxisStopType Stop;
	struct MpAxisLimitLoadType LimitLoad;
	struct MpAxisAutoTuneType AutoTune;
	float Jerk;
} MpAxisBasicParType;

typedef struct MpAxisStatusIDType
{	enum MpAxisErrorEnum ID;
	enum MpComSeveritiesEnum Severity;
	unsigned short Code;
} MpAxisStatusIDType;

typedef struct MpAxisInternalType
{	signed long ID;
	enum MpComSeveritiesEnum Severity;
	enum MpComFacilitiesEnum Facility;
	unsigned short Code;
} MpAxisInternalType;

typedef struct MpAxisDiagExtType
{	struct MpAxisStatusIDType StatusID;
	struct MpAxisInternalType Internal;
	enum MpAxisExecutingCmdEnum ExecutingCommand;
} MpAxisDiagExtType;

typedef struct MpAxisBasicInfoType
{	plcbit CommunicationReady;
	plcbit ReadyToPowerOn;
	plcbit Simulation;
	plcbit Jogging;
	plcbit JogLimitReached;
	plcbit LimitLoadActive;
	enum McAxisPLCopenStateEnum PLCopenState;
	struct McDigitalInputStatusType DigitalInputsStatus;
	struct MpAxisDiagExtType Diag;
	struct McLibraryInfoType LibraryInfo;
	enum McCommunicationStateEnum CommunicationState;
	unsigned long StartupCount;
	plcbit AutoTuneDone;
	float AutoTuneQuality;
	struct McHardwareInfoType HardwareInfo;
	enum McAutoTuneStateEnum AutoTuneState;
	enum McMechDevCompStateEnum MechDeviationCompState;
} MpAxisBasicInfoType;

typedef struct MpAxisOffsetParType
{	double Shift;
	float Velocity;
	float Acceleration;
	struct McAdvOffsetParType Options;
	plcbit CmdIndependentActivation;
} MpAxisOffsetParType;

typedef struct MpAxisPhasingParType
{	double Shift;
	float Velocity;
	float Acceleration;
	struct McAdvPhasingParType Options;
	plcbit CmdIndependentActivation;
} MpAxisPhasingParType;

typedef struct MpAxisCamInfoType
{	plcbit StandBy;
	plcbit InLeadIn;
	plcbit InCam;
	plcbit InLeadOut;
	plcbit EndOfProfile;
	plcbit DataInitialized;
} MpAxisCamInfoType;

typedef struct MpAxisGetCamPositionInfoType
{	double MasterPosition;
	double SlavePosition;
} MpAxisGetCamPositionInfoType;

typedef struct MpAxisRecoveryInfoType
{	double RestartPosition;
} MpAxisRecoveryInfoType;

typedef struct MpAxisCouplingInfoType
{	plcbit SlaveReady;
	plcbit MasterReady;
	struct MpAxisCamInfoType Cam;
	double ActualOffsetShift;
	double ActualPhaseShift;
	struct MpAxisGetCamPositionInfoType GetCamPosition;
	struct MpAxisRecoveryInfoType Recovery;
	struct MpAxisDiagExtType Diag;
	plcbit OffsetValid;
	plcbit PhasingValid;
} MpAxisCouplingInfoType;

typedef struct MpAxisGetCamPositionMoveParType
{	float Velocity;
	float Acceleration;
	float Deceleration;
	float Jerk;
	enum McDirectionEnum Direction;
} MpAxisGetCamPositionMoveParType;

typedef struct MpAxisGetCamPositionParType
{	enum MpAxisGetCamPositionModeEnum Mode;
	struct McCamDefineType Cam;
	signed long MasterFactor;
	signed long SlaveFactor;
	double MasterPosition;
	double SlavePosition;
	double MasterStartPosition;
	struct MpAxisGetCamPositionMoveParType Move;
} MpAxisGetCamPositionParType;

typedef struct MpAxisCamSequencerInfoType
{	plcbit SlaveReady;
	plcbit MasterReady;
	plcbit OffsetValid;
	double ActualOffsetShift;
	plcbit PhasingValid;
	double ActualPhaseShift;
	struct MpAxisDiagExtType Diag;
	struct MpAxisRecoveryInfoType Recovery;
	plcbit ActiveSignal1;
	plcbit ActiveSignal2;
	plcbit ActiveSignal3;
	plcbit ActiveSignal4;
} MpAxisCamSequencerInfoType;

typedef struct MpAxisCamSequenceGetType
{	enum McCamAutGetParCmdEnum Command;
	plcbit GetOnEnable;
} MpAxisCamSequenceGetType;

typedef struct MpAxisCamSequenceSetType
{	enum MpAxisSequenceSetModeEnum Mode;
	enum McCamAutSetParCmdEnum Command;
	plcbit UpdateCamList;
} MpAxisCamSequenceSetType;

typedef struct MpAxisCamSequenceDataType
{	struct McCamAutDefineType Data;
	struct MpAxisCamSequenceGetType Get;
	struct MpAxisCamSequenceSetType Set;
} MpAxisCamSequenceDataType;

typedef struct MpAxisCamListType
{	unsigned short Index;
	struct McCamDefineType Cam;
} MpAxisCamListType;

typedef struct MpAxisSequencerRecoveryParType
{	enum McCamAutPrepRestartModeEnum Mode;
	float Velocity;
	float Acceleration;
	float Deceleration;
	float Jerk;
	struct McAdvCamAutPrepRestartParType Options;
	double MasterPositionOffset;
} MpAxisSequencerRecoveryParType;

typedef struct MpAxisCamSequencerParType
{	float Deceleration;
	struct MpAxisCamSequenceDataType CamSequence;
	enum McCamAutParLockCmdEnum ParLockCommand;
	struct MpAxisOffsetParType Offset;
	struct MpAxisPhasingParType Phasing;
	struct MpAxisCamListType CamList[14];
	struct MpAxisSequencerRecoveryParType Recovery;
} MpAxisCamSequencerParType;

typedef struct MpAxisGearParType
{	signed long RatioNumerator;
	signed long RatioDenominator;
	enum McValueSrcEnum MasterValueSource;
	float Acceleration;
	float Deceleration;
	float Jerk;
	struct McAdvGearInParType Options;
} MpAxisGearParType;

typedef struct MpAxisCamParType
{	unsigned short ID;
	double MasterStartPosition;
	signed long MasterScaling;
	signed long SlaveScaling;
	struct McAdvBrCamInParType Options;
	enum MpAxisCamStartModeEnum Mode;
} MpAxisCamParType;

typedef struct MpAxisGearInPosParType
{	signed long RatioNumerator;
	signed long RatioDenominator;
	enum McValueSrcEnum MasterValueSource;
	double MasterSyncPosition;
	double SlaveSyncPosition;
	enum McSyncModeEnum SyncMode;
	double MasterStartDistance;
	float Velocity;
	float Acceleration;
	float Deceleration;
	float Jerk;
	struct McAdvGearInPosParType Options;
} MpAxisGearInPosParType;

typedef struct MpAxisCouplingRecoveryParType
{	enum McCamAutPrepRestartModeEnum Mode;
	float Velocity;
	float Acceleration;
	float Deceleration;
	float Jerk;
	struct McAdvCamAutPrepRestartParType Options;
	double MasterPositionOffset;
} MpAxisCouplingRecoveryParType;

typedef struct MpAxisCouplingParType
{	struct MpAxisGearParType Gear;
	struct MpAxisCamParType Cam;
	struct MpAxisGearInPosParType GearInPos;
	struct MpAxisOffsetParType Offset;
	struct MpAxisPhasingParType Phasing;
	struct MpAxisGetCamPositionParType GetCamPosition;
	struct MpAxisCamListType CamList[14];
	struct MpAxisCouplingRecoveryParType Recovery;
} MpAxisCouplingParType;

typedef struct MpAxisTorqueControlInfoType
{	plcbit InTorque;
	plcbit DataInitialized;
	plcbit Ready;
	plcbit AxisLimitActive;
} MpAxisTorqueControlInfoType;

typedef struct MpAxisCyclicSetInfoType
{	plcbit AxisReady;
	struct MpAxisTorqueControlInfoType TorqueControl;
	struct MpAxisDiagExtType Diag;
} MpAxisCyclicSetInfoType;

typedef struct MpAxisMoveCyclicPositionParType
{	enum McIplModeEnum InterpolationMode;
	float Velocity;
	float Acceleration;
	float Deceleration;
	float Jerk;
	enum McSwitchEnum DisableJoltLimitation;
	enum McAltValueSrcEnum AlternativeValueSource;
} MpAxisMoveCyclicPositionParType;

typedef struct MpAxisMoveCyclicVelocityParType
{	enum MpAxisMoveCyclicVelocityModeEnum Mode;
	enum McIplModeEnum InterpolationMode;
	float Acceleration;
	float Deceleration;
	float Jerk;
	enum McSwitchEnum DisableJoltLimitation;
	enum McAltValueSrcEnum AlternativeValueSource;
} MpAxisMoveCyclicVelocityParType;

typedef struct MpAxisTorqueControlParType
{	plcbit CmdIndependentActivation;
	float TorqueResolution;
	float TorqueRamp;
	float MaximumVelocity;
	float MinimumVelocity;
	float Acceleration;
	float Jerk;
	struct McAdvBrTorqueControlParType Options;
} MpAxisTorqueControlParType;

typedef struct MpAxisTorqueFFParType
{	enum McIplModeEnum InterpolationMode;
	struct McAdvCyclicTorqueFFParType Options;
} MpAxisTorqueFFParType;

typedef struct MpAxisCyclicSetParType
{	struct MpAxisMoveCyclicPositionParType MoveCyclicPosition;
	struct MpAxisMoveCyclicVelocityParType MoveCyclicVelocity;
	struct MpAxisTorqueControlParType TorqueControl;
	struct MpAxisTorqueFFParType TorqueFeedForward;
} MpAxisCyclicSetParType;

typedef struct MpAXBMotorType
{	enum MpAXBMotorDataTypeEnum Type;
	unsigned long Data;
} MpAXBMotorType;

typedef struct MpAXBModuleType
{	plcstring ModelNumber[251];
	plcstring Location[251];
	enum MpAXBModuleAxTypeEnum AxisType;
	enum MpAXBModuleChannelEnum Channel;
	struct MpAXBMotorType Motor;
} MpAXBModuleType;

typedef struct MpAXBAxMoveLimPosType
{	double LowerLimit;
	double UpperLimit;
} MpAXBAxMoveLimPosType;

typedef struct MpAXBAxMoveLimVelType
{	float Positive;
	float Negative;
} MpAXBAxMoveLimVelType;

typedef struct MpAXBAxMoveLimType
{	enum MpAXBAxMoveLimTypEnum Type;
	struct MpAXBAxMoveLimPosType Position;
	struct MpAXBAxMoveLimVelType Velocity;
	float Acceleration;
	float Deceleration;
	float Jerk;
	float Torque;
	float Force;
	plcstring LimitSetReference[251];
} MpAXBAxMoveLimType;

typedef struct MpAXBAxType
{	enum MpAXBAxBaseTypEnum BaseType;
	enum MpAXBAxMeasUnitEnum MeasurementUnit;
	double MeasurementResolution;
	enum MpAXBAxCntDirEnum CountDirection;
	double Period;
	struct MpAXBAxMoveLimType MovementLimits;
} MpAXBAxType;

typedef struct MpAXBDrvMechElmGBType
{	signed long Input;
	signed long Output;
} MpAXBDrvMechElmGBType;

typedef struct MpAXBDrvMechElmRotToLinTrfType
{	double ReferenceDistance;
} MpAXBDrvMechElmRotToLinTrfType;

typedef struct MpAXBDrvMechElmType
{	struct MpAXBDrvMechElmGBType Gearbox;
	struct MpAXBDrvMechElmRotToLinTrfType RotaryToLinearTransformation;
} MpAXBDrvMechElmType;

typedef struct MpAXBDrvCtrlPosType
{	float ProportionalGain;
	float IntegrationTime;
	float TotalDelayTime;
	float PredictionTime;
	float MaximumProportionalAction;
	float MaximumIntegralAction;
} MpAXBDrvCtrlPosType;

typedef struct MpAXBDrvCtrlSpdType
{	float ProportionalGain;
	float IntegrationTime;
	float FilterTime;
} MpAXBDrvCtrlSpdType;

typedef struct MpAXBDrvCtrlFFwdType
{	enum MpAXBDrvCtrlFFwdModEnum Mode;
	float TorqueLoad;
	float TorquePositive;
	float TorqueNegative;
	float SpeedTorqueFactor;
	float Inertia;
	float AccelerationFilterTime;
	float PredictionTime;
} MpAXBDrvCtrlFFwdType;

typedef struct MpAXBDrvCtrlFdbkType
{	enum MpAXBDrvCtrlFdbkModEnum Mode;
	float SpeedMixingFactor;
	float SpeedProportionalGain;
} MpAXBDrvCtrlFdbkType;

typedef struct MpAXBDrvCtrlMdlMass1Type
{	float Inertia;
	float ViscousFriction;
} MpAXBDrvCtrlMdlMass1Type;

typedef struct MpAXBDrvCtrlMdlMass2Type
{	float Inertia;
	float ViscousFriction;
	float Stiffness;
	float Damping;
} MpAXBDrvCtrlMdlMass2Type;

typedef struct MpAXBDrvCtrlMdlType
{	struct MpAXBDrvCtrlMdlMass1Type Mass1;
	struct MpAXBDrvCtrlMdlMass2Type Mass2;
} MpAXBDrvCtrlMdlType;

typedef struct MpAXBDrvCtrlVFreqCtrlType
{	enum MpAXBDrvCtrlVFreqCtrlAutCfgEnum AutomaticConfiguration;
	float SlipCompensation;
	float TotalDelayTime;
	float BoostVoltage;
	float RatedVoltage;
	float RatedFrequency;
} MpAXBDrvCtrlVFreqCtrlType;

typedef struct MpAXBLoopFltrLP2ndOrdType
{	float CutOffFrequency;
} MpAXBLoopFltrLP2ndOrdType;

typedef struct MpAXBLoopFltrNotchType
{	float CenterFrequency;
	float Bandwidth;
} MpAXBLoopFltrNotchType;

typedef struct MpAXBLoopFltrBiquadType
{	float FrequencyNumerator;
	float DampingNumerator;
	float FrequencyDenominator;
	float DampingDenominator;
} MpAXBLoopFltrBiquadType;

typedef struct MpAXBLoopFltrDiscTimeTranFunType
{	float CoeffA0DenominatorPolynomial;
	float CoeffA1DenominatorPolynomial;
	float CoeffB0NominatorPolynomial;
	float CoeffB1NominatorPolynomial;
	float CoeffB2NominatorPolynomial;
} MpAXBLoopFltrDiscTimeTranFunType;

typedef struct MpAXBLoopFltrLimLimType
{	enum MpAXBLoopFltrLimLimTypEnum Type;
	unsigned short ParID;
	float Value;
} MpAXBLoopFltrLimLimType;

typedef struct MpAXBLoopFltrLimType
{	struct MpAXBLoopFltrLimLimType PositiveLimit;
	struct MpAXBLoopFltrLimLimType NegativeLimit;
} MpAXBLoopFltrLimType;

typedef struct MpAXBLoopFltrLinLimType
{	unsigned short InputParID;
	float InputLimit;
	float Gradient;
} MpAXBLoopFltrLinLimType;

typedef struct MpAXBLoopFltrRiseTimeLimType
{	float RiseTime;
	float NormalizedLimit;
} MpAXBLoopFltrRiseTimeLimType;

typedef struct MpAXBLoopFltrCompType
{	unsigned short MultiplicationFactorParID;
	unsigned short AdditiveValueParID;
} MpAXBLoopFltrCompType;

typedef struct MpAXBDrvCtrlLoopFltrLoopFltrType
{	enum MpAXBDrvCtrlLoopFltrTypEnum Type;
	struct MpAXBLoopFltrLP2ndOrdType Lowpass2ndOrder;
	struct MpAXBLoopFltrNotchType Notch;
	struct MpAXBLoopFltrBiquadType Biquad;
	struct MpAXBLoopFltrDiscTimeTranFunType DiscreteTimeTransferFunction;
	struct MpAXBLoopFltrLimType Limiter;
	struct MpAXBLoopFltrLinLimType LinearLimitation;
	struct MpAXBLoopFltrRiseTimeLimType RiseTimeLimitation;
	struct MpAXBLoopFltrCompType Compensation;
} MpAXBDrvCtrlLoopFltrLoopFltrType;

typedef struct MpAXBDrvCtrlLoopFltrType
{	struct MpAXBDrvCtrlLoopFltrLoopFltrType LoopFilter[3];
} MpAXBDrvCtrlLoopFltrType;

typedef struct MpAXBDrvCtrlCurType
{	enum MpAXBDrvCtrlCurModEnum Mode;
	float StandstillCurrent;
	float ConstantSpeedCurrent;
	float SpeedChangeCurrent;
	float FullStepThreshold;
	float MotorSettlingTime;
	float DelayedCurrentSwitchOff;
} MpAXBDrvCtrlCurType;

typedef struct MpAXBDrvCtrlType
{	enum MpAXBDrvCtrlModEnum Mode;
	struct MpAXBDrvCtrlPosType Position;
	struct MpAXBDrvCtrlSpdType Speed;
	struct MpAXBDrvCtrlFFwdType FeedForward;
	struct MpAXBDrvCtrlFdbkType Feedback;
	struct MpAXBDrvCtrlMdlType Model;
	struct MpAXBDrvCtrlVFreqCtrlType VoltageFrequencyControl;
	struct MpAXBDrvCtrlLoopFltrType LoopFilters;
	struct MpAXBDrvCtrlCurType Current;
} MpAXBDrvCtrlType;

typedef struct MpAXBDrvHomeType
{	enum McHomingModeEnum Mode;
	double Position;
	enum McSwitchEnum ReferencePulse;
	double ReferencePulseBlockingDistance;
	float StartVelocity;
	float HomingVelocity;
	float Acceleration;
	enum McDirectionEnum SwitchEdge;
	enum McDirectionEnum HomingDirection;
	enum McDirectionEnum StartDirection;
	enum McSwitchEnum KeepDirection;
	float TorqueLimit;
	double PositionErrorStopLimit;
	double BlockDetectionPositionError;
	plcstring RestorePositionVariable[251];
} MpAXBDrvHomeType;

typedef struct MpAXBDrvStopReacType
{	enum MpAXBDrvStopReacQstopEnum Quickstop;
	enum MpAXBDrvStopReacDrvErrEnum DriveError;
} MpAXBDrvStopReacType;

typedef struct MpAXBDrvMovementErrorLimitsType
{	double PositionError;
	enum MpAXBDrvMovVelErrMonEnum VelocityErrorMonitoring;
	float VelocityError;
} MpAXBDrvMovementErrorLimitsType;

typedef struct MpAXBDrvJerkFilterType
{	enum MpAXBDrvJerkFilterTypEnum Type;
	float Jerk;
} MpAXBDrvJerkFilterType;

typedef struct MpAXBDrvDigInHomeSwType
{	enum MpAXBDrvDigInLevelEnum Level;
} MpAXBDrvDigInHomeSwType;

typedef struct MpAXBDrvDigInPosLimSwType
{	enum MpAXBDrvDigInLevelEnum Level;
} MpAXBDrvDigInPosLimSwType;

typedef struct MpAXBDrvDigInNegLimSwType
{	enum MpAXBDrvDigInLevelEnum Level;
} MpAXBDrvDigInNegLimSwType;

typedef struct MpAXBDrvDigInTrg1Type
{	enum MpAXBDrvDigInLevelEnum Level;
} MpAXBDrvDigInTrg1Type;

typedef struct MpAXBDrvDigInTrg2Type
{	enum MpAXBDrvDigInLevelEnum Level;
} MpAXBDrvDigInTrg2Type;

typedef struct MpAXBDrvDigInQstopType
{	enum MpAXBDrvDigInQstopInEnum Input;
} MpAXBDrvDigInQstopType;

typedef struct MpAXBDrvDigInType
{	struct MpAXBDrvDigInHomeSwType HomingSwitch;
	struct MpAXBDrvDigInPosLimSwType PositiveLimitSwitch;
	struct MpAXBDrvDigInNegLimSwType NegativeLimitSwitch;
	struct MpAXBDrvDigInTrg1Type Trigger1;
	struct MpAXBDrvDigInTrg2Type Trigger2;
	struct MpAXBDrvDigInQstopType Quickstop;
} MpAXBDrvDigInType;

typedef struct MpAXBDrvType
{	struct MpAXBDrvMechElmType MechanicalElements;
	struct MpAXBDrvCtrlType Controller;
	struct MpAXBDrvHomeType Homing;
	struct MpAXBDrvStopReacType StopReaction;
	struct MpAXBDrvMovementErrorLimitsType MovementErrorLimits;
	struct MpAXBDrvJerkFilterType JerkFilter;
	struct MpAXBDrvDigInType DigitalInputs;
} MpAXBDrvType;

typedef struct MpAxisBasicConfigType
{	struct MpAXBAxType Axis;
	struct MpAXBDrvType Drive;
} MpAxisBasicConfigType;

typedef struct MpAxisBasicConfigParType
{	struct MpAxisBasicConfigType* Data;
	enum MpAxisBasicConfigSectionEnum Section;
	plcstring AxisName[251];
	struct MpAXBModuleType Module;
} MpAxisBasicConfigParType;

typedef struct MpAxisBasic
{
	/* VAR_INPUT (analog) */
	struct McAxisType* MpLink;
	struct MpAxisBasicParType* Parameters;
	/* VAR_OUTPUT (analog) */
	signed long StatusID;
	double Position;
	float Velocity;
	struct MpAxisBasicInfoType Info;
	/* VAR (analog) */
	struct MpComInternalDataType Internal;
	/* VAR_INPUT (digital) */
	plcbit Enable;
	plcbit ErrorReset;
	plcbit Update;
	plcbit Power;
	plcbit Home;
	plcbit MoveVelocity;
	plcbit MoveAbsolute;
	plcbit MoveAdditive;
	plcbit Stop;
	plcbit JogPositive;
	plcbit JogNegative;
	plcbit LimitLoad;
	plcbit ReleaseBrake;
	plcbit Simulate;
	plcbit AutoTune;
	/* VAR_OUTPUT (digital) */
	plcbit Active;
	plcbit Error;
	plcbit UpdateDone;
	plcbit CommandBusy;
	plcbit CommandAborted;
	plcbit PowerOn;
	plcbit IsHomed;
	plcbit InVelocity;
	plcbit InPosition;
	plcbit MoveActive;
	plcbit MoveDone;
	plcbit Stopped;
	plcbit LimitLoadReady;
	plcbit BrakeReleased;
} MpAxisBasic_typ;

typedef struct MpAxisCamSequencer
{
	/* VAR_INPUT (analog) */
	struct McAxisType* MpLink;
	struct MpAxisCamSequencerParType* Parameters;
	struct McAxisType* MpLinkMaster;
	/* VAR_OUTPUT (analog) */
	signed long StatusID;
	unsigned char ActualStateIndex;
	unsigned short ActualStateCamIndex;
	struct MpAxisCamSequencerInfoType Info;
	/* VAR (analog) */
	struct MpComInternalDataType Internal;
	/* VAR_INPUT (digital) */
	plcbit Enable;
	plcbit ErrorReset;
	plcbit Update;
	plcbit Signal1;
	plcbit Signal2;
	plcbit Signal3;
	plcbit Signal4;
	plcbit StartSequence;
	plcbit Continue;
	plcbit EndSequence;
	plcbit GetSequence;
	plcbit ParLock;
	plcbit OffsetShift;
	plcbit PhaseShift;
	plcbit CamPrepare;
	plcbit Recovery;
	/* VAR_OUTPUT (digital) */
	plcbit Active;
	plcbit Error;
	plcbit UpdateDone;
	plcbit Running;
	plcbit Standby;
	plcbit InCompensation;
	plcbit InCam;
	plcbit CommandBusy;
	plcbit CommandAborted;
	plcbit GetSequenceDone;
	plcbit ParLockDone;
	plcbit OffsetDone;
	plcbit PhasingDone;
	plcbit CamPrepareDone;
	plcbit RecoveryDone;
} MpAxisCamSequencer_typ;

typedef struct MpAxisCoupling
{
	/* VAR_INPUT (analog) */
	struct McAxisType* MpLink;
	struct MpAxisCouplingParType* Parameters;
	struct McAxisType* MpLinkMaster;
	/* VAR_OUTPUT (analog) */
	signed long StatusID;
	struct MpAxisCouplingInfoType Info;
	/* VAR (analog) */
	struct MpComInternalDataType Internal;
	/* VAR_INPUT (digital) */
	plcbit Enable;
	plcbit ErrorReset;
	plcbit Update;
	plcbit Gear;
	plcbit Cam;
	plcbit GearInPos;
	plcbit OffsetShift;
	plcbit PhaseShift;
	plcbit GetCamPosition;
	plcbit CamPrepare;
	plcbit Recovery;
	/* VAR_OUTPUT (digital) */
	plcbit Active;
	plcbit Error;
	plcbit UpdateDone;
	plcbit CommandBusy;
	plcbit CommandAborted;
	plcbit InCompensation;
	plcbit InSync;
	plcbit OffsetDone;
	plcbit PhasingDone;
	plcbit RecoveryDone;
	plcbit GetCamPositionDone;
	plcbit CamPrepareDone;
} MpAxisCoupling_typ;

typedef struct MpAxisCyclicSet
{
	/* VAR_INPUT (analog) */
	struct McAxisType* MpLink;
	struct MpAxisCyclicSetParType* Parameters;
	double CyclicPosition;
	float CyclicVelocity;
	float CyclicTorque;
	float CyclicTorqueFeedForward;
	/* VAR_OUTPUT (analog) */
	signed long StatusID;
	struct MpAxisCyclicSetInfoType Info;
	/* VAR (analog) */
	struct MpComInternalDataType Internal;
	/* VAR_INPUT (digital) */
	plcbit Enable;
	plcbit ErrorReset;
	plcbit Update;
	plcbit MoveCyclicPosition;
	plcbit MoveCyclicVelocity;
	plcbit TorqueControl;
	plcbit TorqueFeedForward;
	/* VAR_OUTPUT (digital) */
	plcbit Active;
	plcbit Error;
	plcbit UpdateDone;
	plcbit CommandBusy;
	plcbit CommandAborted;
	plcbit MoveCyclicPositionActive;
	plcbit MoveCyclicVelocityActive;
	plcbit TorqueControlActive;
	plcbit TorqueFeedForwardActive;
} MpAxisCyclicSet_typ;

typedef struct MpAxisBasicConfig
{
	/* VAR_INPUT (analog) */
	struct McAxisType* MpLink;
	struct MpAxisBasicConfigParType Parameters;
	enum MpAxisBasicConfigCmdEnum Command;
	/* VAR_OUTPUT (analog) */
	signed long ErrorID;
	/* VAR (analog) */
	struct MpComInternalDataType Internal;
	/* VAR_INPUT (digital) */
	plcbit Execute;
	/* VAR_OUTPUT (digital) */
	plcbit Done;
	plcbit PLCRestartRequired;
	plcbit Busy;
	plcbit Error;
} MpAxisBasicConfig_typ;



/* Prototyping of functions and function blocks */
_BUR_PUBLIC void MpAxisBasic(struct MpAxisBasic* inst);
_BUR_PUBLIC void MpAxisCamSequencer(struct MpAxisCamSequencer* inst);
_BUR_PUBLIC void MpAxisCoupling(struct MpAxisCoupling* inst);
_BUR_PUBLIC void MpAxisCyclicSet(struct MpAxisCyclicSet* inst);
_BUR_PUBLIC void MpAxisBasicConfig(struct MpAxisBasicConfig* inst);


#ifdef __cplusplus
};
#endif
#endif /* _MPAXIS_ */

