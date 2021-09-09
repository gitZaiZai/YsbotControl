//////////�Ə�������

#include "stdafx.h"
#include "AreaGeneral.h"


BEGIN_MESSAGE_MAP(CAreaGeneral, CGroupFrame)
	//ON_CBN_SELCHANGE(IDC_General_COMBOX_1,OnCbnSelchange)
	ON_CBN_SELCHANGE(IDC_General_COMBOX_1,&CAreaGeneral::OnCbnSelShape)
	ON_CBN_SELCHANGE(IDC_General_COMBOX_1+1,&CAreaGeneral::OnCbnSelType)
	
END_MESSAGE_MAP()


CAreaGeneral::CAreaGeneral()
{
}

CAreaGeneral::~CAreaGeneral()
{
}
 
BOOL CAreaGeneral::Create(CString caption,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CStatic::Create(caption, dwStyle|SS_NOTIFY, rect, pParentWnd, nID);
	//Initialize(rect, pParentWnd, nID);
	_nID=nID;

	font_.CreateFont(20, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("Arial"));
 
	textStr[0]=_T("������״");
	textStr[1]=_T("��������");
	textStr[2]=_T("�ο�����");
	textStr[3]=_T("���÷�ʽ");
	textStr[4]=_T("״̬����");
	textStr[5]=_T("�����ͣ");
	textStr[6]=_T("����");
	
	Area_cmb[3].SetSort(false);
	Area_cmb[4].SetSort(false);
	for (int i=0;i<5;i++)
	{
		CRect rect0,rect1;
		if (i<3)
		{
			rect0.left=10;		rect0.top=35+i*45;
			rect0.right=100;	rect0.bottom=75+i*45;

			rect1.left=100;		rect1.top=30+i*45;
			rect1.right=300;	rect1.bottom=200+i*45;

			Area_cmb[i].Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED |CBS_HASSTRINGS,rect1,this,IDC_General_COMBOX_1+i);
			Area_cmb[i].set_custom_message(FALSE);
 		}
		if(i>=3)
		{
			rect0.left=370;		rect0.top=35+(i-3)*45;
			rect0.right=460;	rect0.bottom=75+(i-3)*45;

		
			rect1.left=460;		rect1.top=30+(i-3)*45;
			rect1.right=660;	rect1.bottom=200+(i-3)*45;

			Area_cmb[i].Create( WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED |CBS_HASSTRINGS, rect1,this,IDC_General_COMBOX_1+i);
			Area_cmb[i].set_custom_message(FALSE);
			
		} //λ��
 
		AreaLabel[i].SetFont(22);
		AreaLabel[i].Create(textStr[i],WS_CHILD|WS_VISIBLE,rect0,this);
		//����
	}
 	
	Area_cmb[0].AddString(_T("BOX"),0);
	Area_cmb[0].AddString(_T("CYLINDER"),0);
	Area_cmb[0].SetCurSel(0);
	Area_cmb[0].SetShowImage(FALSE);

	Area_cmb[1].AddString(_T("BLOCKEDAREA"),0);
    Area_cmb[1].AddString(_T("WORKAREA"),0);
 	Area_cmb[1].SetCurSel(0);
	Area_cmb[1].SetShowImage(FALSE);
    
	Area_cmb[2].AddString(_T("Base"),0);
	Area_cmb[2].SetCurSel(0);
	Area_cmb[2].EnableWindow(FALSE);

	Area_cmb[3].AddString(_T("�ֶ�����"),0);
	Area_cmb[3].AddString(_T("�������0"),0);
	Area_cmb[3].AddString(_T("�������1"),0);
	Area_cmb[3].AddString(_T("ͨ������˿�2"),0);
	Area_cmb[3].AddString(_T("ͨ������˿�3"),0);
	Area_cmb[3].AddString(_T("ͨ������˿�4"),0);
   	Area_cmb[3].SetCurSel(0);
	Area_cmb[3].SetShowImage(FALSE);

	CString str;
	Area_cmb[4].AddString(_T("ȫ�ֲ�������0"),0);
	for ( int i = 0; i < 10; ++i )
	{
		str.Format(_T("ͨ������˿�%d"),i);
		Area_cmb[4].AddString(str,i);
 	}
	Area_cmb[4].SetCurSel(0);
	Area_cmb[4].SetShowImage(FALSE);

	//ѡ��

	AreaLabel[5].Create(textStr[5],WS_CHILD|WS_VISIBLE,CRect(370,126,460,166),this);
	AreaLabel[5].SetFont(22);
	//Deactive.Create(_T(""),WS_CHILD|WS_VISIBLE|WS_GROUP ,CRect(470,120,530,160),this); 
	emgc_check.Create(_T(""),WS_CHILD|WS_VISIBLE|WS_GROUP ,CRect(CPoint(465,120),CSize(30,32)),this); 
    emgc_check.loadBmp(_T("res/unchecked.bmp"),_T("res/checked.bmp"),_T("res/disablecheck.bmp"),_T("res/disableUncheck.bmp"));
	
 
	AreaLabel[6].Create(textStr[6],WS_CHILD|WS_VISIBLE,CRect(600,127,605,167),this);
	AreaLabel[6].SetFont(22);
//	Auto_active.Create(_T(""),WS_CHILD|WS_VISIBLE|WS_GROUP ,CRect(665,120,700,160),this); 
	Auto_active.Create(_T(""),WS_CHILD|WS_VISIBLE|WS_GROUP ,CRect(CPoint(665,120),CSize(30,32)),this); 
	Auto_active.loadBmp(_T("res/unchecked.bmp"),_T("res/checked.bmp"),_T("res/disablecheck.bmp"),_T("res/disableUncheck.bmp"));

	AreaLabel[6].ShowWindow(SW_HIDE);
	Auto_active.ShowWindow(SW_HIDE);

	//NextPage.Create(_T(">>"),WS_CHILD|WS_VISIBLE,CRect(595,125,655,155),this,IDC_GENERAL_NEXT); 
	return TRUE;
    
}
 
// shape type: box or cylinder.
void CAreaGeneral::OnCbnSelShape()
{
	int index = Area_cmb[0].GetCurSel();
	//cout<<"index = "<<index<<endl;
	GetParent()->SendMessage(WM_SELAREASHAPE,(WPARAM)index);
}

// area type: work or block
void CAreaGeneral::OnCbnSelType()
{
	int index = Area_cmb[1].GetCurSel();
	GetParent()->SendMessage(WM_SELAREATYPE,(WPARAM)index);
}

void CAreaGeneral::update_general_show( const SecurArea& area)
{
	// may be refresh illustration and dimension window.

	Area_cmb[0].SetCurSel(area.iShapeType);
	//if ( area.iShapeType != Area_cmb[0].GetCurSel() )
	{
		OnCbnSelShape();
	}
  	Area_cmb[1].SetCurSel(area.iAreaType);
	//if ( area.iAreaType != Area_cmb[1].GetCurSel() )
	{
		OnCbnSelType();
	}

  	Area_cmb[3].SetCurSel(area.iActiveVar); // active method
	Area_cmb[4].SetCurSel(area.iStateVar);   // output state variable

	emgc_check.SetChecked( area.emgcOut );
}

void CAreaGeneral::get_general_setting( SecurArea* pArea )
{
	pArea->iShapeType = Area_cmb[0].GetCurSel();
	pArea->iAreaType  = Area_cmb[1].GetCurSel();
	pArea->iActiveVar = Area_cmb[3].GetCurSel();
	pArea->iStateVar   = Area_cmb[4].GetCurSel();
	pArea->emgcOut    = emgc_check.GetChecked() ? true : false;
}