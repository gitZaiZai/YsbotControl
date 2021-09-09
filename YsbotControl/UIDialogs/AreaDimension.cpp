//////////Ã∆è™è™è™è™

#include "stdafx.h"
#include "AreaDimension.h"
#include "../Angle.h"
#include "../colourcode.h"

BEGIN_MESSAGE_MAP(CAreaDimension, CGroupFrame)
 
END_MESSAGE_MAP()


CAreaDimension::CAreaDimension()
{

}
CAreaDimension::~CAreaDimension()
{

}
 
BOOL CAreaDimension::Create(CString caption,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CStatic::Create(caption, dwStyle|SS_NOTIFY, rect, pParentWnd, nID);
	_nID=nID;

	font_.CreateFont(20, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("Arial"));
 
	CRect rc;
	GetClientRect(&rc);
	LPCTSTR	 strLabelText[] = { _T("X"),  _T("Y"), _T("Z"),    _T("A"), _T("B"), _T("C"), _T("lenX"), _T("lenY"), _T("lenZ"), _T("R"), _T("H") };
	for ( int i =0; i < ARRAY_SIZE(strLabelText); ++i )
	{
		DimensionText[i] = strLabelText[i];
	}

	LPCTSTR	 strUnitLabel[]  = { _T("mm"),  _T("mm"), _T("mm"),  _T("°„"), _T("°„"), _T("°„"),_T("mm"),  _T("mm"),  _T("mm"),   _T("mm"), _T("mm") };
	int iThirdWidth = (int)(rc.Width()/3.0)-10;

	int xstart = 0;
	int ystart = 40;
	int iLableCellHeight = 25;
	int iEditBoxHeight   = 30;
	int iLabelCellWidth  = 40;
	int iLableUnitCellWidth = 25;
	int iEditBoxWidth    = iThirdWidth - (iLabelCellWidth + iLableUnitCellWidth);
	CRect rcLableName = CRect(CPoint(rc.left+xstart,rc.top+ystart),CSize(iLabelCellWidth,iLableCellHeight));
	CRect rcEditBox   = CRect(CPoint(rcLableName.right+5,rc.top+ystart),CSize(iEditBoxWidth,iEditBoxHeight));
	CRect rcLableUnit = CRect(CPoint(rcEditBox.right,rc.top+ystart),CSize(iLableUnitCellWidth,iLableCellHeight));

	int yspace = 10;
	int xspace = 10;

	CString label;
	CRect rectLabel, rectEdit, rectUnit;
	int nRowNum = 3;
 	for ( int i = 0; i < ARRAY_SIZE(DimensionEdit); ++i )
	{
		rectLabel = rcLableName;
		rectEdit  = rcEditBox;
		rectUnit  = rcLableUnit;
 
		rectLabel.OffsetRect((int)(i/nRowNum)*(iThirdWidth+xspace),(int)(i%nRowNum)*(iLableCellHeight+yspace));
		rectEdit.OffsetRect((int)(i/nRowNum)*(iThirdWidth+xspace),(int)(i%nRowNum)*(iLableCellHeight+yspace));
		rectUnit.OffsetRect((int)(i/nRowNum)*(iThirdWidth+xspace),(int)(i%nRowNum)*(iLableCellHeight+yspace));
		 
		DimensionLabel[i].Create( DimensionText[i],WS_CHILD|WS_VISIBLE,rectLabel,this );
		DimensionLabel[i].SetAlignment(2);
		DimensionLabel[i].Initialize(kColor[0]);
		DimensionLabel[i].SetFontType(22,350,_T("ÀŒÃÂ"));
 
		DimensionEdit[i].Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_RIGHT,rectEdit,this, _nID+i );
		DimensionEdit[i].SetFont(&font_);
 		DimensionEdit[i].SetValueRange(-100000.0,100000.0, 3);
		DimensionEdit[i].SetValue(0.00);

		strUnit[i].Create(strUnitLabel[i], WS_CHILD | WS_VISIBLE,rectUnit, this);
		strUnit[i].SetAlignment(1);
		strUnit[i].Initialize(kColor[0]);
		strUnit[i].SetFontType(22,350,_T("ÀŒÃÂ"));

	}
 
	// disable angle A B C setting.
	for ( int i = 3; i < 6; ++i )
 		DimensionEdit[i].EnableWindow(FALSE);
  
	return TRUE;
}

void CAreaDimension::update_text(int ishape)
{
	if (ishape==0)
	{
		DimensionLabel[6].SetWindowTextW(DimensionText[6]);
		DimensionLabel[7].SetWindowTextW(DimensionText[7]);
  		DimensionLabel[8].ShowWindow(SW_SHOW);
		DimensionEdit[8].ShowWindow(SW_SHOW);
		strUnit[8].ShowWindow(SW_SHOW);
// 		DimensionLabel[11].ShowWindow(SW_SHOW);
 	}
	else if (ishape==1)
	{
		DimensionLabel[6].SetWindowTextW(DimensionText[9]);
		DimensionLabel[7].SetWindowTextW(DimensionText[10]);
		DimensionLabel[8].ShowWindow(SW_HIDE);
		DimensionEdit[8].ShowWindow(SW_HIDE);
		strUnit[8].ShowWindow(SW_HIDE);
	} 
	GetParent()->Invalidate();
}

void CAreaDimension::update_dimension_show( const SecurArea& area )
{
 	DimensionEdit[0].SetValue( area.xPos );
	DimensionEdit[1].SetValue( area.yPos );
	DimensionEdit[2].SetValue( area.zPos );

	DimensionEdit[6].SetValue( area.shapeParam[0] );
	DimensionEdit[7].SetValue( area.shapeParam[1] );
	DimensionEdit[8].SetValue( 0.00 );

	if ( area.iShapeType == 0 ) // box
	{
		DimensionEdit[8].SetValue( area.shapeParam[2] );
 	}
}

void CAreaDimension::get_dimension( SecurArea* pArea)
{
	pArea->xPos = DimensionEdit[0].GetValue();
	pArea->yPos = DimensionEdit[1].GetValue();
	pArea->zPos = DimensionEdit[2].GetValue();

	pArea->shapeParam[0] = DimensionEdit[6].GetValue();
	pArea->shapeParam[1] = DimensionEdit[7].GetValue();
	pArea->shapeParam[2] = 0.00;

	if ( pArea->iShapeType == 0 ) // box
	{
 		pArea->shapeParam[2]  = DimensionEdit[8].GetValue();
 	}
}