#include "stdafx.h"
#include "InoutDialog.h"
#include "../stringconvert.h"
#include "../FieldBusDevice.h"
#include "../MotionControl.h"
#include <cstring>
#include "../WorldModel.h"
#include "../colourcode.h"
#include "../LuaScript/ScriptFunc.h"
#include "../SkinScrollBar/skinscrollwnd.h" 

InoutDialog *Inout_Dialog = NULL;

InoutDialog::InoutDialog(const ConfigReader& vr,
	HBITMAP hBmpScrollBar, 
	HBITMAP hBmpPlus, 
	HBITMAP hBmpMinus ) : m_hBmpScrollBar(hBmpScrollBar),
	m_hBmpPlus(hBmpPlus), m_hBmpMinus(hBmpMinus),m_bUseCustomScrollBar(false),m_wndScrollBar(NULL)
{
	for ( int i = 0; i < 3; ++i )
	{
		DIO_Group[i] = NULL;
	}
	DIO_Setting=nullptr;
	 
}

InoutDialog::~InoutDialog() 
{
 	//UnskinWndScroll(this);

 	for ( int i = 0; i < 3; ++i )
	{
		if ( DIO_Group[i] )
		{
			DIO_Group[i]->DestroyWindow();
			delete DIO_Group[i];
			DIO_Group[i] = NULL;
		}
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

	if ( m_wndScrollBar )
	{
		delete m_wndScrollBar;
		m_wndScrollBar = NULL;
	}
}

BEGIN_MESSAGE_MAP(InoutDialog, DialogType)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_MESSAGE(DIO_StnClicked,&InoutDialog::OnDIOStnClicked)
	ON_WM_CREATE()
END_MESSAGE_MAP()

int InoutDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	//m_ScrollBarWnd = SkinWndScroll( this, m_hBmpScrollBar, 0, SKIN_WINDOW_USERLIST );
	return 0;
}

void InoutDialog::DoDataExchange(CDataExchange* pDX) 
{
	;
}

void InoutDialog::on_custom_scroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if ( pScrollBar->m_hWnd != m_wndScrollBar->m_hWnd )
 		return;
 
	int Scroll_Jump = m_YCharHeigth;
	 
	SCROLLINFO scrollinfo = {0};
 	m_wndScrollBar->GetScrollInfo(&scrollinfo, SIF_ALL);
  
	char chpos[50] = {'\0'};
	sprintf(chpos,"scrollpos1 = %d",scrollinfo.nPos);
	int org_npos = scrollinfo.nPos;
	int nYAmountWindow = 0;

 	int SCROLLBAR_STEP = 1;
 	switch(nSBCode) {
 	case SB_BOTTOM:
 		{
			nYAmountWindow = scrollinfo.nPos - scrollinfo.nMax + scrollinfo.nPage;
  			scrollinfo.nPos = scrollinfo.nMax - scrollinfo.nPage;
  		}
 		break;
 	case SB_TOP:
 		{
			nYAmountWindow = scrollinfo.nPos;
  			scrollinfo.nPos = 0;
  
 		}
 		break;
 	case SB_LINEUP://上箭头
 		{
  			scrollinfo.nPos -= m_YCharHeigth;
			if ( scrollinfo.nPos < scrollinfo.nMin )
			{
				scrollinfo.nPos = scrollinfo.nMin;
 			}
            nYAmountWindow = m_YCharHeigth;
  		}
 		break;
 	case SB_LINEDOWN://下箭头
 		{
  			scrollinfo.nPos += m_YCharHeigth;
			if ( scrollinfo.nPos > scrollinfo.nMax  )  
			{  
				scrollinfo.nPos = scrollinfo.nMax; 
 			} 
			nYAmountWindow = -m_YCharHeigth;
   		}
 		break;
 	case SB_PAGEUP://上空白
 		{
 			 ;
 		}
 		break;
 	case SB_PAGEDOWN://下空白
 		{
 			;
 		}
 		break;
 	case SB_ENDSCROLL://结束
  		break;
 	case SB_THUMBPOSITION://拖拽的最终位置
  		break;
 	case SB_THUMBTRACK://拖拽过程中的位置
 		{
 		;
 		}
 		break;
 	}

	CRect rectWind;
	GetClientRect(rectWind);
	rectWind.DeflateRect( 0, 0, 36,0);
 	ScrollWindow( 0, nYAmountWindow, &rectWind, &rectWind );
	RedrawWindow(&rectWind);
	m_wndScrollBar->SetScrollInfo( &scrollinfo );

	//printf("ileft = %d, iJump = %d \n",iLeft,iJump);
 
	if ( org_npos != scrollinfo.nPos )
	{
		printf("%s scrollpos2 = %d | nPos = %d | swin = %d \n",chpos, scrollinfo.nPos, nPos, nYAmountWindow );
 	}


}

