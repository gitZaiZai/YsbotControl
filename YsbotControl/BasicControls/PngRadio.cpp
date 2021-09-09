// PngRadio.cpp : implementation file
//

#include "stdafx.h"
#include "PngRadio.h"


// CPngRadio
IMPLEMENT_DYNAMIC(CPngRadio, CButton)  
 
CPngRadio::CPngRadio()  
{  
	m_bChecked = FALSE;  
	m_crTextColor = RGB(0, 0, 0);  
	m_crBack = RGB(255, 255, 255);  
	m_bSetBack = FALSE;  
	m_bShowTextFrame = FALSE;  
	m_crTextColor = DEF_TEXT_COLOR;  
	m_crTextFrameColor = DEF_TEXT_FRAME_COLOR;  

	for ( int i = 0; i < 4; ++i )
	{
		m_hIcon[i] = NULL;
	}

	m_imgWidth = 16;  
	m_imgHeight = 16;  

}  

CPngRadio::~CPngRadio()  
{
	m_ImageBack.Destroy();
	m_ImageCheck.Destroy();
	m_ImageUncheck.Destroy();
	m_ImageDisable.Destroy();
	m_ImageDisableCheck.Destroy();
	m_font.DeleteObject();

	for ( int i = 0; i < 4; ++i )
	{
		DestroyIcon(m_hIcon[i]);
	}
}  

BEGIN_MESSAGE_MAP(CPngRadio, CButton)  
	ON_WM_CREATE()  
	ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)  
	ON_WM_ERASEBKGND()  
	ON_WM_CTLCOLOR_REFLECT()  
END_MESSAGE_MAP()  


void CPngRadio::PreSubclassWindow()  
{  
	ModifyStyle(0, BS_OWNERDRAW);  
	CButton::PreSubclassWindow();  
}  

void CPngRadio::Init(UINT nBkImg, UINT nCkImg, UINT nUnCkImg, UINT nDisableImg, UINT nDisableCkImg ) 
{  
 	m_ImageCheck.LoadFromResource(AfxGetInstanceHandle(), nCkImg);
	m_ImageUncheck.LoadFromResource(AfxGetInstanceHandle(), nUnCkImg);
	m_ImageDisable.LoadFromResource(AfxGetInstanceHandle(), nDisableImg);
	m_ImageDisableCheck.LoadFromResource(AfxGetInstanceHandle(), nDisableCkImg);
	if ( nBkImg != 0 )
	{
		m_ImageBack.LoadFromResource(AfxGetInstanceHandle(), nBkImg);
		if (m_ImageBack != NULL) 
		{  
			m_bSetBack = TRUE;  
		}  
	}
  
// 	CRect rcButton;  
// 	rcButton = CRect(0, 0, m_ImageCheck.GetWidth(), m_ImageCheck.GetHeight());  
// 
// 	SetWindowPos(NULL, 0, 0, rcButton.Width(), rcButton.Height(), SWP_NOACTIVATE | SWP_NOMOVE);  
}

void CPngRadio::InitImage(LPCTSTR BkPath, LPCTSTR CkPath, LPCTSTR UnCkPath, LPCTSTR DisablePath,  LPCTSTR CkDisablePath)
{
 	if ( BkPath )
 	{
		m_ImageBack.Load(BkPath);
  	}
 	 
	m_ImageCheck.Load(CkPath);
	m_ImageUncheck.Load(UnCkPath);
	m_ImageDisable.Load(DisablePath);
	m_ImageDisableCheck.Load(CkDisablePath);

    m_imgWidth = m_ImageCheck.GetWidth();
	m_imgHeight = m_ImageCheck.GetHeight();
}

