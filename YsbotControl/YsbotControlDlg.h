// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

// YsbotControlDlg.h : header file
//

#pragma once

#include "Servo.h"
#include "RobotKinematics.h"
#include "MotionControl.h"
#include "WorldModel.h"
#include "FieldBusDevice.h"
#include "Time.h"
#include <memory>
#include "SerialPort.h"
#include "AddComUserInterface.h"
#include "SimulationPlatform.h"
#include "VisionProcessing.h"
#include "left_dialog.h"
 
#include "UIDialogs/SqlDatabase.h"
#include "UIDialogs/MainDlg.h"

// CYsbotControlDlg dialog
class CYsbotControlDlg : public CDialogEx
{
// Construction
public:
	CYsbotControlDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CYsbotControlDlg();

	void  OnKeyboardInput(BOOL bUpAndDown, int nVirtualKey);
	int		m_nFocusAxis;
	int		m_nVkFirst;
	CmdInfo cmd;
	int     dir;
	unsigned int nAxis;

// Dialog Data
	enum { IDD = IDD_YSBOTCONTROL_DIALOG };
 
 	LeftDialog*      left_dialog_;      // 左侧窗口
    CMainDlg*        m_pwndMainDlg;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	ConfigReader     vread;

	WorldModel*      the_world_model;
	RobotKinematics* the_robot_kinematics;
    MotionControl*   the_motion_control;
    Servo*           the_servo;
    FieldBusDevice*  the_fieldbus_device;
	CSerialPort      teach_pendant;
	AddComUserInterface* the_teach_pendant;
	SimulationPlatform*  the_simulator;
	VisionProcessing*   the_vision_processing;

 	SqlDatabase*                the_sql_database;

private:
	bool autoRunFlag;

	void auto_run_pro( void );

	int time1;                  // 控制时间控件显示频率

	bool     bSuppliedLogFile;
 
	ofstream fileout;

	int iMainWinWidth;
	int iMainWinHeight;

// Implementation
protected:
	HICON m_hIcon;

 	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnMsgTeachPendant(WPARAM, LPARAM);
 	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg LRESULT OnActiveMsg(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg LRESULT OnAcquireKeyInput(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnReleaseKeyInput(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnExitSystem(WPARAM wParam, LPARAM lParam);
public:
	// 激活窗口
	void ActiveDialog(int dialog_id);
	void creat_fixed_window();

};
