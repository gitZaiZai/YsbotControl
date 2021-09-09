// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "stdafx.h"
#include "program_editor.h"
#include <Shlwapi.h>

#include <iostream>
#include <fstream>  
#include "stringconvert.h"
#include "LuaScript/BinaryData.h"
#include "LuaScript/LuaScript.h"
#include <queue>
#include "WorldModel.h"
#include "MotionControl.h"
#include "YsbotControl.h"
#include "YsbotControlDlg.h"
#include "LuaScript/ScriptFunc.h"

using namespace std;

volatile bool ProgramEditor::is_running_flag        = false;  
volatile bool ProgramEditor::step_running_flag      = false;
volatile bool ProgramEditor::continue_running_flag  = false;
volatile bool ProgramEditor::execute_next_statement = false;
volatile bool ProgramEditor::meet_breakpoint        = false;

string ProgramEditor::file_name = "";
std::map<std::string,std::map<int,bool> > ProgramEditor::breakpoints; //断点记录

IMPLEMENT_DYNAMIC(ProgramEditor, CWnd)

// Lua关键字字符串
const char kLuaKeyWords[] = "and break do else elseif end false for "
	"function if in local nil not or repeat return then true until while ";
// 二级关键字字符串
const char kLuaKeyWords1[] = "ANGLE AxisPos SetRad SetDeg CartPos SetPos Vec3D CartRef PosInfo SetAxis SetCart "
	" DIN Read Wait ReadBit WaitBit ReadNoWait ReadOnPosition "
	" DOUT Set Pulse SetBit SetNoWait"
	" PosInfo read Wait ReadBit WaitBit Set SetBit Pulse "
	" BeginTrack EndTrack DoneTrack WaitObj Coord";
  

ProgramEditor::ProgramEditor(HBITMAP hBmpScrollBar): program_is_ready(false),m_hBmpScrollBar(hBmpScrollBar) 
{
	iStartLineNo = -1;	
	secTime.update();
	m_ScrollBarWnd = NULL;
	pCWnd =NULL;
	program_text_list.clear();
	FreeContext(); 
}

ProgramEditor::~ProgramEditor() 
{
	program_text_list.clear();

	UnskinWndScroll(pCWnd);
	for (std::map<std::string,std::map<int,bool> >::iterator it = 
		ProgramEditor::breakpoints.begin(); 
		it != ProgramEditor::breakpoints.end(); 
	it++) 
	{

		for (std::map<int, bool>::iterator innerIt = 
			it->second.begin(); 
			innerIt != it->second.end(); 
		innerIt++) 
		{
			;       // delete ImageConverter object
		}
		it->second.clear();             // empty the inner map(s)
	}
	ProgramEditor::breakpoints.clear();             // empty the outer map


	if ( Created() )
	{
		Destroy();
	}

	FreeContext();

}

BOOL ProgramEditor::Create( CWnd* pParentWnd, const RECT& rect, UINT nID )
{
// 	CRect rect;
// 	pParentWnd->GetClientRect(&rect);
	int height = rect.bottom - rect.top;
	int width  = rect.right  - rect.left;
	HWND eidthwnd = ::CreateWindowEx(
		0,
		TEXT("Scintilla"),
		TEXT("Source"),
   		WS_CHILD | WS_VISIBLE| WS_VSCROLL| WS_HSCROLL,
		// 		WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		rect.left, rect.top,
		width, height,
		pParentWnd->GetSafeHwnd(),
		0,
		GetModuleHandle(NULL),
		0 );

	if (eidthwnd)
	{
		SetID( eidthwnd );
		unsigned long ttt = (unsigned long)(GetID());
		SetWindowLong( eidthwnd, GWL_ID, nID );

		if ( m_hBmpScrollBar )
		{
			pCWnd = CWnd::FromHandle(eidthwnd);
			m_ScrollBarWnd = SkinWndScroll( pCWnd, m_hBmpScrollBar, 10, SKIN_WINDOW_PROGRAM );
			m_ScrollBarWnd->SendMessage(UM_DESTMOVE);
 		}
		
		return TRUE;
	}
	return FALSE;
}

