// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "stdafx.h"
#include "CRefSettingGuide.h"
#include "../MotionControl.h"
#include "../PosInfo.h"
#include "../resource.h"
#include "../geometry3D.h"
#include "../PoseTransform.h"
#include "../Angle.h"
#include "../colourcode.h"

BEGIN_MESSAGE_MAP(CRefSettingGuide, CGroupFrame)
	ON_WM_CREATE()
	//ON_CONTROL_RANGE( BN_CLICKED ,IDC_UserCoSys_SettingGuid+4, IDC_UserCoSys_SettingGuid+6, OnPosRadioClicked )
	ON_COMMAND_RANGE( REFGUIDE_RADIO_BUTTON_1, REFGUIDE_RADIO_BUTTON_1+2, &CRefSettingGuide::OnPosRadioClicked )
	ON_MESSAGE( WM_KEYBOARD_ENTER,  &CRefSettingGuide::OnKeyboardEnter     )
	ON_BN_CLICKED(REFGUIDE_CHECK_BUTTON, OnPosRevClicked)
	ON_BN_CLICKED(REFGUIDE_TEACH_BUTTON, OnPosTeachClicked)
END_MESSAGE_MAP()

CRefSettingGuide::CRefSettingGuide():m_nPosType(0)
{
 	m_iMethod = 0;
	m_iStep   = 0;

	radio_pos_type.clear();
	for ( unsigned int i = 0; i < 3; ++i )
	{
		teach_point[i].pos  = Vec3D::zero_vector;
		teach_point[i].type = 0;
		teach_point[i].reverse = false;
	}
}

CRefSettingGuide::~CRefSettingGuide() 
{
 	for (unsigned int i = 0; i < radio_pos_type.size(); ++i )
	{
		delete radio_pos_type[i];
	}
	radio_pos_type.clear();

	font_.DeleteObject();
}

BOOL CRefSettingGuide::Create(CString caption,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CGroupFrame::Create(caption, dwStyle|SS_NOTIFY, rect, pParentWnd, nID);
	//Initialize(rect, pParentWnd, nID);		
	_nID=nID;

	font_.CreateFont(20, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("Arial"));
	CRect rc;
	GetClientRect(&rc);
	LPCTSTR	 strEditTitle[] = { _T("X"),  _T("Y"), _T("Z")  };
	LPCTSTR	 strUnitLabel[] = { _T("mm"),  _T("mm"), _T("mm")  };

	int iHalfWidth = (int)(rc.Width()/2.0);

	int xstart = 5;
	int ystart = 143;
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

		rectLabel.OffsetRect(0,(int)(i%nRowNum)*(iLableCellHeight+yspace));
		rectEdit.OffsetRect(0,(int)(i%nRowNum)*(iLableCellHeight+yspace));
		rectUnit.OffsetRect(0,(int)(i%nRowNum)*(iLableCellHeight+yspace));

		label_btn[i].Create( strEditTitle[i],WS_CHILD|WS_VISIBLE,rectLabel,this );
		label_btn[i].SetFont(23);

		pos_edit[i].Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_RIGHT,rectEdit,this, _nID+i+1 );
		pos_edit[i].SetFont(&font_);
		pos_edit[i].SetValueRange(-100000.0,100000.0, 3);

		strUnit[i].Create(strUnitLabel[i], WS_CHILD | WS_VISIBLE,rectUnit, this);
		strUnit[i].SetAlignment(1);
		strUnit[i].Initialize(kColor[0]);
		strUnit[i].SetFontType(22,350,_T("宋体"));

	}