void InoutDialog::on_system_scroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int Scroll_Jump=10;

	SCROLLINFO scrollinfo;
	GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);

	//std::cout <<scrollinfo.nPage <<"; ";

	switch(nSBCode) {
	case SB_BOTTOM:
		{
			ScrollWindow(0, scrollinfo.nPos - scrollinfo.nMax + scrollinfo.nPage);
			scrollinfo.nPos = scrollinfo.nMax - scrollinfo.nPage;
			SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		}
		break;
	case SB_TOP:
		{
			ScrollWindow(0, scrollinfo.nPos);
			scrollinfo.nPos = 0;
			SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		}
		break;
	case SB_LINEUP://上箭头
		{
			int iLeft = scrollinfo.nPos, iJump = (iLeft > Scroll_Jump)?Scroll_Jump:iLeft;
			scrollinfo.nPos -= iJump;
			SetScrollInfo(SB_VERT, &scrollinfo, TRUE);
			ScrollWindow(0, iJump);
		}
		break;
	case SB_LINEDOWN://下箭头
		{
			int iLeft = scrollinfo.nMax - scrollinfo.nPage - scrollinfo.nPos, iJump = (iLeft > Scroll_Jump)?Scroll_Jump:iLeft;
			scrollinfo.nPos += iJump;
			SetScrollInfo(SB_VERT, &scrollinfo, TRUE);
			ScrollWindow(0, 0 - iJump);
		}
		break;
	case SB_PAGEUP://上空白
		{
			int iLeft = scrollinfo.nPos, iJump = (iLeft > Scroll_Jump*Scroll_Jump)?(Scroll_Jump*Scroll_Jump):iLeft;
			scrollinfo.nPos -= iJump;
			SetScrollInfo(SB_VERT, &scrollinfo, TRUE);
			ScrollWindow(0, iJump);
		}
		break;
	case SB_PAGEDOWN://下空白
		{
			int iLeft = scrollinfo.nMax - scrollinfo.nPage - scrollinfo.nPos;
			int iJump = (iLeft > Scroll_Jump*Scroll_Jump)?(Scroll_Jump*Scroll_Jump):iLeft;
			scrollinfo.nPos += iJump;
			SetScrollInfo(SB_VERT, &scrollinfo, TRUE);
			ScrollWindow(0, 0 - iJump);
		}
		break;
	case SB_ENDSCROLL://结束
		// MessageBox("SB_ENDSCROLL");
		break;
	case SB_THUMBPOSITION://拖拽的最终位置
		// ScrollWindow(0,(scrollinfo.nPos-nPos)*SCROLLBAR_STEP);
		// scrollinfo.nPos = nPos;
		// SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);
		break;
	case SB_THUMBTRACK://拖拽过程中的位置
		{
			ScrollWindow(0, scrollinfo.nPos - nPos);
			scrollinfo.nPos = nPos;
			SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		}
		break;
	}
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void InoutDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bUseCustomScrollBar)
	{
		on_custom_scroll(nSBCode,nPos,pScrollBar);
	}
	else
	{
		on_system_scroll(nSBCode,nPos,pScrollBar);
	}
}