//创建
// BOOL ProgramEditor::Create(LPCTSTR lpszWindowName, DWORD dwStyle,
// 	const RECT&rect, CWnd* pParentWnd, UINT nID) 
// {
//  	return CWnd::CreateEx(WS_EX_CLIENTEDGE, STR_SCINTILLAWND, lpszWindowName,
//  		dwStyle, rect, pParentWnd,(UINT)nID );
// }

// 设置风格
void ProgramEditor::SetAStyle(int style, COLORREF fore, COLORREF back,
	int size, const char* face) {
	SciCall(SCI_STYLESETFORE, style, fore);
	SciCall(SCI_STYLESETBACK, style, back);
	if (size >= 1)
		SciCall(SCI_STYLESETSIZE, style, size);
	if (face) 
		SciCall(SCI_STYLESETFONT, style, reinterpret_cast<LPARAM>(face));
	SciCall(SCI_STYLESETBOLD, style, true); 
 
}

void ProgramEditor::InitEditor(void) {
	//936 (Simplified Chinese GBK)，支持中文显示
	SciCall(SCI_SETCODEPAGE,936);  
	//设置全局默认style，默认style只在没有明确的选择时会被应用
	SetAStyle(STYLE_DEFAULT, 0x000000, kColor[3], 18, "Arial");
	SciCall(SCI_STYLECLEARALL);    // 将全局默认style应用到所有
	SetLexer(_T("L_LUA"));       // 初始语法规则
	SciCall(SCI_AUTOCSETSEPARATOR, static_cast<WPARAM>(10), 0);
	// ------------ 光标所在行的风格 ---------------
	// 设置光标所在行显示色带
	SciCall(SCI_SETCARETLINEVISIBLE,TRUE,0);   
	
	// 设置让控件失去焦点时仍然显示色带
	SciCall(SCI_SETCARETLINEVISIBLEALWAYS, TRUE, 0);    
	SciCall(SCI_SETCARETLINEBACK, RGB(88, 148, 24), 0);     // 设置色带颜色
 
//    	SciCall(SCI_SETHSCROLLBAR, FALSE, 0);                 // 隐藏水平滚动条
	SciCall(SCI_SETCARETSTYLE, 0);                        // 隐藏光标
	SciCall(SCI_SETSELFORE, TRUE, RGB(0,0,0));             // 选择字符前景色
  	SciCall(SCI_SETSELBACK, TRUE, RGB(0, 154, 83));       // 选择字符背景色
	SciCall(SCI_SETCARETLINEBACKALPHA, SC_ALPHA_NOALPHA, 0);           // 当前行透明度
	SciCall(SCI_SETUNDOCOLLECTION,0,0);
	
// 	SciCall(SCI_SETMOUSEDOWNCAPTURES,1);
 

	HDC hdc;
	SIZE s;
 	TCHAR str[100];
	hdc = GetDC( pCWnd->GetSafeHwnd()); // 获得相关设备句柄
 	wsprintf(str, _T("dddddddddddddddddddddddddddddddddddddddddddddd"));
    GetTextExtentPoint32(hdc, str, lstrlen(str), &s); // 获取字符串的像素大小
	ReleaseDC(pCWnd->GetSafeHwnd(), hdc);
	SciCall(SCI_SETSCROLLWIDTH,s.cx);

}

