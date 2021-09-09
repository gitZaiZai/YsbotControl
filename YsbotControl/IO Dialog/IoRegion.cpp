#include "StdAfx.h"
#include "IoRegion.h"
#include "../WorldModel.h" 
#include "../MotionControl.h"
#include "../FieldBusDevice.h"

BEGIN_MESSAGE_MAP(CIoRegion, CGroupFrame)
	ON_WM_ERASEBKGND()
	ON_MESSAGE(DIO_StnClicked,&CIoRegion::OnDIOStnClicked)
END_MESSAGE_MAP()

CIoRegion::CIoRegion(void) : CGroupFrame(FALSE) 
{
	caption_default = _T("");
	for ( int i = 0; i < 3; ++i )
	{
		DIO_Group[i] = NULL;
	}
	LIO_Group = NULL;
	DIO_Setting = NULL;
	m_nTotalByteNum = 0;
}
 
CIoRegion::~CIoRegion(void)
{
	font_.DeleteObject();

	for ( int i = 0; i < 3; ++i )
	{
		if ( DIO_Group[i] )
		{
			DIO_Group[i]->DestroyWindow();
			delete DIO_Group[i];
			DIO_Group[i] = NULL;
		}
	}

	if ( LIO_Group )
	{
		LIO_Group->DestroyWindow();
		delete LIO_Group;
		LIO_Group = NULL;
	}


	if (DIO_Setting->GetSafeHwnd()) 
	{
		DIO_Setting->DestroyWindow();
	}

	if ( DIO_Setting ) 
	{
		delete DIO_Setting;
		DIO_Setting = NULL;
	}
}
 
BOOL CIoRegion::Create(CString caption,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CStatic::Create( caption.IsEmpty() ? caption_default : caption, dwStyle|SS_NOTIFY, rect, pParentWnd, nID );

	font_.CreateFont(20, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("Arial"));

	LOGFONT lf;
	font_.GetLogFont(&lf);

	const LPCTSTR strTitle[] = 
	{
		_T("语言"),
		_T("日期"),
		_T("时间"),
		_T("运行时间"),

	};
	CRect clientRect;
	GetClientRect(&clientRect);

	DIO_Setting = new CDIOSetting();
	//创建IO强制窗口
	if ( DIO_Setting->CreateDialogFromTemplate(this,FALSE, WS_POPUP) == FALSE ) 
	{
		printf("\n创建IO设置模块失败！\n");  
	}
	DIO_Setting->ShowWindow(SW_HIDE);

	m_nTotalByteNum = creat_io_board();

	return TRUE;
}

