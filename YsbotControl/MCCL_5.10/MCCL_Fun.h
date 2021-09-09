#ifndef _MCCL_FUN_H_
#define _MCCL_FUN_H_

#include "mccl.h"

#ifdef __cplusplus
	extern "C" {
#endif


//////////////////////////////////////////////////////////////////////////////
// Definitions of Calling Convention & Storage-Class Attributes

#ifdef STATIC_LIB
	#define MCC_CALL
	#define MCC_API
#else
#ifdef LINUX
    #define MCC_CALL
	#define MCC_API
#else	
    #define MCC_CALL		__stdcall

	#ifdef MCCL_EXPORTS
		#define MCC_API		__declspec(dllexport)
	#else
		#define MCC_API		__declspec(dllimport)
	#endif // MCCL_EXPORTS
#endif
#endif // STATIC_LIB


//////////////////////////////////////////////////////////////////////////////
// System Management

// Get Library Version
MCC_API void   MCC_CALL MCC_GetVersion(char *strVersion);

// Create/Close Motion Groups
MCC_API int    MCC_CALL MCC_CreateGroup(int xMapToCh, int yMapToCh, int zMapToCh, int uMapToCh, int vMapToCh, int wMapToCh, int nCardIndex);
MCC_API int    MCC_CALL MCC_CreateGroupEx(int xMapToCh, int yMapToCh, int zMapToCh, int uMapToCh, int vMapToCh, int wMapToCh, int nCardIndex, int nMotionQueueSize = MOTION_QUEUE_SIZE);
MCC_API int    MCC_CALL MCC_CloseGroup(int nGroupIndex);
MCC_API int    MCC_CALL MCC_CloseAllGroups();
MCC_API int    MCC_CALL MCC_SetGroupConfig(SYS_GROUP_CONFIG *pstConfig);

// Set/Get Mechanism Parameters
MCC_API int    MCC_CALL MCC_SetMacParam(SYS_MAC_PARAM *pstMacParam, WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_GetMacParam(SYS_MAC_PARAM *pstMacParam, WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_SetMacParamEx(SYS_MAC_PARAM_EX *pstMacParam, WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_GetMacParamEx(SYS_MAC_PARAM_EX *pstMacParam, WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_SetEncoderConfig(SYS_ENCODER_CONFIG *pstEncoderConfig, WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_GetEncoderConfig(SYS_ENCODER_CONFIG *pstEncoderConfig, WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_UpdateParam();
MCC_API double MCC_CALL MCC_GetConvFactor(WORD wChannel, WORD wCardIndex = 0);

// Set/Get Size of Motion Command Queue
MCC_API int    MCC_CALL MCC_SetCmdQueueSize(int nSize, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_GetCmdQueueSize(WORD wGroupIndex = 0);

// Initialize/Close System
MCC_API int    MCC_CALL MCC_InitSystem(int nInterpolateTime, SYS_CARD_CONFIG *pstCardConfig, WORD wCardNo);
MCC_API int    MCC_CALL MCC_InitSystemEx(double nInterpolateTime, SYS_CARD_CONFIG *pstCardConfig, WORD wCardNo);
MCC_API int    MCC_CALL MCC_InitSimulation(int nInterpolateTime, SYS_CARD_CONFIG *pstCardConfig, WORD wCardNo);
MCC_API int    MCC_CALL MCC_CloseSystem();

// Reset MCCL
MCC_API int    MCC_CALL MCC_ResetMotion();

// Enable/Disable dry-run
MCC_API int    MCC_CALL MCC_EnableDryRun();
MCC_API int    MCC_CALL MCC_DisableDryRun();
MCC_API int    MCC_CALL MCC_CheckDryRun();
MCC_API int    MCC_CALL MCC_EnableDryRunEx(WORD wGroupIndex);
MCC_API int    MCC_CALL MCC_DisableDryRunEx(WORD wGroupIndex);

// Set/Get Max. Feed Speed
MCC_API int    MCC_CALL MCC_SetSysMaxSpeed(double dfMaxSpeed);
MCC_API double MCC_CALL MCC_GetSysMaxSpeed();


//////////////////////////////////////////////////////////////////////////////
// Local I/O Control

// Servo On/Off
MCC_API int    MCC_CALL MCC_SetServoOn(WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_SetServoOff(WORD wChannel, WORD wCardIndex = 0);

// Enable/Disable Position Ready
MCC_API int    MCC_CALL MCC_EnablePosReady(WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_DisablePosReady(WORD wCardIndex = 0);

// Emergency Stop
MCC_API int    MCC_CALL MCC_GetEmgcStopStatus(WORD *pwStatus, WORD wCardIndex = 0);

// Input Signal Trigger
MCC_API int    MCC_CALL MCC_SetLIORoutineEx(LIOISR_EX pfnLIORoutine, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_SetLIOTriggerType(WORD wTriggerType, WORD wPoint, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_EnableLIOTrigger(WORD wPoint, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_DisableLIOTrigger(WORD wPoint, WORD wCardIndex = 0);


//////////////////////////////////////////////////////////////////////////////
// Coordinate Management

// Set/Get Coordinate Type
MCC_API int    MCC_CALL MCC_SetAbsolute(WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_SetIncrease(WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_GetCoordType(WORD wGroupIndex = 0);

// Get Current Position & Pulse Position
MCC_API int    MCC_CALL MCC_GetCurRefPos(double *pdfX, double *pdfY, double *pdfZ, double *pdfU, double *pdfV, double *pdfW, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_GetCurPos(double *pdfX, double *pdfY, double *pdfZ, double *pdfU, double *pdfV, double *pdfW, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_GetPulsePos(long *plX, long *plY, long *plZ, long *plU, long *plV, long *plW, WORD wGroupIndex = 0);

// Regard here as origin
MCC_API int    MCC_CALL MCC_DefineOrigin(WORD wAxis, WORD wGroupIndex = 0);

// Change command and actual positions according to specified value
MCC_API int    MCC_CALL MCC_DefinePos(WORD wAxis, double dfPos, WORD wGroupIndex = 0);

// Align command position with actual position
MCC_API int    MCC_CALL MCC_DefinePosHere(WORD wGroupIndex = 0, DWORD dwAxisMask = EPCIO_AXIS_ALL);


//////////////////////////////////////////////////////////////////////////////
// Software Over Travel Check & Hardware Limit Switch Check

// Enable/Disable Hardware Limit Switch Check
MCC_API int    MCC_CALL MCC_EnableLimitSwitchCheck(int nMode = 0);
MCC_API int    MCC_CALL MCC_DisableLimitSwitchCheck();

// Enable/Disable Software Over Travel Check
MCC_API int    MCC_CALL MCC_SetOverTravelCheck(int nOTCheck0, int nOTCheck1, int nOTCheck2, int nOTCheck3, int nOTCheck4, int nOTCheck5, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_GetOverTravelCheck(int *pnOTCheck0, int *pnOTCheck1, int *pnOTCheck2, int *pnOTCheck3, int *pnOTCheck4, int *pnOTCheck5, WORD wGroupIndex = 0);

// Get Limit Switch Sensor Signal
MCC_API int    MCC_CALL MCC_GetLimitSwitchStatus(WORD *pwStatus, WORD wUpDown, WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_GetLatchedLimitSwitchStatus(WORD *pwStatus, WORD wUpDown, WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_ClearLatchedLimitSwitchStatus(WORD wUpDown, WORD wChannel, WORD wCardIndex = 0);


//////////////////////////////////////////////////////////////////////////////
// General Motions(Line, Arc, Circle Motions)

// Set/Get Accleration & Deceleration Type
MCC_API int    MCC_CALL MCC_SetAccType(char cAccType, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_GetAccType(WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_SetDecType(char cDecType, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_GetDecType(WORD wGroupIndex = 0);

// Set/Get Accleration & Deceleration Time
MCC_API int    MCC_CALL MCC_SetAccTime(double dfAccTime, WORD wGroupIndex = 0);
MCC_API double MCC_CALL MCC_GetAccTime(WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_SetDecTime(double dfDecTime, WORD wGroupIndex = 0);
MCC_API double MCC_CALL MCC_GetDecTime(WORD wGroupIndex = 0);

// Set/Get Feed Speed
MCC_API double MCC_CALL MCC_SetInitSpeed(double dfInitSpeed, WORD wGroupIndex = 0);
MCC_API double MCC_CALL MCC_SetFeedSpeed(double dfFeedSpeed, WORD wGroupIndex = 0);
MCC_API double MCC_CALL MCC_GetFeedSpeed(WORD wGroupIndex = 0);
MCC_API double MCC_CALL MCC_GetCurFeedSpeed(WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_GetSpeed(double *pdfVel0, double *pdfVel1, double *pdfVel2, double *pdfVel3, double *pdfVel4, double *pdfVel5, WORD wGroupIndex = 0);

// Set/Get Point-to-Point Motion Speed Ratio
MCC_API double MCC_CALL MCC_SetPtPSpeed(double dfRatio, WORD wGroupIndex = 0);
MCC_API double MCC_CALL MCC_GetPtPSpeed(WORD wGroupIndex = 0);

// Point-to-Point Motion
MCC_API int    MCC_CALL MCC_PtP(double dfX0, double dfX1, double dfX2, double dfX3, double dfX4, double dfX5, WORD wGroupIndex = 0, DWORD dwAxisMask = EPCIO_AXIS_ALL);
MCC_API int    MCC_CALL MCC_SyncPtP(double dfX0, double dfX1, double dfX2, double dfX3, double dfX4, double dfX5, WORD wGroupIndex = 0, DWORD dwAxisMask = EPCIO_AXIS_ALL);

MCC_API int    MCC_CALL MCC_PtPX(double dfX, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_PtPY(double dfY, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_PtPZ(double dfZ, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_PtPU(double dfU, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_PtPV(double dfV, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_PtPW(double dfW, WORD wGroupIndex = 0);

// Linear Motion
MCC_API int    MCC_CALL MCC_Line(double dfX0, double dfX1, double dfX2, double dfX3, double dfX4, double dfX5, WORD wGroupIndex = 0, DWORD dwAxisMask = EPCIO_AXIS_ALL);

// Arc Motion
MCC_API int    MCC_CALL MCC_ArcXYZ(double dfRX0, double dfRX1, double dfRX2, double dfX0, double dfX1, double dfX2, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_ArcXY(double dfRX0, double dfRX1, double dfX0, double dfX1, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_ArcYZ(double dfRX1, double dfRX2, double dfX1, double dfX2, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_ArcZX(double dfRX2, double dfRX0, double dfX2, double dfX0, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_ArcXYZUVW(double dfRX0, double dfRX1, double dfRX2, double dfX0, double dfX1, double dfX2, double dfX3, double dfX4, double dfX5, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_ArcXYUVW(double dfRX0, double dfRX1, double dfX0, double dfX1, double dfX3, double dfX4, double dfX5, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_ArcYZUVW(double dfRX1, double dfRX2, double dfX1, double dfX2, double dfX3, double dfX4, double dfX5, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_ArcZXUVW(double dfRX2, double dfRX0, double dfX2, double dfX0, double dfX3, double dfX4, double dfX5, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_ArcThetaXY(double dfCX, double dfCY, double dfTheta, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_ArcThetaYZ(double dfCY, double dfCZ, double dfTheta, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_ArcThetaZX(double dfCZ, double dfCX, double dfTheta, WORD wGroupIndex = 0);

// Circular Motion
MCC_API int    MCC_CALL MCC_CircleXY(double dfCX, double dfCY, BYTE byCirDir, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_CircleYZ(double dfCY, double dfCZ, BYTE byCirDir, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_CircleZX(double dfCZ, double dfCX, BYTE byCirDir, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_CircleXYUVW(double dfCX, double dfCY, double dfU, double dfV, double dfW, BYTE byCirDir, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_CircleYZUVW(double dfCY, double dfCZ, double dfU, double dfV, double dfW, BYTE byCirDir, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_CircleZXUVW(double dfCZ, double dfCX, double dfU, double dfV, double dfW, BYTE byCirDir, WORD wGroupIndex = 0);

MCC_API int    MCC_CALL MCC_HelicalXY_Z( double dfCX, double dfCY, double dfPitch, double dfTheta, WORD   wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_HelicalYZ_X( double dfCY, double dfCZ, double dfPitch, double dfTheta, WORD   wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_HelicalZX_Y( double dfCZ, double dfCX, double dfPitch, double dfTheta, WORD   wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_HelicalXY_Z_UVW( double dfCX, double dfCY, double dfPitch, double dfU, double dfV, double dfW, double dfTheta, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_HelicalYZ_X_UVW( double dfCY, double dfCZ, double dfPitch, double dfU, double dfV, double dfW, double dfTheta, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_HelicalZX_Y_UVW( double dfCZ, double dfCX, double dfPitch, double dfU, double dfV, double dfW, double dfTheta, WORD wGroupIndex = 0);

//////////////////////////////////////////////////////////////////////////////
// Jog Motion

MCC_API int    MCC_CALL MCC_JogPulse(int nPulse, char cAxis, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_JogSpace(double dfOffset, double dfRatio, char cAxis, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_JogConti(int nDir, double dfRatio, char cAxis, WORD wGroupIndex = 0);

//////////////////////////////////////////////////////////////////////////////
// Point-to-Point Motion

// Set/Get accleration & deceleration types
MCC_API int    MCC_CALL MCC_SetPtPAccType(char cAccType0, char cAccType1, char cAccType2, char cAccType3, char cAccType4, char cAccType5,  WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_GetPtPAccType(char *pcAccType0, char *pcAccType1, char *pcAccType2, char *pcAccType3, char *pcAccType4, char *pcAccType5, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_SetPtPDecType(char cDecType0, char cDecType1, char cDecType2, char cDecType3, char cDecType4, char cDecType5, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_GetPtPDecType(char *pcDecType0, char *pcDecType1, char *pcDecType2, char *pcDecType3, char *pcDecType4, char *pcDecType5, WORD wGroupIndex = 0);

// Set/Get accleration & deceleration times
MCC_API int    MCC_CALL MCC_SetPtPAccTime(double dfAccTime0, double dfAccTime1, double dfAccTime2, double dfAccTime3, double dfAccTime4, double dfAccTime5, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_GetPtPAccTime(double *pdfAccTime0, double *pdfAccTime1, double *pdfAccTime2, double *pdfAccTime3, double *pdfAccTime4, double *pdfAccTime5, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_SetPtPDecTime(double dfDecTime0, double dfDecTime1, double dfDecTime2, double dfDecTime3, double dfDecTime4, double dfDecTime5, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_GetPtPDecTime(double *pdfDecTime0, double *pdfDecTime1, double *pdfDecTime2, double *pdfDecTime3, double *pdfDecTime4, double *pdfDecTime5, WORD wGroupIndex = 0);

//////////////////////////////////////////////////////////////////////////////
// Custom Motion

MCC_API int    MCC_CALL MCC_CustomMotionEx(
	                        CUSTOM_START_MOTION   pfnStartMotion,
							CUSTOM_INTERPOLATION  pfnInterpolation,
							CUSTOM_CLEANUP        pfnCleanUp,
							CUSTOM_BLENDING_START pfnBlendingStart,
							CUSTOM_BLENDING       pfnBlending,
							CUSTOM_BLENDING_END   pfnBlendingEnd,
							void* pvBuffer,
							BOOL bOverrideMotion = FALSE,
							WORD wGroupIndex =0,
							DWORD dwAxisMask =EPCIO_AXIS_ALL);

MCC_API int    MCC_CALL MCC_CustomMotion(
							CUSTOM_START_MOTION pfnStartMotion,
							CUSTOM_INTERPOLATION pfnInterpolation,
							CUSTOM_CLEANUP pfnCleanUp,
							void* pvBuffer,
							WORD wGroupIndex = 0,
							DWORD dwAxisMask = EPCIO_AXIS_ALL);

//////////////////////////////////////////////////////////////////////////////
// Motion Status

// Get Current Motion Status
MCC_API int    MCC_CALL MCC_GetMotionStatus(WORD wGroupIndex = 0);

// Get Current Executing Motion Command
MCC_API int    MCC_CALL MCC_GetCurCommand(COMMAND_INFO *pstCurCommand, WORD wGroupIndex = 0);

// Get Motion Command Stock Count
MCC_API int    MCC_CALL MCC_GetCommandCount(int *pnCmdCount, WORD wGroupIndex = 0);

MCC_API int    MCC_CALL MCC_ResetCommandIndex(WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_SetCommandIndex(WORD wIndex, BOOL bManualIndex, WORD wGroupIndex);

// Set/Get Hardware Pulse Stock Count
MCC_API int    MCC_CALL MCC_SetMaxPulseStockNum(int nMaxStockNum);
MCC_API int    MCC_CALL MCC_GetMaxPulseStockNum();
MCC_API int    MCC_CALL MCC_GetCurPulseStockCount(WORD *pwStockCount, WORD wChannel, WORD wCardIndex = 0);

// Get/Clear Error Code
MCC_API int    MCC_CALL MCC_GetErrorCode(WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_ClearError(WORD wGroupIndex = 0);


//////////////////////////////////////////////////////////////////////////////
// Go Home

// Set/Get configuration of the homing process
MCC_API int    MCC_CALL MCC_SetHomeConfig(SYS_HOME_CONFIG *pstHomeConfig, WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_GetHomeConfig(SYS_HOME_CONFIG *pstHomeConfig, WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_SetHomeConfigEx(SYS_HOME_CONFIG_EX *pstHomeConfig, WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_GetHomeConfigEx(SYS_HOME_CONFIG_EX *pstHomeConfig, WORD wChannel, WORD wCardIndex = 0);

// Operations
MCC_API int    MCC_CALL MCC_GoHome( int    nXOrder, int    nYOrder, int    nZOrder, int    nUOrder, int    nVOrder, int    nWOrder, WORD wTolerance, double dfEscapeTime, WORD   wCardIndex = 0);
MCC_API int    MCC_CALL MCC_Home(int nOrder0, int nOrder1, int nOrder2, int nOrder3, int nOrder4, int nOrder5, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_GetGoHomeStatus();
MCC_API int    MCC_CALL MCC_AbortGoHome();
MCC_API int    MCC_CALL MCC_AbortGoHomeEx();

// Get Home Sensor Signal
MCC_API int    MCC_CALL MCC_GetHomeSensorStatus(WORD *pwStatus, WORD wChannel, WORD wCardIndex = 0);


//////////////////////////////////////////////////////////////////////////////
// Position Control

// Set/Get P Gain for Position Control Loop
MCC_API int    MCC_CALL MCC_SetPGain(WORD wGain0, WORD wGain1, WORD wGain2, WORD wGain3, WORD wGain4, WORD wGain5, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_GetPGain(WORD *pwGain0, WORD *pwGain1, WORD *pwGain2, WORD *pwGain3, WORD *pwGain4, WORD *pwGain5, WORD wCardIndex = 0);

// Set/Get maximum pulse speed
MCC_API int    MCC_CALL MCC_SetMaxPulseSpeed(int nPulse0, int nPulse1, int nPulse2, int nPulse3, int nPulse4, int nPulse5, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_GetMaxPulseSpeed(int *pnSpeed0, int *pnSpeed1, int *pnSpeed2, int *pnSpeed3, int *pnSpeed4, int *pnSpeed5, WORD wCardIndex = 0);

// Set/Get maximum pulse acceleration
MCC_API int    MCC_CALL MCC_SetMaxPulseAcc(int nPulseAcc0, int nPulseAcc1, int nPulseAcc2, int nPulseAcc3, int nPulseAcc4, int nPulseAcc5, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_GetMaxPulseAcc(int *pnPulseAcc0, int *pnPulseAcc1, int *pnPulseAcc2, int *pnPulseAcc3, int *pnPulseAcc4, int *pnPulseAcc5, WORD wCardIndex = 0);

// In Postion Operations
MCC_API int    MCC_CALL MCC_SetInPosMode(WORD wMode, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_SetInPosMaxCheckTime(WORD wMaxCheckTime, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_SetInPosSettleTime(WORD wSettleTime, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_EnableInPos(WORD wGroupIndex = 0, DWORD dwAxisMask  = EPCIO_AXIS_ALL);
MCC_API int    MCC_CALL MCC_DisableInPos(WORD wGroupIndex = 0, DWORD dwAxisMask = EPCIO_AXIS_ALL);
MCC_API int    MCC_CALL MCC_SetInPosToleranceEx(double dfTolerance0, double dfTolerance1, double dfTolerance2, double dfTolerance3, double dfTolerance4, double dfTolerance5, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_GetInPosToleranceEx(double *pdfTolerance0, double *pdfTolerance1, double *pdfTolerance2, double *pdfTolerance3, double *pdfTolerance4, double *pdfTolerance5, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_GetInPosStatus(BYTE *pbyInPos0, BYTE *pbyInPos1, BYTE *pbyInPos2, BYTE *pbyInPos3, BYTE *pbyInPos4, BYTE *pbyInPos5, WORD wGroupIndex = 0);

// Tracking Error Detection
MCC_API int    MCC_CALL MCC_EnableTrackError( WORD wGroupIndex = 0, DWORD dwAxisMask = EPCIO_AXIS_ALL);
MCC_API int    MCC_CALL MCC_DisableTrackError(WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_SetTrackErrorLimit(double   dfLimit, char cAxis, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_GetTrackErrorLimit(double *pdfLimit, char cAxis, WORD wGroupIndex = 0);

// Link PCL Interrupt Service Routine
MCC_API int    MCC_CALL MCC_SetPCLRoutine(PCLISR pfnPCLRoutine, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_GetErrorCount(int *pnErrCount0, int *pnErrCount1, int *pnErrCount2, int *pnErrCount3, int *pnErrCount4, int *pnErrCount5, WORD wCardIndex = 0);

// Set Compensation Table
MCC_API int    MCC_CALL MCC_SetCompParam(SYS_COMP_PARAM *pstCompParam, WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_UpdateCompParam();


//////////////////////////////////////////////////////////////////////////////
// Trajectory Planning

// Hold/Continue/Abort Motion
MCC_API int    MCC_CALL MCC_HoldMotion(WORD wGroupIndex = 0, BOOL bAfterCurCmd = 0);
MCC_API int    MCC_CALL MCC_ContiMotion(WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_AbortMotionEx(double dfDecTime, WORD wGroupIndex = 0);

// Single Block motion
MCC_API int    MCC_CALL MCC_EnableSingleBlock(WORD wGroupIndex);
MCC_API int    MCC_CALL MCC_DisableSingleBlock(WORD wGroupIndex);
MCC_API int    MCC_CALL MCC_RunSingleBlock(WORD wGroupIndex);

// Enable/Disable Motion Blending
MCC_API int    MCC_CALL MCC_EnableBlendInstant(WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_DisableBlendInstant(WORD wGroupIndex = 0); 
MCC_API int    MCC_CALL MCC_EnableBlend(WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_DisableBlend(WORD wGroupIndex = 0); 
MCC_API int    MCC_CALL MCC_CheckBlend(WORD wGroupIndex = 0);

// Set Delay Time
MCC_API int    MCC_CALL MCC_DelayMotion(DWORD dwTime, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_CheckDelay(WORD wGroupIndex = 0);
MCC_API void   MCC_CALL MCC_TimeDelay(DWORD dwTime);

// Set/Get Over-Speed Ratio for General Motions
MCC_API double MCC_CALL MCC_OverrideSpeed(double dfRate, WORD wGroupIndex = 0);
MCC_API double MCC_CALL MCC_OverrideSpeedEx(double dfRate, BOOL bInstant = 1, WORD wGroupIndex = 0);
MCC_API double MCC_CALL MCC_GetOverrideRate(WORD wGroupIndex = 0);


//////////////////////////////////////////////////////////////////////////////
// Encoder Control

MCC_API int    MCC_CALL MCC_SetENCRoutineEx(ENCISR_EX pfnEncoderRoutine, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_SetENCFilterClock(WORD wDivider, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_SetENCInputRate(WORD wInputRate, WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_ClearENCCounter(WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_GetENCValue(long *plValue, WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_SetENCGearRate(double dfGearRate, WORD wChannel, WORD wCardIndex = 0);

MCC_API int	   MCC_CALL MCC_SetENCLatchType(WORD wType, WORD wChannel, WORD wCardIndex = 0);
MCC_API int	   MCC_CALL MCC_SetENCLatchSource(WORD wSource, WORD wChannel, WORD wCardIndex = 0);
MCC_API int	   MCC_CALL MCC_GetENCLatchValue(long *plLatchValue, WORD wChannel, WORD wCardIndex = 0);

MCC_API int	   MCC_CALL MCC_EnableENCIndexTrigger(WORD wChannel, WORD wCardIndex = 0);
MCC_API int	   MCC_CALL MCC_DisableENCIndexTrigger(WORD wChannel, WORD wCardIndex = 0);
MCC_API int	   MCC_CALL MCC_GetENCIndexStatus(WORD *pwStatus, WORD wChannel, WORD wCardIndex = 0);

MCC_API int    MCC_CALL MCC_SetENCCompValue(long lValue, WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_EnableENCCompTrigger(WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_DisableENCCompTrigger(WORD wChannel, WORD wCardIndex = 0);


//////////////////////////////////////////////////////////////////////////////
// Timer & Watch Dog Control

MCC_API int    MCC_CALL MCC_SetTimer(DWORD dwValue, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_EnableTimer(WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_DisableTimer(WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_EnableTimerTrigger(WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_DisableTimerTrigger(WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_SetWatchDogTimer(WORD wValue, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_SetWatchDogResetPeriod(DWORD dwValue, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_EnableWatchDogTimer(WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_DisableWatchDogTimer(WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_RefreshWatchDogTimer(WORD wCardIndex = 0);


//////////////////////////////////////////////////////////////////////////////
// Remote Input/Output Control

MCC_API int    MCC_CALL MCC_SetRIORoutineEx(RIOISR_EX pfnRIORoutine, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_EnableRIOSetControl(WORD wSet, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_DisableRIOSetControl(WORD wSet, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_EnableRIOSlaveControl(WORD wSet, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_DisableRIOSlaveControl(WORD wSet, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_GetRIOTransStatus(WORD *pwStatus, WORD wSet, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_GetRIOMasterStatus(WORD *pwStatus, WORD wSet, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_GetRIOSlaveStatus(WORD *pwStatus, WORD wSet, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_GetRIOInputValue(WORD *pwValue, WORD wSet, WORD wPort, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_SetRIOOutputValue(WORD wValue, WORD wSet, WORD wPort, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_SetRIOTransError(WORD wTime, WORD wSet, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_SetRIOTriggerType(WORD wType, WORD wSet, WORD wDigitalInput, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_EnableRIOInputTrigger(WORD wSet, WORD wDigitalInput, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_DisableRIOInputTrigger(WORD wSet, WORD wDigitalInput, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_EnableRIOTransTrigger(WORD wSet, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_DisableRIOTransTrigger(WORD wSet, WORD wCardIndex = 0);


//////////////////////////////////////////////////////////////////////////////
// D/A Converter Control

MCC_API int    MCC_CALL MCC_SetDACOutput(float fVoltage, WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_SetDACTriggerOutput(float fVoltage, WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_SetDACTriggerSource(DWORD dwSource, WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_EnableDACTriggerMode(WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_DisableDACTriggerMode(WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_StartDACConv(WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_StopDACConv(WORD wCardIndex = 0);


//////////////////////////////////////////////////////////////////////////////
// A/D Converter Control

MCC_API int    MCC_CALL MCC_SetADCRoutine(ADCISR pfnADCRoutine, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_SetADCConvType(WORD wConvType, WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_GetADCConvType(WORD *pwConvType, WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_SetADCConvMode(WORD wConvMode, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_GetADCInput(float *pfInput, WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_SetADCSingleChannel(WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_GetADCWorkStatus(WORD *pwSTatus, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_EnableADCConvTrigger(WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_DisableADCConvTrigger(WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_SetADCTagChannel(WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_EnableADCTagTrigger(WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_DisableADCTagTrigger(WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_SetADCCompMask(WORD wMask, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_SetADCCompType(WORD wCompType, WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_SetADCCompValue(float fValue, WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_GetADCCompValue(float *pfValue, WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_EnableADCCompTrigger(WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_DisableADCCompTrigger(WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_EnableADCConvChannel(WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_DisableADCConvChannel(WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_StartADCConv(WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_StopADCConv(WORD wCardIndex = 0);


//////////////////////////////////////////////////////////////////////////////
// Obsolete functions in earlier version of MCCL (existed only for
// compatibility and should not be used anymore)

MCC_API int    MCC_CALL MCC_LineX(double dfX, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_LineY(double dfY, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_LineZ(double dfZ, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_LineU(double dfU, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_LineV(double dfV, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_LineW(double dfW, WORD wGroupIndex = 0);

MCC_API int    MCC_CALL MCC_AbortMotion(WORD wGroupIndex = 0, BOOL bAfterCurCmd = 0);

// Helical motion
MCC_API int    MCC_CALL MCC_HelicaXY_Z(double dfCX, double dfCY, double dfPos, double dfPitch, BYTE byCirDir, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_HelicaYZ_X(double dfCY, double dfCZ, double dfPos, double dfPitch, BYTE byCirDir, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_HelicaZX_Y(double dfCZ, double dfCX, double dfPos, double dfPitch, BYTE byCirDir, WORD wGroupIndex = 0);


//////////////////////////////////////////////////////////////////////////////
// Special functions only for fx

// Customize kinematics transformation rules
MCC_API int    MCC_CALL MCC_SetKinematicTrans(
							FWDKINEFUN pfnFwdKinematics,
							INVKINEFUN pfnInvKinematics,
							DWORD dwExtensionSize = 0,
							INIT_EXTENSION pfnInitExtension = 0
							);

MCC_API int    MCC_CALL MCC_SetPreProcessMotionCommandCallback(
							PRE_PROCESS_MOTION_COMMAND pfnPreProcMotionCmd
							);

// Point-to-Point Motion
MCC_API int    MCC_CALL MCC_PtP_V6(
							double j0,
							double j1,
							double j2,
							double j3,
							double j4,
							double j5,
							WORD wGroupIndex = 0,
							DWORD dwAxisMask = EPCIO_AXIS_ALL
							);

// Linear Motion
MCC_API int    MCC_CALL MCC_Line_V6(
							double x,
							double y,
							double z,
							double rx,
							double ry,
							double rz,
							DWORD posture = 0,
							WORD wGroupIndex = 0,
							DWORD dwAxisMask = EPCIO_AXIS_ALL
							);

// Circular Motion
MCC_API int    MCC_CALL MCC_Arc_V6(
							double x0, // ref. point for x axis
							double y0, // ref. point for y axis
							double z0, // ref. point for z axis
							double x1, // target point for x axis
							double y1, // target point for y axis
							double z1, // target point for z axis
							double rx, // target point for x orientation
							double ry, // target point for y orientation
							double rz, // target point for z orientation
							DWORD posture = 0,
							WORD wGroupIndex = 0,
							DWORD dwAxisMask = EPCIO_AXIS_ALL
							);

MCC_API int    MCC_CALL MCC_ArcTheta_V6(
							double cx,    // center point for x axis
							double cy,    // center point for y axis
							double cz,    // center point for z axis
							double nv0,   // normal vector for x direction
							double nv1,   // normal vector for y direction
							double nv2,   // normal vector for z direction
							double theta, // degree, +/- stands for direction
							double rx,    // target point for x orientation
							double ry,    // target point for y orientation
							double rz,    // target point for z orientation
							DWORD posture = 0,
							WORD wGroupIndex = 0,
							DWORD dwAxisMask = EPCIO_AXIS_ALL
							);

MCC_API int    MCC_CALL MCC_CircleXY_V6(
							double cx, // center point for x axis
							double cy, // center point for y axis
							double rx, // target point for x orientation
							double ry, // target point for y orientation
							double rz, // target point for z orientation
							BYTE byCirDir, // CW or CCW
							DWORD posture = 0,
							WORD wGroupIndex = 0);

MCC_API int    MCC_CALL MCC_CircleYZ_V6(
							double cy, // center point for y axis
							double cz, // center point for z axis
							double rx, // target point for x orientation
							double ry, // target point for y orientation
							double rz, // target point for z orientation
							BYTE byCirDir, // CW or CCW
							DWORD posture = 0,
							WORD wGroupIndex = 0);

MCC_API int    MCC_CALL MCC_CircleZX_V6(
							double cz, // center point for z axis
							double cx, // center point for x axis
							double rx, // target point for x orientation
							double ry, // target point for y orientation
							double rz, // target point for z orientation
							BYTE byCirDir, // CW or CCW
							DWORD posture = 0,
							WORD wGroupIndex = 0);

MCC_API int    MCC_CALL MCC_Circle_V6(
							double x0, // 1st ref. point for x axis
							double y0, // 1st ref. point for y axis
							double z0, // 1st ref. point for z axis
							double x1, // 2nd ref. point for x axis
							double y1, // 2nd ref. point for y axis
							double z1, // 2nd ref. point for z axis
							double rx, // target point for x orientation
							double ry, // target point for y orientation
							double rz, // target point for z orientation
							DWORD posture = 0,
							WORD wGroupIndex = 0,
							DWORD dwAxisMask = EPCIO_AXIS_ALL
							);



//////////////////////////////////////////////////////////////////////////////
// Obsolete functions in earlier version of MCCL (existed only for
// compatibility and should not be used anymore)
MCC_API int    MCC_CALL MCC_SetCycleInterruptRoutine(CYCLE_INTERRUPT_ISR pfnCycleIntRoutine);
MCC_API int    MCC_CALL MCC_SetMachParam(SYS_MACH_PARAM *pstMachParam, WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_GetMachParam(SYS_MACH_PARAM *pstMachParam, WORD wChannel, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_UpdateMachParam();

MCC_API int    MCC_CALL MCC_SetLIORoutine(LIOISR pfnLIORoutine, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_SetENCRoutine(ENCISR pfnEncoderRoutine, WORD wMode, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_SetRIORoutine(RIOISR pfnRIORoutine, WORD wSet, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_SetDACClockDivider(WORD wDivider, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_SetDACUpdateRate(double dfUpdateRate, WORD wCardIndex = 0);

MCC_API int    MCC_CALL MCC_SetADCClockDivider(WORD wDivider, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_SetADCUpdateRate(double dfUpdateRate, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_SetRIOClockDivider(WORD wDivider, WORD wSet, WORD wCardIndex = 0);
MCC_API int    MCC_CALL MCC_SetRIOUpdateRate(double dfUpdateRate, WORD wSet, WORD wCardIndex = 0);

MCC_API int    MCC_CALL MCC_SetInPosCheckTime(WORD wCheckTime, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_GetInPosStableTime(WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_SetInPosTolerance(WORD wTolerance0, WORD wTolerance1, WORD wTolerance2, WORD wTolerance3, WORD wTolerance4, WORD wTolerance5, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_GetInPosTolerance(WORD *pwTolerance0, WORD *pwTolerance1, WORD *pwTolerance2, WORD *pwTolerance3, WORD *pwTolerance4, WORD *pwTolerance5, WORD wGroupIndex = 0);

MCC_API double MCC_CALL MCC_SetOverSpeed(double dfRate, WORD wGroupIndex = 0);
MCC_API double MCC_CALL MCC_GetOverSpeed(WORD wGroupIndex = 0);
MCC_API double MCC_CALL MCC_ChangeFeedSpeed(double dfSpeed, WORD wGroupIndex = 0);
MCC_API double MCC_CALL MCC_ChangeSpeedRate(double dfRate, WORD wGroupIndex = 0);

MCC_API int    MCC_CALL MCC_EnableAccDecAfterIPO(WORD wGroupIndex);
MCC_API int    MCC_CALL MCC_DisableAccDecAfterIPO(WORD wGroupIndex);

//removed function header prototype
MCC_API int    MCC_CALL MCC_SetUnit(int nUnitMode, WORD wGroupIndex);
MCC_API int    MCC_CALL MCC_GetUnit(WORD wGroupIndex);


MCC_API int    MCC_CALL MCC_EnableThreading(WORD wGroupIndex);
MCC_API int    MCC_CALL MCC_DisableThreading(WORD wGroupIndex);
MCC_API int    MCC_CALL MCC_EnableConstantVelocity(WORD wGroupIndex);
MCC_API int    MCC_CALL MCC_DisableConstantVelocity(WORD wGroupIndex);
MCC_API int    MCC_CALL MCC_SetSpindleChannel(int nChannel, WORD wCardIndex);

MCC_API BOOL   MCC_CALL MCC_SetSpindleMode(int nMode, double dfMaxSpeedVoltage, double dfMinSpeedVoltage);//This function will work when Command Mode is setting as OCM_VOLTAGE,
                                                                                //Mode = 0 : Hardware Close Loop Control, wMode = 1 : Software DAC Control, dfMaxSpeedVoltage = 0~10V

MCC_API int    MCC_CALL MCC_EnableFeedbySpindle(int nMode, WORD wGroupIndex);   //Using MCC_SetSpeedRefSpindle to set the nAxis to reference the speed of Spindle each other.
                                                                                //nMode = 0 : Disable, nMode = 1 : Feed by Spindle Command, nMode = 2 : Feed by spindle Encoder

MCC_API int    MCC_CALL MCC_EnableFeedtoSpindle(int nMode, double dfReference, WORD wGroupIndex);//Using MCC_SetSpeedRefSpindle to set the nAxis to reference the speed of Spindle each other.
                                                                                                 //nMode = 0 : Disable, nMode = 1 : Enable, dfReference = Spindle Radius Reference point, wGroupIndex = Spindle Group
MCC_API double MCC_CALL MCC_SetDryRunFeedSpeed(double dfFeedSpeed, WORD wGroupIndex = 0);
MCC_API double MCC_CALL MCC_GetDryRunFeedSpeed(WORD wGroupIndex = 0);

MCC_API int    MCC_CALL MCC_SetSpeedRefSpindle(int nAxis, WORD wGroupIndex);//nAxis = -1 : Disable Ref Spindle, else Enable nAxis reference by Spindle
MCC_API int    MCC_CALL MCC_GetEncoderSpeed(double *dfEncoderSpeed, int nAxis, WORD wGroupIndex);

MCC_API int    MCC_CALL MCC_SetMpgAccTime(double dfAccTime, WORD wGroupIndex = 0);
MCC_API double MCC_CALL MCC_GetMpgAccTime(WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_SetMpgDecTime(double dfDecTime, WORD wGroupIndex = 0);
MCC_API double MCC_CALL MCC_GetMpgDecTime(WORD wGroupIndex = 0);

MCC_API int    MCC_CALL MCC_MpgX(WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_MpgY(WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_MpgZ(WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_MpgU(WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_MpgV(WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_MpgW(WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_MpgXY(WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_MpgYZ(WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_MpgZX(WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_AbortMpg(WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_EnableMpg(WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_DisableMpg(WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_SetMpgChannel(WORD wRefGroupIndex, int nChannel, WORD wCardIndex = 0);

MCC_API int    MCC_CALL MCC_QueueRIOOutputValue(WORD wValue, WORD wSet, WORD wPort, WORD wGroupIndex = 0);

//wValue => bit0 : ServoOff_0, bit1 : ServoOff_1, bit2 : ServoOff_2, bit3 : ServoOff_3,  bit4 : ServoOff_4, bit5 : ServoOff_5, bit6 : PRDY_Off
MCC_API int    MCC_CALL MCC_QueueLIOOutputHigh(WORD wValue, WORD wGroupIndex = 0);

//wValue => bit0 : ServoOn_0, bit1 : ServoOn_1, bit2 : ServoOn_2, bit3 : ServoOn_3,  bit4 : ServoOn_4, bit5 : ServoOn_5,, bit6 : PRDY_On
MCC_API int    MCC_CALL MCC_QueueLIOOutputLow(WORD wValue, WORD wGroupIndex = 0);
MCC_API int    MCC_CALL MCC_PickPlace(double dfSafetyHighUp, double dfRadianUp, double dfRadianDown, double dfSafetyHighDown, double dfEndPointX, double dfEndPointY, WORD   wGroupIndex);

#ifdef __cplusplus
	}
#endif


#endif // _MCCL_FUN_H_