void ProgramEditor::SetLexer(CString type)
{
 	// 设置Lua词法
	SciCall(SCI_SETLEXER, SCLEX_LUA);
	set_luascript_keyword();

	// 设置字节中样式位宽，默认5，最大7
	SciCall(SCI_SETSTYLEBITS, 7);    

	// Lua Styles. 
	SetAStyle(SCE_LUA_COMMENTLINE, RGB(0,128,0), kColor[3], 0, 0);  // 注释
	SciCall(SCI_STYLESETITALIC, SCE_LUA_COMMENTLINE, 1);     // 注释斜体
	SetAStyle(SCE_LUA_NUMBER, kColor[4], kColor[3], 0, 0);       // 数字
	SetAStyle(SCE_LUA_STRING, kColor[5], kColor[3], 0, 0);       // 字符串
	SetAStyle(SCE_LUA_CHARACTER, kColor[5], kColor[3], 0, 0);    // 字符
	SetAStyle(SCE_LUA_IDENTIFIER, kColor[6], kColor[3], 0, 0);   // 标识符
	SetAStyle(SCE_LUA_WORD, kColor[7], kColor[3], 0, 0);         // 关键字
	SetAStyle(SCE_LUA_WORD2, kColor[12], kColor[3], 0, 0);       // 二级关键字
// 	SetAStyle(SCE_LUA_WORD3, RGB(128, 0, 0), kColor[3], 0, 0);       // 三级关键字
	SetAStyle(SCE_LUA_OPERATOR, kColor[0], kColor[3], 18, 0);    // 操作符
}

void ProgramEditor::set_luascript_keyword()
{
	string strFuction = "";
	string fname = "";
	for ( int i = SM_PTPABS; i < SM_MAX_COMMANDS; ++i )
	{
		if ( LSCINFO->isLuaSystemFunction( ScriptCommandT(i) ) == false )
		{
			fname = string( LSCINFO->CommandParameters[i]->getClassName() );
			fname += " ";
			strFuction.append(fname); 
		}
	}
	strFuction.append(" ");
	strFuction.append(kLuaKeyWords1);

	// 设置关键字
	SciCall(SCI_SETKEYWORDS, 0, reinterpret_cast<LPARAM>(kLuaKeyWords)); 
	SciCall(SCI_SETKEYWORDS, 1, reinterpret_cast<LPARAM>(strFuction.c_str()));//kLuaKeyWords1 

}

// 显示行号
void ProgramEditor::UpdateLineNumberWidth(void) {
// 	Call(SCI_SETMARGINTYPEN, 0, SC_MARGIN_NUMBER); // SCI_SETMARGINTYPEN, 2, SC_MARGIN_SYMBOL SC_MARGIN_NUMBER
// 	SetAStyle(STYLE_LINENUMBER, kColor[0], kColor[8], 14, 0); // 设置行号风格
// 
// 	Call(SCI_SETMARGINWIDTHN, 1, 30);
// 	Call(SCI_MARKERDEFINE, 0, SC_MARK_ARROW);
// 	Call(SCI_MARKERSETFORE, 1, kColor[9]);
// 	Call(SCI_SETMARGINTYPEN, 1, SC_MARGIN_SYMBOL);
// 	Call(SCI_SETMARGINMASKN, 1, 0x01);

	SciCall(SCI_SETMARGINWIDTHN, 0, 0); // arrow label for debug
	SciCall(SCI_SETMARGINTYPEN, 0, SC_MARGIN_SYMBOL);
    SciCall(SCI_SETMARGINMASKN, 0, 0x01);
	SciCall(SCI_MARKERDEFINE, 0, SC_MARK_SHORTARROW);
	SciCall(SCI_MARKERSETFORE, 0, kColor[0]);
	SciCall(SCI_MARKERSETBACK, 0, kColor[9]);

 	SciCall(SCI_SETMARGINWIDTHN, 1, 33 );
	SciCall(SCI_SETMARGINTYPEN, 1, SC_MARGIN_NUMBER  ); // SCI_SETMARGINTYPEN, 2, SC_MARGIN_SYMBOL SC_MARGIN_NUMBER
	SciCall(SCI_SETMARGINSENSITIVEN,1,true);

	SetAStyle(STYLE_LINENUMBER, kColor[0], kColor[8], 14, 0); // 设置行号风格
	SciCall(SCI_STYLESETBOLD, STYLE_LINENUMBER, false);
	SciCall(SCI_STYLESETFONT, STYLE_LINENUMBER, reinterpret_cast<LPARAM>("Arial Narrow") );
	SciCall(SCI_STYLESETSIZE, STYLE_LINENUMBER, 14);

	SciCall(SCI_STYLESETFONT, STYLE_CONTROLCHAR, reinterpret_cast<LPARAM>("Arial Narrow") );
	SciCall(SCI_STYLESETSIZE, STYLE_CONTROLCHAR, 15);
	SciCall(SCI_STYLESETWEIGHT,STYLE_CONTROLCHAR,5);
}	