int CIoRegion::creat_io_board()
{
	const IoModuleParameters& IO_Para=MWM.get_io_module_parameters(IOSfieldbus);

	X_WordSize=IO_Para.x_word_size;
	MX_WordSize=IO_Para.mx_word_size;
	Y_WordSize=IO_Para.y_word_size;
	MY_WordSize=IO_Para.my_word_size;
	X_Num=IO_Para.x_point_num;
	MX_Num=IO_Para.mx_relay_num;
	Y_Num=IO_Para.y_point_num;
	MY_Num=IO_Para.my_relay_num;
	X_totalSize=X_WordSize+MX_WordSize;
	Y_totalSize=Y_WordSize+MY_WordSize;
	FeildBus_is_connected=IO_Para.is_connected;

	const IoModuleParameters& mccIO_Para=MWM.get_io_module_parameters(IOSmotioncard);
 	mccX_WordSize=mccIO_Para.x_word_size;	
	mccY_WordSize=mccIO_Para.y_word_size;	
	mccX_Num=mccIO_Para.x_point_num;	
	mccY_Num=mccIO_Para.y_point_num;
	mccLocal_input_size=mccIO_Para.local_input_size;
	mcc_is_connected=mccIO_Para.is_connected;	

	IN_num=mccX_Num+X_Num+MX_Num;
	OUT_num=mccY_Num+Y_Num+MY_Num;

	for ( int i = 0; i < 3; ++i )
	{
		DIO_Group[i] = new CDIOGroup(0);
	}

	//创建GPIO模组
	int nWinWidth = 380;
	DIOGroupInfo GInfo;
	GInfo.InIndex=0;
	GInfo.InNum=16;
	GInfo.OutIndex=0;
	GInfo.OutNum=16;
	DIO_Group[0]->Create(_T("GPIO:"),WS_CHILD |WS_VISIBLE |SS_NOTIFY|WS_BORDER ,
		CRect(1,10,nWinWidth,100),this,GInfo);
	DIO_Group[0]->SetDIOFont(18);

	int ByteNum=2+2+1;

	//创建Rio
	GInfo.InIndex=0;
	GInfo.InNum=mccX_Num;
	GInfo.OutIndex=0;
	GInfo.OutNum=mccY_Num;
	DIO_Group[1]->Create(_T("RIO:"),WS_CHILD |WS_VISIBLE |SS_NOTIFY|WS_BORDER ,
		CRect(1,ByteNum*60+5,nWinWidth,100),this,GInfo);
	DIO_Group[1]->SetDIOFont(18);

	ByteNum+=(int)ceil((double)mccX_Num/8.0);
	ByteNum+=(int)ceil((double)mccY_Num/8.0)+1;

	//创建PLC模组	
	GInfo.InIndex=mccX_Num;
	GInfo.InNum=X_Num+MX_Num;
	GInfo.OutIndex=mccY_Num;
	GInfo.OutNum=Y_Num+MY_Num;
	DIO_Group[2]->Create(_T("PLC:"),WS_CHILD |WS_VISIBLE |SS_NOTIFY|WS_BORDER ,
		CRect(1,ByteNum*60+5,nWinWidth,100),this,GInfo);
	DIO_Group[2]->SetDIOFont(18);
	ByteNum+=(int)ceil((double)X_Num/8.0);
	ByteNum+=(int)ceil((double)Y_Num/8.0);
	ByteNum+=(int)ceil((double)MX_Num/8.0);
	ByteNum+=(int)ceil((double)MY_Num/8.0)+1;

 
	//创建LIO模组
	GInfo.InIndex=0;
	GInfo.InNum=34;
	GInfo.OutIndex=0;
	GInfo.OutNum=9;
	LIO_Group = new CLIOGroup();
	LIO_Group->Create(_T("LIO:"),WS_CHILD |WS_VISIBLE |SS_NOTIFY|WS_BORDER ,
		CRect(1,ByteNum*60+5,nWinWidth,100),this,GInfo);
	LIO_Group->SetDIOFont(18);
	//LIO_Group->ShowWindow(SW_HIDE);
	ByteNum+=5;
	ByteNum+=2+1;

	return ByteNum;
}

LRESULT CIoRegion::OnDIOStnClicked(WPARAM wParam,LPARAM lParam)
{
	std::cout << "CIoRegion wParam:"<< wParam <<" ; lParam:" <<lParam <<"\n";
	if ( DIO_Setting )
	{
		DIO_Setting->SetHostPointer((CDIO*)lParam);
 	}
 	return 1;
} 

BOOL CIoRegion::OnEraseBkgnd(CDC* pDC)
{
//  	CBrush brush;
// 	CRect (rc);
// 	GetWindowRect(&rc);
// 	GetParent()->ScreenToClient(&rc);
// 
// 	brush.CreateSolidBrush(RGB(102,102,102)); 
// 	pDC->FillRect(&rc,&brush); // liurundan
//     brush.DeleteObject();

 	return  CStatic::OnEraseBkgnd(pDC);
}

void CIoRegion::UpdateSurface()
{
	CRect (rc);
	GetWindowRect(rc);
	RedrawWindow();

	GetParent()->ScreenToClient(rc);
	GetParent()->InvalidateRect(rc,TRUE);
	GetParent()->UpdateWindow();
}

void CIoRegion::scroll_window(int nYAmount)
{
	ScrollWindow(0,nYAmount);

	UpdateSurface();
}

