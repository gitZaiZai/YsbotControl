#pragma once

#include "UIDialogs/DialogType.h"
#include "YsibotsException.h"
#include "DigitalInput.h"
#include "DigitalOutput.h"
#include "LuaScript/ScriptVariableType.h"
#include "LuaScript/VariableReadWriter.h"
#include "GridCtrl_src/TreeGridCtrl.h"
#include "NewCellTypes/GridURLCell.h"
#include "NewCellTypes/GridCellCombo.h"
#include "NewCellTypes/GridCellCheck.h"
#include "NewCellTypes/GridCellNumeric.h"
#include "NewCellTypes/GridCellDateTime.h"
#include "NewCellTypes/GridCellComposite.h"
#include "GroupPropertiesReadWriter.h"
#include "Keyboard\PopEdit.h"
#include "IO Dialog\StaticTransparent.h"

#define IDC_VariableBuilder_EDIT 4820
#define IDC_VariableBuilder_Grid 4821
#define IDC_VariableBuilder_BNOK 4822
#define WM_VariableBuilder_NewName 4820

enum WriteCtlType
{
	WriteNULL   = 0,
	WriteOnTree ,
	WriteOnEdit ,
};

class VariableBuilderDLG : public DialogType
{

public:
	VariableBuilderDLG( HBITMAP hBmpScrollBar = NULL, 
		HBITMAP hBmpPlus  = NULL, HBITMAP hBmpMinus = NULL );  
	virtual ~VariableBuilderDLG();
	void SetVariable(ScriptVarT sv, std::string varName="", void* pValue=NULL ,CWnd* hostPtr=NULL,int nRow =0);
	void InsertVariable( const char* vartype, const char* vname, void* ptrvalue );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	BOOL CheckRule(std::string str,std::string rule);
	BOOL CheckRule(CString str,std::string rule);

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnMainButtonClicked(WPARAM,LPARAM); 
	afx_msg void OnClickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnKeyboardEnter(WPARAM,LPARAM); 
	afx_msg void OnTreeBeginEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTreeEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg LRESULT OnResizeGrid(WPARAM,LPARAM);
	afx_msg void OnClickEdit();
	afx_msg void OnClickBtn();
private:
	CImageList image_list_;
	int     joint_icon;
	int     motion_icon;
	HTREEITEM* parameter_type;
	VariableReadWriter* var_read_writer;
	CTreeGridCtrl m_Grid;
	CpopEdit variableEdit_;
	CButton btn_ok;
	CStaticTran label_;
	CFont font_;
	HBITMAP	m_hBmpScrollBar;
	HBITMAP	m_hBmpPlus;				 
	HBITMAP	m_hBmpMinus;
	int writeOnCtl;
	ScriptVarT curVarT;
	std::string curVarName;
	void* curValuePtr;
	CString variable_text;
	CWnd* host_;
	int varRow_;
	CCellID       m_CellEdit;

	std::vector<GTV_HTREENODE> hItem_list;

	BOOL create_tree_grid() throw (InvalidConfigurationException);
	BOOL initialize_tree_grid();
	void parse_item_string(int row);
	void insert_varprop_item(GTV_HTREENODE,ScriptVarT);
	void update_tree(GTV_HTREENODE, ScriptVarT sv, void* valueptr);
	void update_check_box(GTV_HTREENODE,BOOL);
};
//
extern VariableBuilderDLG* variableBuilder_DLG;