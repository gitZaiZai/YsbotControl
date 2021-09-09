

//////////Ã∆è™è™è™è™

#pragma once


#include "../IO Dialog/StaticTransparent.h"
#include "../IO Dialog/GroupFrame.h"
#include "../BasicControls/Label.h"
#include "../BasicControls/labels.h"
#include "SecurArea.h"
#include "../Keyboard/PopEdit.h"

#define IDC_SecurArea_Dimension 5030

class CAreaDimension:public CGroupFrame
{
public:
	CAreaDimension();
	virtual ~CAreaDimension();
	BOOL Create(CString caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd=NULL, UINT nID=65535);
	void update_text(int ishape); 
	void show_edit(bool);

	void update_dimension_show( const SecurArea& );

	void get_dimension( SecurArea* );

protected:
 
	DECLARE_MESSAGE_MAP()
private:
	unsigned int _nID;
	CFont	font_;
	BasicLabel DimensionLabel[12];
	CpopEditDouble DimensionEdit[9];
	CString DimensionText[14];
	BasicLabel     strUnit[9];

};