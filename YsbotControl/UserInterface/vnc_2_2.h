/* htqV2.0更新声明
   与上一版本techBoxLib1.2比较增加了一下功能
   1. 增加了IO读取功能，用于读取波段开关，钥匙开关，3位安全开关等信息值，采用窗口消息，可以通过WM_TECHBOX_KEY_CHANGE(OnTeachBoxKeyHandler)的LPARAM l参数读取值.
   2. 增加摇杆数据读取功能
*/

#ifndef  TECHBOXLIB
    #define  TECHBOXLIB



#ifdef __cplusplus
  extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/*The Message ID of Key and Input changed is defineed*/
#define WM_TECHBOX_KEY_CHANGE (WM_USER+100) 
/*The Message ID of MPG changed is defineed*/
#define WM_TECHBOX_MPG (WM_USER+101)
  /*The Message ID of JOYSTICK changed is defineed*/
#define WM_TECHBOX_JOYSTICK (WM_USER+102)

  /*the bit of buzzer*/
#define TEACHBOX_BUZZER	(0x80)

  /*the bit of leds*/
#define TEACHBOX_LED1		(0x1	)
#define TEACHBOX_LED2		(0x1<<1	)
#define TEACHBOX_LED3		(0x1<<2	)
#define TEACHBOX_LED4		(0x1<<3	)
#define TEACHBOX_LED5		(0x1<<4	)
#define TEACHBOX_LED6		(0x1<<5	)
#define TEACHBOX_LED7		(0x1<<6	)
#define TEACHBOX_LED8		(0x1<<7	)
#define TEACHBOX_LED9		(0x1<<8	)
#define TEACHBOX_LED10		(0x1<<9	)
#define TEACHBOX_LED11		(0x1<<10	)
#define TEACHBOX_LED12		(0x1<<11	)
#define TEACHBOX_LED13		(0x1<<12	)
#define TEACHBOX_LED14		(0x1<<13	)
#define TEACHBOX_LED15		(0x1<<14	)
#define TEACHBOX_LED16		(0x1<<15	)
#define TEACHBOX_LED17		(0x1<<16	)
#define TEACHBOX_LED18		(0x1<<17	)
#define TEACHBOX_LED19		(0x1<<18	)
#define TEACHBOX_LED20		(0x1<<19	)
#define TEACHBOX_LED21		(0x1<<20	)
#define TEACHBOX_LED22		(0x1<<21	)
#define TEACHBOX_LED23		(0x1<<22	)
#define TEACHBOX_LED24		(0x1<<23	)
#define TEACHBOX_LED25		(0x1<<24	)
#define TEACHBOX_LED26		(0x1<<25	)
#define TEACHBOX_LED27		(0x1<<26	)
#define TEACHBOX_LED28		(0x1<<27	)
#define TEACHBOX_LED29		(0x1<<28	)
#define TEACHBOX_LED30		(0x1<<29	)
#define TEACHBOX_LED31		(0x1<<30	)
#define TEACHBOX_LED32		(0x1<<31	)

  /*the bit of keys*/
#define TEACHBOX_KEY1		(0x1	)
#define TEACHBOX_KEY2		(0x1<<1	)
#define TEACHBOX_KEY3		(0x1<<2	)
#define TEACHBOX_KEY4		(0x1<<3	)
#define TEACHBOX_KEY5		(0x1<<4	)
#define TEACHBOX_KEY6		(0x1<<5	)
#define TEACHBOX_KEY7		(0x1<<6	)
#define TEACHBOX_KEY8		(0x1<<7	)
#define TEACHBOX_KEY9		(0x1<<8	)
#define TEACHBOX_KEY10		(0x1<<9	)
#define TEACHBOX_KEY11		(0x1<<10	)
#define TEACHBOX_KEY12		(0x1<<11	)
#define TEACHBOX_KEY13		(0x1<<12	)
#define TEACHBOX_KEY14		(0x1<<13	)
#define TEACHBOX_KEY15		(0x1<<14	)
#define TEACHBOX_KEY16		(0x1<<15	)
#define TEACHBOX_KEY17		(0x1<<16	)
#define TEACHBOX_KEY18		(0x1<<17	)
#define TEACHBOX_KEY19		(0x1<<18	)
#define TEACHBOX_KEY20		(0x1<<19	)
#define TEACHBOX_KEY21		(0x1<<20	)
#define TEACHBOX_KEY22		(0x1<<21	)
#define TEACHBOX_KEY23		(0x1<<22	)
#define TEACHBOX_KEY24		(0x1<<23	)
#define TEACHBOX_KEY25		(0x1<<24	)
#define TEACHBOX_KEY26		(0x1<<25	)
#define TEACHBOX_KEY27		(0x1<<26	)
#define TEACHBOX_KEY28		(0x1<<27	)
#define TEACHBOX_KEY29		(0x1<<28	)
#define TEACHBOX_KEY30		(0x1<<29	)
#define TEACHBOX_KEY31		(0x1<<30	)
#define TEACHBOX_KEY32		(0x1<<31	)

  /*the bit of inputs*/
#define TEACHBOX_INPUT1		(0x1	)
#define TEACHBOX_INPUT2		(0x1<<1	)
#define TEACHBOX_INPUT3		(0x1<<2	)
#define TEACHBOX_INPUT4		(0x1<<3	)
#define TEACHBOX_INPUT5		(0x1<<4	)
#define TEACHBOX_INPUT6		(0x1<<5	)
#define TEACHBOX_INPUT7		(0x1<<6	)
#define TEACHBOX_INPUT8		(0x1<<7	)
#define TEACHBOX_INPUT9		(0x1<<8	)
#define TEACHBOX_INPUT10		(0x1<<9	)
#define TEACHBOX_INPUT11		(0x1<<10	)
#define TEACHBOX_INPUT12		(0x1<<11	)
#define TEACHBOX_INPUT13		(0x1<<12	)
#define TEACHBOX_INPUT14		(0x1<<13	)
#define TEACHBOX_INPUT15		(0x1<<14	)
#define TEACHBOX_INPUT16		(0x1<<15	)
#define TEACHBOX_INPUT17		(0x1<<16	)
#define TEACHBOX_INPUT18		(0x1<<17	)
#define TEACHBOX_INPUT19		(0x1<<18	)
#define TEACHBOX_INPUT20		(0x1<<19	)
#define TEACHBOX_INPUT21		(0x1<<20	)
#define TEACHBOX_INPUT22		(0x1<<21	)
#define TEACHBOX_INPUT23		(0x1<<22	)
#define TEACHBOX_INPUT24		(0x1<<23	)
#define TEACHBOX_INPUT25		(0x1<<24	)
#define TEACHBOX_INPUT26		(0x1<<25	)
#define TEACHBOX_INPUT27		(0x1<<26	)
#define TEACHBOX_INPUT28		(0x1<<27	)
#define TEACHBOX_INPUT29		(0x1<<28	)
#define TEACHBOX_INPUT30		(0x1<<29	)
#define TEACHBOX_INPUT31		(0x1<<30	)
#define TEACHBOX_INPUT32		(0x1<<31	)


#endif

