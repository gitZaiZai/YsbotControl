
#pragma once
#include "UIDialogs/DialogType.h"
 

#include "SkinScrollBar/ListCtrlEx.h"
#include "GridCtrl_src/TreeGridCtrl.h"
#include "LuaScript/ScriptVariableType.h"
#include "LuaScript/LuaScript.h"
#include "Keyboard/PopEdit.h"
 
#include "IO Dialog/StaticTransparent.h"
#include "BasicControls/Label.h"
#include "UIDialogs/DlgScriptCommandEdit.h"

#define IDD_InstructionBuilder 4700
 
#define IDC_Instruction_Kinds	4702
#define IDC_Instruction_Type	4703
 

class InstructionBuilderDLG :public DialogType
{
public:
	InstructionBuilderDLG(HBITMAP hBmpScrollBar = NULL);
	virtual ~InstructionBuilderDLG();
  

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnMainButtonClicked(WPARAM,LPARAM); 

 	afx_msg void OnGridEndSelChange(NMHDR* pNMHDR, LRESULT* pResult);
  	afx_msg void OnRightGridEndSelChange(NMHDR* pNMHDR, LRESULT* pResult);
 
private:
	CImageList image_list_;
	CGridCtrl* grid_left;
	CGridCtrl* grid_right;
	 
	CFont font_;
	HBITMAP	         m_hBmpScrollBar;

	int     empty_icon;
	int     selected_icon;
	int     m_last_row;

	CLabel desc_label;
 	CLabel title_grid[2];
    CommandEditInfo cmd_edit_info;

	CGridCtrl* creat_grid( CRect rect, unsigned int gridID );

	void update_window(int nRow);
	void update_right_grid( ScriptCommandSetT svc  );
	void update_edit(int leftRow, int rightRow);
	void update_variable_description(int nRow);

 	void init_window ();


public:
	afx_msg void OnPaint();
};
extern InstructionBuilderDLG *instruction_DLG;