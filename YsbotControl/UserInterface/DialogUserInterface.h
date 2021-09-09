// Copyright 2016, ��˼��Ϣ�Ƽ��������ι�˾
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - ��˼�������˶�����ϵͳ

#pragma once

#include "../UIDialogs/DialogType.h"
#include "../ConfigReader.h"
#include "../SerialPort.h"
#include "../Time.h"
#include "../SimulationPlatform.h"

class DialogUserInterface : public DialogType
{
public:
	enum { COM_SERIAL  = 0,
		   TCP_SOCKET  = 1,
	       PC_KEYBOARD = 2, };

public:
	DialogUserInterface(const ConfigReader&) throw (InvalidConfigurationException);   // standard constructor
	virtual ~DialogUserInterface();

	bool process_messages();
	void process_key_messages();
	void set_pc_keydata(unsigned int modifiers, unsigned int virtual_key);
	void toggle_second_page ();
	void clear_key_value ();
	int  get_pendant_type () { return pendant_type; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnTeachPendantKeyPress(WPARAM, LPARAM);

private:
 	CSerialPort      com_pendant;
	int              pendant_type;
	bool             use_teachpendant;
    unsigned int     com_port;

	bool             second_page; 
	bool             enable_press; 
	int              key_switch; 

	Ysbot::Time      revtime;
	Ysbot::Time      lighttime;
	Ysbot::Time      timeKeyLastPressed;
	bool             isResetKeyState;

	unsigned char    revData[5];
	unsigned char    lastKeyData[5];
	unsigned char    isRiseSignal[5];
	std::string      serveraddress;

	unsigned int techBoxOutput;
	int		m_nVkFirst;

	bool jog_axis ( unsigned char* keyvalue );
	void set_command ( bool cmd, unsigned int sigMask );

	void getEnableSwitch ();
	void getKeySwitch ();
	void getShiftButton ();
	void getStartKey ();
	void getStopKey ();

	void getVelocityAdjust ();
	void getMotKey ();
	void getStepKey ();
	void getMainPageIndex();

	void update_light_state(void);
	void getRobKey(void);

	void getF1RobButton ();
	void getJogCoordinate ();

	void check_rise_signal3( const unsigned char* curr, unsigned char* last, unsigned char* isrise );
	void	OnKeyboardInput(BOOL bUpAndDown, int nVirtualKey);

	SimulationPlatform*  the_simulator;

	bool enable_simulator;

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

};
