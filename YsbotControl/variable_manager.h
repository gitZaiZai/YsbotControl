// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#pragma once

#include "CmdInfo.h"
#include "DigitalInput.h"
#include "DigitalOutput.h"
#include "UIDialogs/DialogType.h"
#include "LuaScript/ScriptVariableType.h"
#include "LuaScript/VariableReadWriter.h"
#include "GridCtrl_src/TreeGridCtrl.h"
#include "NewCellTypes/GridURLCell.h"
#include "NewCellTypes/GridCellCombo.h"
#include "NewCellTypes/GridCellCheck.h"
#include "NewCellTypes/GridCellNumeric.h"
#include "NewCellTypes/GridCellDateTime.h"
#include "NewCellTypes/GridCellComposite.h"
#include "BasicControls/labels.h"
 
#include "DlgNewVariable.h"
#include "LuaScript/ScriptVariables.h"
#include "BasicControls/PngRadio.h"
#include "BasicControls/IconComboBox.h"

// 变量管理窗口类
class VariableDialog : public DialogType 
{
	 enum{
		 VAR_NONE  = -1,
		 VAR_COPY  = 0,
		 VAR_CUT   = 1,
		 VAR_PASTE = 2
	 };

public:
	VariableDialog( HBITMAP hBmpScrollBar = NULL, 
		            HBITMAP hBmpPlus  = NULL, 
					HBITMAP hBmpMinus = NULL,
                    DlgNewVariable* pNewVar = NULL );

	virtual ~VariableDialog();
// 	void update_program_variable_grid();
	void update_variable_grid (ScriptVarSetT svm = SVAR_PROGRAM);
  	void teach_robot_position();
 	bool new_variable_ui( ScriptVarT vartype,const char* varname, ScriptVarSetT svm = SVAR_PROGRAM, bool fromEditWin = false );

private:
	// 插入变量，输入类型、名称、数值 
	// 类型暂时固定成三类，0是系统变量、1是全局变量、2是程序变量
	void insert_script_variable( const std::vector<ScriptVarT>& varTypefilter, ScriptVarSetT svm = SVAR_PROGRAM);
	void InsertVariable( ScriptVarT sv, const char* vname, void* ptrvalue, ScriptVarSetT svm = SVAR_PROGRAM );
	void insert_varprop_item(GTV_HTREENODE,ScriptVarT, ScriptVarSetT svm = SVAR_PROGRAM);
 	void parse_var_key(const VariableKey&);
	void update_tree(GTV_HTREENODE, ScriptVarT sv, void* valueptr);
	void update_check_box(GTV_HTREENODE,BOOL);
	void sort_variable(ScriptVarSetT svm = SVAR_PROGRAM );

	BOOL initialize_tree_grid();
	BOOL create_tree_grid() throw (InvalidConfigurationException);
 
 	void update_grid_data();

	void delete_variable_ui();
	void rename_variable_ui();
	bool copy_variable();
	bool cut_variable();
	bool paste_variable();
	void delete_variable (const VariableKey&);

	GTV_HTREENODE init_variable_grid (ScriptVarSetT svm = SVAR_PROGRAM);
 
	void          update_type_combobox (int radioType);
	void          reload_grid_var ();

	VariableKey   getVarKeyFromRow ( int nRow, bool bNeedName = false );
	VariableKey   getVarKeyCurr  ( bool bNeedName = false );
	VariableKey   getVarKeyFromItem ( GTV_HTREENODE, bool bNeedName = false );

	CTreeGridCtrl m_Grid;

	HBITMAP	m_hBmpScrollBar;
	HBITMAP	m_hBmpPlus;				// bitmap 
	HBITMAP	m_hBmpMinus;		
	DlgNewVariable* pWndNewVar;

	int     m_icon[SVAR_SET_NUM];
	// Tree related stuff
	GTV_HTREENODE m_hItemVar[SVAR_SET_NUM];

	CImageList image_list_;
	BasicLabel title1_;

	GTV_HTREENODE    src_edit_htree;
	 
	int        edit_step;

	std::vector<CPngRadio*> radio_list; 
	std::vector<ScriptVarT> var_filter; 
	int m_nRadioType;
	CIconComboBox type_combox;
 	VariableKey src_var_key;
	unsigned int m_columFitTypeGrid;
	bool m_bResizeGrid;
	CCellID       m_CellEdit;
	CRect         m_rcTextLabel;

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual int OnActivateChildWin(LPARAM);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnClickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnKeyboardEnter(WPARAM,LPARAM); 
	afx_msg LRESULT OnResizeGrid(WPARAM,LPARAM);
	afx_msg LRESULT OnMainButtonClicked(WPARAM,LPARAM); 
	afx_msg void OnTreeBeginEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTreeEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnRadioButtonClicked(UINT id);
	afx_msg void OnSelchangeCommbo(NMHDR *pNotifyStruct, LRESULT* pResult);

};

extern VariableDialog *variable_dialog;