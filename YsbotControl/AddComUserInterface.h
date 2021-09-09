// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef _addcom_user_interface_h
#define _addcom_user_interface_h

#include "Time.h"
#include "ConfigReader.h"
#include "SerialPort.h"
#include "RobotState.h"
 

class AddComUserInterface 
{
public:
	enum SignalAttribute
	{
		tpRunLight = 0,
		tpMotionLight,
		tpErrorLight,
		tpProcessLight,
		tpBuzzer,
	};
	AddComUserInterface(const ConfigReader&, CSerialPort* ) throw (InvalidConfigurationException);
	~AddComUserInterface(void);
	CSerialPort* teach_pendant;

private:
 
	Ysbot::Time revtime;
	Ysbot::Time lighttime;
 
	unsigned int portnr;
	bool use_teachpendant;
	unsigned char revData[5];
	unsigned char lastKeyData[5];
	unsigned char isRiseSignal[5];

 	void getEnableSwitch ();
	void getKeySwitch ();
	void getShiftButton ();
	void getStartKey ();
	void getVelocityAdjust ();
	void getMotKey ();
	void getStepKey ();
	void getMainPageIndex();

	void update_light_state(void);
	void getRobKey(void);

	void getF1RobButton ();
	void getJogCoordinate ();

	void process_key_messages();

	void check_rise_signal( const int curr, int& last, bool& isrise );
	void check_rise_signal2( const unsigned char* curr, unsigned char* last, unsigned char* isrise );
	void check_rise_signal3( const unsigned char* curr, unsigned char* last, unsigned char* isrise );

public:
	int ProcessReceiveChar(unsigned char* chRX);

	void set_command(bool, DWORD sigMask);
	bool process_messages () throw ();
    void set_pc_keydata(unsigned int modifiers, unsigned int virtual_key);
 };

#endif

