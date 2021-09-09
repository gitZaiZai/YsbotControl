// Copyright 2016, ��˼��Ϣ�Ƽ��������ι�˾
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - ��˼�������˶�����ϵͳ

#pragma once

#include "EDITGUI.h"
#include "Time.h"
#include <string>
#include <vector>
#include <map>
#include <list>
#include "SkinScrollBar/skinscrollbar.h"
#include "SkinScrollBar/SkinComboBox.h"
#include "SkinScrollBar/skinscrollwnd.h" 
#include "colourcode.h"

#define STR_SCINTILLAWND _T("Scintilla")

class ProgramEditor : public GUI::ScintillaWindow {
	DECLARE_DYNAMIC(ProgramEditor)
public:
	ProgramEditor(HBITMAP hBmpScrollBar = NULL);
	virtual ~ProgramEditor();
// 	BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect,
// 		CWnd* pParentWnd, UINT nID);
// 
	CSkinScrollWnd * m_ScrollBarWnd;

  	BOOL Create( CWnd* pParentWnd, const RECT& rect, UINT nID );
	// -----------------  ��������  -----------------
	void InitEditor(void);                      // ��ʼ��
	void SetAStyle(int style, COLORREF fore, COLORREF back = kColor[1],
		int size = 0, const char* face = NULL); // ���÷��
	void SetLexer(CString type);                // ���ôʷ�
	void UpdateLineNumberWidth(void);           // ��ʾ�к�
	void ShowMark(int line_number);             // ��ʾ���
	void DeleteMark(int line_number, bool= false);           // ɾ�����
	int  GetRows(void);
	void HighlightLine(int line);
	// -----------------  �ļ�����  -----------------
	void OpenFile(std::string);        // ���ļ�
	std::string GetAllText(void);
	int GetRange(int start, int end, char *text);
	int GetCurrentLineNumber(); 
	int GetCurrentCol ();
	std::string GetLineText(int line=-1);
	CString GetLineTextW(int line=-1);
	void ReplaceLineText(int line,const char* text);
	void ReplaceLineTextW(int line,CString text);
	void InsertBeforeLineText(int line,const char* text);
	void InsertBeforeLineTextW(int line,CString text);
	void InsertAfterLineText(int line,const char* text);
	void InsertAfterLineTextW(int line,CString text);
	void DeleteLineText(int line);

	void DeleteCurrLine ();

	int	PrepareContext();
	int	LoadFile(const char*);
	int	SaveFile(const char*);

	//���е��Բ���
	int	RunScript();
	int StepRunScript();
	int ContinueScript();
	int WatchLocalVar();
	int StopDebug();
    
	void set_continuous_run(void);
	void set_step_run(void);

	void  FreeContext(bool= false);
 	void  ToggleBreakpoint();
	void  set_luascript_keyword ();

	void  set_execute_start ();

	static volatile bool step_running_flag;     // �Ƿ񵥲�ִ��
	static volatile bool continue_running_flag; // �Ƿ񵥲�ִ��
	static volatile bool is_running_flag;       // ���Եĳ�����������
	static volatile bool meet_breakpoint;

	static std::map<std::string,std::map<int,bool> > breakpoints; // �ϵ��¼
	static bool HitBreakPoint (std::string filename,int lineno);
	static void InsertBreakPoint (std::string filename,int lineno);

	static void WaitCommand ();
	static int  line_hook_call (int lineno, bool externalModule = false, const char* srcText = 0 );

	static volatile bool execute_next_statement;
	static std::string file_name;
	long iStartLineNo;

	Ysbot::Time   secTime;
 
	int sci_edit_call(unsigned int msg, uptr_t wParam=0, sptr_t lParam=0, bool threadcall = false );
 
private:
	bool        program_is_ready;
	std::string program_string;
	HBITMAP	         m_hBmpScrollBar;
	CWnd *      pCWnd;

	void GetLineText(char* text, int sizeText, int line );
	int  LoadFile2(const char* FileName);

	void get_execute_text (int startline);
	std::vector<std::string> program_text_list;
};
