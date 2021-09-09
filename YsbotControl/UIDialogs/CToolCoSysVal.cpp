#include "stdafx.h"
#include "CToolCoSysVal.h" 
#include "../colourcode.h"
#include "../Angle.h"

#pragma region	Class CToolCoSysVal
///MPLEMENT_DYNAMIC(CToolCoSysVal, CStatic)

BEGIN_MESSAGE_MAP(CToolCoSysVal, CGroupFrame)
 
END_MESSAGE_MAP()

CToolCoSysVal::CToolCoSysVal() 
{

}

CToolCoSysVal::~CToolCoSysVal() 
{

}

BOOL CToolCoSysVal::Create(CString caption,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CStatic::Create(caption, dwStyle|SS_NOTIFY, rect, pParentWnd, nID);
	Initialize(rect, pParentWnd, nID);
	_nID=nID;

	font_.CreateFont(20, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("宋体"));
	CRect rc;
	GetClientRect(&rc);
	LPCTSTR	 strEditTitle[] = { _T("X"),  _T("Y"), _T("Z"),  _T("A"), _T("B"), _T("C")  };
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
		strUnit[i].SetFontType(22,350,_T("宋体"));

	}
 
	label_joint[6].Create(_T("以当前用户坐标为参考的工具TCP值"),WS_CHILD|WS_VISIBLE,CRect(10,120,300,140),this );
	label_joint[6].SetFont(20);
	return TRUE;
}
  
void CToolCoSysVal::SetData(double* JArray,unsigned int JNum)
{
	double joint[8];
	for (unsigned int i=0;i<JNum;i++)
	{
		if (i>=6) return;
		joint[i]=*(JArray+i);
		edit_joint[i].SetValue(joint[i]);
	}
}

void CToolCoSysVal::GetData(double* JArray,unsigned int JNum)
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

void CToolCoSysVal::SetReadOnly(BOOL bReadOnly)
{
	for(unsigned int i=0;i<6;i++)
	{
		edit_joint[i].SetReadOnly(bReadOnly);
	}

}
#pragma endregion