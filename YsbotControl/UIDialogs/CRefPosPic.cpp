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
		FF_MODERN, _T("����"));

	CRect rectBox = CRect( CPoint(5,220), CSize(350,110) );
	if( !m_GroupBoxStep.GetSafeHwnd() )
	{
		m_GroupBoxStep.Create(_T(""),BS_GROUPBOX|WS_VISIBLE|WS_CHILD, rectBox,this,8888);
	} 

	m_strStepNum[0][1]=_T("1.ʾ��ԭ�㡣\n2.ʾ��X�ᡣ\n3.ʾ��ƽ��XY��");
	m_strStepNum[0][2]=_T("ѡ��ʾ��ԭ��O��\n�������һ��>������");
	m_strStepNum[0][3]=_T("��X����ʾ�̱궨��P1��\n�������һ��>������");
	m_strStepNum[0][4]=_T("��ƽ��XY��ʾ�̱궨��P2��\n�������һ��>������");
	m_strStepNum[0][5]=_T("�����ȷ��������ο�����ϵ��");
	m_strStepNum[1][1]=_T("1.��X����ȷ���궨��P1��\n2.��X����ȷ���궨��P2��\n3��Y����ȷ���궨��P3��");
	m_strStepNum[1][2]=_T("��X����ʾ�̱궨��P1��\n�������һ��������");
	m_strStepNum[1][3]=_T("��X����ʾ�̵ڶ����궨��P2��\n�������һ��>������");
	m_strStepNum[1][4]=_T("��Y����ʾ�̱궨��P3\n�������һ��>������");
	m_strStepNum[1][5]=_T("�����ȷ��������ο�����ϵ��");
	m_strStepNum[2][1]=_T("1.ʾ��ԭ�㡣\nʾ������ϵ���᷽�������������ϵһ�¡�");
	m_strStepNum[2][2]=_T("ʾ�̲ο�����ϵԭ�㡣");
	m_strStepNum[2][3]=_T("�����ȷ��������ο�����ϵ��");

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
 