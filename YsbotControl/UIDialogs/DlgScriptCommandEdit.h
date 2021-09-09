#pragma once

#include "DialogType.h"
#include "../Keyboard/PopEdit.h"
#include "../BasicControls/BtnST.h"
#include "../BasicControls/PngRadio.h"
#include "../LuaScript/ScriptCommandInfo.h"

// CDlgScriptCommandEdit dialog
typedef struct {
	bool maxWin; 
	CString lineCmd;
	ScriptCommandT nCmdType;
 } CommandEditInfo;
 
class CDlgScriptCommandEdit : public DialogType
{
	DECLARE_DYNAMIC(CDlgScriptCommandEdit)

public:
	CDlgScriptCommandEdit(LPRECT pRect = NULL);   // standard constructor
	virtual ~CDlgScriptCommandEdit();

	void set_edit_text(LPCTSTR strcmd) { m_edit.SetWindowText(strcmd); }

// Dialog Data
 
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnMainButtonClicked(WPARAM,LPARAM); 
	afx_msg void OnButtonClicked(UINT id);

private:
	CpopEdit  m_edit;
	CFont     m_font;
	CButtonST m_button[2];
	std::vector<CPngRadio*> radio_list; 

	CRect     m_rcDlgPos;
	bool      m_bMaxWin;

	void calc_rect ( CRect& rcClient, CRect& rcEdit, CRect& rcBtLeft, CRect& rcBtRight );
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