BOOL InoutDialog::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if ( m_bUseCustomScrollBar )
	{
		int Scroll_Jump=10;
 		SCROLLINFO scrollinfo;
		m_wndScrollBar->GetScrollInfo(&scrollinfo, SIF_ALL);

		if(zDelta > 0) { // 向上滚
			int iMulti = zDelta/WHEEL_DELTA;//倍数
			int iLeft = scrollinfo.nPos, iJump = (iLeft > Scroll_Jump*Scroll_Jump*iMulti)?(Scroll_Jump*Scroll_Jump*iMulti):iLeft;
			scrollinfo.nPos -= iJump;
			m_wndScrollBar->SetScrollInfo(&scrollinfo, FALSE);
			ScrollWindow(0, iJump);
		}
		else { // 向下滚
			int iMulti = zDelta/(0 - WHEEL_DELTA);//倍数
			int iLeft = scrollinfo.nMax - scrollinfo.nPage - scrollinfo.nPos, iJump = (iLeft > Scroll_Jump*Scroll_Jump*iMulti)?(Scroll_Jump*Scroll_Jump*iMulti):iLeft;
			scrollinfo.nPos += iJump;
			m_wndScrollBar->SetScrollInfo(&scrollinfo, FALSE);
			ScrollWindow(0, 0 - iJump);
		}
		return CDialog::OnMouseWheel(nFlags, zDelta, pt);
	}
	else
	{
		int Scroll_Jump=10;
 		SCROLLINFO scrollinfo;
		GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		if(zDelta > 0) { // 向上滚
			int iMulti = zDelta/WHEEL_DELTA;//倍数
			int iLeft = scrollinfo.nPos, iJump = (iLeft > Scroll_Jump*Scroll_Jump*iMulti)?(Scroll_Jump*Scroll_Jump*iMulti):iLeft;
			scrollinfo.nPos -= iJump;
			SetScrollInfo(SB_VERT, &scrollinfo, TRUE);
			ScrollWindow(0, iJump);
		}
		else { // 向下滚
			int iMulti = zDelta/(0 - WHEEL_DELTA);//倍数
			int iLeft = scrollinfo.nMax - scrollinfo.nPage - scrollinfo.nPos, iJump = (iLeft > Scroll_Jump*Scroll_Jump*iMulti)?(Scroll_Jump*Scroll_Jump*iMulti):iLeft;
			scrollinfo.nPos += iJump;
			SetScrollInfo(SB_VERT, &scrollinfo, TRUE);
			ScrollWindow(0, 0 - iJump);
		}
		return CDialog::OnMouseWheel(nFlags, zDelta, pt);
	}

}

