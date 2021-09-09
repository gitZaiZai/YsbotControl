#include "stdafx.h"
#include "StaticTransparent.h"

IMPLEMENT_DYNAMIC(CStaticTran, CStatic)

	BEGIN_MESSAGE_MAP(CStaticTran, CStatic)
		//ON_WM_CREATE()
		ON_WM_PAINT()
		ON_WM_ERASEBKGND()
//		ON_CONTROL_REFLECT(STN_CLICKED, &CStaticTran::OnStnClicked)
	END_MESSAGE_MAP()

CStaticTran::CStaticTran()
{
	FontHeight = 16;
	FontStr = _T("Arial");

	VERIFY(m_font.CreateFont(
		FontHeight,                // nHeight
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
		FontStr) );                 // lpszFacename


}

CStaticTran::~CStaticTran()
{
	m_font.DeleteObject();

}


void CStaticTran::OnPaint()
{
	static BOOL bFirst = TRUE;
 	CPaintDC dc(this); // device context for painting
	CRect rcStatic, rcParent;
	GetWindowRect(&rcStatic);
	//GetClientRect(&rcStatic);
	if(bFirst)//这里把父窗口所画的保存起来
	{
		CRect rcStaticInParent;
		bFirst = FALSE;
		CWnd* pParent = GetParent();

		rcStaticInParent = rcStatic;
		pParent->ScreenToClient(&rcStaticInParent);

		CDC* pParentDC = pParent->GetDC();
		CBitmap m_memBp;
		m_memBp.CreateCompatibleBitmap(&dc, rcStaticInParent.Width(), rcStaticInParent.Height());
		m_memDC.CreateCompatibleDC(&dc);
		m_memDC.SelectObject(m_memBp.GetSafeHandle());
		//SYS_DEBUG_OUT(Output_Console,"rcStaticInParent.left=%d; rcStaticInParent.top=%d; rc.bottom=%d;rc.right=%d",rcStaticInParent.left,rcStaticInParent.top,rcStaticInParent.bottom, rcStaticInParent.right);

		m_memDC.StretchBlt(0, 0, rcStaticInParent.Width(), rcStaticInParent.Height(), pParentDC, rcStaticInParent.left, rcStaticInParent.top, rcStaticInParent.Width(), rcStaticInParent.Height(),SRCCOPY);
		GetParent()->ReleaseDC(pParentDC);
	}
	// 写CStatic文本
	CString strWinText;
	GetWindowText(strWinText);
	dc.SelectObject(&m_font);
	dc.SetTextColor(RGB(1,1,1));
	dc.SetBkMode(TRANSPARENT);
	dc.TextOut(0,0,strWinText);
}
BOOL CStaticTran::OnEraseBkgnd(CDC* pDC)
{

	//CRect rc;
	//GetClientRect(&rc);
	//GetWindowRect(&rc);
	//ScreenToClient(&rc);
	////SYS_DEBUG_OUT(Output_Console,"rc.left=%d; rc.top=%d; rc.bottom=%d;rc.right=%d",rc.left,rc.top,rc.bottom, rc.right);
	//pDC->BitBlt(0, 0, rc.Width(),rc.Height(), &m_memDC,0, 0 ,SRCCOPY);

	return FALSE;
}

void  CStaticTran::SetFont(int fontHeight, CString fontStr)
{
	m_font.DeleteObject();

// 	VERIFY(m_font.CreateFont(
// 		fontHeight,                // nHeight
// 		0,                         // nWidth
// 		0,                         // nEscapement
// 		0,                         // nOrientation
// 		FW_NORMAL,                 // nWeight
// 		FALSE,                     // bItalic
// 		FALSE,                     // bUnderline
// 		0,                         // cStrikeOut
// 		DEFAULT_CHARSET,              // nCharSet
// 		OUT_DEFAULT_PRECIS,        // nOutPrecision
// 		CLIP_DEFAULT_PRECIS,       // nClipPrecision
// 		DEFAULT_QUALITY,           // nQuality
// 		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
// 		fontStr) );   

	m_font.CreateFont(fontHeight,0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY,
		FIXED_PITCH | FF_MODERN, FontStr);

	if ( ::IsWindow(GetSafeHwnd()) )
	{
		UpdateSurface();
 	}
}

void CStaticTran::UpdateSurface()
{
	CRect (rc);
	GetWindowRect(rc);
	RedrawWindow();

	GetParent()->ScreenToClient(rc);
	GetParent()->InvalidateRect(rc,TRUE);
	GetParent()->UpdateWindow();
}

//void CStaticTran::OnStnClicked()
//{
//	// TODO: 在此添加控件通知处理程序代码
//		MessageBox(_T("test message"));
//}


