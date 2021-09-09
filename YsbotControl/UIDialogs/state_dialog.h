#pragma once

#include "../colourcode.h"
#include "../common_file_tools.h"
#include "../icon_class.h"
#include "../BasicControls/BtnST.h"
#include "../BasicControls/Label.h"
#include <vector>
#include "JournalData.h"
#include "DialogType.h"

struct LabelInfo
{
	int idLabel;
	int idImage;
	bool hasImage;
	CRect* rect;
	CLabel* label;

	LabelInfo(int idLabel_, int idImage_, 
		CRect* prect = NULL, CLabel* pLabel = NULL,
		bool hasImage_ = true );
};

class StateDialog : public DialogType 
{
public:
	static StateDialog& get_main_state_dialog ();
	static bool is_main_state_dialog_available ();

	StateDialog(HWND hParent);
	virtual ~StateDialog();
 
	enum	
	{	LB_ROBOT_STATE       = 0,
	    LB_OPERATION_MODE	 = 1,			 
 		LB_COORDINATE_SYSTEM = 2,			 
		LB_TOOL_NAME     	 = 3,			 
		LB_ROBOT_SPEED       = 4,					 
		LB_COMPANY_LOGO      = 5,					 
		LB_PROJECT_NAME      = 6,			 
		LB_PROGRAM_STATE	 = 7,
		LB_RUNNING_MODE      = 8,					 
		LB_RESERVE_1         = 9,			 
		LB_RESERVE_2     	 = 10,			 
		LB_RESERVE_3         = 11,					 
		LB_SPACE_WATCH       = 12,			 
		LB_SAFES_TATE	     = 13,
		LB_USER_GRADE	     = 14,
		LB_SYSTEM_TIME	     = 15,
		LB_MESSAGE_BOX	     = 16,
		LB_NUM               = 17   
	};
 
	enum{ JOURNAL_ARISE, JOURNAL_CONFIRMED  };
 	// 设置操作模式（1是自动，2是外部自动，3是手动）
	void SetOperationMode(int mode);
	// 设置机器人使能状态（1是打开，2是关闭）
	void SetRobotState(int state);
	// 设置机器人型号
	void SetRobotModel(CString str);
	// 设置坐标系名称（1是世界，2是关节，3是工具）
	void SetCoordinateSystem(const char* coordname);
	// 设置工具手（1是默认工具）
	void SetTool(const char* toolname);
	// 设置机器人速度
	void SetRobotSpeed(double);
	// 设置项目文件图标
	void SetProjectName(LPCTSTR strName);
 	// 设置程序运行状态（1是运行，2是暂停，3是停止，4是返回）
	void SetProgramState(int state);
	// 设置程序运行模式（1是连续，2是单步，3是运动单步）
	void SetRunningMode(int state);
	// 设置空间监控 from 1
	void SetSpaceWatch(int state);
	// 显示已启用的安全区域总数
	void SetSpaceActiveNum(unsigned int nNum);
	// 设置安全回路状态（1是安全回路导通，2是安全回路断开）
	void SetSafeState(int state);
	// 设置用户等级
	void SetUserGrade(int grade);
	// 更新时间
	void SetSystemTime();

	// 设置信息提示图标
	void SetMessagePrompt(CString str, int nType);

	// 显示信息内容
	void ShowInfo( CString str, int nType = 0, int nMsgCode = 0, bool bNeedConfirm = false );
 	void ShowInfo( LPCTSTR infoText, LPCTSTR caption, CWnd* parent, unsigned int showTimes = 0, int nType = JTMESSAGE, int nMsgCode = 5050 );

	// 点击信息确认按钮
	void set_coordinate_bgcolor();
  	void update_state_bar(void);
	void set_pendant_type(int itype) { pendant_type = itype; }

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	

	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void PromptConFirm();
	afx_msg void OnClickMainMenuBtn();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

private:
	static StateDialog*	 main_state_dialog;
  
 	CFont    m_Font;
	HWND     m_hParent;
	COLORREF m_crBackgroud;
	COLORREF m_crText;
	COLORREF m_crDarkRed, m_crWarning, m_crError;
	CPen     m_whitePen;

	std::vector<LabelInfo*>  label_list;
	CButtonST                prompt_confirm_;
	CButtonST                main_menu_btn;

 	int pendant_type;
 	CString strSpaceBmp[2];

	JournalData m_journal_data;
 
	void init_rect_list();
	void init_label();
	void SetCompanyLogo();

	void insert_journal (LPCTSTR msgText);
	std::vector<CString>  message_list;

public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

#define MSD  StateDialog::get_main_state_dialog()
