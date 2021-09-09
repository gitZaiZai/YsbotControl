#include "stdafx.h"
#include "../TeachDialog/JointData.h"
#include "../colourcode.h"
#include "../Angle.h"

//MPLEMENT_DYNAMIC(CJointData, CStatic)
BEGIN_MESSAGE_MAP(CJointData, CGroupFrame)
	ON_WM_CREATE()

END_MESSAGE_MAP()

CJointData::CJointData() 
{

}

CJointData::~CJointData() 
{

}

BOOL CJointData::Create(CString caption,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	_nID=nID;
	CStatic::Create(caption, dwStyle|SS_NOTIFY, rect, pParentWnd, nID);
	Initialize(rect, pParentWnd, nID);			
	return TRUE;
}

int CJointData::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	font_.CreateFont(20, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("ËÎÌו"));

 	CRect rc;
	GetClientRect(&rc);

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

	for ( int i = 0; i < ARRAY_SIZE(edit_joint); ++i )
	{
		label.Format(_T("A%d"),i+1);
		rectLabel = rcLableName;
		rectEdit  = rcEditBox;
		rectUnit  = rcLableUnit;
		if ( i < 6 )
		{
			 rectLabel.OffsetRect(0,i*(iLableCellHeight+yspace) );
			 rectEdit.OffsetRect(0,i*(iLableCellHeight+yspace));
			 rectUnit.OffsetRect(0,i*(iLableCellHeight+yspace));
 		} 
		else  
		{
			rectLabel.OffsetRect(iHalfWidth,(int)(i%6)*(iLableCellHeight+yspace));
			rectEdit.OffsetRect(iHalfWidth,(int)(i%6)*(iLableCellHeight+yspace));
			rectUnit.OffsetRect(iHalfWidth,(int)(i%6)*(iLableCellHeight+yspace));
		}
		label_joint[i].Create(label,WS_CHILD|WS_VISIBLE,rectLabel,this );
		label_joint[i].SetFont(22);

		edit_joint[i].Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_RIGHT,rectEdit,this, _nID+i+1 );
		edit_joint[i].SetFont(&font_);
		edit_joint[i].SetValueRange(-100000.0,100000.0,3);
		
		strUnit[i].Create(_T("¡ד"), WS_CHILD | WS_VISIBLE,rectUnit, this);
		strUnit[i].SetAlignment(1);
		strUnit[i].Initialize(kColor[0]);
		strUnit[i].SetFontType(20,350,_T("ËÎÌו"));

	}
 
	return 0;
}

void CJointData::SetData(double* JArray,unsigned int JNum)
{
	double joint[8];
	CString Jstr;
	for (unsigned int i=0;i<JNum;i++)
	{
		if (i>=8) return;
		joint[i]=*(JArray+i);
		Jstr.Format(_T("%.3lf"),joint[i]);
		edit_joint[i].SetWindowTextW(Jstr);
	}
}

void CJointData::GetData(double* JArray,unsigned int JNum)
{
	double joint[12];
	CString Jstr;
	for (unsigned int i=0;i<JNum;i++)
	{
		if (i>=8) return;
		edit_joint[i].GetWindowText(Jstr);
		joint[i]=_ttof(Jstr);
		*(JArray+i)=joint[i];
	}
}

void CJointData::SetReadOnly(BOOL bReadOnly)
{
	for(unsigned int i=0;i<8;i++)
	{
		edit_joint[i].SetReadOnly(bReadOnly);
	}

}