// 	int x0=10,y0=143,w0=100,h0=25,Hspace=35,Vspace=220;
// 	int x1=37,y1=140,w1=90,h1=30;
// 
// 	CString label[3];
// 	label[0]=_T("X                       mm");
// 	label[1]=_T("Y                       mm");
// 	label[2]=_T("Z                       mm");
// 
// 	for ( int i=0; i<3; i++ )
// 	{
// 
// 		CRect rect0,rect1;
// 
// 		rect0.left=x0;		rect0.top=y0+i*Hspace;
// 		rect0.right=x0+w0;	rect0.bottom=y0+h0+i*Hspace;
// 		rect1.left=x1;		rect1.top=y1+i*Hspace;
// 		rect1.right=x1+w1;	rect1.bottom=y1+h1+i*Hspace;
// 
// 		label_btn[i].Create(label[i],WS_CHILD|WS_VISIBLE,rect0,this );
// 		label_btn[i].SetFont(22);
// 		pos_edit[i].Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_RIGHT,rect1,this,REFGUIDE_EDIT_1+i);
// 		pos_edit[i].SetFont(&font_);
// 		pos_edit[i].SetValueRange(-9999.9999,9999.9999,3);
// 		pos_edit[i].SetDecimalLoc(2);
// 		pos_edit[i].SetValue(0.00);
// 	}
  
	label_btn[3].Create(_T("反向"),WS_CHILD|WS_VISIBLE,CRect(CPoint(75,95),CSize(280,30)),this );//
	label_btn[3].SetFont(22);

	CRect rcra[3];
	rcra[0] = rcra[1] = rcra[2] = CRect(CPoint(40,40),CSize(60,32)); //CRect(10,60,300,90);

	for ( int i = 0; i < 3; ++i )
	{
		rcra[i].OffsetRect(80*i,0);
		CPngRadio* pRadio = new CPngRadio();
		pRadio->Create(NULL,WS_CHILD|WS_VISIBLE|WS_GROUP,rcra[i],this,REFGUIDE_RADIO_BUTTON_1+i);
		pRadio->SetFontType(20,_T("Arial"));
		pRadio->InitImage(NULL,_T("res/radio-slelected.bmp"),_T("res/radio-empty.bmp"),_T("res/radio-empty_disable.bmp"),_T("res/radio-slelected_disable.bmp") );
		radio_pos_type.push_back(pRadio); 
	}
	radio_pos_type[0]->SetWindowText(_T("X"));
	radio_pos_type[1]->SetWindowText(_T("Y"));
	radio_pos_type[2]->SetWindowText(_T("Z"));
	CPngRadio::AddGroup(radio_pos_type);  
	radio_pos_type[0]->SetChecked(TRUE);


	check_reverse.Create(_T(""),WS_CHILD|WS_VISIBLE|WS_GROUP ,CRect(CPoint(40,90),CSize(30,32)),this,REFGUIDE_CHECK_BUTTON); 
	check_reverse.loadBmp(_T("res/unchecked.bmp"),_T("res/checked.bmp"),_T("res/disablecheck.bmp"),_T("res/disableUncheck.bmp"));
  
	btn_teach.Create(_T("示教"),WS_CHILD|WS_VISIBLE,CRect(CPoint(35,265),CSize(95,50)),this,REFGUIDE_TEACH_BUTTON);  
    btn_teach.SetFont(&font_);

	return TRUE;
}
 
void CRefSettingGuide::update_region_show( int iMethod, int iStep )
{
	CString titleDialog;
	m_iMethod = iMethod;
	m_iStep   = iStep;

	int idx_radio_disable = -1;
 	
	static int last_step = 0;

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
				titleDialog = _T("第2步: 选择原点");
			    show_check_radio(FALSE);
				show_pos_info(0);
  				break;
			case 3: 
 				titleDialog = _T("第3步: 选择轴上的点");
				show_check_radio(TRUE);
				show_pos_info(1);
				change_radio_description(TRUE);
  				break;
			case 4:
 				titleDialog = _T("第4步: 选择平面的点");
				idx_radio_disable = 2 - teach_point[1].type;
				show_pos_info(2);
				show_check_radio(TRUE, iStep > last_step ? idx_radio_disable : -1);
				change_radio_description(FALSE);
   				break;
			case 5: 
				calc_reference_system();
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
				titleDialog = _T("第2步: 选择第1轴的第1点");
				show_check_radio(TRUE);
				show_pos_info(0);
 				break;
			case 3:
				titleDialog = _T("第3步: 选择第1轴的第2点");
 				show_check_radio(FALSE);
				show_pos_info(1);
  				break;
			case 4: 
				titleDialog = _T("第4步: 选择第2轴的点");
  				show_check_radio(TRUE,iStep > last_step ? teach_point[1].type : -1 );
				show_pos_info(2);
  				break;
			case 5:
				calc_reference_system();
				break;
			default:break;
			}
 			change_radio_description(TRUE);
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
 				titleDialog = _T("第2步: 选择原点");
				show_check_radio(FALSE);
				show_pos_info(0);
 				break;
			case 3:
				cout << " calc refdata 2\n";
				break;
			case 4: break;
			case 5: break;
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

void CRefSettingGuide::OnPosRadioClicked(unsigned int btnID)
{
 	m_nPosType = btnID - REFGUIDE_RADIO_BUTTON_1;
	int indexPt = m_iStep - 2;
	teach_point[indexPt].type = m_nPosType;
	 
 	cout << "teach pos type = " << m_nPosType << '\n';
}