//
//// new CStaticTran  
//
//#include "stdafx.h"  
////#include "StockTradeClient.h"  
//#include "StaticTransparent.h"  
//
//
//IMPLEMENT_DYNAMIC(CStaticTran, CStatic)  
//
//CStaticTran::CStaticTran()  
//{  
//	m_text_size = 100;  
//	m_text_high_size = 200;  
//
//	m_text_color = RGB(0,0,0);  
//	m_text_high_color = RGB(255,0,0);  
//	m_curstr = "";  
//	m_bDrawBoard = false;  
//	//  m_high_font.CreateFont(-24,0,0,0,FW_BOLD,FALSE,FALSE,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_SWISS,_T("微软雅黑"));  
//
//}  
//
//CStaticTran::~CStaticTran()  
//{  
//}  
//
//BEGIN_MESSAGE_MAP(CStaticTran, CStatic)  
//	ON_WM_PAINT()  
//END_MESSAGE_MAP()  
//
//
//
//void CStaticTran::SetFont(CFont font)  
//{  
//	//m_font = font;  
//}  
//
//
//void CStaticTran::SetFont(int size,CString fontStr)  
//{  
//	m_font.CreatePointFont(size,fontStr);  
//}  
//
//void CStaticTran::SetTextColor(COLORREF color)  
//{  
//	m_text_color = color;  
//}  
//
//void CStaticTran::SetHightFontColor(COLORREF color)  
//{  
//	m_text_high_color = color;  
//}  
//
//void CStaticTran::SetHightTextSize(int size)  
//{  
//	m_text_high_size = size;  
//}  
//
//
//
//
//void CStaticTran::IsDrawBorad(bool draw) //是否画边界  
//{  
//	m_bDrawBoard = draw;  
//}  
//
//
//void CStaticTran::DrawBoard(CDC* dc,CRect rct) //绘制边界  
//{  
//	CPen pen;  
//	pen.CreatePen(PS_SOLID, 2, RGB(255, 0, 0));  
//	CPen  *oldPen;  
//	oldPen = dc->SelectObject(&pen);  
//	dc->MoveTo(rct.left, rct.top);  
//	dc->LineTo(rct.right, rct.top);  
//	dc->LineTo(rct.right, rct.bottom);  
//	dc->LineTo(rct.left, rct.bottom);  
//	dc->LineTo(rct.left, rct.top);  
//	dc->SelectObject(oldPen);  
//
//}  
//
//void CStaticTran::SetWindowText(LPCTSTR lpszString)  
//{  
//	m_str = (CString)lpszString;  
//	CRect rct;  
//	GetWindowRect(&rct);  
//	GetParent()->ScreenToClient(&rct);  
//	GetParent()->InvalidateRect(&rct); //使区域无效 重绘防止出现文本重叠  
//}  
//
//
//
//void CStaticTran::OnPaint()  
//{  
//	CPaintDC dc(this); // device context for painting  
//	CRect rct;  
//	GetWindowRect(&rct);  
//	ScreenToClient(&rct);  
//
//	CPaintDC *pDC = &dc;  
//	pDC->SetBkMode(TRANSPARENT);  
//
//	/*内存绘图*/  
//	CDC memDC;  
//	memDC.CreateCompatibleDC(&dc);  
//	CBitmap m_MemMap;  
//	m_MemMap.CreateCompatibleBitmap(pDC, rct.Width(), rct.Height());  
//	memDC.SelectObject(&m_MemMap);  
//	memDC.FillSolidRect(rct, RGB(236, 243, 246));  
//	memDC.SetBkMode(TRANSPARENT);  
//
//	if (m_curstr != m_str)  
//	{  
//		m_font.CreatePointFont(200, L"微软雅黑");  
//		memDC.SetTextColor(m_text_high_color);  
//		memDC.DrawText(m_str, rct, DT_VCENTER | DT_VCENTER | DT_SINGLELINE);  
//		if (m_bDrawBoard)  DrawBoard(&memDC,rct);  
//		m_curstr = m_str;  
//	}  
//
//	else  
//	{  
//		memDC.SetTextColor(m_text_color);  
//		memDC.DrawText(m_str, rct, DT_VCENTER | DT_VCENTER | DT_SINGLELINE);  
//		if (m_bDrawBoard)  DrawBoard(&memDC,rct);  
//	}  
//
//	/*复制到界面dc*/  
//	pDC->BitBlt(rct.left, rct.top, rct.Width(), rct.Height(), &memDC, 0, 0, SRCCOPY);  
//
//	/*清理内存*/  
//	m_MemMap.DeleteObject();  
//	memDC.DeleteDC();  
//}  