void CIoRegion::GetLocalIO()
{
	const InOutObjectList& iolist = MWM.get_io_objects(IOSmotioncard);

	unsigned short Alarm = iolist.get_state( InOutObject::alarm_input, 0 );
	unsigned short Home  = iolist.get_state( InOutObject::home_sensor, 0 );
	unsigned short OT_P  = iolist.get_state( InOutObject::ot_positive, 0 );
	unsigned short OT_N  = iolist.get_state( InOutObject::ot_negative, 0 );
	unsigned short E_STOP= iolist.get_state( InOutObject::estop,       0 );

	unsigned short Servo_ON = iolist.get_state( InOutObject::servo_enable,    0 );
	unsigned short Pos_Ready= iolist.get_state( InOutObject::pos_ready,       0 );

	unsigned short TempState=0;
	unsigned short WordNO=0;
	unsigned short BitNO=0;

	//GET IN STATE
	TempState=Alarm;
	WordNO=0;
	for(BitNO=0;BitNO<8;BitNO++)
	{
		LIO_Group->Din[WordNO*8+BitNO].Value=(GETBIT(TempState,BitNO));
		LIO_Group->Din[WordNO*8+BitNO].ShowDIOBitmap();
	}
	TempState=Home;
	WordNO=1;
	for(BitNO=0;BitNO<8;BitNO++)
	{
		LIO_Group->Din[WordNO*8+BitNO].Value=(GETBIT(TempState,BitNO));
		LIO_Group->Din[WordNO*8+BitNO].ShowDIOBitmap();
	}
	TempState=OT_P;
	WordNO=2;
	for(BitNO=0;BitNO<8;BitNO++)
	{
		LIO_Group->Din[WordNO*8+BitNO].Value=(GETBIT(TempState,BitNO));
		LIO_Group->Din[WordNO*8+BitNO].ShowDIOBitmap();
	}
	TempState=OT_N;
	WordNO=3;
	for(BitNO=0;BitNO<8;BitNO++)
	{
		LIO_Group->Din[WordNO*8+BitNO].Value=(GETBIT(TempState,BitNO));
		LIO_Group->Din[WordNO*8+BitNO].ShowDIOBitmap();
	}
	TempState=E_STOP;
	WordNO=4;
	for(BitNO=0;BitNO<2;BitNO++)
	{
		LIO_Group->Din[WordNO*8+BitNO].Value=(GETBIT(TempState,BitNO));
		LIO_Group->Din[WordNO*8+BitNO].ShowDIOBitmap();
	}

	//GET OUT STATE
	TempState=Servo_ON;
	WordNO=0;
	for(BitNO=0;BitNO<8;BitNO++)
	{
		LIO_Group->Dout[WordNO*8+BitNO].Value=(GETBIT(TempState,BitNO));
		LIO_Group->Dout[WordNO*8+BitNO].ShowDIOBitmap();
	}
	TempState=Pos_Ready;
	WordNO=1;
	for(BitNO=0;BitNO<1;BitNO++)
	{
		LIO_Group->Dout[WordNO*8+BitNO].Value=(GETBIT(TempState,BitNO));
		LIO_Group->Dout[WordNO*8+BitNO].ShowDIOBitmap();
	}

}

void CIoRegion::GetGPIO()
{
	const InOutObjectList& iolist = MWM.get_io_objects(IOSmotioncard);

	unsigned short GPin  = iolist.get_state( InOutObject::gpio_input,  0 );
	unsigned short GPout = iolist.get_state( InOutObject::gpio_output, 0 );
	unsigned short TempState=0;
	unsigned short WordNO=0;
	unsigned short BitNO=0;

	//GET IN STATE
	TempState=GPin;
	for(BitNO=0;BitNO<16;BitNO++)
	{
		if((WordNO*16+BitNO)<16) {
			DIO_Group[0]->Din[BitNO].Value=(GETBIT(TempState,BitNO));
			DIO_Group[0]->Din[BitNO].ShowDIOBitmap();}
	}
	//GET OUT STATE

	TempState=GPout;
	for(BitNO=0;BitNO<16;BitNO++)
	{
		if((WordNO*16+BitNO)<16) {
			DIO_Group[0]->Dout[BitNO].Value=(GETBIT(TempState,BitNO));
			DIO_Group[0]->Dout[BitNO].ShowDIOBitmap();}
	}

}

void CIoRegion::GetCardIO()
{
	if (!mcc_is_connected)
	{
		return;
	}
	const InOutObjectList& MotionCard_IO = MWM.get_io_objects(IOSmotioncard);
	unsigned short TempState=0;
	unsigned short WordNO=0;
	unsigned short BitNO=0;

	//GET mccIN STATE
	for(WordNO=0;WordNO<mccX_WordSize;WordNO++)
	{		
		TempState=MotionCard_IO.get_state(InOutObject::ext_input,WordNO);
		for(BitNO=0;BitNO<16;BitNO++)
		{
			if((WordNO*16+BitNO)<mccX_Num) {
				DIO_Group[1]->Din[WordNO*16+BitNO].Value=(GETBIT(TempState,BitNO));
				DIO_Group[1]->Din[WordNO*16+BitNO].ShowDIOBitmap();}
		}
	}
	//GET mccOUT STATE
	for(WordNO=0;WordNO<mccY_WordSize;WordNO++)
	{

		TempState=MotionCard_IO.get_state(InOutObject::ext_output,WordNO);
		for(BitNO=0;BitNO<16;BitNO++)
		{
			if((WordNO*16+BitNO)<mccY_Num) {
				DIO_Group[1]->Dout[WordNO*16+BitNO].Value=(GETBIT(TempState,BitNO));
				DIO_Group[1]->Dout[WordNO*16+BitNO].ShowDIOBitmap();}
		}
	}

}