void CRefSettingGuide::OnPosRevClicked()
{
	int i=0;
	int indexPt = m_iStep - 2;
	teach_point[indexPt].reverse = check_reverse.GetChecked() ? true : false;
}

void CRefSettingGuide::OnPosTeachClicked()
{
	if ( m_iStep < 2 || m_iStep > 4 ) // step = [2,4] enable teach point.
 		return;

    int indexPt = m_iStep - 2;
	PosInfo rpos  = SMC.get_position( PosInfo::CTmachine, LDScommad );
 
	teach_point[indexPt].pos     = rpos.cart.pos;
	teach_point[indexPt].type    = m_nPosType;
	teach_point[indexPt].reverse = check_reverse.GetChecked() ? true : false;

	pos_edit[0].SetValue(teach_point[indexPt].pos.x);
	pos_edit[1].SetValue(teach_point[indexPt].pos.y);
	pos_edit[2].SetValue(teach_point[indexPt].pos.z);
	
	std::cout << "indexPt = " << indexPt << " teach point " << teach_point[indexPt].pos << '\n';
}

void CRefSettingGuide::show_pos_info(int posID)
{
	pos_edit[0].SetValue(teach_point[posID].pos.x);
	pos_edit[1].SetValue(teach_point[posID].pos.y);
	pos_edit[2].SetValue(teach_point[posID].pos.z);

	radio_pos_type[teach_point[posID].type]->SetChecked(TRUE);
	
// 	for (unsigned int i = 0; i < radio_pos_type.size(); ++i )
// 	{
// 		if ( radio_pos_type[i]->IsWindowEnabled() )
// 		{
// 			radio_pos_type[i]->SetChecked(TRUE);
// 			break;
// 		}
// 	}

	check_reverse.SetChecked( teach_point[posID].reverse ? TRUE : FALSE );
  
	//GetParent()->Invalidate();
}

void CRefSettingGuide::show_check_radio(BOOL b, int diableRadioID)
{
	check_reverse.ShowWindow(b);
	label_btn[3].ShowWindow(b);
	for (unsigned int i = 0; i < radio_pos_type.size(); ++i )
	{
		radio_pos_type[i]->ShowWindow(b);
	}

	if ( diableRadioID == -1 )
	{
		for (unsigned int i = 0; i < radio_pos_type.size(); ++i )
 			radio_pos_type[i]->EnableWindow(TRUE);
 	}
	else
	{
		radio_pos_type[diableRadioID]->EnableWindow(FALSE);
 	}

	GetParent()->Invalidate();
}

void CRefSettingGuide::change_radio_description(BOOL bAxis)
{
	if ( bAxis )
	{
		radio_pos_type[0]->SetWindowText(_T("X"));
		radio_pos_type[1]->SetWindowText(_T("Y"));
		radio_pos_type[2]->SetWindowText(_T("Z"));
	}
	else
	{
		radio_pos_type[0]->SetWindowText(_T("XY"));
		radio_pos_type[1]->SetWindowText(_T("ZX"));
		radio_pos_type[2]->SetWindowText(_T("YZ"));
 	}
}

void CRefSettingGuide::reset_all()
{
	m_iMethod  = 0;
	m_iStep    = 0;
 	m_nPosType = 0;
	clear_show_info();
	for ( unsigned int i = 0; i < 3; ++i )
	{
		teach_point[i].pos  = Vec3D::zero_vector;
		teach_point[i].type = 0;
		teach_point[i].reverse = false;
	}
}

void CRefSettingGuide::clear_show_info()
{
	for (unsigned int i = 0; i < radio_pos_type.size(); ++i )
	{
		if ( radio_pos_type[i]->IsWindowEnabled() )
		{
			radio_pos_type[i]->SetChecked(TRUE);
			break;
 		}
	}
	check_reverse.SetChecked(FALSE);
	for (unsigned int i = 0; i < 3; ++i )
		pos_edit[i].SetValue( 0.00 );
}
 
LRESULT CRefSettingGuide::OnKeyboardEnter(WPARAM wp,LPARAM lp)
{
	CString str_result = *((CString *)wp);
	double dfLoc = 0.0;
	int iret = _stscanf(str_result, _T("%lf"), &dfLoc);

	if ( iret == 1 )
	{
		int indexPt = m_iStep - 2;

		int nEditIdx = (int)(lp) - REFGUIDE_EDIT_1;
		switch(nEditIdx)
		{
		case 0:	teach_point[indexPt].pos.x = dfLoc; break;
		case 1: teach_point[indexPt].pos.y = dfLoc; break;
		case 2: teach_point[indexPt].pos.z = dfLoc; break;
		default:break;
		}

// 		cout << "edit value = " << CStringA(str_result) << " : " << nEditIdx << endl;
		return 1;
	}
	else
		return 0;
}


