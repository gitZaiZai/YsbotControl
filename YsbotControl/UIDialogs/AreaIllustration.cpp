//////////Ã∆è™è™è™è™

#include "stdafx.h"
#include "AreaIllustration.h"

BEGIN_MESSAGE_MAP(CAreaIllustration, CGroupFrame)
	ON_WM_CREATE()

END_MESSAGE_MAP()


CAreaIllustration::CAreaIllustration()
{

}
CAreaIllustration::~CAreaIllustration()
{

}

BOOL CAreaIllustration::Create(CString caption,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CGroupFrame::Create(caption, dwStyle|SS_NOTIFY, rect, pParentWnd, nID);
	_nID=nID;

	m_AreaPic.Create( _T(""), WS_CHILD | WS_VISIBLE | DT_LEFT | SS_CENTERIMAGE,CRect( CPoint(5,22), CSize(220,143) ), this, 111778 );
	//m_AreaPic.SetBkColor( RGB(0,0,0));
	//m_AreaPic.SetWindowTextW(_T("aaaaa"));


	DWORD ret = m_AreaPic.SetLabelBitmaps(_T("res//AreaLockB.bmp"));
	m_AreaPic.set_stretch_drawbmp(true);
	return TRUE;
}
  
void CAreaIllustration::update_picture(int ishape,int itype)
{
	if (ishape==0&&itype==0)
	{
		m_AreaPic.SetLabelBitmaps(_T("res//AreaLockB.bmp"));
	} 
	if (ishape==0&&itype==1)
	{
		m_AreaPic.SetLabelBitmaps(_T("res//AreaBox.bmp"));
 	} 

	if (ishape==1&&itype==0)
	{
		m_AreaPic.SetLabelBitmaps(_T("res//AreaLockC.bmp"));
 	}

	if (ishape==1&&itype==1)
	{
		m_AreaPic.SetLabelBitmaps(_T("res//AreaCyl.bmp"));
 	}

}