// 显示标记
void ProgramEditor::ShowMark(int line_number) {
	// 设置0号页边显示标记
	SciCall(SCI_MARKERADD, line_number - 1, 0);
}

// 删除标记
void ProgramEditor::DeleteMark(int line_number,bool threadcall) {
	SciCall(SCI_MARKERDELETE, line_number - 1, 0,threadcall);
}

void ProgramEditor::OpenFile(std::string file_name) {
	std::string file_content;
	if (FileToString(file_name, file_content) != 0) {
		std::cout << "文件打开失败！" << std::endl;
		return;
	}
	SciCall(SCI_CANCEL);
	SciCall(SCI_SETUNDOCOLLECTION,(WPARAM)FALSE);
	SciCall(SCI_CLEARALL, 0, 0);                   // 清除所有文字
	SciCall(SCI_MARKERDELETEALL,(WPARAM)-1,0);     // 删除所有标志
	SciCall(SCI_ADDTEXT, file_content.size(),
	reinterpret_cast<LPARAM>(static_cast<char *>((char *)file_content.c_str())));
}
 

void ProgramEditor::WaitCommand()
{
	static MSG msg;
	while( is_running_flag )
	{
		if ( execute_next_statement )
		{
			execute_next_statement = false;
			break;
		}

// 		if (::PeekMessage (&msg, NULL, 0, 0, PM_REMOVE)) 
// 		{ 
// 			if (msg.message == WM_QUIT) 
// 				break; 
// 			::TranslateMessage (&msg) ; 
// 			::DispatchMessage (&msg) ; 
// 		}
		if ( ScriptFunc::PeekAndPump() == FALSE ) // exit
		{
			is_running_flag   = false;
			step_running_flag = false;
			continue_running_flag = false;
			execute_next_statement = false;
 			break;
		}
 
		Sleep(1);
	}
}

// lua call this function before execute new line code.
int ProgramEditor::line_hook_call(int lineno, bool externalModule, const char* srcText )
{
	if ( is_running_flag == false )
	{
		return -1; // 交出控制权
	}

	std::string sourceText = string(srcText);
// 	if ( sourceText[0] == '@' )
// 	{
// 		sourceText.erase(0,1);
// 		program_dialog->load_script(sourceText.c_str());
// 	}
//  	if ( program_dialog )
// 	{
// 		char lineprogram[1024] = {'\0'};
// 		program_dialog->editor_.GetLineText(lineprogram,1024,lineno-1);
// 		trim_str(lineprogram);
// 		cout << " ***[ " << lineprogram << " ]***" << endl;
// 	}

	static int lastline = 1;
	meet_breakpoint = HitBreakPoint(file_name,lineno);
 	if( meet_breakpoint || step_running_flag )
	{
 		if ( !externalModule )
		{
			CYsbotControlDlg* pMainDlg = (CYsbotControlDlg*)theApp.m_pMainWnd;
			ProgramDialog* pEditProgram = pMainDlg->left_dialog_->get_program_window();
			pEditProgram->editor_.HighlightLine(lastline-1);
			pEditProgram->editor_.DeleteMark(lastline);
			pEditProgram->editor_.ShowMark(lineno);
			pEditProgram->ShowExecuteLineNO(lastline,lineno);

			lastline = lineno;
		}
 		
		WaitCommand( );
	}

	return 0;
}
 
void ProgramEditor::FreeContext(bool threadcall)
{
	if ( CanCall() )
	{
		if ( step_running_flag )//&& !threadcall 
		{
			DeleteMark(ScriptFunc::edit_line_no, threadcall);
 		}
		if ( SciCall( SCI_GETREADONLY, 0, 0, threadcall ) )
		{
			SciCall( SCI_SETREADONLY, false, 0, threadcall  );
		}

		if ( iStartLineNo != -1 )
		{
			DeleteMark(iStartLineNo+1);
			iStartLineNo = -1;
		}
	}

	program_is_ready  = false;
	is_running_flag   = false;
	step_running_flag = false;
	continue_running_flag = false;
	execute_next_statement = false;
}
  