void CPngRadio::InitIcon(LPCTSTR CkPath, LPCTSTR UnCkPath, LPCTSTR DisablePath, LPCTSTR CkDisablePath)
{
 	HINSTANCE hInstance = AfxGetInstanceHandle( );
	m_hIcon[CHECKED] = (HICON)LoadImage(hInstance, CkPath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	m_hIcon[UNCHECKED] = (HICON)LoadImage(hInstance, UnCkPath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	m_hIcon[DISABLEUNCHECK] = (HICON)LoadImage(hInstance, DisablePath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	m_hIcon[DISABLECHECKED] = (HICON)LoadImage(hInstance, CkDisablePath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

	GetIconSize(m_hIcon[CHECKED],&m_imgWidth,&m_imgHeight);
}

void CPngRadio::GetIconSize(HICON hIcon, int* cx, int* cy)
{
	ICONINFO ici;
	::GetIconInfo(hIcon, &ici);
	BITMAP bm;
	::GetObject(ici.hbmColor, sizeof(BITMAP), &bm);
	*cx = bm.bmWidth;
	*cy = bm.bmHeight;
	::DeleteObject(ici.hbmColor);
	::DeleteObject(ici.hbmMask);
}
 
HBRUSH CPngRadio::CtlColor(CDC* pDC, UINT nCtlColor)  
{  
	pDC->SetBkMode(TRANSPARENT);  
	return (HBRUSH)GetStockObject(NULL_BRUSH);  
}  

void CPngRadio::SetFontType(int fontSize, CString fontType) 
{  
	LOGFONT lf;  
	memset(&lf, 0, sizeof(LOGFONT));  
	lf.lfHeight = fontSize;  
	_tcsncpy_s(lf.lfFaceName, LF_FACESIZE, fontType, fontType.GetLength());  
	VERIFY(m_font.CreateFontIndirect(&lf));  
}  

void CPngRadio::DrawItem(LPDRAWITEMSTRUCT lpDIS)  
{ 
   	CDC dc;  
	dc.Attach(lpDIS->hDC);  
	CRect rcItem(lpDIS->rcItem);  
    
	CMemDC MemDC(dc, rcItem);
	CDC& mdc = MemDC.GetDC();

  	mdc.SetStretchBltMode(COLORONCOLOR);
 
	BOOL ret = FALSE;
	if (m_bSetBack)  
	{  
 		//ret = m_ImageBack.Draw( MemDC.GetDC(), rcItem,Gdiplus::InterpolationModeHighQuality ); 
 	}  
  	if ( m_ImageCheck && m_ImageUncheck && m_ImageDisable && m_ImageDisableCheck )  
	{  
 		CRect rcButton = rcItem;
		rcButton.right = rcButton.left + m_imgWidth;
		rcButton.InflateRect(0,0);
		
		HICON hicon_active = NULL;
		if (m_bChecked && IsWindowEnabled())  
		{
			if( m_ImageCheck.IsNull() == false )
			    ret = m_ImageCheck.Draw( MemDC.GetDC(), rcButton ); 
			//hicon_active = m_hIcon[CHECKED];
		}
 		else if (m_bChecked && !IsWindowEnabled())  
		{
			if( m_ImageDisableCheck.IsNull() == false )
				ret = m_ImageDisableCheck.Draw( MemDC.GetDC(), rcButton  );  
			//hicon_active = m_hIcon[DISABLECHECKED];
 		}
  		else if (!m_bChecked && IsWindowEnabled())  
		{
			if( m_ImageUncheck.IsNull() == false )
				ret = m_ImageUncheck.Draw( MemDC.GetDC(), rcButton  );   
			//hicon_active = m_hIcon[UNCHECKED];

		}
 		else  
		{
			if( m_ImageDisable.IsNull() == false )
				ret = m_ImageDisable.Draw( MemDC.GetDC(), rcButton  );  
			//hicon_active = m_hIcon[DISABLEUNCHECK];
 		}
		if ( hicon_active )
		{
			DrawIconEx(mdc, rcButton.left, rcButton.top, hicon_active, rcButton.Width(), rcButton.Height(), 0, 0, DI_NORMAL);
 		}
         
		//绘画字体  
		mdc.SetBkMode(TRANSPARENT);

   		CFont* oldFont = MemDC.GetDC().SelectObject(m_font.GetSafeHandle() ? &m_font : GetFont());  
 
		//绘制编辑框文字
		CString cstrBuffer;
		GetWindowText(cstrBuffer);
		CRect rcText = rcItem;
		 
		rcText.OffsetRect(m_imgWidth-2,0);

		CBrush br(RGB(220,220,220));
 		MemDC.GetDC().FillRect(rcText,&br);
 		br.DeleteObject();

 		//MemDC.GetDC().SetBkColor(m_crTextFrameColor);
		int offsetText = 10;
		MemDC.GetDC().SetTextColor(m_crTextColor);
		rcText.OffsetRect(offsetText,0);
	 
		DrawText(MemDC.GetDC(), cstrBuffer, cstrBuffer.GetLength(), rcText, DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_WORD_ELLIPSIS);

  		//DrawTextString(&MemDC.GetDC(), cstrBuffer, m_crTextColor, m_crTextFrameColor, rcText);  
  		MemDC.GetDC().SelectObject(oldFont);  
	}  
	dc.Detach();  
  
}  
//设置颜色  
bool CPngRadio::SetTextColor(COLORREF crTextColor, COLORREF crTextFrameColor, bool bShowFrame)  
{  
	m_crTextColor = crTextColor;  
	m_bShowTextFrame = bShowFrame;  
	m_crTextFrameColor = crTextFrameColor;  

	if (GetSafeHwnd()) Invalidate(FALSE);  
	return true;  
}  
 
BOOL CPngRadio::OnClicked()  
{  
	SetChecked();  
	return FALSE;  
} 

void CPngRadio::SetChecked(BOOL bChecked)  
{   
	if (bChecked) 
	{  
		if (!m_vcRadio.empty()) 
		{  
			for (unsigned int i = 0; i < m_vcRadio.size(); ++i )  
			{  
				CPngRadio* e = m_vcRadio[i];
// 				if ( !e->IsWindowEnabled() )
// 				{
// 					continue;
// 				}
				e->SetChecked(FALSE);  
			}  
 		}  
	}  
	m_bChecked = bChecked;  
  	//PaintParent();  
	Invalidate(FALSE);  
	CWnd* pWnd = GetParent();
	if ( pWnd != NULL && ::IsWindow(pWnd->m_hWnd) )
	{
		pWnd->SendMessage(WM_COMMAND,BN_CLICKED, (WPARAM)GetDlgCtrlID());
	}
}  

BOOL CPngRadio::OnEraseBkgnd(CDC* pDC)  
{  
	return TRUE;  
} 

void CPngRadio::PaintParent()  
{  
	CRect   rect;  
	GetClientRect(&rect);  
	GetParent()->ScreenToClient(&rect);  
	GetParent()->InvalidateRect(&rect);  
}  

//艺术字体  
void CPngRadio::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, LPRECT lpRect)  
{  
	//变量定义  
	int nStringLength = lstrlen(pszString);  
	int nXExcursion[8] = { 1,1,1,0,-1,-1,-1,0 };  
	int nYExcursion[8] = { -1,0,1,1,1,0,-1,-1 };  

	CRect rcDraw;  
	if (m_bShowTextFrame)  
	{  
		//绘画边框  
		pDC->SetTextColor(crFrame);  
		for (int i = 0; i < sizeof(nXExcursion) / sizeof(nXExcursion[0]); ++i)  
		{  
			rcDraw.CopyRect(lpRect);  
			rcDraw.OffsetRect(nXExcursion[i], nYExcursion[i]);  
			pDC->DrawText(pszString, nStringLength, &rcDraw, DT_VCENTER | DT_CENTER | DT_SINGLELINE | DT_END_ELLIPSIS);  
		}  
	}  

	//绘画字体  
	rcDraw.CopyRect(lpRect);  
	pDC->SetTextColor(crText);  
	pDC->DrawText(pszString, nStringLength, &rcDraw, DT_VCENTER | DT_CENTER | DT_SINGLELINE | DT_END_ELLIPSIS);  
	return;  
}  

//艺术字体  
void CPngRadio::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos)  
{  
	if (nYPos == 0) {  
		LOGFONT lf;  
		if (m_font.GetSafeHandle()) {  
			m_font.GetLogFont(&lf);  
		}  
		else {  
			GetFont()->GetLogFont(&lf);  
		}  
		nYPos = (m_ImageCheck.GetHeight() - abs(lf.lfHeight)) / 2;  
	}  
	//变量定义  
	int nStringLength = lstrlen(pszString);  
	int nXExcursion[8] = { 1,1,1,0,-1,-1,-1,0 };  
	int nYExcursion[8] = { -1,0,1,1,1,0,-1,-1 };  

	if (m_bShowTextFrame)  
	{  
		//绘画边框  
		pDC->SetTextColor(crFrame);  
		for (int i = 0; i < sizeof(nXExcursion) / sizeof(nXExcursion[0]); i++)  
		{  
			pDC->TextOut(nXPos + nXExcursion[i], nYPos + nYExcursion[i], pszString, nStringLength);  
		}  
	}  
	//绘画字体  
	pDC->SetTextColor(crText);  
	pDC->TextOut(nXPos, nYPos, pszString, nStringLength);  
	return;  
}  

void CPngRadio::AddGroup(std::vector<CPngRadio*> vcRadio) 
{  
 	for (unsigned int i = 0; i < vcRadio.size(); ++i )  
	{  
		CPngRadio* e = vcRadio[i];
		e->SetGroup(vcRadio);  
	}  
} 

void CPngRadio::SetGroup(std::vector<CPngRadio*> vcRadio) 
{  
	m_vcRadio = vcRadio;  
}  