// Copyright 2016, ��˼��Ϣ�Ƽ��������ι�˾
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


// ����IO���öԻ�����
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
	RedbudCaption title_;        // ������
	KeyboardButton button_[3];  // ���̰�ť
// 	CButton checkbox_[2];
	CPngCheckBox checkbox_[2];
	CStatic StateBmp;
	CStaticTran Label_[3];
	HBITMAP bmp_Off,bmp_On,bmp_OffForce,bmp_OnForce;
	CDIO* IOPtr;
	int test_;                   // �����ò���
	int IOstate;
};