void CIoRegion::GetFieldBusIO()
{
	if (!FeildBus_is_connected)
	{
		return;
	}
	const InOutObjectList& FeildBus_IO = MWM.get_io_objects(IOSfieldbus);
	unsigned short TempState=0;
	unsigned short WordNO=0;
	unsigned short BitNO=0;

	//GET IN STATE
	for(WordNO=0;WordNO<X_WordSize;WordNO++)
	{
		TempState=FeildBus_IO.get_state(InOutObject::fieldbus_x,WordNO);
		for(BitNO=0;BitNO<16;BitNO++)
		{
			if((WordNO*16+BitNO)<X_Num){
				DIO_Group[2]->Din[WordNO*16+BitNO].Value=(GETBIT(TempState,BitNO));	
				DIO_Group[2]->Din[WordNO*16+BitNO].ShowDIOBitmap();}
		}
	}

	for(unsigned short WordNO=0;WordNO<MX_WordSize;WordNO++)
	{
		TempState=FeildBus_IO.get_state(InOutObject::fieldbus_mx,WordNO);
		for(int BitNO=0;BitNO<16;BitNO++)
		{
			if((WordNO*16+BitNO)<MX_Num){
				DIO_Group[2]->Din[WordNO*16+BitNO+X_Num].Value=(GETBIT(TempState,BitNO));
				DIO_Group[2]->Din[WordNO*16+BitNO+X_Num].ShowDIOBitmap();}
		}
	}

	//GET OUT STATE
	for(WordNO=0;WordNO<Y_WordSize;WordNO++)
	{
		TempState=FeildBus_IO.get_state(InOutObject::filedbus_y,WordNO);
		for(BitNO=0;BitNO<16;BitNO++)
		{
			if((WordNO*16+BitNO)<Y_Num){
				DIO_Group[2]->Dout[WordNO*16+BitNO].Value=(GETBIT(TempState,BitNO));
				DIO_Group[2]->Dout[WordNO*16+BitNO].ShowDIOBitmap();}
		}
	}

	for(unsigned short WordNO=0;WordNO<MY_WordSize;WordNO++)
	{
		TempState=FeildBus_IO.get_state(InOutObject::filedbus_my,WordNO);
		for(int BitNO=0;BitNO<16;BitNO++)
		{
			if((WordNO*16+BitNO)<MY_Num){
				DIO_Group[2]->Dout[WordNO*16+BitNO+Y_Num].Value=(GETBIT(TempState,BitNO));
				DIO_Group[2]->Dout[WordNO*16+BitNO+Y_Num].ShowDIOBitmap();}
		}
	}
}

void CIoRegion::Reflash_Dout()
{

	int ret;
	for (int port=0;port<16;port++)
	{
		if (DIO_Group[0]->Dout[port].Forced)	
		{ //添加GPIO强制输出处理
			SMC.mc_dout(port,DIO_Group[0]->Dout[port].ForceValue,MCS_GPIO);
		}

	}
	for (int port=0;port<mccY_Num;port++)
	{
		if (DIO_Group[1]->Dout[port].Forced)			
			ret=MotionControl::get_main_motion_control().mc_dout(port,DIO_Group[1]->Dout[port].ForceValue,MCS_REMOTEIO);
	}
	for (int port=0;port<MY_Num+Y_Num;port++)
	{
		if (DIO_Group[2]->Dout[port].Forced)
			ret=FieldBusDevice::get_main_fieldbus_device().dout(port,DIO_Group[2]->Dout[port].ForceValue);
	}

	for (int port=0;port<9;port++)
	{
		if (port<8)	//ServoON
		{
			if (LIO_Group->Dout[port].Forced)
			{//添加ServoON强制输出处理
				SMC.mc_dout(port,LIO_Group->Dout[port].ForceValue,MCS_LOCALIO);
			}
		}
		else	//Prdy
		{
			if (LIO_Group->Dout[port].Forced)
			{	//添加Prdy强制输出处理
				if (LIO_Group->Dout[port].Forced)
				{ 
					SMC.mc_dout(10,LIO_Group->Dout[port].ForceValue,MCS_LOCALIO);
				}
			}
		}
	}
}

