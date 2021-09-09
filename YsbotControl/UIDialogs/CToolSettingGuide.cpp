// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "stdafx.h"
#include "CToolSettingGuide.h"
#include "../MotionControl.h"
#include "../PosInfo.h"
#include "../resource.h"
#include "../geometry3D.h"
#include "../PoseTransform.h"
#include "state_dialog.h"

BEGIN_MESSAGE_MAP(CToolSettingGuide, CGroupFrame)
 	ON_MESSAGE( WM_KEYBOARD_ENTER,  &CToolSettingGuide::OnKeyboardEnter     )
	ON_BN_CLICKED( TOOLGUIDE_TEACH_BUTTON, &CToolSettingGuide::OnPosTeachClicked )
	ON_CBN_SELCHANGE( TOOLGUIDE_COMBO_1, &CToolSettingGuide::OnSelChangeCombo1 )
	ON_CBN_SELCHANGE( TOOLGUIDE_COMBO_2, &CToolSettingGuide::OnSelChangeCombo2 )
	ON_COMMAND_RANGE( TOOLGUIDE_CHECK_1, TOOLGUIDE_CHECK_2, &CToolSettingGuide::OnCheckClicked )
 
END_MESSAGE_MAP()

CToolSettingGuide::CToolSettingGuide():m_nPosType(0)
{
	m_iMethod = 0;
	m_iStep   = 0;
	for ( int i = 0; i < 4; ++i )
	{
		teach_result[i].zAssignTo = 2;
		teach_result[i].xAssignTo = 0;
		teach_result[i].zReverse = false;
		teach_result[i].xReverse = false;
	}
 
}

CToolSettingGuide::~CToolSettingGuide() 
{
 	font_.DeleteObject();
}

BOOL CToolSettingGuide::Create(CString caption,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CGroupFrame::Create(caption, dwStyle|SS_NOTIFY, rect, pParentWnd, nID);
	//Initialize(rect, pParentWnd, nID);		
	_nID=nID;

	font_.CreateFont(20, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("Arial"));

	CRect rc;
	GetClientRect(&rc);
	LPCTSTR	 strEdit[] = { _T("X"),  _T("Y"), _T("Z"),  _T("A"), _T("B"), _T("C")  };
	for ( int i = 0; i < ARRAY_SIZE(strEdit); i++ )
	{
		strEditTitle[i] = strEdit[i];
	}
	LPCTSTR	 strUnitLabel[] = { _T("mm"),  _T("mm"), _T("mm"),  _T("°"), _T("°"), _T("°") };

	int iHalfWidth = (int)(rc.Width()/2.0);

	int xstart = 5;
	int ystart = 155;
	int iLabelCellWidth  = 35;
	int iLableCellHeight = 25;
	int iEditBoxHeight   = 30;
	int iEditBoxWidth    = iHalfWidth - (iLabelCellWidth + iLabelCellWidth);
	CRect rcLableName = CRect(CPoint(rc.left+xstart,rc.top+ystart),CSize(iLabelCellWidth,iLableCellHeight));
	CRect rcEditBox   = CRect(CPoint(rcLableName.right,rc.top+ystart),CSize(iEditBoxWidth,iEditBoxHeight));
	CRect rcLableUnit = CRect(CPoint(rcEditBox.right,rc.top+ystart),CSize(iLabelCellWidth,iLableCellHeight));

	int yspace = 10;
	int xspace = 10;
	CString label;
	CRect rectLabel, rectEdit, rectUnit;
	int nRowNum = 3; 
	for ( int i = 0; i < ARRAY_SIZE(pos_edit); ++i )
	{
		rectLabel = rcLableName;
		rectEdit  = rcEditBox;
		rectUnit  = rcLableUnit;

		rectLabel.OffsetRect((int)(i/nRowNum)*(iHalfWidth+xspace),(int)(i%nRowNum)*(iLableCellHeight+yspace));
		rectEdit.OffsetRect((int)(i/nRowNum)*(iHalfWidth+xspace),(int)(i%nRowNum)*(iLableCellHeight+yspace));
		rectUnit.OffsetRect((int)(i/nRowNum)*(iHalfWidth+xspace),(int)(i%nRowNum)*(iLableCellHeight+yspace));

		label_btn[i].Create( strEditTitle[i],WS_CHILD|WS_VISIBLE,rectLabel,this );
		label_btn[i].SetFont(22);

		pos_edit[i].Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_RIGHT,rectEdit,this, TOOLGUIDE_EDIT_1+i );
		pos_edit[i].SetFont(&font_);
		pos_edit[i].SetValueRange(-100000.0,100000.0, 3);

		strUnit[i].Create(strUnitLabel[i], WS_CHILD | WS_VISIBLE,rectUnit, this);
		strUnit[i].SetAlignment(1);
		strUnit[i].Initialize(kColor[0]);
		strUnit[i].SetFontType(20,350,_T("宋体"));

	}

