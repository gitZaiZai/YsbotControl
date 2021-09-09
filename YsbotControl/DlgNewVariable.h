#pragma once

#include "UIDialogs/DialogType.h"
#include "SkinScrollBar/ListCtrlEx.h"
#include "GridCtrl_src/TreeGridCtrl.h"
#include "LuaScript/ScriptVariableType.h"
#include "LuaScript/ScriptCommandInfo.h"
#include "Keyboard/PopEdit.h"
 
#include "BasicControls/Label.h"
#include "BasicControls/IconComboBox.h"

class DlgNewVariable :	public DialogType
{
public:
	DlgNewVariable(HBITMAP hBmpScrollBar = NULL);
	virtual ~DlgNewVariable(void);
    void reset_var_index(int id);
	void init_window ();
	virtual int OnActivateChildWin(LPARAM);

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnMainButtonClicked(WPARAM,LPARAM); 

	afx_msg void OnGridEndSelChange(NMHDR* pNMHDR, LRESULT* pResult);
 	afx_msg void OnRightGridEndSelChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelChangeCombo();
private:
	CGridCtrl* creat_grid( CRect rect, unsigned int gridID );

	void update_window(int nRow);
	void update_right_grid( ScriptVarCategory svc  );
	void update_edit(int leftRow, int rightRow);
 
	int  extract_int(CString str);
	void init_box_grid ( CRect& rcPos);

private:
	CImageList image_list_;
	CGridCtrl* grid_left;
	CGridCtrl* grid_right;
	CpopEdit   m_edit;

	HBITMAP	         m_hBmpScrollBar;

	int     empty_icon;
	int     selected_icon;
 
	CLabel name_label,desc_label;
	CLabel name_scope,desc_scope;
	CLabel title_grid[2];

	int     m_last_row;
  
	CString       m_var_abbreviate;
	unsigned int  m_variable_index;
	CIconComboBox m_ComboVarSet;
	bool          m_bInitWindow;
	int           m_nWinCaller;

	RegParseResult*   m_vkeyResult;

	COLORREF      m_bkCol;
	COLORREF      m_HighLightColDisable;
	COLORREF      m_HighLightColEnable;

public:
	afx_msg void OnPaint();
};

