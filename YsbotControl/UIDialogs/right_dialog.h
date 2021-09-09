#pragma once

#include "DialogType.h"
#include "../colourcode.h"
#include "../Time.h"
#include "../BasicControls/Label.h"
#include <vector>

class RightDialog : public DialogType 
{
public:
	enum	{	RIGHT_LABEL_AXIS	= 0,			 
		RIGHT_LABEL_WORLD,					 
		RIGHT_LABEL_TOOL,			 
		RIGHT_LABEL_USER,				 
		RIGHT_LABEL_SECOND,	
		RIGHT_LABEL_TEACHMOVE,
	};

	RightDialog();
	virtual ~RightDialog();
 
	void SetState(int number, CString text);
	void update_window ( );


	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

private:
	virtual BOOL OnInitDialog();	
	virtual void DoDataExchange(CDataExchange* pDX);

	void init_right_label();

	int labelType;
	CRect rect2nd;

	bool IsCoveredByOtherWindow(HWND hWnd);
	std::vector<CLabel*>  label_list;
	 
};