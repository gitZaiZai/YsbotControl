#include "stdafx.h"
#include "SettingDlg_Mechanism.h"
#include <cstring>
#include "../colourcode.h"
#include "../../roboop/utils.h"
#include "../SQLiteDB.h"
#include "../UIDialogs/state_dialog.h"
using namespace std;

Setting_MechanismDLG::Setting_MechanismDLG(const ConfigReader& cfgReader) :CfgReader(const_cast<ConfigReader&>(cfgReader))
{
	memset(&ARKdata,0,sizeof(ARMKINdata));
}

Setting_MechanismDLG::~Setting_MechanismDLG() 
{
	font_.DeleteObject();
}

BEGIN_MESSAGE_MAP(Setting_MechanismDLG, DialogType)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()

END_MESSAGE_MAP()


void Setting_MechanismDLG::DoDataExchange(CDataExchange* pDX) 
{

}
BOOL Setting_MechanismDLG::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return CDialogEx::OnEraseBkgnd(pDC);
}

void Setting_MechanismDLG::OnOK() {
	//DestroyWindow();
}

// void Setting_MechanismDLG::OnCancel() {
// 	DestroyWindow();
// }

void Setting_MechanismDLG::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialogEx::OnPaint()

}

BOOL Setting_MechanismDLG::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	SetWindowPos(NULL,g_x0 + 22, 95, 724, 439, SWP_NOZORDER);
	SetBackgroundColor(kColor[8], TRUE);
	//创建输入控件
	font_.CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("宋体"));

	Group1.Create(_T(""),WS_CHILD|WS_VISIBLE|WS_GROUP|BS_GROUPBOX,
		CRect(10,40,370,410),this,IDB_SETTINGDLG_MechGroup);
	Group2.Create(_T(""),WS_CHILD|WS_VISIBLE|WS_GROUP|BS_GROUPBOX,
		CRect(380,40,705,410),this,IDB_SETTINGDLG_MechGroup+1);

	//创建文字标题
	title[0].Create(_T("结构示意"),WS_CHILD| WS_VISIBLE,CRect(140,10,250,35),this);
	title[0].SetFont(26);
	title[1].Create(_T("结构参数"),WS_CHILD| WS_VISIBLE,CRect(480,10,600,35),this);
	title[1].SetFont(26);

	//创建图片
	CImage Img;
	HBITMAP bmp;
	Img.Load(_T(".\\res\\robot_mechanism.png"));
	if (!Img.IsNull())
		bmp=Img.Detach();
	mechanismPic.Create(_T(""),WS_CHILD|WS_VISIBLE|SS_BITMAP,CRect(20,60,360,400),this);
	mechanismPic.ModifyStyle(0x0f,SS_BITMAP);
	mechanismPic.SetBitmap(bmp);

	CString strArray[10]={_T("DHP_a1"),_T("DHP_a2"),_T("DHP_a3"),_T("DHP_d3"),_T("DHP_d4"),_T("DHP_d6"),
		_T("Offset_z"),_T("J45_factor"),_T("J56_factor"),_T("DHP_hw")};
	int x0 = 400;
	int y0 = 55;
	int xsize = 80;
	int ysize = 35;
	int iSpaceY = 1;
	CRect strRect[10];
	for (int j=0;j<10;j++)
	{
		strRect[j] = CRect(CPoint( x0, y0+(ysize+iSpaceY)*j ),CSize(xsize,ysize));
		//label[j].Create(strArray[j],WS_CHILD|WS_VISIBLE,CRect(x0,y0+j*35,x0+65,j*35+65),this);
		label[j].Create(strArray[j],WS_CHILD|WS_VISIBLE,strRect[j],this);
		label[j].SetFont(22);
	}
 
 	for (int j=0;j<7;j++)
	{
		label[j+10].Create(_T("mm"),WS_CHILD|WS_VISIBLE,CRect(CPoint( 610, strRect[j].top ),CSize(30,35)),this);
		label[j+10].SetFont(22);
	}

	x0 = 515;
 
	int iWidth  = 88;
	int iHeight = 25;
 
	struct {
		CpopEdit  *pEdit;
		double  dfMin;
		double  dfMax;
		int  nMaxDecimalPlaces;
	}
	edits[] = {
		&_DHP_a1,		-100000,			100000,		3,
		&_DHP_a2,		-100000,			100000,		3,
		&_DHP_a3,		-100000,			100000,		3,
		&_DHP_d3,		-100000,			100000,		3,
		&_DHP_d4,		-100000,			100000,		3,
		&_DHP_d6,		-100000,			100000,		3,
		&_Offset_z,	    -100000,			100000,		3,
		&_J45_factor,	-100000,			100000,		3,
		&_J56_factor,	-100000,			100000,		3,
		&_DHP_hw,	    -100000,			100000,		3,
	};
	for(int i = 0; i < ARRAY_SIZE(edits); ++i)
	{
		edits[i].pEdit->Create(WS_CHILD|WS_VISIBLE|WS_BORDER,CRect(CPoint( x0, strRect[i].top ),CSize(iWidth,iHeight)),this,IDE_SETTINGDLG_DHP+i);
		edits[i].pEdit->SetFont(&font_);
		edits[i].pEdit->SetValueRange(edits[i].dfMin, edits[i].dfMax,edits[i].nMaxDecimalPlaces );

	}