void CIoRegion::CleanForceValue()
{
	DIO_Group[0]->CleanForceValue(0);
	DIO_Group[1]->CleanForceValue(0);
	DIO_Group[2]->CleanForceValue(0);
	LIO_Group->CleanForceValue(0);
}

int CIoRegion::calc_byte_num(int ntype)
{
	int ByteNum=0;
	switch(ntype)
	{
	case 0:
		ByteNum=2+2+1;
		ByteNum+=(int)ceil((double)mccX_Num/8.0);
		ByteNum+=(int)ceil((double)mccY_Num/8.0)+1;
		ByteNum+=(int)ceil((double)X_Num/8.0);
		ByteNum+=(int)ceil((double)Y_Num/8.0);
		ByteNum+=(int)ceil((double)MX_Num/8.0);
		ByteNum+=(int)ceil((double)MY_Num/8.0)+1;
		ByteNum+=5+2+1;

		break;
	case 1:
		ByteNum=2+1;
		ByteNum+=(int)ceil((double)mccX_Num/8.0)+1;
		ByteNum+=(int)ceil((double)X_Num/8.0);
		ByteNum+=(int)ceil((double)MX_Num/8.0)+1;
		break;
	case 2:
		ByteNum=2+1;
		ByteNum+=(int)ceil((double)mccY_Num/8.0)+1;
		ByteNum+=(int)ceil((double)Y_Num/8.0);
		ByteNum+=(int)ceil((double)MY_Num/8.0)+1;
		break;
	default:
		;
	}
	return ByteNum;
}

int CIoRegion::ShowIOType(int ioType)
{
 	//显示筛选的输入
	CRect TempRect(0,0,0,0);
	CRect LastRect(0,0,0,0);

	unsigned int RectHeight;
	DIO_Group[0]->ShowIOType(ioType);

	DIO_Group[1]->ShowIOType(ioType);	
	DIO_Group[0]->GetMyRect(&LastRect);
	DIO_Group[1]->GetMyRect(&TempRect);
	RectHeight=TempRect.Height();
	TempRect.top=LastRect.bottom+15;
	TempRect.bottom=TempRect.top+RectHeight;
	ScreenToClient(&TempRect);
	DIO_Group[1]->SetMyPos(&TempRect);

	DIO_Group[2]->ShowIOType(ioType);
	DIO_Group[1]->GetMyRect(&LastRect);
	DIO_Group[2]->GetMyRect(&TempRect);
	RectHeight=TempRect.Height();
	TempRect.top=LastRect.bottom+15;
	TempRect.bottom=TempRect.top+RectHeight;
	ScreenToClient(&TempRect);
	DIO_Group[2]->SetMyPos(&TempRect);

	if (ioType==0)
	{
		DIO_Group[2]->GetMyRect(&LastRect);
		LIO_Group->GetMyRect(&TempRect);
		RectHeight=TempRect.Height();
		TempRect.top=LastRect.bottom+15;
		TempRect.bottom=TempRect.top+RectHeight;
		ScreenToClient(&TempRect);
		LIO_Group->SetMyPos(&TempRect);
		LIO_Group->ShowIOType(0);
	}
	else
	{
		LIO_Group->ShowIOType(-1);
	}
 

	return 0;
}

void CIoRegion::ShowLIO()
{
 	DIO_Group[0]->ShowIOType(-1);
	DIO_Group[1]->ShowIOType(-1);
	DIO_Group[2]->ShowIOType(-1);
	LIO_Group->ShowIOType(0);

	CRect TempRect(0,0,0,0);
	LIO_Group->GetMyRect(&TempRect);
	int RectHeight=TempRect.Height();
	TempRect.top=105;
	TempRect.bottom=TempRect.top+RectHeight;
	ScreenToClient(&TempRect);
	LIO_Group->SetMyPos(&TempRect);

	Invalidate();
}


