#pragma once

#include "UIDialogs/DialogType.h"
// CDlgDummy dialog
#include "BasicControls/labels.h"
class CDlgDummy : public DialogType
{

public:
	CDlgDummy();   // standard constructor
	virtual ~CDlgDummy();

// Dialog Data
	
	BasicLabel title1_;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg LRESULT OnMainButtonClicked(WPARAM,LPARAM); 
 
 	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
};
