
#pragma once

#include "../IO Dialog/StaticTransparent.h"
#include "../GroupProperties.h"
#include "MechGrid.h"
#include "../Keyboard/PopEdit.h"
#include "../UIDialogs/DialogType.h"

#define IDD_SETTINGDLG_Mechanism 4330
#define IDE_SETTINGDLG_DHP 4331		//DHP parameter 4331-4340
#define IDB_SETTINGDLG_MechGroup 4341 //4341-4342


typedef struct _ARMKINdata
{
	double  DH_a1 ;  // 150
	double  DH_a2 ;  // 570
	double  DH_a3 ;  // 130
	double  DH_d3 ;	//0
	double  DH_d4 ;  // 640
	double  DH_d6;   // 95
	double  DH_offset_z;  // 450

	double  DH_elbow_post_threshold ;

	double  J45_factor;
	double  J56_factor;
	double  DHP_hw;	

}ARMKINdata;


//* 设置界面-机构设置窗口*
class Setting_MechanismDLG : public DialogType {
public:
	Setting_MechanismDLG(const ConfigReader& cfgReader);
	~Setting_MechanismDLG(void);
	enum {IDD = IDD_SETTINGDLG_Mechanism};

 	void UnDoEdit();
	void SaveData();
	void CancelEdit();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog(void);
	virtual void OnOK(); 
	virtual int OnActivateChildWin(LPARAM);

// 	virtual void OnCancel();
	//afx_msg void BtnCancel();
	//afx_msg void BtnOK();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg LRESULT OnMainButtonClicked(WPARAM,LPARAM); 

	DECLARE_MESSAGE_MAP()
	void GetARMKINdata();

private:

	CButton Group1,Group2;
	CStatic mechanismPic;
	CpopEdit _DHP_a1;
	CpopEdit _DHP_a2;
	CpopEdit _DHP_a3;
	CpopEdit _DHP_d3;
	CpopEdit _DHP_d4;
	CpopEdit _DHP_d6;
	CpopEdit _Offset_z;
	CpopEdit _J45_factor;
	CpopEdit _J56_factor;
	CpopEdit _DHP_hw;
	CFont font_;
	CStaticTran label[20];
	CStaticTran title[2];
		
	ConfigReader& CfgReader;
	ARMKINdata ARKdata;
};