// 	int x0=10,y0=155,w0=10,h0=25,Hspace=30,Vspace=180;
// 	int x1=37,y1=155,w1=95,h1=25;
//   
//  	strEditTitle[0]=_T("X");
// 	strEditTitle[1]=_T("Y");
// 	strEditTitle[2]=_T("Z");
// 	strEditTitle[3]=_T("A");
// 	strEditTitle[4]=_T("B");
// 	strEditTitle[5]=_T("C");
//  
// 	for (int i=0;i<6;i++)
// 	{
// 		CRect rect0,rect1;
// 		if (i<3)
// 		{
// 			rect0.left=x0;		rect0.top=y0+i*Hspace;
// 			rect0.right=x0+w0;	rect0.bottom=y0+h0+i*Hspace;
// 			rect1.left=x1;		rect1.top=y1+i*Hspace;
// 			rect1.right=x1+w1;	rect1.bottom=y1+h1+i*Hspace;
// 
// 			strUnit[i].Create(_T("mm"), WS_CHILD | WS_VISIBLE,
// 				CRect(CPoint(rect0.right + 120, rect0.top),CSize(25,30)), this);
// 			strUnit[i].SetAlignment(1);
// 			strUnit[i].Initialize(kColor[0]);
// 			strUnit[i].SetFontType(20,350,_T("Arial"));
// 		} 
// 		else if(i<6)
// 		{
// 			rect0.left=x0+Vspace;		rect0.top=y0+(i-3)*Hspace;
// 			rect0.right=x0+w0+Vspace;	rect0.bottom=y0+h0+(i-3)*Hspace;
// 			rect1.left=x1+Vspace;		rect1.top=y1+(i-3)*Hspace;
// 			rect1.right=x1+w1+Vspace;	rect1.bottom=y1+h1+(i-3)*Hspace;
// 
// 			strUnit[i].Create(_T("°"), WS_CHILD | WS_VISIBLE,
// 				CRect(CPoint(rect0.right + 120, rect0.top),CSize(20,30)), this);
// 			strUnit[i].SetAlignment(1);
// 			strUnit[i].Initialize(kColor[0]);
// 			strUnit[i].SetFontType(20,350,_T("Arial"));
// 		}
// 		else
// 		{
// 			rect0.left=x0+Vspace;		rect0.top=y0+(i-3)*Hspace+30;
// 			rect0.right=x0+w0+Vspace;	rect0.bottom=y0+h0+(i-3)*Hspace+30;
// 			rect1.left=x1+Vspace;		rect1.top=y1+(i-3)*Hspace+30;
// 			rect1.right=x1+w1+Vspace;	rect1.bottom=y1+h1+(i-3)*Hspace+30;
// 		}
// 
// 		label_btn[i].Create(strEditTitle[i],WS_CHILD|WS_VISIBLE,rect0,this );
// 		label_btn[i].SetFont(22);
// 
// 		pos_edit[i].Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_RIGHT,rect1,this, TOOLGUIDE_EDIT_1+i );
// 		pos_edit[i].SetFont(&font_);
// 		pos_edit[i].SetValueRange(-9999.9999,9999.9999,3);
// 
// 	}
 	 

	btn_teach.Create(_T("示教"),WS_CHILD|WS_VISIBLE,CRect(CPoint(35,265),CSize(95,50)),this,TOOLGUIDE_TEACH_BUTTON);  
	btn_teach.SetFont(&font_);

	//////////////////////////////////////////////////////////////////////////
	for ( int i = 0; i < 3; ++i )
	{
		strTitle[i].Create(_T("工具"), WS_CHILD | WS_VISIBLE,
			CRect(CPoint(10 + 115*i,35),CSize(80,30)), this);
		strTitle[i].SetAlignment(1);
		strTitle[i].Initialize(kColor[0]);
		strTitle[i].SetFontType(20,350,_T("Arial"));
	}

	strTitle[1].SetWindowTextW(_T("世界"));
	strTitle[2].SetWindowTextW(_T("反向"));

	COLORREF m_crBackgroud = RGB(220, 220, 220);
	COLORREF m_crText      = RGB(96, 100, 96);

	int topStart = 60;
	for ( int i =0; i < 2; ++i )
	{
		axisLabel[i].Create( _T(""), WS_CHILD | WS_VISIBLE | DT_LEFT | SS_CENTERIMAGE,	
			CRect(CPoint(10,topStart+2+42*i),CSize(80,32)), this );
		axisLabel[i].SetBkColor(m_crBackgroud)
			.SetTextColor(m_crText)
			.SetBorder(TRUE)
			.SetFontBold(TRUE)
			.SetText(_T(" Z"));
 	}
	axisLabel[1].SetText(_T(" X"));

	for ( int i =0; i < 2; ++i )
	{
		axisCombo[i].Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED |CBS_HASSTRINGS,CRect(CPoint(120,topStart+40*i),CSize(95,30)), this,TOOLGUIDE_COMBO_1+i );
		axisCombo[i].set_custom_message(FALSE);
		axisCombo[i].SetShowImage(FALSE);
		axisCombo[i].AddString(_T("X"));
		axisCombo[i].AddString(_T("Y"));
		axisCombo[i].AddString(_T("Z"));
	}
	axisCombo[0].SetCurSel(2);
	axisCombo[1].SetCurSel(0);
 
	for ( int i =0; i < 2; ++i )
	{
		checkReverse[i].Create(_T(""),WS_CHILD|WS_VISIBLE|WS_GROUP ,CRect(CPoint(240,topStart+40*i),CSize(30,32)),this,TOOLGUIDE_CHECK_1+i); 
		checkReverse[i].loadBmp(_T("res/unchecked.bmp"),_T("res/checked.bmp"),_T("res/disablecheck.bmp"),_T("res/disableUncheck.bmp"));
 	}

	return TRUE;
}
 