bool ProgramEditor::HitBreakPoint(std::string filename,int lineno)
{    
	std::map<std::string,std::map<int,bool> >::iterator it = breakpoints.find(filename);
	if( it != breakpoints.end())
	{
		std::map<int,bool>::iterator it2 = it->second.find(lineno);
		if(it2 != it->second.end())
		{
			return it2->second;
		}
	}
	return false;
}

//添加一个断点，文件名加行号的方法
void ProgramEditor::InsertBreakPoint(std::string filename,int lineno)
{
	std::map<std::string,std::map<int,bool> >::iterator it = breakpoints.find(filename);
	if( it != breakpoints.end() ) // 已经存在该文件
	{
		std::map<int,bool>::iterator it2 = it->second.find(lineno);
		if( it2 != it->second.end() ) // 已经存在该行
		{
			it2->second = !(it2->second);
		}
		else
			it->second.insert(std::make_pair(lineno,true));
	}
	else
	{
		std::map<int,bool> tmp;
		tmp.insert(std::make_pair(lineno,true));
		breakpoints.insert(std::make_pair(filename,tmp));
	}
}

int ProgramEditor::LoadFile(const char* FileName)
{
	ifstream ReadFile;  
	int n = 0;  
	char line[256];  

	ReadFile.open(FileName,ios::in);
	CString script;
	while( !ReadFile.eof() )  
	{  
		//ReadFile.read(line,256);  
		ReadFile.getline(line,256);
		//注意,getline如果读不到换行符号,会设置fail,  
		//后果就是eof一直是false,getline也获取不到东西.  
		//所以下次读取要清除它.  
		if( ReadFile.fail() )  
		{  
			ReadFile.clear(ReadFile.rdstate() & ~(ifstream::failbit));  
		}  
		n++;  
		script += CString(line);
// 		script.Trim();
// 		script += "\r\n";
	}  
  
	cout << "gcount: "   << ReadFile.gcount() << endl;  
	//   	cout << "position: " << ReadFile.tellg()  << endl;  
	ReadFile.close();  

	file_name = string(FileName);

// 	SetWindowTextW(script);
	string file_content = Unicode2Ansi( script.GetBuffer(0) );
	SciCall(SCI_CANCEL);
	SciCall(SCI_SETUNDOCOLLECTION,(WPARAM)FALSE);
	SciCall(SCI_CLEARALL, 0, 0);                   // 清除所有文字
	SciCall(SCI_MARKERDELETEALL,(WPARAM)-1,0);     // 删除所有标志
	SciCall(SCI_ADDTEXT, file_content.size(),
	                         reinterpret_cast<LPARAM>(static_cast<char *>((char *)file_content.c_str())) );

	int textLen = file_content.length();

	SciCall(SCI_GRABFOCUS);
  
	return TRUE;
}

int ProgramEditor::LoadFile2(const char* FileName)
{
	ifstream ReadFile;  
	int n = 0;  
	char line[256];  

	ReadFile.open(FileName,ios::in);
	string script;
	while( !ReadFile.eof() )  
	{  
		//ReadFile.read(line,256);  
		ReadFile.getline(line,256);
		//注意,getline如果读不到换行符号,会设置fail,  
		//后果就是eof一直是false,getline也获取不到东西.  
		//所以下次读取要清除它.  
		if( ReadFile.fail() )  
		{  
			ReadFile.clear(ReadFile.rdstate() & ~(ifstream::failbit));  
		}  
		n++;  
		script += string(line) + '\n';
 	}  

	cout << "gcount: "   << ReadFile.gcount() << endl;  
	//   	cout << "position: " << ReadFile.tellg()  << endl;  
	ReadFile.close();  

	file_name = string(FileName);

	// 	SetWindowTextW(script);
	string file_content = script;
	SciCall(SCI_CANCEL);
	SciCall(SCI_SETUNDOCOLLECTION,(WPARAM)FALSE);
	SciCall(SCI_CLEARALL, 0, 0);                   // 清除所有文字
	SciCall(SCI_MARKERDELETEALL,(WPARAM)-1,0);     // 删除所有标志
	SciCall(SCI_ADDTEXT, file_content.size(),
		reinterpret_cast<LPARAM>(static_cast<char *>((char *)file_content.c_str())) );

	SciCall(SCI_GRABFOCUS);

	return TRUE;
}

