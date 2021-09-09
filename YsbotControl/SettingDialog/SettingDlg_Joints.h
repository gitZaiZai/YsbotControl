
#pragma once

#include "../IO Dialog/StaticTransparent.h"
#include "MechGrid.h"
#include "../ConfigReader.h"
#include "../UIDialogs/DialogType.h"

#define IDD_SETTINGDLG_Joints 4400
#define IDC_SETTINGDLG_Joints_Grid 4410	//max 4410-4529
#define IDC_SETTINGDLG_Joints_CTR 4530	//4530-4532

//* 设置界面-配置窗口*
class Setting_JointsDLG : public DialogType {
public:
	Setting_JointsDLG(const ConfigReader& cfgReader);
	~Setting_JointsDLG(void);
	enum {IDD = IDD_SETTINGDLG_Joints};

 	void SaveData();
	void CancelEdit();

	void UnDoEdit();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog(void);
	virtual void OnOK(); 
// 	virtual void OnCancel();
	//afx_msg void BtnCancel();
	//afx_msg void BtnOK();
	afx_msg LRESULT OnMainButtonClicked(WPARAM,LPARAM); 
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//afx_msg void ON_EnSetFocus();
	DECLARE_MESSAGE_MAP()

private:
	CFont font_;
	CButton Group1,Group2;
	CMechGrid mg_;
	GroupProperties *gp_;

	CStaticTran CTRlabel[6];
	CpopEdit joint_acc_,cartesian_vel_,cartesian_acc_,external_vel_;
	ConfigReader& CfgReader;

};