// 	_DHP_a1.Create(WS_CHILD|WS_VISIBLE|WS_BORDER,CRect(x0,y0,x0+88,y0+iHeight),this,IDE_SETTINGDLG_DHP);
// 	_DHP_a1.SetFont(&font_);
//  
// 	y0+=35;
// 	_DHP_a2.Create(WS_CHILD|WS_VISIBLE|WS_BORDER,CRect(x0,y0,x0+88,y0+iHeight),this,IDE_SETTINGDLG_DHP+1);
// 	_DHP_a2.SetFont(&font_);
//  
// 	y0+=35;
// 	_DHP_a3.Create(WS_CHILD|WS_VISIBLE|WS_BORDER,CRect(x0,y0,x0+88,y0+iHeight),this,IDE_SETTINGDLG_DHP+2);
// 	_DHP_a3.SetFont(&font_);
//  
// 	y0+=35;
// 	_DHP_d3.Create(WS_CHILD|WS_VISIBLE|WS_BORDER,CRect(x0,y0,x0+88,y0+iHeight),this,IDE_SETTINGDLG_DHP+3);
// 	_DHP_d3.SetFont(&font_);
// 	 
// 	y0+=35;
// 	_DHP_d4.Create(WS_CHILD|WS_VISIBLE|WS_BORDER,CRect(x0,y0,x0+88,y0+iHeight),this,IDE_SETTINGDLG_DHP+4);
// 	_DHP_d4.SetFont(&font_);
//  
// 	y0+=35;
// 	_DHP_d6.Create(WS_CHILD|WS_VISIBLE|WS_BORDER,CRect(x0,y0,x0+88,y0+iHeight),this,IDE_SETTINGDLG_DHP+5);
// 	_DHP_d6.SetFont(&font_);
//  
// 	y0+=35;
// 	_Offset_z.Create(WS_CHILD|WS_VISIBLE|WS_BORDER,CRect(x0,y0,x0+88,y0+iHeight),this,IDE_SETTINGDLG_DHP+6);
// 	_Offset_z.SetFont(&font_);
//  
// 	y0+=35;
// 	_J45_factor.Create(WS_CHILD|WS_VISIBLE|WS_BORDER,CRect(x0,y0,x0+88,y0+iHeight),this,IDE_SETTINGDLG_DHP+7);
// 	_J45_factor.SetFont(&font_);
// 	 
// 	y0+=35;
// 	_J56_factor.Create(WS_CHILD|WS_VISIBLE|WS_BORDER,CRect(x0,y0,x0+88,y0+iHeight),this,IDE_SETTINGDLG_DHP+8);
// 	_J56_factor.SetFont(&font_);
//  
// 	y0+=35;
// 	_DHP_hw.Create(WS_CHILD|WS_VISIBLE|WS_BORDER,CRect(x0,y0++,x0+88,y0+iHeight),this,IDE_SETTINGDLG_DHP+9);
// 	_DHP_hw.SetFont(&font_);
 



	GetARMKINdata();




	return FALSE;
}

LRESULT Setting_MechanismDLG::OnMainButtonClicked( WPARAM wp, LPARAM lp)
{
	int btnid  = (int)(wp);
	int menuid = (int)(lp);

	switch(btnid)
	{
	case 0:	
		printf("Reboot soft control!\n");
		GetParent()->SendMessage(WM_REBOOT_SYSTEM,0,0);
		break;
	case 1:	
		break;
	case 2:	UnDoEdit();	
		break;
	case 3:	
		SaveData();	
		break;
	case 4: 
		break;
	case 5:	
		ShowWindow(SW_HIDE);
		GetParent()->SendMessage(WM_ACTIVATE_WINDOW, winLastWindow);
		break;
	default:break;
	}
	return 1;
}

