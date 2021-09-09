// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef _window_button_menu_
#define _window_button_menu_

#define  MAIN_BUTTON_BASE_ID  1000

enum ChilidWindowKind
{ 
	winMainMenuDlg       = -3,
	winMainMenu          = -2, 
	winLastWindow        = -1, 
	winUserDefine        = 0, 
	winSetting           = 1, 
	winVariableData      = 2, 
	winProject           = 3, 
	winProgram           = 4, 
	winPostionTeach      = 5, 
	winLogInformation    = 6, // don't change the order of previous 7 items, correspond to hardware keypannel.
	winNewScriptVariable = 7,
	winInOutDeviece      = 8,
	winSettingJoints     = 9,
	winSettingMechanism  = 10,
	winSettingOrigin     = 11,

	winTeachDLG			 = 12,
	winUserCoSysDLG		 = 13,
	winToolCoSysDLG		 = 14,
	winPalletDLG		 = 15,
	winInstructionDLG	 = 16,
	winInstructionEdit	 = 17,
	winVariableBuilder	 = 18,
	winEditCommand       = 19,
	winSecurityArea      = 20,
	winTeachPanelDlg     = 21,
	winNum               = 22,

	

};

static const int NUM_MAIN_BUTTON = 7; 
static const int MAX_MENU_ITEM   = 12; 

class ButtonMenu 
{
public:
	ButtonMenu();
	virtual ~ButtonMenu();

	LPCTSTR             button_text;
	int                 menu_count;
	LPCTSTR             menu_text[MAX_MENU_ITEM];   
} ;

class MainButtonMenu
{
public:
	MainButtonMenu();
	virtual ~MainButtonMenu();

 	const int  GetMenuCount(int mainbutton);
	const LPCTSTR* get_menu_text(int mainbutton);
private:
	enum { IDM_USER, IDM_SETTING, IDM_VARIABLE, IDM_PROJECT, IDM_PROGRAM, IDM_JOG, IDM_LOG };
 	ButtonMenu* button_menu;

	void init_menu_table();
};

static const int NUM_CHILD_BUTTON = 6; 

class ChildWindowButtonMenu
{
public:
	ChildWindowButtonMenu();
	virtual ~ChildWindowButtonMenu();

	const LPCTSTR get_button_caption(ChilidWindowKind kind, int button_id);
	const int GetMenuCount(ChilidWindowKind kind, int button_id);
	const LPCTSTR* get_menu_text(ChilidWindowKind kind, int button_id);
	void reset_button_table ( bool bIsRunning);
	void update_button_text( ChilidWindowKind iWin, int iButton, LPCTSTR strText );

private:


	void init_button_table();
	void init_table_running ();
	bool bRunFlag;
	ButtonMenu** button_caption;
	
	void set_win_posteach ();
	void set_win_logfile ();
	void set_win_userdefine ();
};

#endif