int ProgramEditor::SaveFile(const char* fname)
{
	CString pro;

// 	GetWindowText( pro );
// 	pro.Trim();

// 	string strpro = Unicode2Ansi( pro.GetBuffer(0) );
// 	string strpro = GetAllText(); 
// 	
	std::ofstream out ( fname, ios::out );
// 	if (!out) 
// 	{
// 		cout << ("trying to write buffered data but could not open file for writing");
// 	}
// 
// 	out.write( strpro.c_str(), strpro.length() );
// 	out.close();

	// 	pro.ReleaseBuffer();
	int linecount = GetRows();
	char linetext[512];
	string strLine = "";
	string strRet = "\r\n";
	for ( int i = 0; i < GetRows(); ++i )
	{
		GetLineText(linetext,512,i);
		if ( i == GetRows() - 1)
		{
			strLine = string(linetext);
		}
		else
			strLine = string(linetext) + strRet;
 		out << strLine;
 	}
	out.close();
	return 0;
}

int ProgramEditor::RunScript()
{
 	SciCall( SCI_SETREADONLY, true );

    int ret = 0;
	if ( !program_is_ready )
	{
		program_is_ready = true;
 		program_string = GetAllText(); 
 	}

	if ( iStartLineNo != -1 )
	{
		DeleteMark(iStartLineNo+1);
 	}

	if ( program_string.empty() )
 		return ret;
 
	ret = LSCRIPT->doString(program_string.c_str());
	if ( ret != 0 )
	{
		FreeContext();
		cout << "程序终止\n"  << endl;
	}

// 	ret = LSCRIPT->load_string(program_string.c_str());
// 	if ( ret != 0 )
// 	{
// 		is_running_flag   = false;
// 		step_running_flag = false;
// 		cout << "程序终止\n"  << endl;
// 		return -11;
// 	}
// 
// 	ret = LSCRIPT->luaExecute(0,0);
// 	if ( ret != 0 )
// 	{
// 		LSCRIPT->print (lua_tostring (LSCRIPT->getState(), -1));
// 		is_running_flag   = false;
// 		step_running_flag = false;
// 		cout << "程序终止\n"  << endl;
// 		return -11;
// 	}

	return 0;
}

int ProgramEditor::StepRunScript()
{
	step_running_flag = true;

	if( is_running_flag )
	{
		// 		printf("程序已经在运行\n");
		execute_next_statement = true;
 		return 1;
	}

	RunScript();

	return 0;
}

int ProgramEditor::GetCurrentLineNumber() 
{
	return SciCall( SCI_LINEFROMPOSITION, 
		                SciCall(SCI_GETCURRENTPOS) );
}

int ProgramEditor::GetCurrentCol()
{
	return SciCall(SCI_GETCOLUMN,SciCall(SCI_GETCURRENTPOS));
}

void ProgramEditor::ToggleBreakpoint()
{
	if (!CanCall())
		return;

 	int lineno = GetCurrentLineNumber() ;    // 获取当前行的行号
	ShowMark(lineno+1);
	InsertBreakPoint(ProgramEditor::file_name,lineno+1);
}

int ProgramEditor::ContinueScript()
{
	if( is_running_flag )
	{
		ProgramEditor::step_running_flag      = false;
		ProgramEditor::execute_next_statement = true;
 	}
	return 0;
}

int ProgramEditor::WatchLocalVar()
{
	LSCRIPT->debug_local_var();
	return 0;
}