BOOL InoutDialog::OnInitDialog() 
{
 	CDialogEx::OnInitDialog();
	SetWindowPos(NULL,g_x0 + 22, 95, 724, 439, SWP_NOZORDER);
	SetBackgroundColor(kColor[8], TRUE);
	TEXTMETRIC tm;
	HDC  hdc= GetDC()->GetSafeHdc();
	GetTextMetrics(hdc,&tm);
	long xChar=tm.tmAveCharWidth;
	long xUpper=(tm.tmPitchAndFamily?3:2)/2*xChar;
	m_YCharHeigth=tm.tmExternalLeading+tm.tmHeight;
	//初始化控件
#pragma region Get IO parameters
	FeildBus_is_connected=0;
	if (MWM.is_main_world_model_available()==false)
	{
		is_MWM_enable=false;
		MessageBox (_T("you must initialize the Worldmodel before this dialog"));
		return FALSE;
	}
	is_MWM_enable=true;
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
#pragma endregion

#pragma region Get MotionCard_IO parameters
	mcc_is_connected=0;
	const IoModuleParameters& mccIO_Para=MWM.get_io_module_parameters(IOSmotioncard);
	mccX_WordSize=mccIO_Para.x_word_size;	
	mccY_WordSize=mccIO_Para.y_word_size;	
	mccX_Num=mccIO_Para.x_point_num;	
	mccY_Num=mccIO_Para.y_point_num;
	mccLocal_input_size=mccIO_Para.local_input_size;
	mcc_is_connected=mccIO_Para.is_connected;	
#pragma endregion

#pragma region Initialize IO array

	IN_num=mccX_Num+X_Num+MX_Num;
	OUT_num=mccY_Num+Y_Num+MY_Num;


#pragma endregion

	////////创建IO模块组/////////////////
	//创建IO设置弹窗

	DIO_Setting = new CDIOSetting;
	//创建IO强制窗口
	if ( DIO_Setting->CreateDialogFromTemplate(this,FALSE, WS_POPUP) == FALSE ) 
	{
		printf("\n创建IO设置模块失败！\n");  
	}
 	DIO_Setting->ShowWindow(SW_HIDE);

	int nWidthChild = 0;
	if ( m_bUseCustomScrollBar )
	{
		nWidthChild = 40;
	}

	for ( int i = 0; i < 3; ++i )
	{
		DIO_Group[i] = new CDIOGroup(nWidthChild);
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
	LIO_Group.Create(_T("LIO:"),WS_CHILD |WS_VISIBLE |SS_NOTIFY|WS_BORDER ,
		CRect(1,ByteNum*60+5,nWinWidth,100),this,GInfo);
	LIO_Group.SetDIOFont(18);
	//LIO_Group.ShowWindow(SW_HIDE);
	ByteNum+=5;
	ByteNum+=2+1;
 
// 	//设置滚动条信息
 	SCROLLINFO scrollinfo;
 	scrollinfo.cbSize = sizeof(SCROLLINFO);
 	scrollinfo.fMask = SIF_ALL;
 	scrollinfo.nMin = 0;
 	scrollinfo.nMax = 10+ByteNum*60;
 	scrollinfo.nPage = 439;//每页的高度也就是窗口高度，滚动条的长度=nPage/nMax
 	scrollinfo.nPos = 0;
 
	m_YCharHeigth = 60;

	if ( m_bUseCustomScrollBar )
	{
		CRect rectWind;
		GetClientRect(rectWind);
		int iHeight = rectWind.Height();
		int iWidth = rectWind.Width();

		m_wndScrollBar = new CSkinScrollBar();
	    m_wndScrollBar->Create(WS_CHILD | WS_VISIBLE | SBS_VERT | SBS_LEFTALIGN ,//子控件 可见 垂直滚动条
		CRect(CPoint(iWidth-32,0),CSize(32,iHeight)), this, IDC_VERT_SCROLLBAR ); 
	    //m_wndScrollBar->SetScrollRange(0, 999999);
		m_wndScrollBar->SetScrollInfo(&scrollinfo, FALSE);
		m_wndScrollBar->SetBitmap(m_hBmpScrollBar);

  	}
	else
	{
	    SetScrollInfo(SB_VERT, &scrollinfo, TRUE);
	}

  	return FALSE;
}

BOOL InoutDialog::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return CDialogEx::OnEraseBkgnd(pDC);
}

void InoutDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialogEx::OnPaint()
}

void InoutDialog::OnOK() {
	//DestroyWindow();
}

// void InoutDialog::OnCancel() {
// 	DestroyWindow();
// }

LRESULT InoutDialog::OnDIOStnClicked(WPARAM wParam,LPARAM lParam)
{
	std::cout << "InoutDialog wParam:"<< wParam <<" ; lParam:" <<lParam <<"\n";
	DIO_Setting->SetHostPointer((CDIO*)lParam);
// 	AfxMessageBox(_T("clicked io checkbox!"));

	return 1;
}

LRESULT InoutDialog::OnMainButtonClicked( WPARAM wp, LPARAM lp)
{
	int btnid  = (int)(wp);
	int menuid = (int)(lp);

	switch(btnid)
	{
	case 0:
		ShowIOType(0);
 		break;
	case 1:
		ShowIOType(1);
 		break;
	case 2:
		ShowIOType(2);
 		break;
	case 3:
		ShowLIO();
 		break;
	case 4:
		CleanForceValue();
		break;
	case 5:
		//CRect rect;
		//DIO_Group[0].GetClientRect(rect);
		break;

	default:
		break;
	}
	return 1;
}

