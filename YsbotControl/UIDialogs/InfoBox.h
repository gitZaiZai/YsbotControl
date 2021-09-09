// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.

#pragma once
//#include "StaticImage.h"
#include "../IO Dialog/StaticTransparent.h"
#include "../Keyboard/redbud_caption.h"
#include "../Keyboard/keyboard_button.h"
#include "../BasicControls/buttons.h"
#include "DialogType.h"
#include "../Singleton.h"
#include "../Time.h" 

#define IDD_INFOBOX_DLG 4810     
#define IDC_INFOBOX_TITLE 4811
#define IDC_INFOBOX_BUTTON 4812
#define IDC_INFOBOX_EDIT 4813

enum InfoBoxShowType
{
	IB_INFO		=	0,
	IB_ALARM,
	IB_ERROR,
	IB_OPRATION,
	IB_SYSTEM,
};

// 信息提示对话框类
class CInfoBox : public DialogType 
{
public:
	CInfoBox(void);
	~CInfoBox(void);
	enum {IDD = IDD_INFOBOX_DLG};
	static CInfoBox& GetInfoBox();
	static bool is_main_infobox_available ();

	void ShowInfoBox( CWnd* hostPtr,CString infoTXT,CString caption=_T("")
					,unsigned int showTimeS=0,unsigned int uTYPE = 0);

	void update_ui_status();

protected:
 	BOOL OnInitDialog(void);
	afx_msg void BtnCancel();
	afx_msg void BtnOK();
	DECLARE_MESSAGE_MAP()

private:
	static CInfoBox* _theInfoBox;
	RedbudCaption title_;        // 标题栏	
	CWnd* _theHost;
	CEdit edit_info;
	BasicButton btn_OK;
	CFont font_;
	unsigned int showtime_;
	CString caption_;
	CString info_;
	Ysbot::Time m_ElspTime;
};

#define INFOBOX CInfoBox::GetInfoBox()
//#define INFOBOX Singleton<CInfoBox>::Instance()