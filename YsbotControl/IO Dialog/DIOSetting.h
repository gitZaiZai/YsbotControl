// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.

#pragma once
//#include "StaticImage.h"
#include "StaticTransparent.h"
#include "../Keyboard/redbud_caption.h"
#include "../Keyboard/keyboard_button.h"
#include "../UIDialogs/DialogType.h"
#include "DIO_module.h"
#include "../BasicControls/PngCheckBox.h"

#define IDD_DIO_SETTING_DLG 4101     
#define IDC_DIO_SETTING_TITLE 4102
#define IDC_DIO_SETTING_BUTTON 4103		//4103-4109
#define IDC_DIO_SETTING_CHECKBOX 4110	//410-4111


// 数字IO设置对话框类
class CDIOSetting : public DialogType {
public:
	CDIOSetting(void);
	~CDIOSetting(void);
	enum {IDD = IDD_DIO_SETTING_DLG};
	void SetHostPointer(CDIO* Ptr);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	BOOL OnInitDialog(void);
// 	afx_msg void OnCancel(void);
// 	afx_msg void PostNcDestroy(void);
	afx_msg void BtnCancel();
	afx_msg void BtnOK();
	DECLARE_MESSAGE_MAP()
	void ShowIOState();

	void UpdateSurface();

private:
	RedbudCaption title_;        // 标题栏
	KeyboardButton button_[3];  // 键盘按钮
// 	CButton checkbox_[2];
	CPngCheckBox checkbox_[2];
	CStatic StateBmp;
	CStaticTran Label_[3];
	HBITMAP bmp_Off,bmp_On,bmp_OffForce,bmp_OnForce;
	CDIO* IOPtr;
	int test_;                   // 测试用参数
	int IOstate;
};