void CToolSettingGuide::update_guide( int iMethod, int iStep )
{
	CString titleDialog;
	m_iMethod = iMethod;
	m_iStep   = iStep;

	int idx_radio_disable = -1;

	static int last_step = 0;

	if ( iMethod == 2 && iStep == 4 )
 		show_check_combo(TRUE);
 	else
		show_check_combo(FALSE);

	switch( iMethod )
	{
	case 0:
		{
			switch(iStep)
			{
			case 0:
				break;
			case 1:
				titleDialog = _T("第1步: 选择标定方法");
				break;
			case 2:
				titleDialog = _T("第2步: 无工具测量标定点或输入标定点数值");
 				show_pos_info(0);
				break;
			case 3: 
				titleDialog = _T("第3步: 带工具示教标定点");
 				show_pos_info(1);
 				break;
			case 4:
				titleDialog = _T("结果: 一点法姿态保持");
				calc_reference_system();
  				break;
			case 5: 
				break;
			default:break;
			}
		}
		break;
	case 1:
		{
			switch(iStep)
			{
			case 0: break;
			case 1:
				titleDialog = _T("第1步: 选择标定方法");
				break;
			case 2: 
				titleDialog = _T("第2步: 四点法XYZ，选择第1点");
 				show_pos_info(0);
				break;
			case 3:
				titleDialog = _T("第3步: 四点法XYZ，选择第2点");
 				show_pos_info(1);
				break;
			case 4: 
				titleDialog = _T("第4步: 四点法XYZ，选择第3点");
 				show_pos_info(2);
				break;
			case 5:
				titleDialog = _T("第5步: 四点法XYZ，选择第4点");
 				show_pos_info(3);
 				break;
			case 6:
				titleDialog = _T("结果: 四点法XYZ");
				calc_reference_system();
				break;
			default:break;
			}
 		}
		break;
	case 2:
		{
			switch(iStep)
			{
			case 0: break;
			case 1:
				titleDialog = _T("第1步: 选择标定方法");
				break;
			case 2:
				titleDialog = _T("第2步: 输入已知的标定点");
 				show_pos_info(0);
				break;
			case 3:
				titleDialog = _T("第3步: 带工具示教标定点");
  				show_pos_info(1);
				break;
 			case 4:
 				titleDialog = _T("第4步: 姿态示教");
 				show_pos_info(2);
				break;
			case 5:	
				titleDialog = _T("结果: 一点法姿态标定");
 				calc_reference_system(); 
				break;
			default:break;
			}
		}
		break;
	default: break;
	}

	last_step = iStep;

	SetWindowText(titleDialog);

	GetParent()->RedrawWindow();

}

