#include "stdafx.h"
#include "SettingDlg_Joints.h"
#include "../WorldModel.h"
#include <cstring>
#include "../colourcode.h"
#include "../SQLiteDB.h"
#include "../UIDialogs/state_dialog.h"

Setting_JointsDLG::Setting_JointsDLG(const ConfigReader& cfgReader) :CfgReader(const_cast<ConfigReader&>(cfgReader))
{
}

Setting_JointsDLG::~Setting_JointsDLG() 
{
	delete gp_;
}

BEGIN_MESSAGE_MAP(Setting_JointsDLG, DialogType)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()

	//ON_EN_SETFOCUS(IDC_SETTINGDLG_Joints_CTR,ON_EnSetFocus)
END_MESSAGE_MAP()


void Setting_JointsDLG::DoDataExchange(CDataExchange* pDX) 
{
	
}

BOOL Setting_JointsDLG::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return CDialogEx::OnEraseBkgnd(pDC);
}

void Setting_JointsDLG::OnOK() {
	//DestroyWindow();
}

// void Setting_JointsDLG::OnCancel() {
// 	DestroyWindow();
// }

void Setting_JointsDLG::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialogEx::OnPaint()

}

BOOL Setting_JointsDLG::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	SetWindowPos(NULL,g_x0 + 22, 95, 724, 439, SWP_NOZORDER);
	SetBackgroundColor(kColor[8], TRUE);
	//gp_=MWM.get_group_properties();
	gp_=new GroupProperties(CfgReader);

	int groupTop=20;
	int groupButtom=gp_->axis_num*26+68;
	groupButtom=groupButtom>350 ? 350:groupButtom;
	Group1.Create(_T("轴参数"),WS_CHILD|WS_VISIBLE|WS_GROUP|BS_GROUPBOX,
		CRect(7,groupTop,713,groupButtom),this,IDB_SETTINGDLG_MechGroup);
	mg_.Create(gp_,_T(""),WS_CHILD|WS_VISIBLE,CRect(10,40,710,300),this,IDC_SETTINGDLG_Joints_Grid);
 
	Group2.Create(_T("运动学参数"),WS_CHILD|WS_VISIBLE|WS_GROUP|BS_GROUPBOX,
		CRect(7,groupButtom+20,713,430),this,IDB_SETTINGDLG_MechGroup+1);
	CTRlabel[0].Create(_T("最大角加速度："),WS_CHILD|WS_VISIBLE|SS_CENTER,
		CRect(20,groupButtom+50,150,groupButtom+80),this);
	CTRlabel[1].Create(_T("最大线速度："),WS_CHILD|WS_VISIBLE|SS_CENTER,
		CRect(20,groupButtom+90,150,groupButtom+120),this);
	CTRlabel[2].Create(_T("最大线加速度："),WS_CHILD|WS_VISIBLE|SS_CENTER,
		CRect(20,groupButtom+130,150,groupButtom+160),this);
	CTRlabel[3].Create(_T("deg/sec^2"),WS_CHILD|WS_VISIBLE|SS_CENTER,
		CRect(255,groupButtom+50,350,groupButtom+80),this);
	CTRlabel[4].Create(_T("mm/sec"),WS_CHILD|WS_VISIBLE|SS_CENTER,
		CRect(255,groupButtom+90,350,groupButtom+120),this);
	CTRlabel[5].Create(_T("mm/sec^2"),WS_CHILD|WS_VISIBLE|SS_CENTER,
		CRect(255,groupButtom+130,350,groupButtom+160),this);

	VERIFY(font_.CreateFont(
		22,                // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		DEFAULT_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("arial")));  
	CString str;
	joint_acc_.Create(WS_VISIBLE|WS_CHILD|WS_BORDER|ES_CENTER,
		CRect(150,groupButtom+45,250,groupButtom+71),this,IDC_SETTINGDLG_Joints_CTR);
	joint_acc_.SetFont(&font_);
 	str.Format(_T("%.4f"),gp_->profile.dfMaxJAccel);
	joint_acc_.SetWindowTextW(str);
	cartesian_vel_.Create(WS_VISIBLE|WS_CHILD|WS_BORDER|ES_CENTER,
		CRect(150,groupButtom+85,250,groupButtom+111),this,IDC_SETTINGDLG_Joints_CTR+1);
	cartesian_vel_.SetFont(&font_);
 	str.Format(_T("%.4f"),gp_->profile.dfMaxCSpeed);
	cartesian_vel_.SetWindowTextW(str);
	cartesian_acc_.Create(WS_VISIBLE|WS_CHILD|WS_BORDER|ES_CENTER,
		CRect(150,groupButtom+125,250,groupButtom+151),this,IDC_SETTINGDLG_Joints_CTR+2);
	cartesian_acc_.SetFont(&font_);
 	str.Format(_T("%.4f"),gp_->profile.dfMaxCAccel);
	cartesian_acc_.SetWindowTextW(str);
	return FALSE;
}

LRESULT Setting_JointsDLG::OnMainButtonClicked( WPARAM wp, LPARAM lp)
{
	int btnid  = (int)(wp);
	int menuid = (int)(lp);

	switch(btnid)
	{
	case 0:	break;
	case 1:	break;
	case 2:	break;
	case 3:	break;
	case 4: CancelEdit();	break;
	case 5:	SaveData();	break;
	default:break;
	}
	return 1;
}
 
void Setting_JointsDLG::SaveData()
{
	mg_.SaveMechGridData();
	CString str;
	joint_acc_.GetWindowTextW(str);
	gp_->profile.dfMaxJAccel=_ttof(str);
	cartesian_vel_.GetWindowTextW(str);
	gp_->profile.dfMaxCAccel=_ttof(str);
	cartesian_acc_.GetWindowTextW(str);
	gp_->profile.dfMaxCSpeed=_ttof(str);
	unsigned long long id;
	for (unsigned int i=0;i<gp_->axis_num;i++)
	{
		id=i+1;
		string prefix = "AXIS_" + std::to_string(id);
		CfgReader.set((prefix+string("::servo_type")).c_str(), gp_->servoName[i] ); 
		CfgReader.set((prefix+string("::reverse")).c_str(), gp_->mp[i].reverse ); 
		CfgReader.set((prefix+string("::encoder_dir")).c_str(), gp_->mp[i].encoder_dir ); 
		CfgReader.set((prefix+string("::rated_speed")).c_str(), gp_->mp[i].rpm ); 
		CfgReader.set((prefix+string("::driver_ppr")).c_str(), gp_->mp[i].ppr ); 
		CfgReader.set((prefix+string("::gear_ratio")).c_str(), gp_->mp[i].gear_ratio ); 
		CfgReader.set((prefix+string("::positive_limit")).c_str(), gp_->mp[i].high_limit ); 
		CfgReader.set((prefix+string("::negative_limit")).c_str(), gp_->mp[i].low_limit ); 

	}

	CfgReader.set(("MOTION"+string("::max_joint_acc")).c_str(), gp_->profile.dfMaxJAccel ); 
	CfgReader.set(("MOTION"+string("::max_cartesian_vel")).c_str(), gp_->profile.dfMaxCSpeed ); 
	CfgReader.set(("MOTION"+string("::max_cartesian_acc")).c_str(), gp_->profile.dfMaxCAccel ); 
	CfgReader.write_all();

	MSD.ShowInfo(_T("修改系统参数"));
 
	ShowWindow(SW_HIDE);
	GetParent()->SendMessage(WM_ACTIVATE_WINDOW, winLastWindow);

}

void Setting_JointsDLG::UnDoEdit()
{
	mg_.RecoverMechGridData();
	CString str;
	str.Format(_T("%.4f"),gp_->profile.dfMaxJAccel);
	joint_acc_.SetWindowTextW(str);
	str.Format(_T("%.4f"),gp_->profile.dfMaxCSpeed);
	cartesian_vel_.SetWindowTextW(str);
	str.Format(_T("%.4f"),gp_->profile.dfMaxCAccel);
	cartesian_acc_.SetWindowTextW(str);
}

void Setting_JointsDLG::CancelEdit()
{
	UnDoEdit();
	ShowWindow(SW_HIDE);
	GetParent()->SendMessage(WM_ACTIVATE_WINDOW, winLastWindow);

}