void InoutDialog::GetLocalIO()
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
		LIO_Group.Din[WordNO*8+BitNO].Value=(GETBIT(TempState,BitNO));
		LIO_Group.Din[WordNO*8+BitNO].ShowDIOBitmap();
	}
	TempState=Home;
	WordNO=1;
	for(BitNO=0;BitNO<8;BitNO++)
	{
		LIO_Group.Din[WordNO*8+BitNO].Value=(GETBIT(TempState,BitNO));
		LIO_Group.Din[WordNO*8+BitNO].ShowDIOBitmap();
	}
	TempState=OT_P;
	WordNO=2;
	for(BitNO=0;BitNO<8;BitNO++)
	{
		LIO_Group.Din[WordNO*8+BitNO].Value=(GETBIT(TempState,BitNO));
		LIO_Group.Din[WordNO*8+BitNO].ShowDIOBitmap();
	}
	TempState=OT_N;
	WordNO=3;
	for(BitNO=0;BitNO<8;BitNO++)
	{
		LIO_Group.Din[WordNO*8+BitNO].Value=(GETBIT(TempState,BitNO));
		LIO_Group.Din[WordNO*8+BitNO].ShowDIOBitmap();
	}
	TempState=E_STOP;
	WordNO=4;
	for(BitNO=0;BitNO<2;BitNO++)
	{
		LIO_Group.Din[WordNO*8+BitNO].Value=(GETBIT(TempState,BitNO));
		LIO_Group.Din[WordNO*8+BitNO].ShowDIOBitmap();
	}

	//GET OUT STATE
	TempState=Servo_ON;
	WordNO=0;
	for(BitNO=0;BitNO<8;BitNO++)
	{
		LIO_Group.Dout[WordNO*8+BitNO].Value=(GETBIT(TempState,BitNO));
		LIO_Group.Dout[WordNO*8+BitNO].ShowDIOBitmap();
	}
	TempState=Pos_Ready;
	WordNO=1;
	for(BitNO=0;BitNO<1;BitNO++)
	{
		LIO_Group.Dout[WordNO*8+BitNO].Value=(GETBIT(TempState,BitNO));
		LIO_Group.Dout[WordNO*8+BitNO].ShowDIOBitmap();
	}

}
	
// void InoutDialog::GetCardIO()
// {
// 	if (!mcc_is_connected)
// 	{
// 		return;
// 	}
// 	const InOutObjectList& MotionCard_IO = MWM.get_io_objects(IOSmotioncard);
// 	unsigned short TempState=0;
// 	unsigned short WordNO=0;
// 	unsigned short BitNO=0;
// 
// 	//GET mccIN STATE
// 	for(WordNO=0;WordNO<mccX_WordSize;WordNO++)
// 	{		
// 		TempState=MotionCard_IO.get_state(InOutObject::ext_input,WordNO);
// 		for(BitNO=0;BitNO<16;BitNO++)
// 		{
// 			if((WordNO*16+BitNO)<mccX_Num) {
// 				DIO_Group[0].Din[WordNO*16+BitNO].Value=(GETBIT(TempState,BitNO));
// 				DIO_Group[0].Din[WordNO*16+BitNO].ShowDIOBitmap();}
// 		}
// 	}
// 	//GET mccOUT STATE
// 	for(WordNO=0;WordNO<mccY_WordSize;WordNO++)
// 	{
//  		
// 		TempState=MotionCard_IO.get_state(InOutObject::ext_output,WordNO);
// 		for(BitNO=0;BitNO<16;BitNO++)
// 		{
// 			if((WordNO*16+BitNO)<mccY_Num) 
// 			{
//  				DIO_Group[0].Dout[WordNO*16+BitNO].Value=(GETBIT(TempState,BitNO));
// 				DIO_Group[0].Dout[WordNO*16+BitNO].ShowDIOBitmap();
//  			}
//  		}
// 	}
// 
// }

void InoutDialog::GetGPIO()
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

void InoutDialog::GetCardIO()
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

void InoutDialog::GetFieldBusIO()
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

void InoutDialog::Reflash_Dout()
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
			if (LIO_Group.Dout[port].Forced)
			{//添加ServoON强制输出处理
				SMC.mc_dout(port,LIO_Group.Dout[port].ForceValue,MCS_LOCALIO);
			}
		}
		else	//Prdy
		{
			if (LIO_Group.Dout[port].Forced)
			{	//添加Prdy强制输出处理
				if (LIO_Group.Dout[port].Forced)
				{ 
					SMC.mc_dout(10,LIO_Group.Dout[port].ForceValue,MCS_LOCALIO);
				}
			}
		}
 	}
 }