void CToolSettingGuide::OnPosTeachClicked()
{
	if ( m_iStep < 2 || m_iStep > 6) // step = [2,4] enable teach point.
		return;

	int indexPt = m_iStep - 2;
	PosInfo rpos  = SMC.get_position( PosInfo::CTmachine, LDScommad );

	teach_result[indexPt].tpos = rpos.cart ;
    
	if ( axisCombo[0].IsWindowVisible() )
	{
		teach_result[indexPt].zAssignTo = axisCombo[0].GetCurSel();
		teach_result[indexPt].xAssignTo = axisCombo[1].GetCurSel();
		teach_result[indexPt].zReverse = checkReverse[0].GetChecked() ? true : false;
		teach_result[indexPt].xReverse = checkReverse[1].GetChecked() ? true : false;
	}

    show_pos_info(indexPt);

	cout << " pos id = " << indexPt << '\n';
}

void CToolSettingGuide::show_pos_info( int posID )
{
	if ( m_iMethod == 2 && posID == 2 )
	{
		pos_edit[0].SetValue(rad2deg(teach_result[posID].tpos.ang.x));
		pos_edit[1].SetValue(rad2deg(teach_result[posID].tpos.ang.y));
		pos_edit[2].SetValue(rad2deg(teach_result[posID].tpos.ang.z)); 

	}
	else
	{
		pos_edit[0].SetValue(teach_result[posID].tpos.pos.x);
		pos_edit[1].SetValue(teach_result[posID].tpos.pos.y);
		pos_edit[2].SetValue(teach_result[posID].tpos.pos.z); 
		pos_edit[3].SetValue(rad2deg(teach_result[posID].tpos.ang.x));
		pos_edit[4].SetValue(rad2deg(teach_result[posID].tpos.ang.y));
		pos_edit[5].SetValue(rad2deg(teach_result[posID].tpos.ang.z)); 
 	}
}

void CToolSettingGuide::show_check_combo(BOOL b)
{
	 
	if ( b ) // show check and combo
	{
		for ( int i = 0; i < 3; ++i )
		{
			strTitle[i].ShowWindow(SW_SHOW);
 			strUnit[i+3].ShowWindow(SW_HIDE);
			label_btn[i+3].ShowWindow(SW_HIDE);
			pos_edit[i+3].ShowWindow(SW_HIDE);

			label_btn[i].SetWindowTextW( strEditTitle[i+3] );
			strUnit[i].SetWindowTextW(_T("°"));
		}
		for ( int i = 0; i < 2; ++i )
		{
			axisLabel[i].ShowWindow(SW_SHOW);
			axisCombo[i].ShowWindow(SW_SHOW);
 			checkReverse[i].ShowWindow(SW_SHOW);
 		}
  	}
	else
	{
		for ( int i = 0; i < 3; ++i )
		{
			strTitle[i].ShowWindow(SW_HIDE);
 			label_btn[i+3].ShowWindow(SW_SHOW);
			pos_edit[i+3].ShowWindow(SW_SHOW);
			strUnit[i+3].ShowWindow(SW_SHOW);

			label_btn[i].SetWindowTextW( strEditTitle[i] );
			strUnit[i].SetWindowTextW(_T("mm"));

		}
		for ( int i = 0; i < 2; ++i )
		{
			axisLabel[i].ShowWindow(SW_HIDE);
			axisCombo[i].ShowWindow(SW_HIDE);
			checkReverse[i].ShowWindow(SW_HIDE);
		}

 	}

	GetParent()->Invalidate();
}
 
void CToolSettingGuide::reset_all()
{
	m_iMethod  = 0;
	m_iStep    = 0;
	m_nPosType = 0;
	for ( int i = 0; i < 2; ++i )
	{
		axisCombo[i].SetCurSel(0);
		checkReverse[i].SetChecked(FALSE);
	}
	axisCombo[0].SetCurSel(2);
	clear_show_info();

	show_check_combo(FALSE);

	for ( int i = 0; i < 4; ++i )
	{
		teach_result[i].zAssignTo = 2;
		teach_result[i].xAssignTo = 0;
		teach_result[i].zReverse = false;
		teach_result[i].xReverse = false;
		teach_result[i].tpos = CartPos();
	}
}

