// PngCheckBox.cpp : implementation file
//

#include "stdafx.h"
// #include "YsbotControl.h"
#include "PngCheckBox.h"
#include <atlimage.h>

// CPngCheckBox
 
IMPLEMENT_DYNAMIC(CPngCheckBox, CButton)  

BEGIN_MESSAGE_MAP(CPngCheckBox, CButton)  
	ON_WM_CREATE()  
	ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)  
	ON_WM_ERASEBKGND()  
	ON_WM_CTLCOLOR_REFLECT()  
	ON_CONTROL_REFLECT(BN_DOUBLECLICKED, &CPngCheckBox::OnBnDoubleclicked)
END_MESSAGE_MAP()  

//////////////////////////////////////////////////////////////////////////  

CPngCheckBox::CPngCheckBox()  
{  
	m_bChecked      = FALSE;  
	m_crTextColor   = RGB(0, 0, 0);  
	m_crBack        = RGB(255, 255, 255);  
	m_bSetBack      = FALSE;  
	m_bShowTextFrame = FALSE;  
	m_crTextColor       = DEF_TEXT_COLOR;  
	m_crTextFrameColor  = DEF_TEXT_FRAME_COLOR;  
  
}  

CPngCheckBox::~CPngCheckBox()  
{  
	if (m_ImageCheck)
	{
		delete m_ImageCheck;
		m_ImageCheck = NULL;
	}

	if (m_ImageBack)
	{
		delete m_ImageBack;
 		m_ImageBack = NULL;
	}

	if (m_ImageDisableCheck)
	{
		delete m_ImageDisableCheck;
		m_ImageDisableCheck = NULL;
	}

	if ( m_ImageDisableUncheck)
	{
		delete m_ImageDisableUncheck;
		m_ImageDisableUncheck = NULL;
	}
}  


void CPngCheckBox::PreSubclassWindow()  
{  
	ModifyStyle(0, BS_OWNERDRAW);  
	CButton::PreSubclassWindow();  
}  
 
void CPngCheckBox::Init(UINT nBkImg, UINT nCkImg, UINT nDisableImg, UINT nDisableUnChkImg ) {  
	m_ImageBack  = ImageFromResource(AfxGetResourceHandle(), nBkImg, L"PNG");  
	m_ImageCheck = ImageFromResource(AfxGetResourceHandle(), nCkImg, L"PNG");  
	m_ImageDisableCheck = ImageFromResource(AfxGetResourceHandle(), nDisableImg, L"PNG");
	m_ImageDisableUncheck = ImageFromResource(AfxGetResourceHandle(), nDisableUnChkImg, L"PNG");
	if (m_ImageBack != NULL) {  
		m_bSetBack = TRUE;  
	}  
 
	CRect rcButton;  
// 	rcButton = CRect(0, 0, m_ImageCheck->GetWidth() / 4, m_ImageCheck->GetHeight());  
	rcButton = CRect(0, 0, m_ImageCheck->GetWidth(), m_ImageCheck->GetHeight());  

	SetWindowPos(NULL, 0, 0, rcButton.Width(), rcButton.Height(), SWP_NOACTIVATE | SWP_NOMOVE);  
}  
HBRUSH CPngCheckBox::CtlColor(CDC* pDC, UINT nCtlColor)  
{  
	pDC->SetBkMode(TRANSPARENT);  
	return (HBRUSH)GetStockObject(NULL_BRUSH);  
}  
void CPngCheckBox::SetFontType(int fontSize, CString fontType) {  
	LOGFONT lf;  
	memset(&lf, 0, sizeof(LOGFONT));  
	lf.lfHeight = fontSize;  
	_tcsncpy_s(lf.lfFaceName, LF_FACESIZE, fontType, fontType.GetLength());  
	VERIFY(m_font.CreateFontIndirect(&lf));  
}  

