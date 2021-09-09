#include "stdafx.h"
#include "SettingDlg_Origin.h"
#include <cstring>
#include "../Keyboard/digital_keyboard.h"
#include "../colourcode.h"
#include "../Time.h"
#include "../Servo.h"
#include "../MotionControl.h"
#include "../WorldModel.h"
#include "../UIDialogs/state_dialog.h"
#include "../Servo.h"
#include "../SQLiteDB.h"
#include "../LuaScript/ScriptVariables.h"

Setting_OriginDLG::Setting_OriginDLG(const ConfigReader& vread):CfgReader(const_cast<ConfigReader&>(vread))
{
	OriginPos.set_axis_deg( 0, 0, 90, 0, 0,0,0,0);

}

Setting_OriginDLG::~Setting_OriginDLG() 
{


}

BEGIN_MESSAGE_MAP(Setting_OriginDLG, DialogType)
// 	ON_WM_ERASEBKGND()
// 	ON_WM_PAINT()
	//ON_BN_CLICKED(IDB_SETTINGDLG_LOGINBUTTON,LoginBtn_clicked)
	//ON_EN_CHANGE(IDE_SETTINGDLG_USERPASSWORD,UserPwd_change)
	//ON_CBN_SELENDOK(IDC_SETTINGDLG_USERNAME,UserName_SelEndOK)
	//ON_CBN_SELENDOK(IDC_SETTINGDLG_LANGUAGE,Language_SelEndOK)

END_MESSAGE_MAP()


void Setting_OriginDLG::DoDataExchange(CDataExchange* pDX) 
{

}

// BOOL Setting_OriginDLG::OnEraseBkgnd(CDC* pDC)
// {
// 	// TODO: 在此添加消息处理程序代码和/或调用默认值
// 
// 	return CDialogEx::OnEraseBkgnd(pDC);
// }
// 
// 
// void Setting_OriginDLG::OnPaint()
// {
// 	CPaintDC dc(this); // device context for painting
// 	// TODO: 在此处添加消息处理程序代码
// 	// 不为绘图消息调用 CDialogEx::OnPaint()
// 
// }



BOOL Setting_OriginDLG::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	SetWindowPos(NULL,g_x0 + 22, 95, 724, 439, SWP_NOZORDER);
	SetBackgroundColor(kColor[8], TRUE);
	Group1.Create(_T(""),WS_CHILD|WS_VISIBLE|WS_GROUP|BS_GROUPBOX,
		CRect(10,40,370,410),this,IDB_SETTINGDLG_OriginGROUP);
	Group2.Create(_T(""),WS_CHILD|WS_VISIBLE|WS_GROUP|BS_GROUPBOX,
		CRect(380,40,705,410),this,IDB_SETTINGDLG_OriginGROUP+1);
	char chr[10];
	CString str;
	//创建文字标签
	label[0].Create(_T("定义原点"),WS_CHILD| WS_VISIBLE,CRect(140,10,250,35),this);
	label[0].SetFont(26);
	label[1].Create(_T("重置选定"),WS_CHILD| WS_VISIBLE,CRect(480,10,600,35),this);
	label[1].SetFont(26);

	//创建编辑控件
	font_.CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("宋体"));
	//创建原点位置控件
	for (int i=0;i<8;i++)
	{
		sprintf(chr,"J%d轴:",i+1);
		str=chr;
		label[2+i].Create(str,WS_CHILD| WS_VISIBLE,CRect(25,65+i*40,60,85+i*40),this);
		label[2+i].SetFont(20);
		orgPos[i].Create(WS_CHILD|WS_VISIBLE|WS_BORDER,
			CRect(100,60+i*40,240,85+i*40),this,IDE_SETTINGDLG_OriginPos);
		orgPos[i].SetFont(&font_);
		//orgPos[i].SetKB(DKB_);
		label[10+i].Create(_T("(度)"),WS_CHILD| WS_VISIBLE,
			CRect(250,65+i*40,300,85+i*40),this);
		label[10+i].SetFont(20);
	}
	//创建重置选定控件
	for (int i=0;i<8;i++)
	{
		sprintf(chr,"J%d",i+1);
		str=chr;
		resetCheck[i].Create(str,WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX,
			CRect(400,60+i*40,440,85+i*40),this,IDB_SETTINGDLG_OriginRstChk);
		resetCheck[i].SetFont(&font_);

		orgPulse[i].Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_READONLY,
			CRect(480,60+i*40,640,85+i*40),this,IDE_SETTINGDLG_OriginPulse);
		orgPulse[i].SetFont(&font_);