void Setting_MechanismDLG::GetARMKINdata()
{
	if ( CfgReader.get("ARMKIN::DHP_a1", ARKdata.DH_a1) < 0 || ARKdata.DH_a1 < 0 )
		throw  InvalidConfigurationException ("DHP_a1");
	if ( CfgReader.get("ARMKIN::DHP_a2", ARKdata.DH_a2) < 0 || ARKdata.DH_a2 < 0 )
		throw  InvalidConfigurationException ("DHP_a2");
	if ( CfgReader.get("ARMKIN::DHP_a3", ARKdata.DH_a3) < 0 || ARKdata.DH_a3 < 0 )
		throw  InvalidConfigurationException ("DHP_a3");
	if (CfgReader.get("ARMKIN::DHP_d3", ARKdata.DH_d3)< 0  )
		throw  InvalidConfigurationException ("DHP_d3");
	if ( CfgReader.get("ARMKIN::DHP_d4", ARKdata.DH_d4) < 0 || ARKdata.DH_d4 < 0 )
		throw  InvalidConfigurationException ("DHP_a1");
	if ( CfgReader.get("ARMKIN::DHP_d6", ARKdata.DH_d6) < 0 || ARKdata.DH_d6 < 0 )
		throw  InvalidConfigurationException ("DHP_a2");
	if ( CfgReader.get("ARMKIN::DHP_offset_z", ARKdata.DH_offset_z) < 0 || ARKdata.DH_offset_z < 0 )
		throw  InvalidConfigurationException ("DHP_offset_z");

	UnDoEdit();

}

void Setting_MechanismDLG::UnDoEdit()
{
	CString str;
	str.Format(_T("%.3f"),ARKdata.DH_a1);
	_DHP_a1.SetWindowTextW(str);
	str.Format(_T("%.3f"),ARKdata.DH_a2);
	_DHP_a2.SetWindowTextW(str);
	str.Format(_T("%.3f"),ARKdata.DH_a3);
	_DHP_a3.SetWindowTextW(str);
	str.Format(_T("%.3f"),ARKdata.DH_d3);
	_DHP_d3.SetWindowTextW(str);
	str.Format(_T("%.3f"),ARKdata.DH_d4);
	_DHP_d4.SetWindowTextW(str);
	str.Format(_T("%.3f"),ARKdata.DH_d6);
	_DHP_d6.SetWindowTextW(str);
	str.Format(_T("%.3f"),ARKdata.DH_offset_z);
	_Offset_z.SetWindowTextW(str);
	str.Format(_T("%.3f"),ARKdata.J45_factor);
	_J45_factor.SetWindowTextW(str);
	str.Format(_T("%.3f"),ARKdata.J56_factor);
	_J56_factor.SetWindowTextW(str);
	str.Format(_T("%.3f"),ARKdata.DHP_hw);
	_DHP_hw.SetWindowTextW(str);

}

void Setting_MechanismDLG::CancelEdit()
{
	UnDoEdit();
	ShowWindow(SW_HIDE);
	GetParent()->SendMessage(WM_ACTIVATE_WINDOW, winLastWindow);
}

void Setting_MechanismDLG::SaveData()
{
	CString str;
	_DHP_a1.GetWindowTextW(str);
	ARKdata.DH_a1=_ttof(str);
	_DHP_a2.GetWindowTextW(str);
	ARKdata.DH_a2=_ttof(str);
	_DHP_a3.GetWindowTextW(str);
	ARKdata.DH_a3=_ttof(str);
	_DHP_d3.GetWindowTextW(str);
	ARKdata.DH_d3=_ttof(str);
	_DHP_d4.GetWindowTextW(str);
	ARKdata.DH_d4=_ttof(str);
	_DHP_d6.GetWindowTextW(str);
	ARKdata.DH_d6=_ttof(str);
	_Offset_z.GetWindowTextW(str);
	ARKdata.DH_offset_z=_ttof(str);
	_J45_factor.GetWindowTextW(str);
	ARKdata.J45_factor=_ttof(str);
	_J56_factor.GetWindowTextW(str);
	ARKdata.J56_factor=_ttof(str);
	_DHP_hw.GetWindowTextW(str);
	ARKdata.DHP_hw=_ttof(str);

	CfgReader.set(("ARMKIN"+string("::DHP_a1")).c_str(), ARKdata.DH_a1 ); 
	CfgReader.set(("ARMKIN"+string("::DHP_a2")).c_str(), ARKdata.DH_a2 ); 
	CfgReader.set(("ARMKIN"+string("::DHP_a3")).c_str(), ARKdata.DH_a3 ); 
	CfgReader.set(("ARMKIN"+string("::DHP_d3")).c_str(), ARKdata.DH_d3 ); 
	CfgReader.set(("ARMKIN"+string("::DHP_d4")).c_str(), ARKdata.DH_d4 ); 
	CfgReader.set(("ARMKIN"+string("::DHP_d6")).c_str(), ARKdata.DH_d6 ); 
	CfgReader.set(("ARMKIN"+string("::DHP_offset_z")).c_str(), ARKdata.DH_offset_z ); 

	/*CfgReader.set(("ARMKIN"+string("::J45_factor")).c_str(), ARKdata.J45_factor ); 
	CfgReader.set(("ARMKIN"+string("::J56_factor")).c_str(), ARKdata.J56_factor ); 
	CfgReader.set(("ARMKIN"+string("::DHP_hw")).c_str(), ARKdata.DHP_hw ); */
	CfgReader.write_all();
	MSD.ShowInfo(_T("修改机构参数"));
}

int Setting_MechanismDLG::OnActivateChildWin(LPARAM lp)
{
 	UpdateWindow();
	return 1;
}