void CPngCheckBox::DrawItem(LPDRAWITEMSTRUCT lpDIS)  
{  
 	CDC dc;  
	dc.Attach(lpDIS->hDC);  
	CRect rcItem(lpDIS->rcItem);  

	Gdiplus::Graphics graph(dc.GetSafeHdc()); 
	Gdiplus::Image* currImg = NULL;
	if ( IsWindowEnabled() )
	{
		currImg = m_bChecked? m_ImageCheck:m_ImageBack;
 	}
	else
	{
		currImg = m_bChecked? m_ImageDisableCheck : m_ImageDisableUncheck;
  	}
	graph.DrawImage(currImg, Gdiplus::RectF(0, 0, static_cast<Gdiplus::REAL>(rcItem.Width()),
		static_cast<Gdiplus::REAL>(rcItem.Height()) ), 
		static_cast<Gdiplus::REAL>(rcItem.left), 
		static_cast<Gdiplus::REAL>(rcItem.top), 
		static_cast<Gdiplus::REAL>(rcItem.Width()), 
		static_cast<Gdiplus::REAL>(rcItem.Height()), 
		Gdiplus::UnitPixel );  
	graph.ReleaseHDC(dc.GetSafeHdc());  
	std::cout << "draw img ptr = " << (long)currImg << '\n';
	 
// 	if (m_bSetBack)  
// 	{  
// 		Graphics graph(lpDIS->hDC);  
// 		graph.DrawImage(m_ImageBack, RectF(0, 0, rcItem.Width(), rcItem.Height()), rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height(), UnitPixel);  
// 		graph.ReleaseHDC(lpDIS->hDC);  
// 	}  
// 	if (m_ImageCheck != NULL)  
// 	{  
// 		CRect rcButton;  
// // 		int imgWidth = m_ImageCheck->GetWidth() / 4;  
// 		int imgWidth = m_ImageCheck->GetWidth();  
// 		int imgHeight = m_ImageCheck->GetHeight();  
// 		if (m_bChecked && IsWindowEnabled())  
// 			rcButton = CRect(0, 0, imgWidth, imgHeight);  
// 		else if (m_bChecked && !IsWindowEnabled())  
// 			rcButton = CRect(2* imgWidth, 0, 3*imgWidth, imgHeight);  
// 		else if (!m_bChecked && IsWindowEnabled())  
// 			rcButton = CRect(imgWidth, 0, 2*imgWidth, imgHeight);  
// 		else  
// 			rcButton = CRect(3* imgWidth, 0, 4*imgWidth, imgHeight);  
// 
// 		Graphics graph(dc.GetSafeHdc());  
// 		graph.DrawImage(m_ImageCheck, 0, 0, rcButton.left, rcButton.top, rcButton.Width(), rcButton.Height(), UnitPixel);  
// 		graph.ReleaseHDC(dc.GetSafeHdc());  
// 		//绘画字体  
// 		CString szText;  
// 		GetWindowText(szText);  
// 		dc.SetBkMode(TRANSPARENT);  
// 		CFont* oldFont = dc.SelectObject(m_font.GetSafeHandle() ? &m_font : GetFont());  
// 
// 		DrawTextString(&dc, szText, m_crTextColor, m_crTextFrameColor, imgWidth + 4, 0);  
// 		dc.SelectObject(oldFont);  
// 	}  
	dc.Detach();  
}  
 
void CPngCheckBox::SetCheckBoxTextColor(COLORREF crTextColor)  
{  
	m_crTextColor = crTextColor;  
}  
//设置颜色  
bool CPngCheckBox::SetTextColor(COLORREF crTextColor, COLORREF crTextFrameColor, bool bShowFrame)  
{  
	m_crTextColor = crTextColor;  
	m_bShowTextFrame = bShowFrame;  
	m_crTextFrameColor = crTextFrameColor;  

	if (GetSafeHwnd()) Invalidate(FALSE);  
	return true;  
} 

Gdiplus::Image * CPngCheckBox::ImageFromResource(HINSTANCE hInstance, UINT uImgID, LPCTSTR lpType)  
{  
 	HRSRC hResInfo = ::FindResource(hInstance, MAKEINTRESOURCE(uImgID), lpType);  
	if (hResInfo == NULL)  
		return NULL; //fail    
	DWORD dwSize;  
	dwSize = SizeofResource(hInstance, hResInfo); //get resource size(bytes)     
	HGLOBAL hResData;  
	hResData = ::LoadResource(hInstance, hResInfo);  
	if (hResData == NULL)  
		return NULL; //fail    
	HGLOBAL hMem;  
	hMem = ::GlobalAlloc(GMEM_MOVEABLE, dwSize);  
	if (hMem == NULL) {  
		::FreeResource(hResData);  
		return NULL;  
	}  
	LPVOID lpResData, lpMem;  
	lpResData = ::LockResource(hResData);  
	lpMem = ::GlobalLock(hMem);  
	::CopyMemory(lpMem, lpResData, dwSize); //copy memory    
	::GlobalUnlock(hMem);  
	::FreeResource(hResData); //free memory    

	IStream *pStream;  
	HRESULT hr;  
	hr = ::CreateStreamOnHGlobal(hMem, TRUE, &pStream);//create stream object    
	Gdiplus::Image *pImage = NULL;  
	if (SUCCEEDED(hr)) {  
		pImage = Gdiplus::Image::FromStream(pStream);//get GDI+ pointer    
		pStream->Release();  
	}  
	::GlobalFree(hMem);  
	return pImage;  
}  

BOOL CPngCheckBox::OnClicked()  
{  
	if (!m_bChecked)  
		m_bChecked = TRUE;  
	else  
		m_bChecked = FALSE;  

	Invalidate();
	PaintParent();  
	std::cout << "checkBox = " << (int)m_bChecked << '\n';

	return FALSE;  
}  

BOOL CPngCheckBox::OnEraseBkgnd(CDC* pDC)  
{  
	return TRUE;  
}  
void CPngCheckBox::PaintParent()  
{  
	CRect   rect;  
	GetWindowRect(&rect);  
	GetParent()->ScreenToClient(&rect);  
	GetParent()->InvalidateRect(&rect);  
}  
//艺术字体  
void CPngCheckBox::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, LPRECT lpRect)  
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
void CPngCheckBox::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos)  
{  
	if (nYPos == 0) {  
		LOGFONT lf;  
		if (m_font.GetSafeHandle()) {  
			m_font.GetLogFont(&lf);  
		}  
		else {  
			GetFont()->GetLogFont(&lf);  
		}  
		nYPos = (m_ImageCheck->GetHeight()-abs(lf.lfHeight))/2;  
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

void CPngCheckBox::OnBnDoubleclicked()
{
	m_bChecked = TRUE;  
	PaintParent();  
}
