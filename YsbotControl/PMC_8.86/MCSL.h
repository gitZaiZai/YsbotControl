#ifndef _MCSL_H_
#define _MCSL_H_

#ifdef __cplusplus
	extern "C" {
#endif
		
/////////////////////////////////////////////////////////////////////
// Variable definitions

// Comment the next line if the platform is non-Windows
#define UNDER_WINDOWS

// Comment the next line if supports single card only
#define MULTI_CARD_SUPPORT


#ifdef UNDER_WINDOWS
	#include <windows.h>
#else
	#undef DWORD
	#undef WORD
	#undef BYTE
	#undef BOOL

    #define STATIC_LIB 
	typedef unsigned long				DWORD;
	typedef unsigned short				WORD;
	typedef unsigned char				BYTE;
	typedef int							BOOL;
#endif

#ifdef STATIC_LIB
	#define MCS_CALL
	#define MCS_API
#else
	#define MCS_CALL		__stdcall
#endif // STATIC_LIB

#ifdef MULTI_CARD_SUPPORT
	#define MAX_CARD_NUM				12
#else
	#define MAX_CARD_NUM				1
#endif


/////////////////////////////////////////////////////////////////////
// Common Definitions

#define MAX_AXIS_NUM                    8
#define MAX_MPG_NUM                     2
#define MAX_GROUP_NUM					(MAX_CARD_NUM * MAX_AXIS_NUM)

#define MOTION_QUEUE_SIZE				10000

#define GROUP_VALID						0
#define GROUP_INVALID					-1
#define AXIS_INVALID					-1
                              
#define _YES_                           1
#define _NO_                            0
/////////////////////////////////////////////////////////////////////
// Range Definitions of Interpolation Period (ms)

#define IPO_PERIOD_MIN                  1
#define IPO_PERIOD_MAX					50
#define IPO_PERIOD_DEFAULT				5

/////////////////////////////////////////////////////////////////////
// Definitions of Output Command Modes

#define OCM_PULSE						0
#define OCM_VOLTAGE						1
#define OCM_SWCLOSELOOP                 2


/////////////////////////////////////////////////////////////////////
// Definitions of Sensor Logic

#define SL_NORMAL_OPEN					0
#define SL_NORMAL_CLOSE					1
#define SL_UNUSED						2


/////////////////////////////////////////////////////////////////////
// Definition(s) of motion card types

#define PMC_8000_CARD				4
#define PMC_6000_CARD				5
#define PMC_4000_CARD				6


/////////////////////////////////////////////////////////////////////
// Unit Definitions

#define UNIT_MM                         1
#define UNIT_INCH                       2

/////////////////////////////////////////////////////////////////////
// Home Mode Definitions

#define HM_NORMAL						0 // obsolete, just for compatibility
#define HM_SENSOR_ONLY					1 // obsolete, just for compatibility
#define HM_INDEX_ONLY					2 // obsolete, just for compatibility
#define HM_FIRST						3
#define HM_LAST                         20


/////////////////////////////////////////////////////////////////////
// Definitions of Modes of Hardware Limit Protection

#define HLP_BIDIR                       0
#define HLP_UNIDIR                      1
#define HLP_BIDIR_GEN_ERR               2
#define HLP_UNIDIR_GEN_ERR              3

/////////////////////////////////////////////////////////////////////
// Definitions of circular directions

#define CIR_CW							0 // clockwise
#define CIR_CCW							1 // counter-clockwise


/////////////////////////////////////////////////////////////////////
// Definitions of Group Motion Status

#define GMS_RUNNING						0
#define GMS_STOP						1
#define GMS_HOLD						2
#define GMS_DELAYING					3
#define GMS_BLOCKHOLD                   4
#define GMS_MPGING                      5


/////////////////////////////////////////////////////////////////////
// Definitions of In-Position Modes

#define IPM_ONETIME_BLOCK				0
#define IPM_ONETIME_UNBLOCK				1
#define IPM_SETTLE_BLOCK				2
#define IPM_SETTLE_UNBLOCK				3


/////////////////////////////////////////////////////////////////////
// Definitions of MCCL Axis Flag

#define AXIS_X							0x0001
#define AXIS_Y							0x0002
#define AXIS_Z							0x0004
#define AXIS_U							0x0008
#define AXIS_V							0x0010
#define AXIS_W							0x0020
#define AXIS_A							0x0040
#define AXIS_B							0x0080
#define AXIS_ALL						0x00FF


/////////////////////////////////////////////////////////////////////
// MCCL Error Codes Definitions

#define NO_ERR							  0
#define INITIAL_MOTION_ERR				 -1
#define COMMAND_BUFFER_FULL_ERR			 -2
#define COMMAND_NOTACCEPTED_ERR			 -3 
#define COMMAND_NOTFINISHED_ERR			 -4
#define PARAMETER_ERR					 -5
#define GROUP_PARAMETER_ERR				 -6
#define FEED_RATE_ERR					 -7
#define BLEND_COMMAND_NOTCALLED_ERR		 -8
#define VOLTAGE_COMMAND_CHANNEL_ERR		 -9
#define HOME_COMMAND_NOTCALLED_ERR		-10
#define HOLD_ILLEGAL_ERR				-11
#define CONTI_ILLEGAL_ERR				-12
#define ABORT_ILLEGAL_ERR				-13
#define RUN_TIME_ERR					-14
#define ABORT_NOT_FINISH_ERR			-15
#define GROUP_RAN_OUT_ERR				-16


/////////////////////////////////////////////////////////////////////
// Compensation-related Definitions

#define MAX_COMP_POINT					256

#define DDA_FMT_NO                0x0 // No pulse output
#define DDA_FMT_PD                0x1 // Pulse/Dir
#define DDA_FMT_CW                0x2 // CW/CCW
#define DDA_FMT_AB                0x3 // A/B Phase

#define ENC_TYPE_NO               0x0 // No pulse input
#define ENC_TYPE_PD               0x1 // Pulse/Dir
#define ENC_TYPE_CW               0x2 // CW/CCW
#define ENC_TYPE_AB               0x3 // A/B Phase

#ifndef _IMC_DEFINE_H_
/////////////////////////////////////////////////////////////////////
// Output Pulse Format Definitions

#define PGE_FMT_NO                0x0 // No pulse output
#define PGE_FMT_PD                0x1 // Pulse/Dir
#define PGE_FMT_CW                0x2 // CW/CCW
#define PGE_FMT_AB                0x3 // A/B Phase


/////////////////////////////////////////////////////////////////////
// Encoder Type Definitions

#define ENC_FMT_NO               0x0 // No pulse input
#define ENC_FMT_PD               0x1 // Pulse/Dir
#define ENC_FMT_CW               0x2 // CW/CCW
#define ENC_FMT_AB               0x3 // A/B Phase

#define MPG_FMT_NO               0x0 // No pulse input
#define MPG_FMT_PD               0x1 // Pulse/Dir
#define MPG_FMT_CW               0x2 // CW/CCW
#define MPG_FMT_AB               0x3 // A/B Phase

/////////////////////////////////////////////////////////////////////
// Encoder Latch Trigger Mode Definitions

#define ENC_TRIG_FIRST					0x0
#define ENC_TRIG_LAST					0x1


/////////////////////////////////////////////////////////////////////
// Encoder Trigger Source Definitions

#define ENC_TRIG_NO               0x00000000
#define ENC_TRIG_INDEX0           0x00000001
#define ENC_TRIG_INDEX1           0x00000002
#define ENC_TRIG_INDEX2           0x00000004
#define ENC_TRIG_INDEX3           0x00000008
#define ENC_TRIG_INDEX4           0x00000010
#define ENC_TRIG_INDEX5           0x00000020
#define ENC_TRIG_INDEX6           0x00000040
#define ENC_TRIG_INDEX7           0x00000080
#define ENC_TRIG_OTP0             0x00000100
#define ENC_TRIG_OTP1             0x00000200
#define ENC_TRIG_OTP2             0x00000400
#define ENC_TRIG_OTP3             0x00000800
#define ENC_TRIG_OTP4             0x00001000
#define ENC_TRIG_OTP5             0x00002000
#define ENC_TRIG_OTP6             0x00004000
#define ENC_TRIG_OTP7             0x00008000
#define ENC_TRIG_OTN0             0x00010000
#define ENC_TRIG_OTN1             0x00020000
#define ENC_TRIG_OTN2             0x00040000
#define ENC_TRIG_OTN3             0x00080000
#define ENC_TRIG_OTN4             0x00100000
#define ENC_TRIG_OTN5             0x00200000
#define ENC_TRIG_OTN6             0x00400000
#define ENC_TRIG_OTN7             0x00800000
#define ENC_TRIG_TMR              0x01000000


/////////////////////////////////////////////////////////////////////
// Local I/O Interrupt Trigger Type Definitions

#define LIO_INT_NO                0x0
#define LIO_INT_RISE              0x1
#define LIO_INT_FALL              0x2
#define LIO_INT_LEVEL             0x3

#define GPIO_INT_NO               0x0
#define GPIO_INT_RISE             0x1
#define GPIO_INT_FALL             0x2
#define GPIO_INT_LEVEL            0x3

#define ALM_INT_NO                0x0
#define ALM_INT_RISE              0x1
#define ALM_INT_FALL              0x2
#define ALM_INT_LEVEL             0x3

/////////////////////////////////////////////////////////////////////
// Local I/O Interrupt Trigger Source Definitions
#define LIO_LDI_OTP0              0
#define LIO_LDI_OTP1              1
#define LIO_LDI_OTP2              2
#define LIO_LDI_OTP3              3
#define LIO_LDI_OTP4              4
#define LIO_LDI_OTP5              5
#define LIO_LDI_OTP6              6
#define LIO_LDI_OTP7              7
#define LIO_LDI_OTN0              8
#define LIO_LDI_OTN1              9
#define LIO_LDI_OTN2              10
#define LIO_LDI_OTN3              11
#define LIO_LDI_OTN4              12
#define LIO_LDI_OTN5              13
#define LIO_LDI_OTN6              14
#define LIO_LDI_OTN7              15
#define LIO_LDI_HOME0             16
#define LIO_LDI_HOME1             17
#define LIO_LDI_HOME2             18
#define LIO_LDI_HOME3             19
#define LIO_LDI_HOME4             20
#define LIO_LDI_HOME5             21
#define LIO_LDI_HOME6             22
#define LIO_LDI_HOME7             23

#define LIO_LDI_ALM0              0
#define LIO_LDI_ALM1              1
#define LIO_LDI_ALM2              2
#define LIO_LDI_ALM3              3
#define LIO_LDI_ALM4              4
#define LIO_LDI_ALM5              5
#define LIO_LDI_ALM6              6
#define LIO_LDI_ALM7              7


/////////////////////////////////////////////////////////////////////
// DAC Interrupt Trigger Source Definitions

#define DAC_TRIG_ENC0	            0x00000001
#define DAC_TRIG_ENC1	            0x00000002
#define DAC_TRIG_ENC2	            0x00000004
#define DAC_TRIG_ENC3	            0x00000008
#define DAC_TRIG_ENC4	            0x00000010
#define DAC_TRIG_ENC5	            0x00000020
#define DAC_TRIG_ENC6	            0x00000040
#define DAC_TRIG_ENC7	            0x00000080

/////////////////////////////////////////////////////////////////////
// LED Trigger Source Definitions
#define LED_TRIG_ENC0             0x00000001
#define LED_TRIG_ENC1             0x00000002
#define LED_TRIG_ENC2             0x00000004
#define LED_TRIG_ENC3             0x00000008
#define LED_TRIG_ENC4             0x00000010
#define LED_TRIG_ENC5             0x00000020
#define LED_TRIG_ENC6             0x00000040
#define LED_TRIG_ENC7             0x00000080


// GPIO Trigger Source Definitions
#define GPIO_TRIG_ENC0             0x00000001
#define GPIO_TRIG_ENC1             0x00000002
#define GPIO_TRIG_ENC2             0x00000004
#define GPIO_TRIG_ENC3             0x00000008
#define GPIO_TRIG_ENC4             0x00000010
#define GPIO_TRIG_ENC5             0x00000020
#define GPIO_TRIG_ENC6             0x00000040
#define GPIO_TRIG_ENC7             0x00000080
#define GPIO_TRIG_MPG0             0x00000100
#define GPIO_TRIG_MPG1             0x00000200

/////////////////////////////////////////////////////////////////////
// ADC Compare Type Definitions

#define ADC_COMP_RISE				       0x0
#define ADC_COMP_FALL	                   0x1
#define ADC_COMP_LEVEL	                   0x2


/////////////////////////////////////////////////////////////////////
// ADC Compare Mask Definitions

#define ADC_MASK_NO	                       0x0
#define ADC_MASK_BIT1	                   0x1
#define ADC_MASK_BIT2	                   0x2
#define ADC_MASK_BIT3	                   0x3

/////////////////////////////////////////////////////////////////////
// ADC Converting Type Definitions

#define ADC_TYPE_BIP_DIFF	               0x0
#define ADC_TYPE_UNI_DIFF	               0x1
#define ADC_TYPE_BIP_SING                  0x2
#define ADC_TYPE_UNI_SING                  0x3


/////////////////////////////////////////////////////////////////////
// ADC Converting Mode Definitions

#define ADC_MODE_SINGLE                    0x0
#define ADC_MODE_FREE	                   0x1


/////////////////////////////////////////////////////////////////////
// Remote I/O Set Definitions

#define RIO_SET0						   0x0
#define RIO_SET1						   0x1


/////////////////////////////////////////////////////////////////////
// Remote I/O Port Definitions

#define RIO_PORT0						   0x0
#define RIO_PORT1						   0x1
#define RIO_PORT2						   0x2
#define RIO_PORT3						   0x3


/////////////////////////////////////////////////////////////////////
// Remote I/O Interrupt Trigger Source Definitions

#define RIO_DI0							   0x0
#define RIO_DI1							   0x1
#define RIO_DI2							   0x2
#define RIO_DI3							   0x3
#define RIO_DI4							   0x4
#define RIO_DI5							   0x5
#define RIO_DI6							   0x6
#define RIO_DI7							   0x7


/////////////////////////////////////////////////////////////////////
// Remote I/O Interrupt Trigger Type Definitions

#define RIO_INT_NO   					   0x0
#define RIO_INT_RISE					   0x1
#define RIO_INT_FALL					   0x2
#define RIO_INT_LEVEL					   0x3

#endif // _IMC_DEFINE_H_


/////////////////////////////////////////////////////////////////////
// Type Definitions
typedef void (MCS_CALL *CYCLE_INTERRUPT_ISR)();

typedef struct _COMMAND_INFO
{
	int     nType; //  0: Pont to Point
				   //  1: Line
				   //  2: Clockwise Arc/Circle
				   //  3: Counterclockwise Arc/Circle
				   //  6: Delay
				   //  7: Enable Blend
				   //  8: Disable Blend
				   //  9: Enable In-Position
				   // 10: Disable In-Position
	int     nCommandIndex;
	double  dfFeedSpeed;
	double  dfPos[MAX_AXIS_NUM];
	int     nMotionPhase;
} COMMAND_INFO;

typedef struct _SYS_MAC_PARAM
{
	WORD    wPosToEncoderDir;
	WORD    wRPM;
	DWORD   dwPPR;
	double  dfPitch;
	double  dfGearRatio;
	double  dfHighLimit;
	double  dfLowLimit;
	double  dfHighLimitOffset;
	double  dfLowLimitOffset;

	WORD    wPulseMode;
	WORD    wPulseWidth;
	WORD    wCommandMode;
	WORD    wPaddle;

	WORD    wOverTravelUpSensorMode;
	WORD    wOverTravelDownSensorMode;
} SYS_MAC_PARAM;

typedef struct _SYS_MAC_PARAM_EX
{
	WORD    wPosToEncoderDir;
	WORD    wPulseMode;
	WORD    wPulseWidth;
	WORD    wCommandMode;

	DWORD   dwRPM;
	DWORD   dwPPR;
	double  dfPitch;
	double  dfGearRatio;
	double  dfHighLimit;
	double  dfLowLimit;
	double  dfHighLimitOffset;
	double  dfLowLimitOffset;

	WORD    wOverTravelUpSensorMode;
	WORD    wOverTravelDownSensorMode;
} SYS_MAC_PARAM_EX;

typedef struct _SYS_ENCODER_CONFIG
{              
	WORD    wType;
	WORD    wAInverse;
	WORD    wBInverse;
	WORD    wCInverse;
	WORD    wABSwap;
	WORD    wInputRate;
	WORD    wPaddle[2];
} SYS_ENCODER_CONFIG, ENCODER_CONFIG;

typedef struct _SYS_HOME_CONFIG
{              
	WORD    wMode;   
	WORD    wDirection; 
 	WORD    wSensorMode;
	WORD    wPaddel0;

	int     nIndexCount;
	int     nPaddel1;

	double  dfAccTime;
	double  dfDecTime;
	double  dfHighSpeed;
	double  dfLowSpeed;
	double  dfOffset;
} SYS_HOME_CONFIG;

typedef struct _SYS_HOME_CONFIG_EX
{              
	WORD    wMode;   
	WORD    wDirection; 
 	WORD    wSensorMode;
	WORD    wPaddel0;

	int     nIndexCount;
	int     nPaddel1;

	double  dfAccTime;
	double  dfDecTime;
	double  dfHighSpeed;
	double  dfLowSpeed;
	double  dfOffset;

	double  dfStableTime;
	double  dfCheckTime;
	double  dfEscapeTime;
	WORD    wStaticError;
	WORD    wTolerance;

} SYS_HOME_CONFIG_EX;

typedef struct _SYS_COMP_PARAM
{
	DWORD   dwInterval;
	WORD    wHome_No;
	WORD    wPaddle;
	int     nForwardTable[MAX_COMP_POINT];
	int     nBackwardTable[MAX_COMP_POINT];
} SYS_COMP_PARAM;

typedef struct _SYS_CARD_CONFIG
{
	WORD    wCardType;
	WORD    wCardAddress;
	WORD    wIRQ_No;
	WORD    wPaddle;
} SYS_CARD_CONFIG;


//++
//
// Description:
//
//     The prototype of custom motion profiling functions.
//
// Arguments:
//
//     nGroup - [in] Group index.
//     nSynAxisNum - [in] Amount of the axes of this group.
//
// Return Value:
//
//     Total distance this motion will walk through (in user-unit).
//
// Remarks:
//
//
//--
typedef BOOL   (MCS_CALL *PTP_START_MOTION) (
							int nGroup,
							int nSynAxisNum,
							int nCommandIndex,
							BOOL bAbsolute, 
							const double* pdfStartPos,
							const double* pdfMidPos,
							const double* pdfEndPos,
							void* pvBuffer
							);

typedef BOOL   (MCS_CALL *PTP_INTERPOLATION) (
							int nGroup,
							int nSynAxisNum,
							int nCommandIndex,
							const double* pdfStartPos,
							const double* pdfCurPos,
							double dfCurDist,
							double dfTotDist,
							void* pvBuffer
							);

typedef void   (MCS_CALL *PTP_CLEANUP) (
							int nGroup,
							int nCommandIndex,
							void* pvBuffer
							);

typedef double (MCS_CALL *CUSTOM_START_MOTION) (
							int nGroup,
							int nSynAxisNum,
							int nCommandIndex,
							BOOL bAbsolute,
							const double* pdfStartPos,
							void* pvBuffer
							);

typedef BOOL   (MCS_CALL *CUSTOM_INTERPOLATION) (
							int nGroup,
							int nSynAxisNum,
							int nCommandIndex,
							const double* pdfStartPos,
							const double* pdfCurPos,
							double dfCurDist,
							double dfTotDist,
							double* pdfNextPosOffset,
							void* pvBuffer
							);

typedef void   (MCS_CALL *CUSTOM_CLEANUP) (
							int nGroup,
							int nCommandIndex,
							void* pvBuffer
							);


typedef BOOL   (MCS_CALL *CUSTOM_BLENDING_START) (
							int nGroup,
							int nSynAxisNum,
							int nCommandIndex,
							BOOL bAbsolute, 
							const double* pdfStartPos0,
							const double* pdfStartPos1,
							const double* pdfStartPos2,
							const double* pdfMidPos,
							const double* pdfEndPos0,
							const double* pdfEndPos1,
							const double* pdfEndPos2,
							double dfBlendInterval,
							void* pvBuffer
							);

typedef BOOL   (MCS_CALL *CUSTOM_BLENDING) (
							int nGroup,
							int nSynAxisNum,
							int nCommandIndex,
							const double* pdfBlendStartPos,
							const double* pdfBlendEndPos,
							const double* pdfCurPos,
							double dfCurDist,
							double* pdfNextPosOffset,
							void* pvBuffer
							);


typedef BOOL   (MCS_CALL *CUSTOM_BLENDING_END) (
							int nGroup,
							int nCommandIndex,
							void* pvBuffer
							);


typedef void (MCS_CALL *INIT_EXTENSION) (
							void* pvExtension
							);

typedef BOOL (MCS_CALL *FWDKINEFUN) (
							int nGroup,
							int nSynAxisNum,
							const double* pdfFrom,
							double* pdfTo,
							DWORD& dwPosture,
							void* pvExtension
							);

typedef BOOL (MCS_CALL *INVKINEFUN) (
							int nGroup,
							int nSynAxisNum,
							const double* pdfFrom,
							const double* pdfPrevResult,
							double* pdfTo,
							DWORD& dwPosture,
							void* pvExtension
							);

typedef void (MCS_CALL *PRE_PROCESS_MOTION_COMMAND) (
							int nGroup,
							int nSynAxisNum,
							const double* pdfCurCPos,
							const double* pdfTgtCPos,
							double* pdfOffset,
							void* pvExtension
							);

typedef double (MCS_CALL *INFINITE_START_MOTION) (
						    int nGroup,
						    int nSynAxisNum,
						    int nCommandIndex,
						    BOOL bAbsolute,
						    const double* pdfStartPos,
						    void* pvBuffer
							);

typedef BOOL   (MCS_CALL *INFINITE_INTERPOLATION) (
							int nGroup,
							int nSynAxisNum,
							int nCommandIndex,
							const double* pdfStartPos,
							const double* pdfCurPos,
							double dfCurDist,
							double dfTotDist,
							double* pdfNextPosOffset,
							void* pvBuffer,
							BOOL* AbortThisCommand
							);

typedef void   (MCS_CALL *INFINITE_CLEANUP) (
							int nGroup,
							int nCommandIndex,
							void* pvBuffer
							);

/////////////////////////////////////////////////////////////////////
// Old Definitions in earlier MCCL

typedef struct _HOME_CONFIG
{              
    WORD    		wType;   
    WORD    		wApproachDir; 
    WORD    		wLeaveDir; 
    WORD    		wSensorMode;
    double  		dfOffset;
} HOME_CONFIG;

typedef struct _SYS_MACH_PARAM
{
	WORD		   wPosToEncoderDir;
	WORD		   wRPM;
	DWORD		   dwPPR;
	
	double		   dfPitch;
	double		   dfGearRatio;
	double		   dfHighLimit;
	double		   dfLowLimit;
	double		   dfHighLimitOffset;
	double		   dfLowLimitOffset;

	WORD		   wPulseMode;
	WORD           wPulseWidth;
	WORD           wCommandMode;
	WORD           wPaddle;

	HOME_CONFIG    stHome; 
	ENCODER_CONFIG stEncoder;

	WORD           wOverTravelUpSensorMode;
	WORD           wOverTravelDownSensorMode;
} SYS_MACH_PARAM;

typedef struct _SYS_GROUP_INFO
{
    int  nCardIndex;
    int  nChannel[MAX_AXIS_NUM];
} SYS_GROUP_INFO;

typedef struct _SYS_GROUP_CONFIG
{
    int            nGroupUsed[MAX_GROUP_NUM];
    SYS_GROUP_INFO stGroupInfo[MAX_GROUP_NUM];
} SYS_GROUP_CONFIG;


#if !defined(_PMC_LIB_H_) && !defined(__PMCLITE_LIB_H__)

typedef struct _ENC_INT
{
  BYTE INDEX0;
  BYTE INDEX1;
  BYTE INDEX2;
  BYTE INDEX3;
  BYTE INDEX4;
  BYTE INDEX5;
  BYTE INDEX6;
  BYTE INDEX7;
  BYTE COMP0;
  BYTE COMP1;
  BYTE COMP2;
  BYTE COMP3;
  BYTE COMP4;
  BYTE COMP5;
  BYTE COMP6;
  BYTE COMP7;
} ENCINT;

typedef struct _PCL_INT
{
	BYTE OVP0;
	BYTE OVP1;
	BYTE OVP2;
	BYTE OVP3;
	BYTE OVP4;
	BYTE OVP5;
	BYTE OVP6;
	BYTE OVP7;
	BYTE OVN0;
	BYTE OVN1;
	BYTE OVN2;
	BYTE OVN3;
	BYTE OVN4;
	BYTE OVN5;
	BYTE OVN6;
	BYTE OVN7;
} PCLINT;

typedef struct _ADC_INT
{
  BYTE COMP0;
  BYTE COMP1;
  BYTE COMP2;
  BYTE COMP3;
  BYTE COMP4;
  BYTE COMP5;
  BYTE COMP6;
  BYTE COMP7;
  BYTE CONV;
  BYTE TAG;
} ADCINT;

typedef struct _LIO_INT
{
  BYTE OTP0;
  BYTE OTP1;
  BYTE OTP2;
  BYTE OTP3;
  BYTE OTP4;
  BYTE OTP5;
  BYTE OTP6;
  BYTE OTP7;
  BYTE OTN0;
  BYTE OTN1;
  BYTE OTN2;
  BYTE OTN3;
  BYTE OTN4;
  BYTE OTN5;
  BYTE OTN6;
  BYTE OTN7;
  BYTE HOME0;
  BYTE HOME1;
  BYTE HOME2;
  BYTE HOME3;
  BYTE HOME4;
  BYTE HOME5;
  BYTE HOME6;
  BYTE HOME7;
} LIOINT;

typedef struct _ALM_INT
{
	BYTE ALM0;
	BYTE ALM1;
	BYTE ALM2;
	BYTE ALM3;
	BYTE ALM4;
	BYTE ALM5;
	BYTE ALM6;
	BYTE ALM7;
} ALMINT;

typedef struct _GPIO_INT
{
	BYTE GPIO0;
	BYTE GPIO1;
	BYTE GPIO2;
	BYTE GPIO3;
	BYTE GPIO4;
	BYTE GPIO5;
	BYTE GPIO6;
	BYTE GPIO7;
	BYTE GPIO8;
	BYTE GPIO9;
	BYTE GPIO10;
	BYTE GPIO11;
	BYTE GPIO12;
	BYTE GPIO13;
	BYTE GPIO14;
	BYTE GPIO15;
	BYTE GPIO16;
	BYTE GPIO17;
	BYTE GPIO18;
	BYTE GPIO19;
	BYTE GPIO20;
	BYTE GPIO21;
	BYTE GPIO22;
	BYTE GPIO23;
	BYTE GPIO24;
	BYTE GPIO25;
	BYTE GPIO26;
	BYTE GPIO27;
	BYTE GPIO28;
	BYTE GPIO29;
	BYTE GPIO30;
	BYTE GPIO31;
} GPIOINT;

typedef struct _RIO_INT
{
  BYTE RDI0;
  BYTE RDI1;
  BYTE RDI2;
  BYTE RDI3;
  BYTE RDI4;
  BYTE RDI5;
  BYTE RDI6;
  BYTE RDI7;
} RIOINT;

typedef struct _TMR_INT
{
  BYTE TIMER;
} TMRINT;

typedef struct _MPG_INT
{
  BYTE INDEX0;
  BYTE INDEX1;
  BYTE COMP0;
  BYTE COMP1;
} MPGINT;

#endif

typedef void(MCS_CALL *PCLISR)(PCLINT*);
typedef void(MCS_CALL *ADCISR)(ADCINT*);
typedef void(MCS_CALL *LIOISR)(LIOINT*);
typedef void(MCS_CALL *ENCISR)(ENCINT*);
typedef void(MCS_CALL *RIOISR)(RIOINT*);
typedef void(MCS_CALL *TMRISR)(TMRINT*);
typedef void(MCS_CALL *ALMISR)(ALMINT*);
typedef void(MCS_CALL *GPIOISR)(GPIOINT*);
typedef void(MCS_CALL *MPGISR)(MPGINT*);

// !defined(_EPCIO_DEV_H_) && !defined(_ACTADRV_H_)

#ifdef __cplusplus
	}
#endif

#endif // _MCCL_H_