void CToolSettingGuide::clear_show_info()
{
	for (unsigned int i = 0; i < 6; ++i )
		pos_edit[i].SetValue( 0.00 );

	for ( int i = 0; i < 2; ++i )
	{
		axisCombo[i].SetCurSel(0);
		checkReverse[i].SetChecked(FALSE);
	}
	axisCombo[0].SetCurSel(2);

}

LRESULT CToolSettingGuide::OnKeyboardEnter(WPARAM wp,LPARAM lp)
{
	CString str_result = *((CString *)wp);
	double dfLoc = 0.0;
	int iret = _stscanf(str_result, _T("%lf"), &dfLoc);

	if ( iret == 1 )
	{
		int indexPt = m_iStep - 2;

		int nEditIdx = (int)(lp) - TOOLGUIDE_EDIT_1;

		if ( m_iMethod == 2 && indexPt == 2 )
		{
 			switch(nEditIdx)
			{
			case 0:	teach_result[indexPt].tpos.ang.x = deg2rad(dfLoc); break;
			case 1: teach_result[indexPt].tpos.ang.y = deg2rad(dfLoc); break;
			case 2: teach_result[indexPt].tpos.ang.z = deg2rad(dfLoc); break;
			default:break;
			}
 		}
		else
		{
			switch(nEditIdx)
			{
			case 0:	teach_result[indexPt].tpos.pos.x = dfLoc; break;
			case 1: teach_result[indexPt].tpos.pos.y = dfLoc; break;
			case 2: teach_result[indexPt].tpos.pos.z = dfLoc; break;
			case 3:	teach_result[indexPt].tpos.ang.x = deg2rad(dfLoc); break;
			case 4: teach_result[indexPt].tpos.ang.y = deg2rad(dfLoc); break;
			case 5: teach_result[indexPt].tpos.ang.z = deg2rad(dfLoc); break;
			default:break;
			}
		}


		cout << "edit value = " << CStringA(str_result) << " : " << nEditIdx << endl;
		return 1;
	}
	else
		return 0;
}


void CToolSettingGuide::calc_reference_system()
{
	for ( int i = 0; i < 4; ++i )
	{
		std::cout << "indexPt = " << i << " teach point [ " << teach_result[i].tpos << " ] [ " <<  teach_result[i].zAssignTo << " ] : " << teach_result[i].zReverse << '\n';
	}

 	switch( m_iMethod )
	{
	case 0: calc_one_point_xyz();  break;
	case 1: calc_four_point_xyz(); break;
	case 2: calc_one_point_6D();   break;
	default: break;
	}
}
 
void CToolSettingGuide::OnSelChangeCombo1()
{
	cout << " combo1 = " << axisCombo[0].GetCurSel() << '\n';
	int indexPt = m_iStep - 2;
 	teach_result[indexPt].zAssignTo = axisCombo[0].GetCurSel();

}

void CToolSettingGuide::OnSelChangeCombo2()
{
 	cout << " combo2 = " << axisCombo[1].GetCurSel() << '\n';
	int indexPt = m_iStep - 2;
  	teach_result[indexPt].xAssignTo = axisCombo[1].GetCurSel();

}

void CToolSettingGuide::OnCheckClicked(UINT id)
{
	int index = id - TOOLGUIDE_CHECK_1;
 	int indexPt = m_iStep - 2;
    if ( index == 0 )
    {
		cout << "check0 = " << checkReverse[0].GetChecked() << '\n';
 		teach_result[indexPt].zReverse = checkReverse[0].GetChecked()  ? true : false;
    }
	else if ( index == 1)
	{
		cout << "check1 = " << checkReverse[1].GetChecked() << '\n';
		teach_result[indexPt].xReverse = checkReverse[1].GetChecked() ? true : false;
	}
}

