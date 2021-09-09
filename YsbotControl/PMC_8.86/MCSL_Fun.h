#ifndef _MCSL_FUN_H_
#define _MCSL_FUN_H_

#ifdef __cplusplus
	extern "C" {
#endif

#include "mcsl.h"

//////////////////////////////////////////////////////////////////////////////
// Definitions of Calling Convention & Storage-Class Attributes
#ifdef STATIC_LIB
	#define MCS_CALL
	#define MCS_API
#else
	#ifdef MCSL_EXPORTS
		#define MCS_API		__declspec(dllexport)
	#else
		#define MCS_API		__declspec(dllimport)
	#endif // MCSL_EXPORTS

	#define MCS_CALL		__stdcall
#endif // STATIC_LIB


//////////////////////////////////////////////////////////////////////////////
// System Management

// Get Library Version
MCS_API void   MCS_CALL MCS_GetVersion(char *strVersion);

// Create/Close Motion Groups
MCS_API int    MCS_CALL MCS_CreateGroup(int xMapToCh, int yMapToCh, int zMapToCh, int uMapToCh, int vMapToCh, int wMapToCh, int aMapToCh, int bMapToCh, int nCardIndex);
MCS_API int    MCS_CALL MCS_CreateGroupEx(int xMapToCh, int yMapToCh, int zMapToCh, int uMapToCh, int vMapToCh, int wMapToCh, int aMapToCh, int bMapToCh, int nCardIndex, int nMotionQueueSize = MOTION_QUEUE_SIZE);
MCS_API int    MCS_CALL MCS_CloseGroup(int nGroupIndex);
MCS_API int    MCS_CALL MCS_CloseAllGroups();


// Set/Get Mechanism Parameters
MCS_API int    MCS_CALL MCS_SetMacParam(SYS_MAC_PARAM *pstMacParam, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetMacParam(SYS_MAC_PARAM *pstMacParam, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetEncoderConfig(SYS_ENCODER_CONFIG *pstEncoderConfig, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetEncoderConfig(SYS_ENCODER_CONFIG *pstEncoderConfig, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_UpdateParam();
MCS_API double MCS_CALL MCS_GetConvFactor(WORD wChannel, WORD wCardIndex = 0);

// Set/Get Size of Motion Command Queue
MCS_API int    MCS_CALL MCS_SetCmdQueueSize(int nSize, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_GetCmdQueueSize(WORD wGroupIndex = 0);

// Initialize/Close System
MCS_API int    MCS_CALL MCS_InitSystem(int nInterpolateTime, SYS_CARD_CONFIG *pstCardConfig, WORD wCardNo);
MCS_API int    MCS_CALL MCS_InitSystemEx(double dfInterpolateTime, SYS_CARD_CONFIG *pstCardConfig, WORD wCardNo);
MCS_API int    MCS_CALL MCS_InitSimulation(double nInterpolateTime, SYS_CARD_CONFIG *pstCardConfig, WORD wCardNo);
MCS_API int    MCS_CALL MCS_CloseSystem();
MCS_API double MCS_CALL MCS_GetIPOTime();

// Reset MCCL
MCS_API int    MCS_CALL MCS_ResetMotion();

// Enable/Disable dry-run
MCS_API int    MCS_CALL MCS_EnableDryRun();
MCS_API int    MCS_CALL MCS_DisableDryRun();
MCS_API int    MCS_CALL MCS_CheckDryRun();
MCS_API int    MCS_CALL MCS_EnableDryRunEx(WORD wGroupIndex = 0, WORD wMode = 0);//wMode = 0 : Disable Pulse Output, wMode = 1 : Enable Pulse Output
MCS_API int    MCS_CALL MCS_DisableDryRunEx(WORD wGroupIndex = 0);

// Set/Get Max. Feed Speed
MCS_API int    MCS_CALL MCS_SetSysMaxSpeed(double dfMaxSpeed);
MCS_API double MCS_CALL MCS_GetSysMaxSpeed();


//////////////////////////////////////////////////////////////////////////////
// Local I/O Control

// Servo On/Off
MCS_API int    MCS_CALL MCS_SetServoOn(WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetServoOff(WORD wChannel, WORD wCardIndex = 0);

// Enable/Disable Position Ready
MCS_API int    MCS_CALL MCS_EnablePosReady(WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_DisablePosReady(WORD wCardIndex = 0);

// Emergency Stop
MCS_API int    MCS_CALL MCS_GetEmgcStopStatus(WORD *pwStatus, WORD wCardIndex = 0);

// Input Signal Trigger
MCS_API int    MCS_CALL MCS_SetLIORoutine(LIOISR pfnLIORoutine, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetLIOTriggerType(WORD wTriggerType, WORD wPoint, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_EnableLIOTrigger(WORD wPoint, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_DisableLIOTrigger(WORD wPoint, WORD wCardIndex = 0);

// LED On/Off Control
MCS_API int    MCS_CALL MCS_SetLedLightOn(WORD Channel, WORD wCardIndex);
MCS_API int    MCS_CALL MCS_SetLedLightOff(WORD Channel, WORD wCardIndex);
MCS_API int    MCS_CALL MCS_GetLedLightStatus(WORD Channel, WORD *Status, WORD wCardIndex);

MCS_API int    MCS_CALL MCS_SetLIOTrigOutEnable(WORD wEnable, WORD  wPoint, WORD  wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetLIOTrigOutValue(WORD wValue, WORD  wPoint, WORD  wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetLIOTrigOutSource(WORD wSource, WORD  wPoint, WORD  wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetLIOTrigOutPeriod(DWORD dwPeriod, WORD  wCardIndex = 0);

//////////////////////////////////////////////////////////////////////////////
// Coordinate Management

// Set/Get Coordinate Type
MCS_API int    MCS_CALL MCS_SetAbsolute(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_SetIncrease(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_GetCoordType(WORD wGroupIndex = 0);

// Get Current Position & Pulse Position
MCS_API int    MCS_CALL MCS_GetCurRefPos(double *pdfX, double *pdfY, double *pdfZ, double *pdfU, double *pdfV, double *pdfW, double *pdfA, double *pdfB, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_GetCurPos(double *pdfX, double *pdfY, double *pdfZ, double *pdfU, double *pdfV, double *pdfW, double *pdfA, double *pdfB, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_GetPulsePos(long *plX, long *plY, long *plZ, long *plU, long *plV, long *plW, long *plA, long *plB, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_GetPulsePosEx(long long *plX, long long *plY, long long *plZ, long long *plU, long long *plV, long long *plW, long long *plA, long long *plB, WORD wGroupIndex = 0);

// Regard here as origin
MCS_API int    MCS_CALL MCS_DefineOrigin(WORD wAxis, WORD wGroupIndex = 0);

// Change command and actual positions according to specified value
MCS_API int    MCS_CALL MCS_DefinePos(WORD wAxis, double dfPos, WORD wGroupIndex = 0);

// Align command position with actual position
MCS_API int    MCS_CALL MCS_DefinePosHere(WORD wGroupIndex = 0, DWORD dwAxisMask = AXIS_ALL);


//////////////////////////////////////////////////////////////////////////////
// Software Over Travel Check & Hardware Limit Switch Check

// Enable/Disable Hardware Limit Switch Check
MCS_API int    MCS_CALL MCS_EnableLimitSwitchCheck(int nMode = 0);
MCS_API int    MCS_CALL MCS_DisableLimitSwitchCheck();

// Enable/Disable Software Over Travel Check
MCS_API int    MCS_CALL MCS_SetOverTravelCheck(int nOTCheck0, int nOTCheck1, int nOTCheck2, int nOTCheck3, int nOTCheck4, int nOTCheck5, int nOTCheck6, int nOTCheck7, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_GetOverTravelCheck(int *pnOTCheck0, int *pnOTCheck1, int *pnOTCheck2, int *pnOTCheck3, int *pnOTCheck4, int *pnOTCheck5, int *pnOTCheck6, int *pnOTCheck7, WORD wGroupIndex = 0);

// Get Limit Switch Sensor Signal
MCS_API int    MCS_CALL MCS_GetLimitSwitchStatus(WORD *pwStatus, WORD wUpDown, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetLatchedLimitSwitchStatus(WORD *pwStatus, WORD wUpDown, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_ClearLatchedLimitSwitchStatus(WORD wUpDown, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetLDIInputStatus(DWORD *dwLDIStatus, WORD wCardIndex = 0);


//////////////////////////////////////////////////////////////////////////////
// General Motions(Line, Arc, Circle Motions)

// Set/Get Accleration & Deceleration Type
MCS_API int    MCS_CALL MCS_SetAccType(char cAccType, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_GetAccType(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_SetDecType(char cDecType, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_GetDecType(WORD wGroupIndex = 0);

// Set/Get Accleration & Deceleration Time
MCS_API int    MCS_CALL MCS_SetAccTime(double dfAccTime, WORD wGroupIndex = 0);
MCS_API double MCS_CALL MCS_GetAccTime(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_SetDecTime(double dfDecTime, WORD wGroupIndex = 0);
MCS_API double MCS_CALL MCS_GetDecTime(WORD wGroupIndex = 0);
MCS_API double MCS_CALL MCS_GetConstTime(WORD wGroupIndex = 0);

// Set/Get Feed Speed
MCS_API double MCS_CALL MCS_SetInitSpeed(double dfInitSpeed, WORD wGroupIndex = 0);
MCS_API double MCS_CALL MCS_SetFeedSpeed(double dfFeedSpeed, WORD wGroupIndex = 0);
MCS_API double MCS_CALL MCS_GetFeedSpeed(WORD wGroupIndex = 0);
MCS_API double MCS_CALL MCS_GetCurFeedSpeed(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_GetSpeed(double *pdfVel0, double *pdfVel1, double *pdfVel2, double *pdfVel3, double *pdfVel4, double *pdfVel5, double *pdfVel6, double *pdfVel7, WORD wGroupIndex = 0);

// Set/Get Point-to-Point Motion Speed Ratio
MCS_API double MCS_CALL MCS_SetPtPSpeed(double dfRatio, WORD wGroupIndex = 0);
MCS_API double MCS_CALL MCS_GetPtPSpeed(WORD wGroupIndex = 0);

// Point-to-Point Motion
MCS_API int    MCS_CALL MCS_PtP(double dfX0, double dfX1, double dfX2, double dfX3, double dfX4, double dfX5, double dfX6, double dfX7, WORD wGroupIndex = 0, DWORD dwAxisMask = AXIS_ALL);
MCS_API int    MCS_CALL MCS_SyncPtP(double dfX0, double dfX1, double dfX2, double dfX3, double dfX4, double dfX5, double dfX6, double dfX7, WORD wGroupIndex = 0, DWORD dwAxisMask = AXIS_ALL);

MCS_API int    MCS_CALL MCS_PtPX(double dfX, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_PtPY(double dfY, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_PtPZ(double dfZ, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_PtPU(double dfU, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_PtPV(double dfV, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_PtPW(double dfW, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_PtPA(double dfA, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_PtPB(double dfB, WORD wGroupIndex = 0);

// Linear Motion
MCS_API int    MCS_CALL MCS_Line(double dfX0, double dfX1, double dfX2, double dfX3, double dfX4, double dfX5, double dfX6, double dfX7, WORD wGroupIndex = 0, DWORD dwAxisMask = AXIS_ALL);

// Arc Motion
MCS_API int    MCS_CALL MCS_ArcXYZ(double dfRX0, double dfRX1, double dfRX2, double dfX0, double dfX1, double dfX2, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_ArcXY(double dfRX0, double dfRX1, double dfX0, double dfX1, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_ArcYZ(double dfRX1, double dfRX2, double dfX1, double dfX2, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_ArcZX(double dfRX2, double dfRX0, double dfX2, double dfX0, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_ArcXYZ_Aux(double dfRX0, double dfRX1, double dfRX2, double dfX0, double dfX1, double dfX2, double dfX3, double dfX4, double dfX5, double dfX6, double dfX7, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_ArcXY_Aux(double dfRX0, double dfRX1, double dfX0, double dfX1, double dfX3, double dfX4, double dfX5, double dfX6, double dfX7, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_ArcYZ_Aux(double dfRX1, double dfRX2, double dfX1, double dfX2, double dfX3, double dfX4, double dfX5, double dfX6, double dfX7, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_ArcZX_Aux(double dfRX2, double dfRX0, double dfX2, double dfX0, double dfX3, double dfX4, double dfX5, double dfX6, double dfX7, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_ArcThetaXY(double dfCX, double dfCY, double dfTheta, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_ArcThetaYZ(double dfCY, double dfCZ, double dfTheta, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_ArcThetaZX(double dfCZ, double dfCX, double dfTheta, WORD wGroupIndex = 0);

// Circular Motion
MCS_API int    MCS_CALL MCS_CircleXY(double dfCX, double dfCY, BYTE byCirDir, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_CircleYZ(double dfCY, double dfCZ, BYTE byCirDir, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_CircleZX(double dfCZ, double dfCX, BYTE byCirDir, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_CircleXY_Aux(double dfCX, double dfCY, double dfU, double dfV, double dfW, double dfA, double dfB, BYTE byCirDir, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_CircleYZ_Aux(double dfCY, double dfCZ, double dfU, double dfV, double dfW, double dfA, double dfB, BYTE byCirDir, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_CircleZX_Aux(double dfCZ, double dfCX, double dfU, double dfV, double dfW, double dfA, double dfB, BYTE byCirDir, WORD wGroupIndex = 0);

MCS_API int    MCS_CALL MCS_HelicalXY_Z( double dfCX, double dfCY, double dfPitch, double dfTheta, WORD   wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_HelicalYZ_X( double dfCY, double dfCZ, double dfPitch, double dfTheta, WORD   wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_HelicalZX_Y( double dfCZ, double dfCX, double dfPitch, double dfTheta, WORD   wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_HelicalXY_Z_Aux( double dfCX, double dfCY, double dfPitch, double dfU, double dfV, double dfW, double dfA, double dfB, double dfTheta, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_HelicalYZ_X_Aux( double dfCY, double dfCZ, double dfPitch, double dfU, double dfV, double dfW, double dfA, double dfB, double dfTheta, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_HelicalZX_Y_Aux( double dfCZ, double dfCX, double dfPitch, double dfU, double dfV, double dfW, double dfA, double dfB, double dfTheta, WORD wGroupIndex = 0);

//////////////////////////////////////////////////////////////////////////////
// Jog Motion

MCS_API int    MCS_CALL MCS_JogPulse(int nPulse, char cAxis, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_JogSpace(double dfOffset, double dfRatio, char cAxis, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_JogConti(int nDir, double dfRatio, char cAxis, WORD wGroupIndex = 0);

//////////////////////////////////////////////////////////////////////////////
// Point-to-Point Motion

// Set/Get accleration & deceleration types
MCS_API int    MCS_CALL MCS_SetPtPAccType(char cAccType0, char cAccType1, char cAccType2, char cAccType3, char cAccType4, char cAccType5, char cAccType6, char cAccType7,  WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_GetPtPAccType(char *pcAccType0, char *pcAccType1, char *pcAccType2, char *pcAccType3, char *pcAccType4, char *pcAccType5, char *pcAccType6, char *pcAccType7, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_SetPtPDecType(char cDecType0, char cDecType1, char cDecType2, char cDecType3, char cDecType4, char cDecType5, char cDecType6, char cDecType7, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_GetPtPDecType(char *pcDecType0, char *pcDecType1, char *pcDecType2, char *pcDecType3, char *pcDecType4, char *pcDecType5, char *pcDecType6, char *pcDecType7, WORD wGroupIndex = 0);

// Set/Get accleration & deceleration times
MCS_API int    MCS_CALL MCS_SetPtPAccTime(double dfAccTime0, double dfAccTime1, double dfAccTime2, double dfAccTime3, double dfAccTime4, double dfAccTime5, double dfAccTime6, double dfAccTime7, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_GetPtPAccTime(double *pdfAccTime0, double *pdfAccTime1, double *pdfAccTime2, double *pdfAccTime3, double *pdfAccTime4, double *pdfAccTime5, double *pdfAccTime6, double *pdfAccTime7, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_SetPtPDecTime(double dfDecTime0, double dfDecTime1, double dfDecTime2, double dfDecTime3, double dfDecTime4, double dfDecTime5, double dfDecTime6, double dfDecTime7, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_GetPtPDecTime(double *pdfDecTime0, double *pdfDecTime1, double *pdfDecTime2, double *pdfDecTime3, double *pdfDecTime4, double *pdfDecTime5, double *pdfDecTime6, double *pdfDecTime7, WORD wGroupIndex = 0);

//////////////////////////////////////////////////////////////////////////////
// Motion Status

// Get Current Motion Status
MCS_API int    MCS_CALL MCS_GetMotionStatus(WORD wGroupIndex = 0);

// Get Current Executing Motion Command
MCS_API int    MCS_CALL MCS_GetCurCommand(COMMAND_INFO *pstCurCommand, WORD wGroupIndex = 0);

// Get Motion Command Stock Count
MCS_API int    MCS_CALL MCS_GetCommandCount(int *pnCmdCount, WORD wGroupIndex = 0);

MCS_API int    MCS_CALL MCS_ResetCommandIndex(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_SetCommandIndex(WORD wIndex, BOOL bManualIndex, WORD wGroupIndex = 0);

// Set/Get Hardware Pulse Stock Count
MCS_API int    MCS_CALL MCS_SetMaxPulseStockNum(int nMaxStockNum);
MCS_API int    MCS_CALL MCS_GetMaxPulseStockNum();
MCS_API int    MCS_CALL MCS_GetCurPulseStockCount(WORD *pwStockCount, WORD wChannel, WORD wCardIndex = 0);

// Get/Clear Error Code
MCS_API int    MCS_CALL MCS_GetErrorCode(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_ClearError(WORD wGroupIndex = 0);


//////////////////////////////////////////////////////////////////////////////
// Go Home

// Set/Get configuration of the homing process
MCS_API int    MCS_CALL MCS_SetHomeConfig(SYS_HOME_CONFIG *pstHomeConfig, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetHomeConfig(SYS_HOME_CONFIG *pstHomeConfig, WORD wChannel, WORD wCardIndex = 0);

// Operations
MCS_API int    MCS_CALL MCS_Home(int nOrder0, int nOrder1, int nOrder2, int nOrder3, int nOrder4, int nOrder5, int nOrder6, int nOrder7, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetGoHomeStatus();
MCS_API int    MCS_CALL MCS_AbortGoHome();
MCS_API int    MCS_CALL MCS_AbortGoHomeEx();

// Get Home Sensor Signal
MCS_API int    MCS_CALL MCS_GetHomeSensorStatus(WORD *pwStatus, WORD wChannel, WORD wCardIndex = 0);


//////////////////////////////////////////////////////////////////////////////
// Position Control

// Set/Get P Gain for Position Control Loop
MCS_API int    MCS_CALL MCS_SetPGain(WORD wGain0, WORD wGain1, WORD wGain2, WORD wGain3, WORD wGain4, WORD wGain5, WORD wGain6, WORD wGain7, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetPGain(WORD *pwGain0, WORD *pwGain1, WORD *pwGain2, WORD *pwGain3, WORD *pwGain4, WORD *pwGain5, WORD *pwGain6, WORD *pwGain7, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetIGain(WORD wGain0, WORD wGain1, WORD wGain2, WORD wGain3, WORD wGain4, WORD wGain5, WORD wGain6, WORD wGain7, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetIGain(WORD *pwGain0, WORD *pwGain1, WORD *pwGain2, WORD *pwGain3, WORD *pwGain4, WORD *pwGain5, WORD *pwGain6, WORD *pwGain7, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetDGain(WORD wGain0, WORD wGain1, WORD wGain2, WORD wGain3, WORD wGain4, WORD wGain5, WORD wGain6, WORD wGain7, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetDGain(WORD *pwGain0, WORD *pwGain1, WORD *pwGain2, WORD *pwGain3, WORD *pwGain4, WORD *pwGain5, WORD *pwGain6, WORD *pwGain7, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetFGain(WORD wGain0, WORD wGain1, WORD wGain2, WORD wGain3, WORD wGain4, WORD wGain5, WORD wGain6, WORD wGain7, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetFGain(WORD *pwGain0, WORD *pwGain1, WORD *pwGain2, WORD *pwGain3, WORD *pwGain4, WORD *pwGain5, WORD *pwGain6, WORD *pwGain7, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetIGainClockDivider(DWORD dwClockDivider, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetDGainClockDivider(DWORD dwClockDivider, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetErrorCount(int *pnErrCount0, int *pnErrCount1, int *pnErrCount2, int *pnErrCount3, int *pnErrCount4, int *pnErrCount5, int *pnErrCount6, int *pnErrCount7, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetErrorCountThreshold(WORD wChannel, WORD wPlusThreshold, WORD wMinusThreshold, WORD wCardIndex = 0);

// Set/Get maximum pulse speed
MCS_API int    MCS_CALL MCS_SetMaxPulseSpeed(int nPulse0, int nPulse1, int nPulse2, int nPulse3, int nPulse4, int nPulse5, int nPulse6, int nPulse7, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetMaxPulseSpeed(int *pnSpeed0, int *pnSpeed1, int *pnSpeed2, int *pnSpeed3, int *pnSpeed4, int *pnSpeed5, int *pnSpeed6, int *pnSpeed7, WORD wCardIndex = 0);

// Set/Get maximum pulse acceleration
MCS_API int    MCS_CALL MCS_SetMaxPulseAcc(int nPulseAcc0, int nPulseAcc1, int nPulseAcc2, int nPulseAcc3, int nPulseAcc4, int nPulseAcc5, int nPulseAcc6, int nPulseAcc7, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetMaxPulseAcc(int *pnPulseAcc0, int *pnPulseAcc1, int *pnPulseAcc2, int *pnPulseAcc3, int *pnPulseAcc4, int *pnPulseAcc5, int *pnPulseAcc6, int *pnPulseAcc7, WORD wCardIndex = 0);

// In Postion Operations
MCS_API int    MCS_CALL MCS_SetInPosMode(WORD wMode, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_SetInPosMaxCheckTime(WORD wMaxCheckTime, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_SetInPosSettleTime(WORD wSettleTime, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_EnableInPos(WORD wGroupIndex = 0, DWORD dwAxisMask  = AXIS_ALL);
MCS_API int    MCS_CALL MCS_DisableInPos(WORD wGroupIndex = 0, DWORD dwAxisMask = AXIS_ALL);
MCS_API int    MCS_CALL MCS_SetInPosToleranceEx(double dfTolerance0, double dfTolerance1, double dfTolerance2, double dfTolerance3, double dfTolerance4, double dfTolerance5, double dfTolerance6, double dfTolerance7, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_GetInPosToleranceEx(double *pdfTolerance0, double *pdfTolerance1, double *pdfTolerance2, double *pdfTolerance3, double *pdfTolerance4, double *pdfTolerance5, double *pdfTolerance6, double *pdfTolerance7, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_GetInPosStatus(BYTE *pbyInPos0, BYTE *pbyInPos1, BYTE *pbyInPos2, BYTE *pbyInPos3, BYTE *pbyInPos4, BYTE *pbyInPos5, BYTE *pbyInPos6, BYTE *pbyInPos7, WORD wGroupIndex = 0);

// Tracking Error Detection
MCS_API int    MCS_CALL MCS_EnableTrackError( WORD wGroupIndex = 0, DWORD dwAxisMask = AXIS_ALL);
MCS_API int    MCS_CALL MCS_DisableTrackError(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_SetTrackErrorLimit(double   dfLimit, char cAxis, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_GetTrackErrorLimit(double *pdfLimit, char cAxis, WORD wGroupIndex = 0);

// Link PCL Interrupt Service Routine
MCS_API int    MCS_CALL MCS_SetPCLRoutine(PCLISR pfnPCLRoutine, WORD wCardIndex = 0);

// Set Compensation Table
MCS_API int    MCS_CALL MCS_SetCompParam(SYS_COMP_PARAM *pstCompParam, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_UpdateCompParam();


//////////////////////////////////////////////////////////////////////////////
// Trajectory Planning

// Hold/Continue/Abort Motion
MCS_API int    MCS_CALL MCS_HoldMotion(WORD wGroupIndex = 0, BOOL bBlockEnd = 0);
MCS_API int    MCS_CALL MCS_ContiMotion(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_AbortMotion(WORD wGroupIndex = 0, BOOL bBlockEnd = 0);
MCS_API int    MCS_CALL MCS_AbortMotionEx(double dfDecTime, WORD wGroupIndex = 0);

// Single Block motion
MCS_API int    MCS_CALL MCS_EnableSingleBlock(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_DisableSingleBlock(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_RunSingleBlock(WORD wGroupIndex = 0);

// Enable/Disable Motion Blending
MCS_API int    MCS_CALL MCS_EnableBlendInstant(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_DisableBlendInstant(WORD wGroupIndex = 0); 
MCS_API int    MCS_CALL MCS_EnableBlend(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_DisableBlend(WORD wGroupIndex = 0); 
MCS_API int    MCS_CALL MCS_CheckBlend(WORD wGroupIndex = 0);

// Set Delay Time
MCS_API int    MCS_CALL MCS_DelayMotion(DWORD dwTime, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_CheckDelay(WORD wGroupIndex = 0);
MCS_API void   MCS_CALL MCS_TimeDelay(DWORD dwTime);

// Set/Get Over-Speed Ratio for General Motions
MCS_API double MCS_CALL MCS_OverrideSpeed(double dfRate, WORD wGroupIndex = 0);
MCS_API double MCS_CALL MCS_OverrideSpeedEx(double dfRate, BOOL bInstant = 1, WORD wGroupIndex = 0);
MCS_API double MCS_CALL MCS_GetOverrideRate(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_FixSpeed(BOOL bFix, WORD wGroupIndex = 0);


//////////////////////////////////////////////////////////////////////////////
// Encoder Control

MCS_API int    MCS_CALL MCS_SetENCRoutine(ENCISR pfnEncoderRoutine, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetENCFilterClock(WORD wDivider, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetENCInputRate(WORD wInputRate, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_ClearENCCounter(WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetENCValue(long *plValue, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetENCValueEx(long long *pllValue, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetENCValue(long lValue, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetENCGearRate(double dfGearRate, WORD wChannel, WORD wCardIndex = 0);

MCS_API int	   MCS_CALL MCS_SetENCLatchType(WORD wType, WORD wChannel, WORD wCardIndex = 0);
MCS_API int	   MCS_CALL MCS_SetENCLatchSource(DWORD wSource, WORD wChannel, WORD wCardIndex = 0);
MCS_API int	   MCS_CALL MCS_GetENCLatchValue(long *plLatchValue, WORD wChannel, WORD wCardIndex = 0);

MCS_API int	   MCS_CALL MCS_EnableENCIndexTrigger(WORD wChannel, WORD wCardIndex = 0);
MCS_API int	   MCS_CALL MCS_DisableENCIndexTrigger(WORD wChannel, WORD wCardIndex = 0);
MCS_API int	   MCS_CALL MCS_GetENCIndexStatus(WORD *pwStatus, WORD wChannel, WORD wCardIndex = 0);

MCS_API int    MCS_CALL MCS_SetENCCompValue(long lValue, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetENCCompValue(long *lValue, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_EnableENCCompTrigger(WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_DisableENCCompTrigger(WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetENCCompStockCount(WORD *wCount, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_EraseENCCompValue(WORD wChannel, WORD wCardIndex = 0);

MCS_API int    MCS_CALL MCS_SetMPGRoutine(MPGISR pfnMpgRoutine, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetMPGInputRate(WORD wInputRate, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetMPGInputFormat(WORD wInputFormat, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetMPGCompValue(long lValue, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetMPGCompValue(long *lValue, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_EraseMPGCompValue(WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetMPGCompStockCount(WORD *wCount, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetMPGFilterClock(long lValue, WORD wChannel, WORD wCardIndex = 0);

MCS_API int    MCS_CALL MCS_EnableMPGCompTrigger(WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_DisableMPGCompTrigger(WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_ClearMPGCounter(WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetMPGValue(long *plValue, WORD wChannel, WORD wCardIndex = 0);
MCS_API int	   MCS_CALL MCS_GetMPGLatchValue(long *plLatchValue, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_StartMPG(WORD wChannel, WORD wCardIndex);
MCS_API int    MCS_CALL MCS_StopMPG(WORD wChannel, WORD wCardIndex);

//////////////////////////////////////////////////////////////////////////////
// Timer & Watch Dog Control
MCS_API int    MCS_CALL MCS_SetTMRRoutine(TMRISR pfnLIORoutine, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetTimer(DWORD dwValue, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_EnableTimer(WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_DisableTimer(WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_EnableTimerTrigger(WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_DisableTimerTrigger(WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetWatchDogTimer(DWORD wValue, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetWatchDogResetPeriod(DWORD dwValue, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_EnableWatchDogTimer(WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_DisableWatchDogTimer(WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_RefreshWatchDogTimer(WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_QueueLIOOutputValue(WORD wBitMask, WORD wValue, double dfDelayTime = 0, WORD wGroupIndex = 0);


//////////////////////////////////////////////////////////////////////////////
// Remote Input/Output Control
MCS_API int    MCS_CALL MCS_SetRIORoutine(RIOISR pfnRIORoutine, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_EnableRIOSetControl(WORD wSet, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_DisableRIOSetControl(WORD wSet, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_EnableRIOSlaveControl(WORD wSet, WORD wSlave = 0, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_DisableRIOSlaveControl(WORD wSet, WORD wSlave = 0, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetRIOTransStatus(WORD *pwStatus, WORD wSet, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetRIOMasterStatus(WORD *pwStatus, WORD wSet, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetRIOSlaveStatus(WORD *pwStatus, WORD wSet, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetRIOSlaveFail(DWORD *pdwStatus, WORD wSet, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetRIOInputValue(WORD wValue, WORD wSet, WORD wPort, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetRIOInputValue(WORD *pwValue, WORD wSet, WORD wPort, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetRIOOutputValue(WORD wValue, WORD wSet, WORD wPort, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetRIOOutputValue(WORD *pwValue, WORD wSet, WORD wPort, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetRIOTransError(WORD wTime, WORD wSet, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetRIOTriggerType(WORD wType, WORD wSet, WORD wDigitalInput, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_EnableRIOInputTrigger(WORD wSet, WORD wDigitalInput, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_DisableRIOInputTrigger(WORD wSet, WORD wDigitalInput, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_EnableRIOTransTrigger(WORD wSet, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_DisableRIOTransTrigger(WORD wSet, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_QueueRIOOutputValue(WORD wBitMask, WORD wValue, WORD wSet, WORD wPort, double dfDelayTime = 0, WORD wGroupIndex = 0);

MCS_API int    MCS_CALL MCS_SetARIOUpdateRate(double dfUpdateRate, WORD wSet, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetARIOClockDivider(WORD wDivider, WORD wSet, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_EnableARIOSlaveControl(WORD wSet, WORD wSlave = 0, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_DisableARIOSlaveControl(WORD wSet, WORD wSlave = 0, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetARIOTransStatus(WORD *pwStatus, WORD wSet, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetARIOMasterStatus(WORD *pwStatus, WORD wSet, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetARIOSlaveStatus(WORD *pwStatus, WORD wSet, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetARIOSlaveFail(DWORD *pdwStatus, WORD wSet, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetARIOInputValue(WORD *pwValue, WORD wSet, WORD wPort, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetARIOOutputValue(WORD wValue, WORD wSet, WORD wPort, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetARIOOutputValue(WORD *pwValue, WORD wSet, WORD wPort, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetARIOInputValue(WORD wValue, WORD wSet, WORD wPort, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_QueueARIOOutputValue(WORD wBitMask, WORD wValue, WORD wSet, WORD wPort, double dfDelayTime = 0, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_SetARIONode2NodeDelay(DWORD dwDelay, WORD wCardIndex = 0);
//////////////////////////////////////////////////////////////////////////////
// GPIO Converter Control
MCS_API int    MCS_CALL MCS_SetGPIOInputValue(DWORD dwValue, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetGPIOInputValue(DWORD *pdwValue, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetAuxGPIOInputValue(DWORD *pdwValue, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetGPIOOutputValue(DWORD dwValue, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetGPIOOutputValue(DWORD *pdwValue, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetGPIOOutputEnable(WORD wByte, WORD wEnable, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetGPIOGateEnable(WORD wEnable, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_QueueGPIOOutputValue(DWORD dwBitMask, DWORD dwValue, double dfDelayTime = 0, WORD wGroupIndex = 0);


MCS_API int    MCS_CALL MCS_SetGPIOTrigOutEnable(WORD wEnable, WORD  wPoint, WORD  wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetGPIOTrigOutValue(WORD wValue, WORD  wPoint, WORD  wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetGPIOTrigOutSource(WORD wSource, WORD  wPoint, WORD  wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetGPIOTrigOutPeriod(DWORD dwPeriod, WORD  wCardIndex = 0);

MCS_API int    MCS_CALL MCS_SetGPIORoutine(GPIOISR pfnGPIORoutine, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_EnableGPIOTrigger(WORD wPoint, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_DisableGPIOTrigger(WORD wPoint, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetGPIOTriggerType(WORD wTriggerType, WORD wPoint, WORD wCardIndex = 0);

MCS_API int    MCS_CALL MCS_SetAlarmRoutine(ALMISR pfnAlarmRoutine, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_EnableAlarmTrigger(WORD wPoint, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_DisableAlarmTrigger(WORD wPoint, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetAlarmTriggerType(WORD wTriggerType, WORD wPoint, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetAlarmInputValue(DWORD *pdwValue, WORD wCardIndex = 0);

//////////////////////////////////////////////////////////////////////////////
// D/A Converter Control

MCS_API int    MCS_CALL MCS_SetDACOutput(float fVoltage, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetDACTriggerOutput(float fVoltage, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetDACTriggerSource(DWORD dwSource, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_EnableDACTriggerMode(WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_DisableDACTriggerMode(WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_StartDACConv(WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_StopDACConv(WORD wCardIndex = 0);


//////////////////////////////////////////////////////////////////////////////
// A/D Converter Control

MCS_API int    MCS_CALL MCS_SetADCRoutine(ADCISR pfnADCRoutine, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetADCConvType(WORD wConvType, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetADCConvType(WORD *pwConvType, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetADCConvMode(WORD wConvMode, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetADCInput(float *pfInput, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetADCInput(float fVoltage, WORD wChannel, WORD wCardIndex = 0);//for Simulation Mode
MCS_API int    MCS_CALL MCS_SetADCSingleChannel(WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetADCWorkStatus(WORD *pwSTatus, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_EnableADCConvTrigger(WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_DisableADCConvTrigger(WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetADCTagChannel(WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_EnableADCTagTrigger(WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_DisableADCTagTrigger(WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetADCCompMask(WORD wMask, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetADCCompType(WORD wCompType, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetADCCompValue(float fValue, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetADCCompValue(float *pfValue, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_EnableADCCompTrigger(WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_DisableADCCompTrigger(WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_EnableADCConvChannel(WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_DisableADCConvChannel(WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_StartADCConv(WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_StopADCConv(WORD wCardIndex = 0);


//////////////////////////////////////////////////////////////////////////////
// Obsolete functions in earlier version of MCCL (existed only for
// compatibility and should not be used anymore)

MCS_API int    MCS_CALL MCS_LineX(double dfX, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_LineY(double dfY, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_LineZ(double dfZ, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_LineU(double dfU, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_LineV(double dfV, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_LineW(double dfW, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_LineA(double dfA, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_LineB(double dfB, WORD wGroupIndex = 0);


// Helical motion
MCS_API int    MCS_CALL MCS_HelicaXY_Z(double dfCX, double dfCY, double dfPos, double dfPitch, BYTE byCirDir, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_HelicaYZ_X(double dfCY, double dfCZ, double dfPos, double dfPitch, BYTE byCirDir, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_HelicaZX_Y(double dfCZ, double dfCX, double dfPos, double dfPitch, BYTE byCirDir, WORD wGroupIndex = 0);

MCS_API int    MCS_CALL MCS_ConeXY_Z(double dfCX, double dfCY, double dfPos, double dfPitch, double CenterRate, BYTE byCirDir, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_ConeYZ_X(double dfCY, double dfCZ, double dfPos, double dfPitch, double CenterRate, BYTE byCirDir, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_ConeZX_Y(double dfCZ, double dfCX, double dfPos, double dfPitch, double CenterRate, BYTE byCirDir, WORD wGroupIndex = 0);


//////////////////////////////////////////////////////////////////////////////
// Special functions only for Robot Control

// Customize kinematics transformation rules
MCS_API int    MCS_CALL MCS_SetKinematicTrans(
							FWDKINEFUN pfnFwdKinematics,
							INVKINEFUN pfnInvKinematics,
							DWORD dwExtensionSize = 0,
							INIT_EXTENSION pfnInitExtension = 0,
							int nGroup = 0
							);

MCS_API int    MCS_CALL MCS_SetPreProcessMotionCommandCallback(
							PRE_PROCESS_MOTION_COMMAND pfnPreProcMotionCmd,
							int nGroup = 0
							);

// Point-to-Point Motion
MCS_API int    MCS_CALL MCS_PtP_V6(
							double j0,
							double j1,
							double j2,
							double j3,
							double j4,
							double j5,
							double j6,
							double j7,
							WORD wGroupIndex = 0,
							DWORD dwAxisMask = AXIS_ALL
							);

// Linear Motion
MCS_API int    MCS_CALL MCS_Line_V6(
							double x,
							double y,
							double z,
							double rx,
							double ry,
							double rz,
							double a,
							double b,
							DWORD posture = 0,
							WORD wGroupIndex = 0,
							DWORD dwAxisMask = AXIS_ALL
							);

// Circular Motion
MCS_API int    MCS_CALL MCS_Arc_V6(
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
							DWORD dwAxisMask = AXIS_ALL
							);

MCS_API int    MCS_CALL MCS_ArcTheta_V6(
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
							DWORD dwAxisMask = AXIS_ALL
							);

MCS_API int    MCS_CALL MCS_CircleXY_V6(
							double cx, // center point for x axis
							double cy, // center point for y axis
							double rx, // target point for x orientation
							double ry, // target point for y orientation
							double rz, // target point for z orientation
							BYTE byCirDir, // CW or CCW
							DWORD posture = 0,
							WORD wGroupIndex = 0);

MCS_API int    MCS_CALL MCS_CircleYZ_V6(
							double cy, // center point for y axis
							double cz, // center point for z axis
							double rx, // target point for x orientation
							double ry, // target point for y orientation
							double rz, // target point for z orientation
							BYTE byCirDir, // CW or CCW
							DWORD posture = 0,
							WORD wGroupIndex = 0);

MCS_API int    MCS_CALL MCS_CircleZX_V6(
							double cz, // center point for z axis
							double cx, // center point for x axis
							double rx, // target point for x orientation
							double ry, // target point for y orientation
							double rz, // target point for z orientation
							BYTE byCirDir, // CW or CCW
							DWORD posture = 0,
							WORD wGroupIndex = 0);

MCS_API int    MCS_CALL MCS_Circle_V6(
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
							DWORD dwAxisMask = AXIS_ALL
							);



//////////////////////////////////////////////////////////////////////////////
// Obsolete functions in earlier version of MCCL (existed only for
// compatibility and should not be used anymore)
MCS_API int    MCS_CALL MCS_SetCycleInterruptRoutine(CYCLE_INTERRUPT_ISR pfnCycleIntRoutine);
MCS_API int    MCS_CALL MCS_SetMachParam(SYS_MACH_PARAM *pstMachParam, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_GetMachParam(SYS_MACH_PARAM *pstMachParam, WORD wChannel, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_UpdateMachParam();

MCS_API int    MCS_CALL MCS_SetDACClockDivider(WORD wDivider, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetDACUpdateRate(double dfUpdateRate, WORD wCardIndex = 0);

MCS_API int    MCS_CALL MCS_SetADCClockDivider(WORD wDivider, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetADCUpdateRate(double dfUpdateRate, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetRIOClockDivider(WORD wDivider, WORD wSet, WORD wCardIndex = 0);
MCS_API int    MCS_CALL MCS_SetRIOUpdateRate(double dfUpdateRate, WORD wSet, WORD wCardIndex = 0);

MCS_API int    MCS_CALL MCS_SetInPosCheckTime(WORD wCheckTime, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_GetInPosStableTime(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_SetInPosTolerance(WORD wTolerance0, WORD wTolerance1, WORD wTolerance2, WORD wTolerance3, WORD wTolerance4, WORD wTolerance5, WORD wTolerance6, WORD wTolerance7, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_GetInPosTolerance(WORD *pwTolerance0, WORD *pwTolerance1, WORD *pwTolerance2, WORD *pwTolerance3, WORD *pwTolerance4, WORD *pwTolerance5, WORD *pwTolerance6, WORD *pwTolerance7, WORD wGroupIndex = 0);

MCS_API double MCS_CALL MCS_SetOverSpeed(double dfRate, WORD wGroupIndex = 0);
MCS_API double MCS_CALL MCS_GetOverSpeed(WORD wGroupIndex = 0);
MCS_API double MCS_CALL MCS_ChangeFeedSpeed(double dfSpeed, WORD wGroupIndex = 0);

MCS_API int    MCS_CALL MCS_EnableAccDecAfterIPO(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_DisableAccDecAfterIPO(WORD wGroupIndex = 0);

//removed function header prototype
MCS_API int    MCS_CALL MCS_SetUnit(int nUnitMode, WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_GetUnit(WORD wGroupIndex = 0);

MCS_API int    MCS_CALL MCS_EnableMovingAverage(WORD wGroupIndex);
MCS_API int    MCS_CALL MCS_DisableMovingAverage(WORD wGroupIndex);

MCS_API int    MCS_CALL MCS_SetMpgAccTime(double dfAccTime, WORD wGroupIndex = 0);
MCS_API double MCS_CALL MCS_GetMpgAccTime(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_SetMpgDecTime(double dfDecTime, WORD wGroupIndex = 0);
MCS_API double MCS_CALL MCS_GetMpgDecTime(WORD wGroupIndex = 0);

MCS_API int    MCS_CALL MCS_Mpg(WORD wGroupIndex = 0, DWORD dwAxisMask = AXIS_ALL);
MCS_API int    MCS_CALL MCS_MpgX(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_MpgY(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_MpgZ(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_MpgU(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_MpgV(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_MpgW(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_MpgA(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_MpgB(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_MpgXY(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_MpgYZ(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_MpgZX(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_AbortMpg(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_EnableMpg(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_DisableMpg(WORD wGroupIndex = 0);
MCS_API int    MCS_CALL MCS_SetMpgChannel(WORD wRefGroupIndex, WORD wChannel, WORD wCardIndex = 0);
// Custom Motion

MCS_API int    MCS_CALL MCS_CustomMotionEx(
                        CUSTOM_START_MOTION   pfnStartMotion,
                        CUSTOM_INTERPOLATION  pfnInterpolation,
                        CUSTOM_CLEANUP        pfnCleanUp,
                        CUSTOM_BLENDING_START pfnBlendingStart,
                        CUSTOM_BLENDING       pfnBlending,
                        CUSTOM_BLENDING_END   pfnBlendingEnd,
                        void* pvBuffer,
                        BOOL bOverrideMotion = 0,
                        WORD wGroupIndex =0,
                        DWORD dwAxisMask =AXIS_ALL);

MCS_API int    MCS_CALL MCS_CustomMotion(
                        CUSTOM_START_MOTION pfnStartMotion,
                        CUSTOM_INTERPOLATION pfnInterpolation,
                        CUSTOM_CLEANUP pfnCleanUp,
                        void* pvBuffer,
                        WORD wGroupIndex = 0,
                        DWORD dwAxisMask = AXIS_ALL);

MCS_API int    MCS_CALL MCS_InfiniteMotion(
							INFINITE_START_MOTION pfnStartMotion,
							INFINITE_INTERPOLATION pfnInterpolation,
							INFINITE_CLEANUP pfnCleanUp,
							void* pvBuffer,
							BOOL bOverrideMotion /*= FALSE*/,
							WORD wGroupIndex = 0,
							DWORD dwAxisMask = AXIS_ALL);

#ifdef __cplusplus
	}
#endif


#endif // _MCSL_FUN_H_