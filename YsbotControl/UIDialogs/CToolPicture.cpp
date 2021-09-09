#include "stdafx.h"
#include "CToolPicture.h"
#include "../colourcode.h"


BEGIN_MESSAGE_MAP(CToolPicture, CGroupFrame)
	ON_WM_CREATE()
END_MESSAGE_MAP()

CToolPicture::CToolPicture() 
{
	for ( int i = 0; i < MAX_TOOL_METHOD; ++i )
	{
		for ( int j = 0; j < MAX_TOOL_STEP; ++j )
		{
			m_strBmpName[i][j] = _T("");
			m_strStepNum[i][j] = _T("");
 		}
	}
}

CToolPicture::~CToolPicture() 
{
	;
}

BOOL CToolPicture::Create(CString caption,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CGroupFrame::Create(caption, dwStyle|SS_NOTIFY, rect, pParentWnd, nID);
	// Initialize(rect, pParentWnd, nID);		
	_nID=nID;

	font_.CreateFont(20, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("宋体"));


	// 	_Text.Create(textStr[0],WS_CHILD|WS_VISIBLE|ES_MULTILINE,CRect(15,260,360,340),this);
	if( !m_GroupBoxStep.GetSafeHwnd())
	{
		m_GroupBoxStep.Create(_T(""),BS_GROUPBOX|WS_CHILD,CRect(CPoint(5,240),CSize(350,90)),this,8888);
	} 

	m_strStepNum[1][1]=_T("一点法：先使用默认工具测量标定点，再用工具示教标定点，或是直接输入已知位置的标定点。工具姿态保持不变。");
	m_strStepNum[1][2]=_T("示教第一个标定点。\n点击【下一步>】继续");
	m_strStepNum[1][3]=_T("示教第二个标定点。\n点击【下一步>】继续");
	m_strStepNum[1][4]=_T("点击【确定】保存参考数据。");
	m_strStepNum[2][1]=_T("四点法：在不同的方向上示教标定点。");
	m_strStepNum[2][2]=_T("示教第一个标定点。\n点击【下一步>】继续");
	m_strStepNum[2][3]=_T("示教第二个标定点。\n点击【下一步>】继续");
	m_strStepNum[2][4]=_T("示教第三个标定点。\n点击【下一步>】继续");
	m_strStepNum[2][5]=_T("示教第四个标定点。\n点击【下一步>】继续");
	m_strStepNum[2][6]=_T("点击【确定】保存参考数据。");
	m_strStepNum[3][1]=_T("一点法6D：改变姿态，先测量标定点，再用工具示教标定点。");
	m_strStepNum[3][2]=_T("示教第一个标定点。\n点击【下一步>】继续");
	m_strStepNum[3][3]=_T("示教第二个标定点。\n点击【下一步>】继续");
	m_strStepNum[3][4]=_T("选择与工具平行的方向轴，对齐工具并点击【示教】");
	m_strStepNum[3][5]=_T("点击【确定】保存参考数据。");

	_StepText.Create( _T(""), WS_CHILD | WS_VISIBLE | DT_LEFT ,CRect(CPoint(10,255),CSize(340,65)), this );
	_StepText.SetBkColor( kColor[8] );

	CString strFileName;
	int index_img = 0;

	for( int i=0; i<MAX_TOOL_METHOD; ++i )
	{
		for ( int j=0; j < MAX_TOOL_STEP; ++j )
		{
			strFileName.Format(_T(".\\res\\tool_method_%d%d.bmp"),i,j );
			m_strBmpName[i][j] = strFileName;
			if ( i == 0 || ( i == 1 && j == 3) || ( i == 2 && j == 5 ) || ( i == 3 && j == 4 ) )
			{
				break;
			}
		}
	}

	m_RegionPic.Create( _T(""), WS_CHILD | WS_VISIBLE | DT_LEFT | SS_CENTERIMAGE,CRect( CPoint(1,22), CSize(356,320) ), this );
	//m_RegionPic.SetBkColor( kColor[8] );
	m_RegionPic.set_stretch_drawbmp(true);
	m_RegionPic.SetLabelBitmaps( m_strBmpName[0][0] );

	return TRUE;
}
 
// void CToolPicture::SelectPicture(unsigned int npos)
// {
// 	if (npos>2 || npos<0) return;
// 	_Text.SetWindowTextW(textStr[npos]);
// }

void CToolPicture::update_illustration(int iMethod, int iStep)
{
	if ( iMethod >= MAX_TOOL_METHOD || iMethod < 0 || iStep > MAX_TOOL_STEP || iStep < 0 )
		return;
  
// 	if ( iStep == 0 )
//  		m_RegionPic.SetLabelBitmaps( m_strBmpName[0][0] ); 
//  	else
// 		m_RegionPic.SetLabelBitmaps( m_strBmpName[iMethod][iStep-1] ); 

	if ( iStep == 0 )
	{
		m_RegionPic.SetLabelBitmaps( m_strBmpName[0][0] ); 
		if ( m_GroupBoxStep.IsWindowVisible() )
		{
			m_GroupBoxStep.ShowWindow(SW_HIDE);
			m_RegionPic.MoveWindow(1,22,350,315,FALSE);
		}
	}
	else
	{

		m_RegionPic.SetLabelBitmaps( m_strBmpName[iMethod][iStep-1] ); 
		_StepText.SetText(m_strStepNum[iMethod][iStep]);
		if ( !m_GroupBoxStep.IsWindowVisible() )
		{
			m_RegionPic.MoveWindow(1,22,350,220,FALSE);
			m_GroupBoxStep.ShowWindow(SW_SHOW);
		}

	}
	//std::cout << "show pic = " << CStringA(m_strBmpName[iMethod][iStep-1]) << '\n';


}