// 		orgPulse[i].SetKB(DKB_);
	}

	if ( SMC.isCodeSysMotion )
	{
		OriginPos.set_axis_deg( 0, -20, -10, 0, 35,0,0,0);
	}

	PutOrgPosData();

	return FALSE;
}

LRESULT Setting_OriginDLG::OnMainButtonClicked( WPARAM wp, LPARAM lp)
{
	int btnid  = (int)(wp);
	int menuid = (int)(lp);

	const GroupProperties& gprop = MWM.get_group_properties();

	switch(btnid)
	{
	case 0:	//定义原点按钮 
		{
			printf("define all home position \n");	
		    define_motion_start_position();
 			MSD.ShowInfo(_T("定义原点"),JTMESSAGE);
		}
 		break;
	case 1:	//全部重置按钮
		{
			if ( SMC.isCodeSysMotion )
			{
				// save home position to file
				save_home_postion_encode();
			}
			else
			{
				int ret = 0;
				bool resetSuccess = true;
				for (unsigned int i = 0; i < gprop.axis_num; ++i )
				{
					ret = Servo::get_main_servo().clear_pos(i);
					if ( ret != 0 )
						resetSuccess = false;
				}
				if ( resetSuccess )
					define_motion_start_position();

			}
 		}
		printf("reset all home position \n");	
 		MSD.ShowInfo(_T("重置所有原点"),JTMESSAGE);
		break;
	case 2: //重置选定按钮
		{
			PosInfo currpos = SMC.get_position();

			BOOL bReset[MAX_JOINT_NUM] = {FALSE};
			memset(bReset,FALSE,sizeof(bReset));
			getAxisChkValue( bReset, gprop.axis_num );

			char axisName[VAR_STR_LEN];
			char axisVal[VAR_STR_LEN];

			bool resetSuccess = true;

			for (unsigned int i = 0; i < gprop.axis_num; ++i )
			{
				if ( bReset[i] )
				{
					int ret = Servo::get_main_servo().clear_pos(i);
					if ( ret == 0 )
					{
						sprintf(axisName,"a%d: ANGLE",i+1);
						sprintf(axisVal,"%lf", get_define_origin_pos(i+1) );
						currpos.set_value( axisName, axisVal );
					}
 				}
			}

			CmdInfo cmd;
			cmd.iCommandType   = CMD_SETPOSITION;
			cmd.customStartPos = true;
			cmd.startPos = currpos; 
  			SMC.setCmdInfo(cmd); 
		}
		printf("reset select home position\n");	
 		MSD.ShowInfo(_T("重置选定原点"),JTMESSAGE);

 		break;
	case 3: //读取数据按钮
		{
			if ( SMC.isCodeSysMotion )
			{
				PosInfo currpos = SMC.get_position();
				setOrgPluse(currpos);
 			}
			else
			{
				const PosInfo& spos = MWM.get_servo_position();
				if ( spos.valid )
				{
					// setOrgPos(spos);
					setOrgPluse(spos);
				}
			}

		}
		printf("read home position\n");	
		break;
	case 4: 
		{
			printf("ptp home position\n");
			if ( MWM.get_robot_state().operate == omTEACH )
			{
				CmdInfo cmd;
				cmd.iCommandType  = CMD_PTP;
				cmd.destPos       = getOrgPos(); 
				cmd.dyn.custom_parameter = false;
				cmd.dyn.JointVel  = MWM.get_external_speed() > 10 ? 10 : MWM.get_external_speed();
				SMC.setCmdInfo(cmd); 
 				MSD.ShowInfo(_T("返回原点"),JTMESSAGE);

 			}
			else
			{
			    MSD.ShowInfo(_T("请切换到示教模式！"),JTMESSAGE);
 			}
 		}
		break;
	case 5: 
		ShowWindow(SW_HIDE); 
		GetParent()->SendMessage(WM_ACTIVATE_WINDOW,winLastWindow,0);
		break;
	default:break;
	}
	return 1;
}

void Setting_OriginDLG::define_motion_start_position()
{
	CmdInfo cmd;
 	cmd.iCommandType   = CMD_SETPOSITION;
	cmd.customStartPos = true;
	cmd.startPos       = getOrgPos(); 
	SMC.setCmdInfo(cmd); 
}