void CToolSettingGuide::get_desired_orientation(Matrix& matAng)
{
	if ( axisCombo[0].GetCurSel() == axisCombo[1].GetCurSel() )
	{
 		MSD.ShowInfo(_T("axis error when calculate tool!"),JTWARNING);
 		return;
	}
	int indexPt = m_iStep - 3; // indexPt = 2 for save orientation data.

	int zAssignTo = teach_result[indexPt].zAssignTo;
	int xAssignTo = teach_result[indexPt].xAssignTo;
 
	Matrix onesMat(3,3);
	Matrix dirMat(3,3);

	onesMat << threebythreeident;
	dirMat.SubMatrix(1,3,3,3) = onesMat.SubMatrix(1,3,zAssignTo+1,zAssignTo+1) * ( teach_result[indexPt].zReverse ? -1 : 1 ); // z(tool) -> ?(world)
	dirMat.SubMatrix(1,3,1,1) = onesMat.SubMatrix(1,3,xAssignTo+1,xAssignTo+1) * ( teach_result[indexPt].xReverse ? -1 : 1 ); // x(tool) -> ?(world)
	dirMat.SubMatrix(1,3,2,2) = crossproduct( dirMat.SubMatrix(1,3,3,3), dirMat.SubMatrix(1,3,1,1) );// y = (z) × (x),  dirMat.SubMatrix(1,3,2,2) = yMat / yMat.NormFrobenius();

	matAng.SubMatrix(1,3,1,3) = dirMat;

// 	cout << "calc dir = \n" << dirMat << endl;
}
 
void CToolSettingGuide::calc_one_point_xyz()
{
	Vec3D toolPos = teach_result[0].tpos.pos - teach_result[1].tpos.pos;
	CartPos toolRef;
	toolRef.pos = toolPos;
	toolRef.ang = Vec3D::zero_vector;

	m_toolResult.cpos = toolRef;

}

void CToolSettingGuide::calc_one_point_6D()
{
	Matrix matDisiredAng(4,4);
	matDisiredAng = 0.0;
	matDisiredAng(4,4) = 1.0;
	get_desired_orientation(matDisiredAng);
	cout << "tool target dir = \n" << matDisiredAng << endl;

	Vec3D toolPos = teach_result[0].tpos.pos - teach_result[1].tpos.pos;
 
	//Matrix matFlange = PoseTransform::CartPosToHomogeneousMat( teach_result[2].tpos, AM_EULER );
	Matrix matFlange = PoseTransform::VecToRotMat( teach_result[2].tpos.ang, AM_EULER );
	Matrix toolAng = matFlange.i() * matDisiredAng;

	Vec3D tooldir = PoseTransform::RotMatToVec( toolAng, AM_EULER );
 	
	m_toolResult.cpos.pos = toolPos;
	m_toolResult.cpos.ang = tooldir;

}
 
void CToolSettingGuide::calc_four_point_xyz()
{
	Matrix rotsub(9,3);
	Matrix psub(9,1);
	Matrix rsub_inv(3,9);
	Matrix etreslut(3,1);
	etreslut = 0;

	Matrix rot[4];
	Matrix pos[4];
	for ( int i = 0; i < 4; ++i )
	{
		rot[i].ReSize(3,3);
		pos[i].ReSize(3,1);
	}

	Matrix mathors(4,4);
	for ( int i = 0; i < 4; ++i )
	{
		mathors = PoseTransform::CartPosToHomogeneousMat( teach_result[i].tpos, AM_EULER );
		rot[i] = mathors.SubMatrix(1,3,1,3);
		pos[i] = mathors.SubMatrix(1,3,4,4);
	}

	rotsub.SubMatrix(1,3,1,3) = rot[0] - rot[1];
	rotsub.SubMatrix(4,6,1,3) = rot[1] - rot[2];
	rotsub.SubMatrix(7,9,1,3) = rot[2] - rot[3];
  
	psub.SubMatrix(1,3,1,1) = pos[1] - pos[0];
	psub.SubMatrix(4,6,1,1) = pos[2] - pos[1];
	psub.SubMatrix(7,9,1,1) = pos[3] - pos[2];

	try
	{
		rsub_inv = ( rotsub.t() * rotsub ).i() * rotsub.t();
		etreslut = rsub_inv * psub;
	}
 	catch (SingularException)
	{
 		MSD.ShowInfo(_T("示教点位数据不正确或是标定矩阵运算错误!"),JTWARNING);
 		return;
	}
 	 
 	m_toolResult.cpos = CartPos(etreslut(1,1),etreslut(2,1),etreslut(3,1),0,0,0);
 
	cout << " calibration res = \n " << etreslut << endl;

	 
}