void CRefSettingGuide::calc_reference_system()
{
	for ( int i = 0; i < 3; ++i )
	{
		std::cout << "indexPt = " << i << " teach point [ " << teach_point[i].pos << " ] [ " << teach_point[i].type << " ] : " << (int)teach_point[i].reverse << '\n';
	}

	switch( m_iMethod )
	{
	case 0: calc_coord_aixs_plane(); break;
	case 1: calc_coord_axes(); break;
	case 2: calc_coord_orgpoint(); break;
	default: break;
	}


}

void CRefSettingGuide::calc_coord_aixs_plane()
{
	Line3D line_axis;
	Vec3D xUnitAxis, yUnitAxis, zUnitAxis;
	Vec3D PerpendicularPos, unitAxis;

	line_axis.p1 = teach_point[0].pos; // org pos
 	line_axis.p2 = teach_point[1].pos;

	PerpendicularPos = line_axis.perpendicular_point(teach_point[2].pos);
	unitAxis = ( teach_point[2].pos - PerpendicularPos ).normalize();

	if (teach_point[1].type == 0 )// x-axis
	{
 		xUnitAxis = ( line_axis.p2 - line_axis.p1 ).normalize();
		if ( teach_point[1].reverse )
 			xUnitAxis *= -1;

 		if ( teach_point[2].type == 0 ) // xy-plane
		{
			yUnitAxis = unitAxis * ( teach_point[2].reverse ? -1 : 1 );
			zUnitAxis = xUnitAxis.crossProduct(yUnitAxis);
			zUnitAxis = zUnitAxis.normalize();
		}
		else if ( teach_point[2].type == 1 )// zx-plane
		{
			zUnitAxis = unitAxis * ( teach_point[2].reverse ? -1 : 1 );
			yUnitAxis = zUnitAxis.crossProduct(xUnitAxis);
			yUnitAxis = yUnitAxis.normalize();
		}
		else
			return;
 	}
	else if ( teach_point[1].type == 1 ) // y-axis
	{
		yUnitAxis = ( line_axis.p2 - line_axis.p1 ).normalize();
		if ( teach_point[1].reverse )
			yUnitAxis *= -1;

		if ( teach_point[2].type == 0 ) // xy-plane
		{
			xUnitAxis = unitAxis * ( teach_point[2].reverse ? -1 : 1 );
			zUnitAxis = xUnitAxis.crossProduct(yUnitAxis);
			zUnitAxis = zUnitAxis.normalize();
 		}
		else if ( teach_point[2].type == 2 ) // yz-plane
		{
			zUnitAxis = unitAxis * ( teach_point[2].reverse ? -1 : 1 );
			xUnitAxis = yUnitAxis.crossProduct(zUnitAxis);
			xUnitAxis = xUnitAxis.normalize();
 		}
		else
			return;

	}
	else if ( teach_point[2].type == 2 ) // z-axis
	{
		zUnitAxis = ( line_axis.p2 - line_axis.p1 ).normalize();
		if ( teach_point[1].reverse )
			zUnitAxis *= -1;

		if ( teach_point[2].type == 1 ) // zx-plane
		{
			xUnitAxis = unitAxis * ( teach_point[2].reverse ? -1 : 1 );
			yUnitAxis = zUnitAxis.crossProduct(xUnitAxis);
			yUnitAxis = yUnitAxis.normalize();
 		}
		else if ( teach_point[2].type == 2 ) // yz-plane
		{
			yUnitAxis = unitAxis * ( teach_point[2].reverse ? -1 : 1 );
			xUnitAxis = yUnitAxis.crossProduct(zUnitAxis);
			xUnitAxis = xUnitAxis.normalize();
 		}
		else
			return;

	}

	Matrix mrot(3,3);
	mrot(1,1) = xUnitAxis.x;
	mrot(2,1) = xUnitAxis.y;
	mrot(3,1) = xUnitAxis.z;

	mrot(1,2) = yUnitAxis.x;
	mrot(2,2) = yUnitAxis.y;
	mrot(3,2) = yUnitAxis.z;

	mrot(1,3) = zUnitAxis.x;
	mrot(2,3) = zUnitAxis.y;
	mrot(3,3) = zUnitAxis.z;

	Vec3D dirRefSys = PoseTransform::RotMatToVec(mrot,AM_EULER);

	CartRef refSys;
	refSys.cpos.pos = teach_point[0].pos;
	refSys.cpos.ang = dirRefSys;

	m_refSysResult = refSys;
	cout << " refsys = [ " << refSys << " ] \n";
}

