#pragma once
#include "UIDialogs/DialogType.h"
#include "YsibotsException.h"
#include "GridCtrl_src/TreeGridCtrl.h"
#include "NewCellTypes/GridURLCell.h"
#include "NewCellTypes/GridCellCombo.h"
#include "NewCellTypes/GridCellCheck.h"
#include "NewCellTypes/GridCellNumeric.h"
#include "NewCellTypes/GridCellDateTime.h"
#include "NewCellTypes/GridCellComposite.h"
#include <vector>
#include <map>
#include "Keyboard/PopEdit.h"
#include "LuaScript/VariableReadWriter.h"
#include "LuaScript/LuaScript.h"

#define IDC_InstructionEDIT_EDIT 4720
#define IDC_InstructionEDIT_Grid 4721
 

class InstructionEditDLG : public DialogType
{

public:
	InstructionEditDLG( HBITMAP hBmpScrollBar = NULL, 
		HBITMAP hBmpPlus  = NULL, HBITMAP hBmpMinus = NULL );  
	virtual ~InstructionEditDLG();
	
	void insert_script_command ( ScriptCommandT, bool bNewCreate = false );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual int OnActivateChildWin(LPARAM);


	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnMainButtonClicked(WPARAM,LPARAM); 
	afx_msg LRESULT OnResizeGrid(WPARAM,LPARAM);
	 
	 
	afx_msg void OnTreeBeginEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTreeEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnClickTree(NMHDR* pNMHDR, LRESULT* pResult); 
	afx_msg LRESULT OnKeyboardEnter(WPARAM,LPARAM =NULL); 
	 
private:
	CImageList image_list_;
	 
 	CTreeGridCtrl m_Grid;
	CCellID       m_CellEdit;

	GTV_HTREENODE hItemCommand;
	std::vector<GTV_HTREENODE> item_para_list;
	VariableKey m_vkeyMsg;
	CpopEdit instructionEdit_;
	CFont font_;
	HBITMAP	m_hBmpScrollBar;
	HBITMAP	m_hBmpPlus;				 
	HBITMAP	m_hBmpMinus;
 
	CStringArray varNameArray[svTypeNum];
	VariableKey vkey_before_edit;
	bool isCreatCommand;

	CRect m_rectAll;  // 所有区域 
	CRect m_rectEdit; // 编辑区域
	bool  m_bShowAllRegion;

	BOOL create_tree_grid() throw (InvalidConfigurationException);
	BOOL initialize_tree_grid();
	void update_edit ();

	void update_var_name_array ();
	void insert_varprop_item(GTV_HTREENODE,ScriptVarT,ScriptVarSetT svm = SVAR_PROGRAM);
	void update_tree(GTV_HTREENODE, ScriptVarT sv, void* valueptr);
	void update_check_box(GTV_HTREENODE,BOOL);
	
	UINT DivStr(CString str,CStringArray& Arr,CString strDiv);

	void update_grid_command ( ScriptCommandT sct );
	 
	VariableKey   getVarKeyFromRow ( int nRow, bool bNeedName = true );
	VariableKey   getVarKeyCurr  ( bool bNeedName = true );
	VariableKey   getVarKeyFromItem ( GTV_HTREENODE, bool bNeedName = true );
	void clear_all ();

	void update_parameter ( GTV_HTREENODE hItem, ScriptVarT vtyp, LPCTSTR strVarName, ScriptVarSetT vset );

	void set_show_region ( bool bShowAll );
	void parse_var_key(const VariableKey&);

	int last_winID;
	int m_iGridHeight;

};

extern InstructionEditDLG* instructionEdit_DLG;