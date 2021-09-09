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
 	// ���ò���ģʽ��1���Զ���2���ⲿ�Զ���3���ֶ���
	void SetOperationMode(int mode);
	// ���û�����ʹ��״̬��1�Ǵ򿪣�2�ǹرգ�
	void SetRobotState(int state);
	// ���û������ͺ�
	void SetRobotModel(CString str);
	// ��������ϵ���ƣ�1�����磬2�ǹؽڣ�3�ǹ��ߣ�
	void SetCoordinateSystem(const char* coordname);
	// ���ù����֣�1��Ĭ�Ϲ��ߣ�
	void SetTool(const char* toolname);
	// ���û������ٶ�
	void SetRobotSpeed(double);
	// ������Ŀ�ļ�ͼ��
	void SetProjectName(LPCTSTR strName);
 	// ���ó�������״̬��1�����У�2����ͣ��3��ֹͣ��4�Ƿ��أ�
	void SetProgramState(int state);
	// ���ó�������ģʽ��1��������2�ǵ�����3���˶�������
	void SetRunningMode(int state);
	// ���ÿռ��� from 1
	void SetSpaceWatch(int state);
	// ��ʾ�����õİ�ȫ��������
	void SetSpaceActiveNum(unsigned int nNum);
	// ���ð�ȫ��·״̬��1�ǰ�ȫ��·��ͨ��2�ǰ�ȫ��·�Ͽ���
	void SetSafeState(int state);
	// �����û��ȼ�
	void SetUserGrade(int grade);
	// ����ʱ��
	void SetSystemTime();

	// ������Ϣ��ʾͼ��
	void SetMessagePrompt(CString str, int nType);

	// ��ʾ��Ϣ����
	void ShowInfo( CString str, int nType = 0, int nMsgCode = 0, bool bNeedConfirm = false );
 	void ShowInfo( LPCTSTR infoText, LPCTSTR caption, CWnd* parent, unsigned int showTimes = 0, int nType = JTMESSAGE, int nMsgCode = 5050 );

	// �����Ϣȷ�ϰ�ť
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
