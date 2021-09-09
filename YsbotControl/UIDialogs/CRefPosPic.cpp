#include "stdafx.h"
#include "CRefPosPic.h"
#include "../colourcode.h"

 
BEGIN_MESSAGE_MAP(CRefPosPic, CGroupFrame)
	 
END_MESSAGE_MAP()

CRefPosPic::CRefPosPic() 
{
	for ( int i = 0; i < MAX_TEACHUSER_METHOD; ++i )
	{
		for ( int j = 0; j < MAX_TEACHUSER_STEP; ++j )
		{
 			m_strBmpName[i][j] = _T("");
			m_strStepNum[i][j] = _T("");
 		}
 	}
}

CRefPosPic::~CRefPosPic() 
{
    ;
}

BOOL CRefPosPic::Create(CString caption,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CGroupFrame::Create(caption, dwStyle|SS_NOTIFY, rect, pParentWnd, nID);
	// Initialize(rect, pParentWnd, nID);		
	_nID=nID;

	font_.CreateFont(20, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("宋体"));

	CRect rectBox = CRect( CPoint(5,220), CSize(350,110) );
	if( !m_GroupBoxStep.GetSafeHwnd() )
	{
		m_GroupBoxStep.Create(_T(""),BS_GROUPBOX|WS_VISIBLE|WS_CHILD, rectBox,this,8888);
	} 

	m_strStepNum[0][1]=_T("1.示教原点。\n2.示教X轴。\n3.示教平面XY。");
	m_strStepNum[0][2]=_T("选择并示教原点O。\n点击【下一步>】继续");
	m_strStepNum[0][3]=_T("在X轴上示教标定点P1。\n点击【下一步>】继续");
	m_strStepNum[0][4]=_T("在平面XY上示教标定点P2。\n点击【下一步>】继续");
	m_strStepNum[0][5]=_T("点击【确定】保存参考坐标系。");
	m_strStepNum[1][1]=_T("1.在X轴上确定标定点P1。\n2.在X轴上确定标定点P2。\n3在Y轴上确定标定点P3。");
	m_strStepNum[1][2]=_T("在X轴上示教标定点P1。\n点击【下一步】继续");
	m_strStepNum[1][3]=_T("在X轴上示教第二个标定点P2。\n点击【下一步>】继续");
	m_strStepNum[1][4]=_T("在Y轴上示教标定点P3\n点击【下一步>】继续");
	m_strStepNum[1][5]=_T("点击【确定】保存参考坐标系。");
	m_strStepNum[2][1]=_T("1.示教原点。\n示教坐标系各轴方向需与基础坐标系一致。");
	m_strStepNum[2][2]=_T("示教参考坐标系原点。");
	m_strStepNum[2][3]=_T("点击【确定】保存参考坐标系。");

	_StepText.Create( _T(""), WS_CHILD | WS_VISIBLE | DT_LEFT ,CRect(CPoint(10,240),CSize(340,85)), this );
	_StepText.SetBkColor( kColor[8] );

	CString strFileName;
	int index_img = 0;
	for( int i=0; i<MAX_TEACHUSER_METHOD; ++i )
	{
		for ( int j=0; j < MAX_TEACHUSER_STEP; ++j )
		{
			index_img = j+10*(i+1);
			strFileName.Format(_T(".\\res\\user_method_%d.bmp"),index_img );
			if ( index_img <= 32 )
			{
				m_strBmpName[i][j] = strFileName;
			}
		}
	}

	m_RegionPic.Create( _T(""), WS_CHILD | WS_VISIBLE | DT_LEFT | SS_CENTERIMAGE,CRect( CPoint(1,22), CSize(356,200) ), this ); // 320
	m_RegionPic.SetBkColor( kColor[8] );
	m_RegionPic.set_stretch_drawbmp(true);
	//m_RegionPic.SetLabelBitmaps( m_strBmpName[0][0] );

	// 	CRect rect = CRect( CPoint(5,240), CSize(350,96) );
	// 	if( !m_GroupBox.GetSafeHwnd() )
	// 	{
	// 		m_GroupBox.Create(_T(""),BS_GROUPBOX|WS_VISIBLE|WS_CHILD, rect,this,8888);
	// 	} 
	// 
	// 	CRect rectText = rect;
	// 	rectText.OffsetRect(0,0);
	// 	rectText.DeflateRect(10,10);
	// 	m_textTip.Create( _T(""), WS_CHILD|WS_VISIBLE | DT_LEFT | DT_TOP ,rectText, this ); // 320
	//   	m_textTip.SetBkColor( kColor[1] );
	return TRUE;
}
 
void CRefPosPic::update_picture_region(int iMethod, int iStep)
{
	if ( iMethod >= MAX_TEACHUSER_METHOD || iMethod < 0 || iStep > MAX_TEACHUSER_STEP || iStep < 1 )
		return;


	m_RegionPic.SetLabelBitmaps( m_strBmpName[iMethod][iStep-1] ); 
    _StepText.SetText(m_strStepNum[iMethod][iStep]);

// 	std::cout << "show pic = " << CStringA(m_strBmpName[iMethod][iStep-1]) << '\n';
    
}
 