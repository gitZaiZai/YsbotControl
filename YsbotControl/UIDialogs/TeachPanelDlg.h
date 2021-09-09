#pragma once

#include "DialogType.h"
#include "../IO Dialog/StaticTransparent.h"
#include "../IO Dialog/GroupFrame.h"
#include "../ConfigReader.h"
#include "../BasicControls/HelperButton.h"
#include "../BasicControls/BkDialogST.h"

class CTeachPanelDlg : public CBkDialogST
{
public:
	CTeachPanelDlg(const ConfigReader& );
	virtual ~CTeachPanelDlg();
 
protected:
	virtual BOOL OnInitDialog();
 
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnMainButtonClicked(WPARAM,LPARAM); 
 	
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual int OnActivateChildWin(LPARAM);

private:
    HBITMAP m_bmpBackGroud;
	HBITMAP m_bmpCartTeachBk;

	CHelperButton m_ButtonJogLeft[6];
	CHelperButton m_ButtonJogRight[6];
  	HICON     m_iconDirection[6];

	void OnPostEraseBkgnd(CDC* pDC);
	void teach_axis_coord ();
	void teach_cart_coord ();
};

 