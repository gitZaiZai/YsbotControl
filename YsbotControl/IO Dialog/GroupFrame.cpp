#include "stdafx.h"
#include "GroupFrame.h"
#include "..\colourcode.h"

void DrawGroupFrame(CWindowDC *dc,CString caption,int r, int g, int b,unsigned int fontHeight,CString fontStr) {
	CRect rect;
	dc->GetWindow()->GetWindowRect(&rect);

	// 计算高度（宽度是固定的，高度根据项目数量决定）
	int height = rect.Height();
	int width = rect.Width();

	// 顶部边框色彩
	COLORREF color_top[] = {
		RGB(r + 20, g + 20, b + 20), RGB(r + 19, g + 19, b + 19),
		RGB(r + 18, g + 18, b + 18),RGB(r + 18, g + 18, b + 18),
		RGB(r + 17, g + 17, b + 17),RGB(r + 16, g + 16, b + 16),
		RGB(r + 17, g + 17, b + 17),RGB(r + 17, g + 17, b + 17),
		RGB(r + 17, g + 17, b + 17),RGB(r + 17, g + 17, b + 17),
		RGB(r + 17, g + 17, b + 17),RGB(r + 17, g + 17, b + 17),
		RGB(r + 17, g + 17, b + 17),RGB(r + 17, g + 17, b + 17),
		RGB(r + 17, g + 17, b + 17),RGB(r + 17, g + 17, b + 17),
		RGB(r + 17, g + 17, b + 17),RGB(r + 17, g + 17, b + 17),
		RGB(r + 27, g + 27, b + 27), RGB(r + 13, g + 13, b + 13)
	};
	// 底部边框色彩
	COLORREF color_bottom[] = {
		RGB(r - 56, g - 56, b - 56), RGB(r - 114, g - 114, b - 114),
		RGB(r - 91, g - 91, b - 91), RGB(r - 58, g - 58, b - 58),
		RGB(r - 27, g - 27, b - 27)
	};
	// 左侧边框色彩
	COLORREF color_left[] = {
		RGB(160, 160, 160), RGB(190, 190, 190)
	};
	// 右侧边框色彩
	COLORREF color_right[] = {
		RGB(190, 190, 190), RGB(160, 160, 160)
	};
	// 分隔条色彩
	COLORREF color_split[] = {
		RGB(r - 20, g - 20, b - 20), RGB(r - 120, g - 120, b - 120),RGB(r - 18, g - 18, b - 18)
	};

	// 绘制顶部边框
	for (unsigned i = 0; i < 20; i++) {
		unsigned int color;
		if (r+30>255)
			color=255;
		else
			color=r+30;
		DrawGroupLine(dc, RGB(color-i,color-i,color-i), 0, i, width-1, i);
	}

	// 绘制分割线
	for (unsigned i = 0; i < 3; i++) {
		DrawGroupLine(dc, color_split[i], 5, i+20, width-5, i+20);
	}

	//绘制左右边框
	for (unsigned i = 0; i < 2; i++) {
		DrawGroupLine(dc, color_left[i], i, 0, i, height);
		DrawGroupLine(dc, color_right[i], width-i-1, 0, width-i-1, height);
	}

	//绘制底部边框，由下往上绘制
	for (unsigned i = 0; i < 5; i++) {
		DrawGroupLine(dc, color_bottom[i], 0, height - i-1, width, height - i-1);
	}

	GroupTextOut(dc,caption,CRect(5,2,width,18),RGB(0,0,0),fontHeight,fontStr);

	return;
}

void GroupTextOut(CWindowDC *dc,CString caption,CRect rect,COLORREF color ,unsigned int FontHeigth,CString FontStr) 
{
		CFont font;
		font.CreateFont(FontHeigth,0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY,
			FIXED_PITCH | FF_MODERN, FontStr);
		
		CFont *old_font = dc->SelectObject(&font);
		dc->SetBkMode(TRANSPARENT); 		
		dc->TextOutW(rect.left,rect.top,caption,lstrlen(caption));
		dc->SelectObject(old_font);

		return;
}

void DrawGroupLine(
	CWindowDC *dc, COLORREF color, int x0, int y0, int x1, int y1) {
		CPen pen(PS_SOLID, 1, color);
		CPen *old_pen = dc->SelectObject(&pen);
		dc->SelectObject(pen);
		dc->MoveTo(x0, y0);
		dc->LineTo(x1, y1);
		dc->SelectObject(old_pen);
		pen.DeleteObject();
		return;
}


//

IMPLEMENT_DYNAMIC(CGroupFrame, CStatic)

	BEGIN_MESSAGE_MAP(CGroupFrame, CStatic)
		ON_WM_CREATE()
		ON_WM_CTLCOLOR()

	END_MESSAGE_MAP()

CGroupFrame::CGroupFrame(BOOL bEnableTitle) : m_enableTitle(bEnableTitle)
{
	_fontStr=_T("Arial");
	_fontHeight=18;
}

CGroupFrame::~CGroupFrame() 
{
	m_brush.DeleteObject();
}

BOOL CGroupFrame::Create(CString caption,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if ( caption.IsEmpty() )
	{
		m_enableTitle = FALSE;
	}

	CStatic::Create(caption, dwStyle|SS_NOTIFY, rect, pParentWnd, nID);
	Initialize(rect, pParentWnd, nID);

	m_brush.CreateSolidBrush(kColor[8]);
	return TRUE;
}

int CGroupFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	return 0;
}

void CGroupFrame::PreSubclassWindow()  
{
	DWORD dwStyle = GetStyle();  
	SetWindowLong(GetSafeHwnd(),GWL_STYLE,dwStyle | SS_OWNERDRAW  );  
	CStatic::PreSubclassWindow();  
}


void CGroupFrame::DrawItem(LPDRAWITEMSTRUCT )  
{  
	if (m_enableTitle )
	{
		CWindowDC dc(this);
		GetWindowTextW(caption_string);
		DrawGroupFrame(&dc ,caption_string,220,220,220,_fontHeight,_fontStr);
 	}
}

void CGroupFrame::SetFont(unsigned int fontHeight,CString fontStr)
{
	_fontStr=fontStr;
	_fontHeight=fontHeight;
	Invalidate();
}

 HBRUSH CGroupFrame::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
 {
 	HBRUSH hbr = CStatic::OnCtlColor(pDC, pWnd, nCtlColor);
 
 	//CTLCOLOR_STATIC为 CStatic
 	//CTLCOLOR_EDIT
 	//CTLCOLOR_LISTBOX
 	//CTLCOLOR_BTN
 	//CTLCOLOR_DLG  等等
 
 	if ( nCtlColor == CTLCOLOR_STATIC )
 	{
 // 		static HBRUSH hbrStatic = ::CreateSolidBrush(kColor[8]);
 
 		//COLOR是你想设置的背景颜色 此处必须为静态变量，否则不能实现
//  		pDC->SetTextColor(RGB(0,0,0));
//  		pDC->SetBkColor(kColor[8]); 
//  		pDC->SetBkMode(TRANSPARENT);
//  		return (HBRUSH)m_brush;
// 		pDC->SetBkMode(TRANSPARENT);
// 		return (HBRUSH)::GetStockObject(NULL_BRUSH);
 	}
 	return hbr;
 }

void CGroupFrame:: Initialize(const RECT& rect, CWnd* pParentWnd, UINT nID)
{


}





