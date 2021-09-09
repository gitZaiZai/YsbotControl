// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#pragma once

#include "program_editor.h"
#include "UIDialogs/DialogType.h"
#include "BasicControls/Label.h"
#include "UIDialogs/DlgScriptCommandEdit.h"
#include "LuaScript/ScriptCommandInfo.h"
 

class ProgramDialog : public DialogType {
public:
	ProgramDialog(std::string homepath="", HBITMAP hBmpScrollBar = NULL);
	virtual ~ProgramDialog();

	ProgramEditor editor_;   // 编辑框

	void on_run_script();
	void on_step_run_script();

	void on_free_script(bool threadcall = false);
	void stop_script ();

	void save_script(const char* fn ) { editor_.SaveFile(fn); }
	void load_script(const char* fn ) ;

	void update_program_window(void);

	void ShowExecuteLineNO( int cursorRow, int arrowRow = 0, long nCol = 0 );   // 显示步数
	void SetPrgName(CString prgName);
	void process_trigger(int current_index);

	CString GetLineText(int line) { return editor_.GetLineTextW(line);};
	void ReplaceLineText(int line,CString text) { editor_.ReplaceLineTextW(editor_.GetCurrentLineNumber(),text);};
	void InsertBeforeLineText(int line,CString text){ editor_.InsertBeforeLineTextW(editor_.GetCurrentLineNumber(),text);};
	void InsertAfterLineText(int line,CString text){ editor_.InsertAfterLineTextW(editor_.GetCurrentLineNumber(),text);};
	void modify_command_from_wizards( LPCTSTR text, int nEditType = 0, 
		bool fromSimpleEdit = false, bool newCreate = false, bool bParenthesisSelect = false ); // editType [ 0 : replace, 1 : before add, 2: after add, 3 :del ]

	void DeleteLineText(int line){ editor_.DeleteLineText(line);};
	CWnd* getEditWin () { return (CWnd*)( m_pwndDlgEditCommand ); }

	void show_edit_win (BOOL bShow = TRUE, LPARAM = 0);
	
private:
 	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnMainButtonClicked(WPARAM,LPARAM); 
	afx_msg LRESULT OnKeyboardEnter(WPARAM,LPARAM); 
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	void get_select_text(char* text, bool delText = false);
	void append_comment(bool isAppend);
	int save_linenumber_to_database(int nLine);
	int load_linenumber_from_database(int& nLine);

private:
	CLabel file_name_;   // 文件名
	CLabel step_text_;   // step文字
	CLabel step_;        // 步数
	std::string m_homepath;
	CString currProjectName;
	CString clipBoard;
	CDlgScriptCommandEdit* m_pwndDlgEditCommand;
	CRect m_rcCmdEdit, m_rcPrgEdit;

	RegParseResult* parse_result;
	CommandEditInfo ceInfo;
	Sci_CharacterRange m_SelectRange;

	int m_iLineNumberExecute;

	bool m_bRunProgramClicked;

};

extern ProgramDialog *program_dialog;