PosInfo Setting_OriginDLG::getOrgPos()
{
	UpdateData(TRUE);

 	double jointPos[8];
	for ( int i=0; i<8; i++ )
	{
		jointPos[i] = get_define_origin_pos(i+1);
	}

	OriginPos.set_axis_deg(jointPos[0],jointPos[1],jointPos[2],jointPos[3],jointPos[4],jointPos[5],jointPos[6],jointPos[7]);
	SMC.set_home_pos(OriginPos);

	return OriginPos;
}

double Setting_OriginDLG::get_define_origin_pos(int axisID)
{
	CString str;
 	if ( axisID > 0 && axisID < 8 )
 		orgPos[axisID-1].GetWindowTextW(str);
 
	return _ttof(str);
}

void Setting_OriginDLG::setOrgPos(PosInfo orgAxisPos)
{
	OriginPos=orgAxisPos;
	PutOrgPosData();
} 

void Setting_OriginDLG::PutOrgPosData()
{
	double jointPos[8];
	CString str;
	AxisPos PosTemp=OriginPos.axis;
	jointPos[0]=PosTemp.q1;
	jointPos[1]=PosTemp.q2;
	jointPos[2]=PosTemp.q3;
	jointPos[3]=PosTemp.q4;
	jointPos[4]=PosTemp.q5;
	jointPos[5]=PosTemp.q6;
	for (int i=0;i<6;i++)
	{		
		str.Format(_T("%.4lf"),rad2deg(jointPos[i]));
		orgPos[i].SetWindowTextW(str);
	}
	str.Format(_T("%.4lf"), rad2deg(OriginPos.auxi.q1) );
	orgPos[6].SetWindowTextW(str);
	str.Format(_T("%.4lf"), rad2deg(OriginPos.auxi.q2) );
	orgPos[7].SetWindowTextW(str);

}  

void Setting_OriginDLG::setOrgPluse(PosInfo orgAxisPluse)
{
	OriginPulse=orgAxisPluse;
	AxisPos PosTemp=OriginPulse.axis;
	double jointPos[8];
	CString str;
	jointPos[0]=PosTemp.q1;
	jointPos[1]=PosTemp.q2;
	jointPos[2]=PosTemp.q3;
	jointPos[3]=PosTemp.q4;
	jointPos[4]=PosTemp.q5;
	jointPos[5]=PosTemp.q6;

	for (int i=0;i<6;i++)
	{		
		str.Format(_T("%.4lf"),rad2deg(jointPos[i]));
		orgPulse[i].SetWindowTextW(str);
	}
	str.Format(_T("%.4lf"), rad2deg(OriginPulse.auxi.q1) );
	orgPulse[6].SetWindowTextW(str);
	str.Format(_T("%.4lf"), rad2deg(OriginPulse.auxi.q2) );
	orgPulse[7].SetWindowTextW(str);

}

void Setting_OriginDLG::getAxisChkValue(BOOL* BooleanArray,unsigned int arraySize)
{
	UpdateData(TRUE);

	for (unsigned int i=0;i<arraySize;i++)
	{
		if( i>=8 ) {return;}
		if(resetCheck[i].GetCheck()==BST_CHECKED)
		{
			*(BooleanArray+i)=TRUE;
		}
		else	
		{
			*(BooleanArray+i)=FALSE;
		}

	}
}


int Setting_OriginDLG::OnActivateChildWin(LPARAM lp)
{
	UpdateWindow();
	return 1;
}

void Setting_OriginDLG::save_home_postion_encode()
{
 	PosInfo currpos = SMC.get_position();
 
	unsigned int num = 0;

	std::vector<std::string> strAxis;
	strAxis.clear();

	CfgReader.get("joints", strAxis);
	if ( strAxis.size() == 0 )
		throw InvalidConfigurationException ("joints home");
	else
		num = strAxis.size();
 
	char strValue[VAR_STR_LEN] = {'\0'};
	char prop[VAR_STR_LEN] = {'\0'};

	for ( unsigned int j = 0; j < num; j++ )
	{
		VARINFO->vardesc[svAXISPOS]->getValue(j+1,prop);
		currpos.get_value (prop, strValue );
		if ( strstr(prop,"ANGLE") )
		{
			double tmp = atof( strValue ? strValue : 0 );
			CfgReader.set((strAxis[j]+string("::encoder_deg")).c_str(), rad2deg(tmp) );
 		}
	}
	strAxis.clear();

	CfgReader.write_all();

	MSD.ShowInfo(_T("保存编码器原点"),JTMESSAGE);
	 
}