void CRefSettingGuide::calc_coord_axes()
{
	Line3D line_axis;
	Vec3D xUnitAxis, yUnitAxis, zUnitAxis;
	Vec3D PerpendicularPos, unitAxis;

	line_axis.p1 = teach_point[0].pos; // axis first point
	line_axis.p2 = teach_point[1].pos; // second point on axis

	PerpendicularPos = line_axis.perpendicular_point(teach_point[2].pos); // org
	unitAxis = ( teach_point[2].pos - PerpendicularPos ).normalize();

	if ( teach_point[1].type == 0 )// x-axis
	{
		xUnitAxis = ( line_axis.p2 - line_axis.p1 ).normalize();
		if ( teach_point[1].reverse )
			xUnitAxis *= -1;

		if ( teach_point[2].type == 1 ) // y
		{
			yUnitAxis = unitAxis * ( teach_point[2].reverse ? -1 : 1 );
			zUnitAxis = xUnitAxis.crossProduct(yUnitAxis);
			zUnitAxis = zUnitAxis.normalize();
		}
		else if ( teach_point[2].type == 2 )// z
		{
			zUnitAxis = unitAxis * ( teach_point[2].reverse ? -1 : 1 );
			yUnitAxis = zUnitAxis.crossProduct(xUnitAxis);
			yUnitAxis = yUnitAxis.normalize();
		}
	}
	else if ( teach_point[1].type == 1 ) // y-axis
	{
		yUnitAxis = ( line_axis.p2 - line_axis.p1 ).normalize();
		if ( teach_point[1].reverse )
			yUnitAxis *= -1;

		if ( teach_point[2].type == 0 ) // x
		{
			xUnitAxis = unitAxis * ( teach_point[2].reverse ? -1 : 1 );
			zUnitAxis = xUnitAxis.crossProduct(yUnitAxis);
			zUnitAxis = zUnitAxis.normalize();
		}
		else if ( teach_point[2].type == 2 ) // z
		{
			zUnitAxis = unitAxis * ( teach_point[2].reverse ? -1 : 1 );
			xUnitAxis = yUnitAxis.crossProduct(zUnitAxis);
			xUnitAxis = xUnitAxis.normalize();
		}
	}
	else if ( teach_point[2].type == 2 ) // z-axis
	{
		zUnitAxis = ( line_axis.p2 - line_axis.p1 ).normalize();
		if ( teach_point[1].reverse )
			zUnitAxis *= -1;

		if ( teach_point[2].type == 0 ) // x
		{
			xUnitAxis = unitAxis * ( teach_point[2].reverse ? -1 : 1 );
			yUnitAxis = zUnitAxis.crossProduct(xUnitAxis);
			yUnitAxis = yUnitAxis.normalize();
		}
		else if ( teach_point[2].type == 1 ) // y
		{
			yUnitAxis = unitAxis * ( teach_point[2].reverse ? -1 : 1 );
			xUnitAxis = yUnitAxis.crossProduct(zUnitAxis);
			xUnitAxis = xUnitAxis.normalize();
		}
	}

	Matrix mrot(3,3);
	mrot(1,1) = xUnitAxis.x;
	mrot(2,1) = xUnitAxis.y;
	mrot(3,1) = xUnitAxis.z;

	mrot(1,2) = yUnitAxis.x;
	mrot(2,2) = yUnitAxis.y;
	mrot(3,2) = yUnitAxis.z;

	mrot(1,3) = zUnitAxis.x;
	mrot(2,3) = zUnitAxis.y;
	mrot(3,3) = zUnitAxis.z;

	Vec3D dirRefSys = PoseTransform::RotMatToVec(mrot,AM_EULER);

	CartRef refSys;
	refSys.cpos.pos = PerpendicularPos;
	refSys.cpos.ang = dirRefSys;

	m_refSysResult = refSys;

	cout << " refsys = [ " << refSys << " ] \n";
}

void CRefSettingGuide::calc_coord_orgpoint()
{
	CartRef refSys;
	refSys.cpos.pos = teach_point[0].pos;
	refSys.cpos.ang = Vec3D::zero_vector;

	m_refSysResult = refSys;

	cout << " refsys = [ " << refSys << " ] \n";
}