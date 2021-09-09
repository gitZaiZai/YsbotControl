#include "stdafx.h"
#include "../TeachDialog/PoseData.h"
#include "../colourcode.h"
#include "../Angle.h"

BEGIN_MESSAGE_MAP(CPoseData, CGroupFrame)
	ON_WM_CREATE()

END_MESSAGE_MAP()

CPoseData::CPoseData() 
{

}

CPoseData::~CPoseData() 
{

}

BOOL CPoseData::Create(CString caption,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	_nID=nID;
	CStatic::Create(caption, dwStyle|SS_NOTIFY, rect, pParentWnd, nID);
	Initialize(rect, pParentWnd, nID);	
	return TRUE;
}

int CPoseData::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	font_.CreateFont(20, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("ËÎÌå"));

	CRect rc;
	GetClientRect(&rc);
	LPCTSTR	 strEditTitle[] = { _T("X"),  _T("Y"), _T("Z"),  _T("A"), _T("B"), _T("C"),_T("A7"), _T("A8"), _T("Fig") };
	LPCTSTR	 strUnitLabel[] = { _T("mm"),  _T("mm"), _T("mm"),  _T("¡ã"), _T("¡ã"), _T("¡ã"),_T("¡ã"), _T("¡ã"), _T("") };

	int iHalfWidth = (int)(rc.Width()/2.0);

	int xstart = 5;
	int ystart = 30;
	int iLabelCellWidth  = 35;
	int iLableCellHeight = 25;
	int iEditBoxHeight   = 30;
	int iEditBoxWidth    = iHalfWidth - (iLabelCellWidth + iLabelCellWidth);
	CRect rcLableName = CRect(CPoint(rc.left+xstart,rc.top+ystart),CSize(iLabelCellWidth,iLableCellHeight));
	CRect rcEditBox   = CRect(CPoint(rcLableName.right,rc.top+ystart),CSize(iEditBoxWidth,iEditBoxHeight));
	CRect rcLableUnit = CRect(CPoint(rcEditBox.right,rc.top+ystart),CSize(iLabelCellWidth,iLableCellHeight));

	int yspace = 10;

	CString label;
	CRect rectLabel, rectEdit, rectUnit;
	int nRowNum = 8; 
	for ( int i = 0; i < ARRAY_SIZE(edit_joint); ++i )
	{
 		rectLabel = rcLableName;
		rectEdit  = rcEditBox;
		rectUnit  = rcLableUnit;
		if ( i < nRowNum )
		{
			rectLabel.OffsetRect(0,i*(iLableCellHeight+yspace) );
			rectEdit.OffsetRect(0,i*(iLableCellHeight+yspace));
			rectUnit.OffsetRect(0,i*(iLableCellHeight+yspace));
		} 
		else  
		{
			rectLabel.OffsetRect(iHalfWidth,(int)(i%nRowNum)*(iLableCellHeight+yspace));
			rectEdit.OffsetRect(iHalfWidth,(int)(i%nRowNum)*(iLableCellHeight+yspace));
			rectUnit.OffsetRect(iHalfWidth,(int)(i%nRowNum)*(iLableCellHeight+yspace));
		}
		label_joint[i].Create( strEditTitle[i],WS_CHILD|WS_VISIBLE,rectLabel,this );
		label_joint[i].SetFont(23);

		edit_joint[i].Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_RIGHT,rectEdit,this, _nID+i+1 );
		edit_joint[i].SetFont(&font_);
		if ( i == ARRAY_SIZE(edit_joint)-1 )
			edit_joint[i].SetValueRange(0, 50, 0);
 		else
			edit_joint[i].SetValueRange(-100000.0,100000.0, 3);

 	 
 		strUnit[i].Create(strUnitLabel[i], WS_CHILD | WS_VISIBLE,rectUnit, this);
		strUnit[i].SetAlignment(1);
		strUnit[i].Initialize(kColor[0]);
		strUnit[i].SetFontType(22,350,_T("ËÎÌå"));

	}
 	return 0;
}

void CPoseData::SetData(double* PArray,unsigned int JNum)
{
	double joint[8];
	//CString Jstr;
	for (unsigned int i=0;i<JNum;i++)
	{
		if (i>=8) return;
		joint[i]=*(PArray+i);
// 		Jstr.Format(_T("%.3lf"),joint[i]);
// 		edit_joint[i].SetWindowTextW(Jstr);
		edit_joint[i].SetValue(joint[i]);
	}
}

void CPoseData::GetData(double* PArray,unsigned int JNum)
{
	double joint[12];
	CString Jstr;
	for (unsigned int i=0;i<JNum;i++)
	{
		if (i>=8) return;
		edit_joint[i].GetWindowText(Jstr);
		joint[i]=_ttof(Jstr);
		*(PArray+i)=joint[i];
	}
}

void CPoseData::SetMode(int pmode)
{
	CString str;
	str.Format(_T("%d"),pmode);
	//mode_.SetWindowTextW(str);
	edit_joint[8].SetWindowTextW(str);
}

int CPoseData::GetMode()
{
	CString str;
	//mode_.GetWindowTextW(str);
	edit_joint[8].GetWindowTextW(str);
	return _ttol(str);
}

void CPoseData::SetReadOnly(BOOL bReadOnly)
{
	for(unsigned int i=0;i<9;i++)
	{
		edit_joint[i].SetReadOnly(bReadOnly);
	}
	//mode_.SetReadOnly(bReadOnly);
}

