
#pragma once

#include "../UIDialogs//DialogType.h"
#include "../IO Dialog/StaticTransparent.h"
#include "../Keyboard/PopEdit.h"
#include "../PosInfo.h"
#include "../ConfigReader.h"

#define IDD_SETTINGDLG_Origin 4300
#define IDB_SETTINGDLG_OriginGROUP 4301 //4301-4302
#define IDE_SETTINGDLG_OriginPos 4303 //4303-4310
#define IDE_SETTINGDLG_OriginPulse 4311 //4311-4318
#define IDB_SETTINGDLG_OriginRstChk 4319 //4319-4326

//* 设置界面-原点设置窗口*
class Setting_OriginDLG : public DialogType 
{
public:
	Setting_OriginDLG(const ConfigReader& vread);
	~Setting_OriginDLG(void);
	enum {IDD = IDD_SETTINGDLG_Origin};

 	PosInfo getOrgPos() ;
	void setOrgPos(PosInfo orgAxisPos) ;
	void setOrgPluse(PosInfo orgAxisPluse) ;
	void getAxisChkValue(BOOL* BooleanArray,unsigned int arraySize=6);//Array size must >=6 and <=8

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual int OnActivateChildWin(LPARAM);

	virtual BOOL OnInitDialog(void);
	//virtual void OnOK(); 
// 	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
// 	afx_msg void OnPaint();
	afx_msg LRESULT OnMainButtonClicked(WPARAM,LPARAM); 

	DECLARE_MESSAGE_MAP()
	void PutOrgPosData();


private:
	CButton Group1,Group2;
	CStaticTran label[18];
	CpopEdit orgPos[8];
	CpopEdit orgPulse[8];
	CButton resetCheck[8];

	CFont font_;
	PosInfo OriginPos;
	PosInfo OriginPulse;
//	bool AxisChecked[8];
	ConfigReader& CfgReader;

	void define_motion_start_position();

	double get_define_origin_pos(int axisID);
	void save_home_postion_encode();

};

 