int ProgramEditor::StopDebug()
{
// 	ProgramEditor::is_running_flag   = false;
// 	ProgramEditor::step_running_flag = false;
	FreeContext();
	SMC.setCmdInfo(CMD_ABORT);

	return 0;
}

std::string ProgramEditor::GetAllText(void) {
	
	if (!CanCall())
		return "";

 	std::string str;
	int len = SciCall(SCI_GETTEXTLENGTH, 0, 0);
	char *c1 = new char[len + 1];
	SciCall(SCI_GETTEXT, len + 1, (LPARAM)c1);
	str = string(c1);
	delete[] c1;
	return str;
}

std::string ProgramEditor::GetLineText(int line)
{
	//std::string str;
	//int len = SciCall( SCI_LINELENGTH, line );
	//int linestart = SciCall(SCI_POSITIONFROMLINE,line);
	//int lineend   = SciCall(SCI_GETLINEENDPOSITION,line);
	//len=lineend-linestart;
	//char *c1 = new char[len];
	//SciCall(SCI_GETLINE, len + 1, (LPARAM)c1);
	//str = string(c1);
	//delete[] c1;
	int iline;
	if(line == -1) 
		iline=GetCurrentLineNumber();
	else
		iline=line;
	char chr[512];
	GetLineText(chr,512,iline);
	string str(chr);
	return str;
}

void ProgramEditor::set_execute_start()
{
	if ( CanCall() == false )
		return;

	int currLineNo = GetCurrentLineNumber();
	if ( iStartLineNo != -1 )
	{
		DeleteMark( iStartLineNo+1 );
	}
	ShowMark( currLineNo+1 );

	//get_execute_text( currLineNo );

	iStartLineNo = currLineNo;

	cout << " <<<<<<<<<<<<<<<< program is start No. " << GetCurrentLineNumber() << '\n';

}

void ProgramEditor::get_execute_text(int startline)
{
//  	int linestart = SciCall(SCI_POSITIONFROMLINE,startline);
// 	int lenAll = SciCall(SCI_GETTEXTLENGTH, 0, 0);
// 	char *text = new char[lenAll - linestart + 1];
// 	 
// 	int len = GetRange(linestart,-1,text);
// 	text[len] = '\0';
// 	string strTextPrg(text);
// 
// 	delete[] text;
// 
//  	return strTextPrg; 

//////////////////////////////////////////////////////////////////////////
	program_text_list.clear();
	int linecount = GetRows();
	char linetext[512] = {'\0'};
	string strLine = "";
	string strRet = "\r\n";
	for ( int i = 0; i < GetRows(); ++i )
	{
		if ( i < startline )
		{
			linetext[0] = '\0';
		}
		else
		   GetLineText(linetext,512,i);
		strLine = string(linetext) + strRet;
		program_text_list.push_back(strLine);
	}
}
 
CString ProgramEditor::GetLineTextW(int line)
{
	char chr[512];
	GetLineText(chr,512,line);
	CString wstr(chr);
	return wstr;
}

void ProgramEditor::ReplaceLineText(int line,const char* text)
{
	int linestart = SciCall(SCI_POSITIONFROMLINE,line);
	int lineend   = SciCall(SCI_GETLINEENDPOSITION,line);
	if (linestart<0) return;
	SciCall(SCI_DELETERANGE, linestart,lineend-linestart);
	SciCall(SCI_INSERTTEXT,linestart,reinterpret_cast<LPARAM>(text));
		
}

void ProgramEditor::ReplaceLineTextW(int line,CString text)
{	
	string str;
	str=Unicode2Ansi((std::wstring)text);
	const char* chr=str.c_str();
	ReplaceLineText(line,chr);
}

void ProgramEditor::InsertBeforeLineText(int line,const char* text)
{
	int linestart = SciCall(SCI_POSITIONFROMLINE,line);
	if (linestart<0) return;
	SciCall(SCI_INSERTTEXT,linestart,reinterpret_cast<LPARAM>(text));
}

