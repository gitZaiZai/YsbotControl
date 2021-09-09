#include "stdafx.h"
#include "SettingIODlg.h"
#include "../stringconvert.h"
#include "../FieldBusDevice.h"
#include "../WorldModel.h"
#include "../colourcode.h"
#include "../LuaScript/ScriptFunc.h"
#include "../SkinScrollBar/skinscrollwnd.h" 

CSettingIODlg *m_pwndDlgIO = NULL;

CSettingIODlg::CSettingIODlg(const ConfigReader& vr,
	HBITMAP hBmpScrollBar, 
	HBITMAP hBmpPlus, 
	HBITMAP hBmpMinus ) : m_hBmpScrollBar(hBmpScrollBar),
	m_hBmpPlus(hBmpPlus), m_hBmpMinus(hBmpMinus),m_bUseCustomScrollBar(true),m_wndScrollBar(NULL),m_wndIoRegion(NULL)
{
 
}

CSettingIODlg::~CSettingIODlg() 
{
 	if ( m_wndScrollBar )
	{
		m_wndScrollBar->DestroyWindow();
 		delete m_wndScrollBar;
		m_wndScrollBar = NULL;
	}
	if ( m_wndIoRegion )
	{
		m_wndIoRegion->DestroyWindow();
		delete m_wndIoRegion;
		m_wndIoRegion = NULL;
	}
}

BEGIN_MESSAGE_MAP(CSettingIODlg, DialogType)
 	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_ERASEBKGND()

END_MESSAGE_MAP()

BOOL CSettingIODlg::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	//SetWindowPos(NULL,g_x0 + 22, 95, 730, 439, SWP_NOZORDER);

	if ( GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetSafeHwnd() )
	{
		GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetClientRect(&clientRect);
		SetWindowPos(NULL,clientRect.left, clientRect.top, clientRect.Width(), clientRect.Height(), SWP_NOZORDER);
	}

	SetBackgroundColor(kColor[8], TRUE);
	CRect rectClient;
	GetClientRect(rectClient);
	int iHeight = rectClient.Height();
	int iWidth = rectClient.Width();
	m_iBarWidth = 35;
	m_rectIoRegion = rectClient;

	m_rectIoRegion.DeflateRect(0,0,m_iBarWidth,0);
	// 	TEXTMETRIC tm;
	// 	HDC  hdc= GetDC()->GetSafeHdc();
	// 	GetTextMetrics(hdc,&tm);
	// 	long xChar=tm.tmAveCharWidth;
	// 	long xUpper=(tm.tmPitchAndFamily?3:2)/2*xChar;
	// 	m_YCharHeigth=tm.tmExternalLeading+tm.tmHeight;
 
	m_wndIoRegion = new CIoRegion();
    m_wndIoRegion->Create( _T(""), WS_CHILD|SS_NOTIFY|WS_VISIBLE | WS_CLIPSIBLINGS, m_rectIoRegion, this, IDC_IO_REGION );
 	int ByteNum = m_wndIoRegion->m_nTotalByteNum;

	m_YCharHeigth = 50;

	SCROLLINFO scrollinfo;
	scrollinfo.cbSize = sizeof(SCROLLINFO);
	scrollinfo.fMask = SIF_ALL;
	scrollinfo.nMin = 0;
	scrollinfo.nMax = 10+ByteNum*m_YCharHeigth;
	scrollinfo.nPage = scrollinfo.nMax/rectClient.Height(); // 每页的高度也就是窗口高度，滚动条的长度=nPage/nMax
	scrollinfo.nPos = 0;

 
	m_wndScrollBar = new CSkinScrollBar();
	m_wndScrollBar->Create(WS_CHILD | WS_VISIBLE | SBS_VERT | SBS_LEFTALIGN , 
		CRect(CPoint(iWidth-m_iBarWidth,0),CSize(m_iBarWidth,iHeight)), this, IDC_VERT_SCROLLBAR ); 
	m_wndScrollBar->SetScrollInfo(&scrollinfo, FALSE);
	m_wndScrollBar->SetBitmap(m_hBmpScrollBar);

	return FALSE;
}

void CSettingIODlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if ( m_bUseCustomScrollBar )
	{
		// 		CRect rectWind;
		// 		this->GetClientRect(rectWind);
		// 		int iHeight = rectWind.Height();
		// 		int iWidth = rectWind.Width();
		// 		//CRect rect(iWidth-30, 0, iWidth, iHeight);//滚动条显示在对话框右边，宽为15 
		// 		CRect rect(CPoint(iWidth-35,0),CSize(35,iHeight));
		// 		m_wndScrollBar->MoveWindow(rect);
		// 		m_wndScrollBar->Invalidate();	
	}
}
  
