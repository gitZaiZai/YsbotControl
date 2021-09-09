#include "stdafx.h"
#include "CRefCoSysSetting.h"
#include "../Angle.h"
#include "../colourcode.h"

////-----------�ڲ��ӿ��� ��ʼ--------------
#pragma region Class CRefCoSysSetting
//MPLEMENT_DYNAMIC(CRefCoSysSetting, CStatic)
BEGIN_MESSAGE_MAP(CRefCoSysSetting, CGroupFrame)
 	ON_CBN_SELCHANGE(IDC_UserCoSys_RefSetting+9, &CRefCoSysSetting::OnSelchangeBaseRef )	

END_MESSAGE_MAP()

CRefCoSysSetting::CRefCoSysSetting() 
{

}

CRefCoSysSetting::~CRefCoSysSetting() 
{

}

BOOL CRefCoSysSetting::Create(CString caption,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CStatic::Create(caption, dwStyle|SS_NOTIFY, rect, pParentWnd, nID);
	Initialize(rect, pParentWnd, nID);		
	_nID=nID;

	font_.CreateFont(20, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("����"));

	CRect rc;
	GetClientRect(&rc);
	LPCTSTR	 strEditTitle[] = { _T("X"),  _T("Y"), _T("Z"),  _T("A"), _T("B"), _T("C")  };
	LPCTSTR	 strUnitLabel[] = { _T("mm"),  _T("mm"), _T("mm"),  _T("��"), _T("��"), _T("��") };

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
 	for ( int i = 0; i < ARRAY_SIZE(edit_joint); ++i )
	{
		rectLabel = rcLableName;
		rectEdit  = rcEditBox;
		rectUnit  = rcLableUnit;
 
		rectLabel.OffsetRect((int)(i/nRowNum)*(iHalfWidth+xspace),(int)(i%nRowNum)*(iLableCellHeight+yspace));
		rectEdit.OffsetRect((int)(i/nRowNum)*(iHalfWidth+xspace),(int)(i%nRowNum)*(iLableCellHeight+yspace));
		rectUnit.OffsetRect((int)(i/nRowNum)*(iHalfWidth+xspace),(int)(i%nRowNum)*(iLableCellHeight+yspace));
		 
		label_joint[i].Create( strEditTitle[i],WS_CHILD|WS_VISIBLE,rectLabel,this );
		label_joint[i].SetFont(23);

		edit_joint[i].Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_RIGHT,rectEdit,this, _nID+i+1 );
		edit_joint[i].SetFont(&font_);
 		edit_joint[i].SetValueRange(-100000.0,100000.0, 3);

 		strUnit[i].Create(strUnitLabel[i], WS_CHILD | WS_VISIBLE,rectUnit, this);
		strUnit[i].SetAlignment(1);
		strUnit[i].Initialize(kColor[0]);
		strUnit[i].SetFontType(22,350,_T("����"));

	}
 

	label_joint[6].Create(_T("������ϵ"),WS_CHILD|WS_VISIBLE,CRect(10,35,300,65),this );
	label_joint[6].SetFont(20);
	label_joint[7].Create(_T("�Ի�����ϵΪ�ο���λ�ú���̬"),WS_CHILD|WS_VISIBLE,CRect(10,120,300,140),this );
	label_joint[7].SetFont(20);

	// 	base_ref.Create(WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|CBS_NOINTEGRALHEIGHT,CRect(10,60,340,305),this,IDC_UserCoSys_RefSetting+9);
	base_ref.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED |CBS_HASSTRINGS,CRect(10,60,340,305),this,IDC_UserCoSys_RefSetting+9);
	base_ref.set_custom_message(FALSE);
	base_ref.EnableWindow(FALSE);
	// 	base_ref.SetMyFont();
	// 	base_ref.BringWindowToTop();
	////test data
	return TRUE;
}
 
void CRefCoSysSetting::OnSelchangeBaseRef()
{
	int index=base_ref.GetCurSel();
	//CString* str;
	//variable_Combobox.GetLBText(index,*str);
	GetParent()->SendMessage(WM_SelChangeBaseRef,(WPARAM)index);
}

void CRefCoSysSetting::SetData(double* JArray,unsigned int JNum)
{
	double joint[8];
	CString Jstr;
	for (unsigned int i=0;i<JNum;i++)
	{
		if (i>=6) return;
		joint[i]=*(JArray+i);
		edit_joint[i].SetValue(joint[i]);
	}
}

void CRefCoSysSetting::GetData(double* JArray,unsigned int JNum)
{
	double joint[8];
	CString Jstr;
	for (unsigned int i=0;i<JNum;i++)
	{
		if (i>=6) return;
		edit_joint[i].GetWindowText(Jstr);
		joint[i]=_ttof(Jstr);
		*(JArray+i)=joint[i];
	}
}

void CRefCoSysSetting::SetReadOnly(BOOL bReadOnly)
{
	for(unsigned int i=0;i<6;i++)
	{
		edit_joint[i].SetReadOnly(bReadOnly);
	}
}

void CRefCoSysSetting::clear_show()
{
	for (unsigned int i=0;i<6;i++)
	{
 		edit_joint[i].SetValue(0.00);
	}
}

#pragma endregion