void InoutDialog::CleanForceValue()
{
	DIO_Group[0]->CleanForceValue(0);
	DIO_Group[1]->CleanForceValue(0);
	DIO_Group[2]->CleanForceValue(0);
	LIO_Group.CleanForceValue(0);
}

void InoutDialog::ShowIOType(int ioType)
{
	SCROLLINFO scrollinfo;
	//移动滚动条到顶部
	GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
	ScrollWindow(0, scrollinfo.nPos);
	scrollinfo.nPos = 0;
	int ByteNum=0;
	switch(ioType)
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
	scrollinfo.nMax = 10+ByteNum*60;
	SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);

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
		LIO_Group.GetMyRect(&TempRect);
		RectHeight=TempRect.Height();
		TempRect.top=LastRect.bottom+15;
		TempRect.bottom=TempRect.top+RectHeight;
		ScreenToClient(&TempRect);
		LIO_Group.SetMyPos(&TempRect);
		LIO_Group.ShowIOType(0);
	}
	else
	{
		LIO_Group.ShowIOType(-1);
	}


	Invalidate();
}

void InoutDialog::ShowLIO()
{
	SCROLLINFO scrollinfo;
	//移动滚动条到顶部
	GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
	ScrollWindow(0, scrollinfo.nPos);
	scrollinfo.nPos = 0;
	scrollinfo.nMax = 10+8*60;
	//scrollinfo.nPage = 439;//每页的高度也就是窗口高度，滚动条的长度=nPage/nMax
	SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);

	DIO_Group[0]->ShowIOType(-1);
	DIO_Group[1]->ShowIOType(-1);
	DIO_Group[2]->ShowIOType(-1);
	LIO_Group.ShowIOType(0);

	CRect TempRect(0,0,0,0);
	LIO_Group.GetMyRect(&TempRect);
	int RectHeight=TempRect.Height();
	TempRect.top=105;
	TempRect.bottom=TempRect.top+RectHeight;
	ScreenToClient(&TempRect);
	LIO_Group.SetMyPos(&TempRect);

	Invalidate();
}


void InoutDialog::Updata_IOState()
{
	if(!is_MWM_enable) return;
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

int InoutDialog::Dout(const int port, const bool val)
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

int InoutDialog::Din(const int port, bool* val)
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

int InoutDialog::Mcc_in(const int port, bool* val)
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

int InoutDialog::Mcc_out(const int port, const bool val)
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

int InoutDialog::GP_in(const int port, bool* val)
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

int InoutDialog::GP_out(const int port, const bool val)
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

int InoutDialog::LIO_in(const int port, bool* val)
{
	if (port<34)
	{
		if (DIO_Group[0]->Din[port].Forced)
			*val=LIO_Group.Din[port].ForceValue;
		else
			*val=LIO_Group.Din[port].Value;
	}
	else
	{
		return -1;
	}
	return 0;
}

int InoutDialog::LIO_out(const int port, const bool val)
{
	int ret=0;
	if (port<9)
	{	//0-7:ALM, 8-15:Home, 16-23:ot+, 24-31:ot-, 32-33,estop
		if (!LIO_Group.Dout[port-mccY_Num].Forced)
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


int InoutDialog::get_virtual_input( const int deviceID, const int port, bool* val )
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

bool InoutDialog::is_forced_output( const int deviceID, const int port )
{
	bool ret = false;
	switch (deviceID)
	{
	case MCS_GPIO:     ret = DIO_Group[0]->Dout[port].Forced;	break;
	case MCS_REMOTEIO: ret = DIO_Group[1]->Dout[port].Forced;	break;
	case MCS_LOCALIO:  
		if ( port < 8 )
 			ret = LIO_Group.Dout[port].Forced;		
 		else
			ret = LIO_Group.Dout[8].Forced;	
		break; // ????????????????? prdy
	case PLC_DEVICE:   ret = DIO_Group[2]->Dout[port].Forced;	break;
	default: break;
	}

	return ret;
}