void ProgramEditor::InsertBeforeLineTextW(int line,CString text)
{
	string str;
	str=Unicode2Ansi((std::wstring)text);
	str=str + "\n";
	const char* chr=str.c_str();
	InsertBeforeLineText(line,chr);
}

void ProgramEditor::InsertAfterLineText(int line,const char* text)
{
	int lineend = SciCall(SCI_GETLINEENDPOSITION,line);
	if (lineend<0) return;
	SciCall(SCI_INSERTTEXT,lineend,reinterpret_cast<LPARAM>(text));
}

void ProgramEditor::InsertAfterLineTextW(int line,CString text)
{
	string str;
	str=Unicode2Ansi((std::wstring)text);
	str= "\n"+str;
	const char* chr=str.c_str();
	InsertAfterLineText(line,chr);
}

void ProgramEditor::DeleteLineText(int line)
{
	int linestart = SciCall(SCI_POSITIONFROMLINE,line);
	int lineend   = SciCall(SCI_GETLINEENDPOSITION,line);
	if (linestart<0) return;
	SciCall(SCI_DELETERANGE, linestart,lineend-linestart);
}

void ProgramEditor::DeleteCurrLine()
{
	int line = GetCurrentLineNumber();
	SciCall(SCI_LINEDELETE,line);
}

void ProgramEditor::GetLineText(char* text, int sizeText, int line )
{
	if ( line < 0 )
	{
		line = GetCurrentLineNumber();
	}

	int linestart = SciCall(SCI_POSITIONFROMLINE,line);
	int lineend   = SciCall(SCI_GETLINEENDPOSITION,line);
	int linemax   = linestart + sizeText - 1;
	if ( lineend > linemax )
	{
		lineend = linemax;
	}
	GetRange(linestart,lineend,text);
	text[lineend - linestart] = '\0';
}

int ProgramEditor::GetRange(int start, int end, char *text)
{
    Sci_TextRange tr;
	tr.chrg.cpMin = start;
	tr.chrg.cpMax = end;
	tr.lpstrText  = text;
	int length = SciCall(SCI_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&tr));
    return length;
}

// 获取总行数
int ProgramEditor::GetRows(void)
{
	if (!CanCall())
		return 0;

	return SciCall(SCI_GETLINECOUNT);
}

void ProgramEditor::HighlightLine(int line)
{
	if (!CanCall())
		return;

	SciCall(SCI_GOTOLINE,line);
}

void ProgramEditor::set_continuous_run()
{
	if ( step_running_flag )
	{
 		MSD.ShowInfo(_T("改变程序运行模式，请先终止程序运行！"),JTWARNING);
 		return;
	}

	if ( !is_running_flag )
	{
		is_running_flag        = true;
		step_running_flag      = false;
		continue_running_flag  = true;
		execute_next_statement = false;

 		RunScript();

 		//FreeContext();
	}
	else
	{
		if ( meet_breakpoint )
		{
			execute_next_statement = true;
		}
		if ( MWM.get_robot_state().iGroupStatus == GrpHold )
		{
			SMC.setCmdInfo(CMD_CONTINUE);
		}
	}
 }

void ProgramEditor::set_step_run()
{
	if ( continue_running_flag )
	{
		MSD.ShowInfo(_T("改变程序运行模式，请先终止程序运行！"),JTWARNING);
		return;
	}

	if ( !is_running_flag )
	{
		is_running_flag        = true;
		step_running_flag      = true;
		continue_running_flag  = false;
		execute_next_statement = false;
 
		RunScript();
 		 
		//FreeContext();
	}
	else
	{
		// 		printf("程序已经在运行\n");
		if ( MWM.get_robot_state().iGroupStatus == GrpHold )
		{
			SMC.setCmdInfo(CMD_CONTINUE);
		}
		else
 		    execute_next_statement = true;
	}
}

int ProgramEditor::sci_edit_call(unsigned int msg, uptr_t wParam/* =0 */, sptr_t lParam/* =0 */, bool threadcall /* = false */ )
{
	return SciCall(msg,wParam,lParam,threadcall);
}