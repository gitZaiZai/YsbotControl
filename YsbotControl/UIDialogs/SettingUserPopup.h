#pragma once
#include "../IO Dialog/StaticTransparent.h"
#include "../Keyboard/redbud_caption.h"
#include "../Keyboard/keyboard_button.h"
#include "../BasicControls/IconComboBox.h"
#include "../BasicControls/PngCheckBox.h"
#include "../BasicControls/Label.h"
#include "../BasicControls/buttons.h"
#include "../Keyboard/PopEdit.h"
#include "DialogType.h"
#include "UserData.h"

#define IDC_SETTING_POPUP          7000
#define IDC_SETTING_POPUP_EDIT     7001 // ~7003
#define IDC_SETTING_POPUP_COMBBOX  7004 // 7005
#define IDC_SETTING_POPUP_CHECKBOX 7006
 
#define IDC_SETTING_POPUP_BUTTON   7007 // 7008
#define IDC_SETTING_POPUP_TEXT     7009 // 7014

#define WM_POPUPBTN_YESNO               7010

enum UserPopupType
{
	popLoginUser,
	popEditUser,
	popCreateUser,
 	popCount
};

class CSettingUserPopup : public DialogType 
{
public:
	CSettingUserPopup(void);
	~CSettingUserPopup(void);
	void activate_settingUserPopup( HWND hwnd, UserPopupType winType, const UserData& udata  );
	void close_settingUserPopup();


protected:
 	BOOL OnInitDialog(void);
 
	afx_msg void BtnCancel();
	afx_msg void BtnOK();
	DECLARE_MESSAGE_MAP()
 

private:
	RedbudCaption title_;  
	BasicButton button_[2];

 	HWND m_hwndResponse;
	UserPopupType m_nParentWinType;
	int  m_nPrimaryKeyEdit;

	CStaticTran textLabel[6];
	CpopEdit    editBox[3];
	CIconComboBox paraCombbox[2];
	CPngCheckBox	  admin_check;
	CFont font_;

	void show_child_controls(UserPopupType );
 
	int get_input_userdata(UserData*);
};