void CIoRegion::Updata_IOState()
{
	if( MWM.is_main_world_model_available == false ) 
		return;

	GetLocalIO();
	GetGPIO();
	GetCardIO();
	GetFieldBusIO();
	Reflash_Dout();

	//Time ti2;
	//GetInOutState();
	////ShowButtonChl();
	//ProcessIO();

}

int CIoRegion::Dout(const int port, const bool val)
{
	int ret=0;
	if (port<mccY_Num)
	{
		if (!DIO_Group[2]->Dout[port].Forced)
			ret=FieldBusDevice::get_main_fieldbus_device().dout(port,val);
	}
	else
	{
		return -1;
	}
	return ret;
}

int CIoRegion::Din(const int port, bool* val)
{
	//	int ret;
	if (port<mccX_Num)
	{
		if (DIO_Group[2]->Din[port].Forced)
			*val=DIO_Group[2]->Din[port].ForceValue;
		else
			*val=DIO_Group[2]->Din[port].Value;
	}
	else
	{
		return -1;
	}

	return 0;
}

int CIoRegion::Mcc_in(const int port, bool* val)
{
	if (port<X_Num+MX_Num)
	{
		if (DIO_Group[1]->Din[port].Forced)
			*val=DIO_Group[1]->Din[port].ForceValue;
		else
			*val=DIO_Group[1]->Din[port].Value;
	}
	else
	{
		return -1;
	}
	return 0;
}

int CIoRegion::Mcc_out(const int port, const bool val)
{
	int ret=0;
	if (port>=Y_Num+MY_Num) 
	{
		if (!DIO_Group[1]->Dout[port].Forced)
		{
			ret=MotionControl::get_main_motion_control().mc_dout(port,val);
		}
	}
	else
	{
		return -1;
	}
	return ret;
}

int CIoRegion::GP_in(const int port, bool* val)
{
	if (port<16)
	{
		if (DIO_Group[0]->Din[port].Forced)
			*val=DIO_Group[0]->Din[port].ForceValue;
		else
			*val=DIO_Group[0]->Din[port].Value;
	}
	else
	{
		return -1;
	}
	return 0;
}

int CIoRegion::GP_out(const int port, const bool val)
{
	int ret=0;
	if (port<16)
	{
		if (!DIO_Group[0]->Dout[port-mccY_Num].Forced)
		{
			//	ret=FieldBusDevice::get_main_fieldbus_device().dout(port,val);
			//添加GPiO的输出处理

		}
	} 
	else
	{
		return -1;
	}

	return ret;
}

int CIoRegion::LIO_in(const int port, bool* val)
{
	if (port<34)
	{
		if (DIO_Group[0]->Din[port].Forced)
			*val=LIO_Group->Din[port].ForceValue;
		else
			*val=LIO_Group->Din[port].Value;
	}
	else
	{
		return -1;
	}
	return 0;
}

int CIoRegion::LIO_out(const int port, const bool val)
{
	int ret=0;
	if (port<9)
	{	//0-7:ALM, 8-15:Home, 16-23:ot+, 24-31:ot-, 32-33,estop
		if (!LIO_Group->Dout[port-mccY_Num].Forced)
		{
			//	ret=FieldBusDevice::get_main_fieldbus_device().dout(port,val);
			//添加GPiO的输出处理
		}
	} 
	else
	{
		return -1;
	}

	return ret;
}


int CIoRegion::get_virtual_input( const int deviceID, const int port, bool* val )
{
	int ret = 0;
	switch (deviceID)
	{
	case MCS_GPIO:     ret = GP_in( port, val );		break;
	case MCS_REMOTEIO: ret = Mcc_in( port, val );		break;
	case MCS_LOCALIO:  ret = LIO_in( port, val );		break;
	case PLC_DEVICE:   ret = Din( port, val );		    break;
	default: break;
	}

	return ret;
}

bool CIoRegion::is_forced_output( const int deviceID, const int port )
{
	bool ret = false;
	switch (deviceID)
	{
	case MCS_GPIO:     ret = DIO_Group[0]->Dout[port].Forced;	break;
	case MCS_REMOTEIO: ret = DIO_Group[1]->Dout[port].Forced;	break;
	case MCS_LOCALIO:  
		if ( port < 8 )
			ret = LIO_Group->Dout[port].Forced;		
		else
			ret = LIO_Group->Dout[8].Forced;	
		break; // ????????????????? prdy
	case PLC_DEVICE:   ret = DIO_Group[2]->Dout[port].Forced;	break;
	default: break;
	}

	return ret;
}