void CSettingIODlg::on_custom_scroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if ( pScrollBar->m_hWnd != m_wndScrollBar->m_hWnd )
		return;

	CRect rectClient;
	m_wndIoRegion->GetClientRect(rectClient);

	int Scroll_Jump = m_YCharHeigth;

	SCROLLINFO scrollinfo = {0};
	scrollinfo.fMask = SIF_ALL;
	m_wndScrollBar->GetScrollInfo(&scrollinfo, SIF_ALL);

	char chpos[50] = {'\0'};
	sprintf(chpos,"scrollpos1 = %d",scrollinfo.nPos);
	int org_npos = scrollinfo.nPos;
	int nYAmountWindow = 0;

 	bool bScrollWindow = true;
	switch(nSBCode) 
	{
	case SB_BOTTOM:
		{
			nYAmountWindow = -rectClient.Height();
 		}
		break;
	case SB_TOP:
		{
			;
 		}
		break;
	case SB_LINEUP: 
		{
			scrollinfo.nPos -= m_YCharHeigth;
			if ( scrollinfo.nPos < scrollinfo.nMin )
			{
				scrollinfo.nPos = scrollinfo.nMin;
				bScrollWindow = false;
 			}
			nYAmountWindow = m_YCharHeigth;
		}
		break;
	case SB_LINEDOWN: 
		{
			scrollinfo.nPos += m_YCharHeigth;
			if ( scrollinfo.nPos > (int)(scrollinfo.nMax - scrollinfo.nMin-scrollinfo.nPage+m_YCharHeigth)  )  
			{  
				scrollinfo.nPos = scrollinfo.nMax - scrollinfo.nMin-scrollinfo.nPage+m_YCharHeigth; 
				bScrollWindow = false;
			} 
			nYAmountWindow = -m_YCharHeigth;
		}
		break;
	case SB_PAGEUP: 
		{
			scrollinfo.nPos -= 5*m_YCharHeigth; 
			if ( scrollinfo.nPos < scrollinfo.nMin )  
			{  
				scrollinfo.nPos = scrollinfo.nMin; 
				bScrollWindow = false;
 			}  
			nYAmountWindow = 5*m_YCharHeigth;
 		}
		break;
	case SB_PAGEDOWN: 
		{
			scrollinfo.nPos += 5*m_YCharHeigth; 
			if ( scrollinfo.nPos > (int)(scrollinfo.nMax - scrollinfo.nMin-scrollinfo.nPage+m_YCharHeigth)  )  
			{  
				scrollinfo.nPos = scrollinfo.nMax - scrollinfo.nMin-scrollinfo.nPage+m_YCharHeigth; 
				bScrollWindow = false;
 			} 
			nYAmountWindow = -1 * (int)(5*m_YCharHeigth);
 		}
		break;
	case SB_ENDSCROLL: 
		break;
	case SB_THUMBPOSITION://拖拽的最终位置
		break;
	case SB_THUMBTRACK://拖拽过程中的位置
		{
			nYAmountWindow = (int)((scrollinfo.nPos-nPos));  
			scrollinfo.nPos = nPos; 
 		}
		break;
	}
 	 
	//printf("ileft = %d, iJump = %d \n",iLeft,iJump);

	if ( org_npos != scrollinfo.nPos && bScrollWindow )
	{
		m_wndIoRegion->scroll_window( nYAmountWindow );
		 
 		scrollinfo.fMask = SIF_POS;
		m_wndScrollBar->SetScrollInfo( &scrollinfo );

//  		CRect rect;
//  		(GetDlgItem(IDC_IO_REGION))->GetWindowRect(rect);     // 获取要刷新的STATIC控件的刷新区域
//  		ScreenToClient(rect);   // 屏幕上指定点的屏幕坐标转换成用户坐标
//  		InvalidateRect(rect);
//  		UpdateWindow() ;
 		 
	}

	printf("%s scrollpos2 = %d | nPos = %d | swin = %d \n",chpos, scrollinfo.nPos, nPos, nYAmountWindow );
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}
 
void CSettingIODlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
 	on_custom_scroll(nSBCode,nPos,pScrollBar);
}

BOOL CSettingIODlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	 return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}
 
LRESULT CSettingIODlg::OnMainButtonClicked( WPARAM wp, LPARAM lp)
{
	int btnid  = (int)(wp);
	int menuid = (int)(lp);

	switch(btnid)
	{
	case 0:	case 1:	case 2:
		filiter_io_show(btnid);
		break;
 	case 3:show_local_io();	break;
	case 4:clear_force_value();	break;
	case 5:		break;
 	default:		break;
	}
	return 1;
}
 
BOOL CSettingIODlg::OnEraseBkgnd(CDC* pDC)
{
	return  CDialogEx::OnEraseBkgnd(pDC);
}

void CSettingIODlg::filiter_io_show( int nType )
{
 	SCROLLINFO scrollinfo = {0};
	scrollinfo.fMask = SIF_ALL;
 	 
	m_wndScrollBar->GetScrollInfo(&scrollinfo, SIF_ALL);
	m_wndIoRegion->scroll_window(scrollinfo.nPos);
  	scrollinfo.nPos = 0;

	int ByteNum=m_wndIoRegion->calc_byte_num(nType);
 
	scrollinfo.nMax = 10+ByteNum*60;
	m_wndScrollBar->SetScrollInfo(&scrollinfo);

	m_wndIoRegion->ShowIOType(nType);


	Invalidate();


}

void CSettingIODlg::show_local_io()
{
	SCROLLINFO scrollinfo = {0};
	scrollinfo.fMask = SIF_ALL;

 	m_wndScrollBar->GetScrollInfo(&scrollinfo, SIF_ALL);
	m_wndIoRegion->scroll_window(scrollinfo.nPos);
	scrollinfo.nPos = 0;
	scrollinfo.nMax = 10+8*60;
 	m_wndScrollBar->SetScrollInfo(&scrollinfo);

	m_wndIoRegion->ShowLIO();

	Invalidate();
}

void CSettingIODlg::clear_force_value()
{
	m_wndIoRegion->CleanForceValue();
}

void CSettingIODlg::update_io_status()
{
	m_wndIoRegion->Updata_IOState();
}

int CSettingIODlg::get_virtual_input( const int deviceID, const int port, bool* val )
{
 	return m_wndIoRegion->get_virtual_input(deviceID,port,val);
}

bool CSettingIODlg::is_forced_output( const int deviceID, const int port )
{
 	return m_wndIoRegion->is_forced_output(deviceID,port);
}