// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "stdafx.h"
#include "program_dialog.h"
#include "WorldModel.h"
#include "UIDialogs/state_dialog.h"
#include "InstructionBuilderDLG.h"
#include "InstructionEditDLG.h"
#include "LuaScript/ScriptFunc.h"
#include "stringconvert.h"
#include "LuaScript/ScriptVariables.h"
#include "UIDialogs/InfoBox.h"
 
#include "MotionControl.h"
#include <regex>

#define SAVE_LINE_NUMBER_TO_DATABASE     (1)

#define VAL_NAME_PROGRAM_LINE            ("ProgramLineNo")

ProgramDialog *program_dialog = nullptr;

ProgramDialog::ProgramDialog(std::string homepath, HBITMAP hBmpScrollBar) : m_homepath(homepath), editor_(hBmpScrollBar) 
{
	clipBoard=_T("");
	parse_result = new RegParseResult(false);
	parse_result->nFromID = 2;
	m_iLineNumberExecute = 0;
	m_bRunProgramClicked = false;
}

ProgramDialog::~ProgramDialog() 
{
	if (m_pwndDlgEditCommand->GetSafeHwnd()) 
	{
		m_pwndDlgEditCommand->DestroyWindow();
	}

	if ( m_pwndDlgEditCommand ) 
	{
		delete m_pwndDlgEditCommand;
		m_pwndDlgEditCommand = NULL;
	}

	if ( parse_result )
	{
		parse_result->clear();
		delete parse_result;
	}
}

BEGIN_MESSAGE_MAP(ProgramDialog, DialogType)
END_MESSAGE_MAP()

void ProgramDialog::DoDataExchange(CDataExchange* pDX) {

}

BOOL ProgramDialog::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	if ( GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetSafeHwnd() )
	{
		GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetWindowRect(&clientRect);
		SetWindowPos(NULL, clientRect.left, clientRect.top, clientRect.Width(), clientRect.Height(), SWP_NOZORDER);
	}

	SetBackgroundColor(kColor[8], TRUE);
	// 	SetBackgroundColor(RGB(0,0,0), TRUE);
	// 	ModifyStyleEx(WS_EX_STATICEDGE, WS_EX_CLIENTEDGE, SWP_DRAWFRAME);

	// 文件名称
	COLORREF m_crBackgroud = RGB(220, 220, 220);
	int nTopPosition = 3;
	file_name_.Create(_T("NewProgram"), WS_CHILD | WS_VISIBLE | SS_LEFT,
		CRect(35, nTopPosition, 400, 20), this);
	file_name_.SetBkColor(m_crBackgroud);
	// 	file_name_.Initialize(kColor[0]);
	// 	file_name_.SetAlignment(1);
	step_text_.Create(_T("CONT"), WS_CHILD | WS_VISIBLE | SS_LEFT,
		CRect(410, nTopPosition,460, 20), this);


	step_text_.SetBkColor(m_crBackgroud); 
	step_.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_LEFT,
		CRect(464, nTopPosition, 600, 20), this);
	step_.SetBkColor(m_crBackgroud); 
	// 编辑框
	// 	0, 10,
	// 		730, 423,
	int infoBarHeight = 10;
	m_rcPrgEdit = CRect(CPoint(clientRect.left,infoBarHeight),CSize(clientRect.Width(),clientRect.Height()-infoBarHeight));
	if ( editor_.Create( this, m_rcPrgEdit, IDC_PROGRAM_EDIT) ) 
	{
		editor_.InitEditor();
		editor_.UpdateLineNumberWidth();
		editor_.ShowMark(3);
		//editor_.LoadFile("UserPrograms\\NewProgram13.prg");
		editor_.Show();

	}

	int iWinEditHeight = 90;
	m_rcCmdEdit = CRect( CPoint(clientRect.left,clientRect.bottom-iWinEditHeight), CSize(clientRect.Width(),iWinEditHeight) );
	m_pwndDlgEditCommand = new CDlgScriptCommandEdit(&m_rcCmdEdit);
	m_pwndDlgEditCommand->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE,this);

	// 	BOOL ret = editor_.Create(STR_SCINTILLAWND, WS_CHILD | WS_VISIBLE,
	// 		CRect(5, 40, 704, 444), this, IDC_PROGRAM_EDIT);
	// 	editor_.InitEditor();
	// 	editor_.UpdateLineNumberWidth();
	// 	editor_.ShowMark(10);
	// 	editor_.LoadFile("UserPrograms\\4.lua");
	// 	editor_.SetFocus();
	load_linenumber_from_database(m_iLineNumberExecute);
	ShowExecuteLineNO(m_iLineNumberExecute);
 
	return FALSE;
}

void ProgramDialog::ShowExecuteLineNO( int cursorRow, int arrowRow, long nCol ) 
{
	CString str;
	if ( ProgramEditor::step_running_flag )
	{
		str.Format(_T("行 %d (%d)"), arrowRow, cursorRow );
	}
	else
	{
		str.Format(_T("行 %d   列 %d"), cursorRow+1, nCol );
	}

	CString textCurr;
	step_.GetWindowText(textCurr);
	if ( textCurr.Compare(str) != 0 )
	{
		step_.SetText(str);
	}
}

int ProgramDialog::save_linenumber_to_database(int nLine)
{
	//////////////////////////////////////////////////////////////////////////
	// save line number to database.

	if ( nLine != m_iLineNumberExecute )
	{
// 		sql::Record strValRecord(SQL.getFieldSet(TBLValInt));
// 		strValRecord.setString(1, "ProgramLineNo" );
// 		int nkey = SQL.find_primary_key(TBLValInt,&strValRecord);
// 		int nRet = -1;
// 		if ( nkey < 0 )
// 		{
// 			strValRecord.setInteger(2, nLine );
// 			strValRecord.setString(3, "lastProgramLineNumber" );
// 			strValRecord.setInteger(4, 1 );
// 			nRet = SQL.insert_record(TBLValInt,&strValRecord);
// 		}
// 		else
// 		{
// 			sql::Record* record = SQL.getTable(TBLValInt)->getRecordByKeyId(nkey);
// 			record->setInteger(2,nLine);
// 			nRet = SQL.modify_record(TBLValInt,record);
// 		}

		m_iLineNumberExecute = nLine;
		SQL.set( VAL_NAME_PROGRAM_LINE, nLine );
  	}

	return 0;
}

int ProgramDialog::load_linenumber_from_database(int& nLine)
{
//   	sql::Record strValRecord(SQL.getFieldSet(TBLValInt));
// 	strValRecord.setString(1, "ProgramLineNo" );
// 	int nkey = SQL.find_primary_key(TBLValInt,&strValRecord);
// 	int nRet = -1;
// 	if ( nkey < 0 )
// 	{
// 		strValRecord.setInteger(2, 0 );
// 		strValRecord.setString(3, "lastProgramLineNumber" );
// 		strValRecord.setInteger(4, 1 );
// 		nRet = SQL.insert_record(TBLValInt,&strValRecord);
// 	}
// 	else
// 	{
// 		sql::Record* record = SQL.getTable(TBLValInt)->getRecordByKeyId(nkey);
// 		if ( record )
// 		{
// 			sql::integer iNoLine = record->getValue(SQL.getFieldSet(TBLValInt)->getByIndex(2)->getName())->asInteger();
// 			nLine = (int)iNoLine;
// 			nRet = 0;
// 		}
//   	}

	SQL.get(VAL_NAME_PROGRAM_LINE,nLine);

 	return 0;
}

void ProgramDialog::SetPrgName(CString prgName) 
{
	file_name_.SetText(prgName);
}

void ProgramDialog::on_run_script()
{
	if ( MWM.get_robot_state().servo_on )
	{
		ScriptFunc::init_script_function(); // ?????????

		editor_.set_continuous_run();
		CString pn;
		file_name_.GetWindowText(pn);
 
		CString msg;
		msg.Format(_T("执行程序%s"),pn);
		MSD.ShowInfo(msg,JTMESSAGE);

		step_text_.SetText(_T("CONT"));
	}
	else
	{
 	    MSD.ShowInfo(_T(" 设备未打开伺服使能！"),JTWARNING);
 	}
}

void ProgramDialog::on_step_run_script()
{
	if ( MWM.get_robot_state().servo_on )
	{
		editor_.set_step_run();
		CString pn;
		file_name_.GetWindowText(pn);

		CString msg;
		msg.Format(_T("调试运行程序%s"),pn);
		MSD.ShowInfo(msg,JTMESSAGE);

 		step_text_.SetText(_T("STEP"));

	}
	else
	{
 		MSD.ShowInfo(_T("设备未打开伺服使能！"),JTWARNING);
 	}
}

void ProgramDialog::update_program_window()
{
	const RobotState& robotstate = MWM.get_robot_state();
	unsigned long long mcindex = robotstate.mc_info.nCommandIndex;
	ProgCommandInfo g_pci = MWM.get_prog_command_info();
	//    	cout << "--" << mcindex << "--" << g_pci.nCurCmd << ' ' <<  g_pci.nCmdCount << '\n';
	unsigned int  line_no =0;
	unsigned long long cmd_idx = 0;

	if ( m_bRunProgramClicked )
	{
		m_bRunProgramClicked = false;
		if ( MWM.get_robot_state().operate == omTEACH && MWM.get_robot_state().enable_press == false )
		{  
			MSD.ShowInfo(_T("示教模式需要按下使能开关"));
			return;
		}

		if ( MWM.get_robot_state().program_run == prSTEP )
		{
			program_dialog->on_step_run_script();
		}
		else 
		{
			program_dialog->on_run_script();
		}
	}

	if ( !ProgramEditor::is_running_flag )
 		return;

	if ( SMC.isCodeSysMotion )
	{
		for( int i = 0; i < MAX_PROG_COMMAND; ++i )
		{
 			line_no = g_pci.index_line_list[i].line_no;
			cmd_idx = g_pci.index_line_list[i].command_index;
 			if( cmd_idx == mcindex )
			{
				if ( editor_.step_running_flag && robotstate.mc_info.iMotionStatus == MotionCoreInfo::msSTOP )
				{
					break;
				}

				if ( ProgramEditor::continue_running_flag )
				{
					ShowExecuteLineNO(line_no);
 				}
 				editor_.HighlightLine(line_no);
 				break;
			}
		}

	}
	else
	{
		for(int i = 0; i < MAX_PROG_COMMAND; ++i)
		{
 			line_no = LOWORD(g_pci.dwCmdList[i]);
			cmd_idx = HIWORD(g_pci.dwCmdList[i]);
 			if( cmd_idx == mcindex )
			{
				if ( editor_.step_running_flag && robotstate.mc_info.iMotionStatus == MotionCoreInfo::msSTOP )
				{
					break;
				}

				if ( ProgramEditor::continue_running_flag )
				{
					ShowExecuteLineNO(line_no);

#if SAVE_LINE_NUMBER_TO_DATABASE
					save_linenumber_to_database(line_no);
#endif
				}

				editor_.HighlightLine(line_no);
// 				g_pci.nCurCmd = i;
// 				MWM.set_prog_command_info(g_pci);
				break;
			}
		}
	}



	// 	process_trigger(mcindex);
}

// void ProgramDialog::process_trigger(int current_index)
// {
// 	std::vector<Trigger>& trigger_list   = MWM.get_trigger_list();
// 	 	
// 	std::vector<Trigger>::iterator it    = trigger_list.begin();
// 	std::vector<Trigger>::iterator itend = trigger_list.end();
// 
// 	PosInfo rpos  = SMC.get_position(PosInfo::machineCoordinate,LDScommad);
// 	CartPos cpos;
// 	double curr_dist = 0;
// 	double dist = 0;
// 	bool   val  = false;
//  	while( it < itend )
// 	{
// 		if ( current_index == (*it).command_index )
// 		{
// 			val  = (*it).dout.output_value;
// 			dist = (*it).distance;
// 			
// 		    if ( (*it).distance_type == FROMBEGIN )
// 		    {
//  				cpos = rpos.cart - ((*it).path_start.cart);
// 			    curr_dist = cpos.pos.length();
// 				if ( curr_dist > dist )
// 				{
// 					(*it).dout.set( val );
// 
// 					it    = trigger_list.erase(it);
// 					itend = trigger_list.end();
// 				}
// 				else
// 					it++;
// 				cout << " trigger curr_dist S = " << curr_dist << '\n';
// 		    }
// 			else
// 			{
//  				cpos = rpos.cart - ((*it).path_end.cart);
// 				curr_dist = cpos.pos.length();
// 				if ( curr_dist < dist )
// 				{
// 					(*it).dout.set( val );
// 
// 					it = trigger_list.erase(it);
// 					itend = trigger_list.end();
// 				}
// 				else
// 					it++;
// 				cout << " trigger curr_dist E = " << curr_dist << '\n';
// 			}	
//  		}
// 		else
// 			it++;
// 	}
//  
// }

void ProgramDialog::on_free_script(bool threadcall)
{
	if ( MWM.get_robot_state().iGroupStatus == GrpHold )
	{
 		MSD.ShowInfo( _T("先退出程序暂停状态！"), JTWARNING );
		return;
	}

// 	if ( MWM.get_robot_state().iGroupStatus == GrpWait )
// 	{
// 		
// 		cmd.iCommandType = CMD_WAITIO;
// 		MWM.update_robot_state(cmd);
// 	}

	editor_.FreeContext(threadcall);
 
	if ( SMC.is_main_motion_control_available() )
	{
		SMC.setCmdInfo(CMD_ABORT); // or hold
	}

	if ( MWM.is_main_world_model_available() )
	{
		MWM.set_prog_command_info(ProgCommandInfo());
		SMC.clear_trigger();
	}


	step_text_.SetText(_T("STOP"));

}

void ProgramDialog::stop_script()
{
	editor_.FreeContext();
}

BOOL ProgramDialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class
	NMHDR* lpnmhdr = (LPNMHDR) lParam;

	if ( lpnmhdr->hwndFrom == static_cast<HWND>(editor_.GetID()) )
	{
		SCNotification *pSCNotification = (SCNotification*)lParam;
		if ( pSCNotification->nmhdr.code == SCN_SET_SCROLLBAR )
		{
			if ( pSCNotification->wParam != 0 )
			{
				LPSCROLLINFO lpsi = (LPSCROLLINFO)(pSCNotification->wParam);
				// 				SCROLLINFO sci;
				// 				memcpy(&sci,lpsi,sizeof(sci));
				// 				int nBar = (int)( pSCNotification->ch );
				// 				editor_.m_ScrollBarWnd->SetSkinScrollInfo( nBar, lpsi,(BOOL)(pSCNotification->updated), FALSE);

				if (pSCNotification->ch == SB_VERT )
				{
					editor_.m_ScrollBarWnd->m_sbVert.SetScrollInfo(lpsi,(BOOL)(pSCNotification->updated));
				}
				else
				{
					editor_.m_ScrollBarWnd->m_sbHorz.SetScrollInfo(lpsi,(BOOL)(pSCNotification->updated));
				}

				// 				printf("ScrollInfo %d mask=%x min=%d max=%d page=%d pos=%d track=%d\n",-1, sci.fMask,
				// 					sci.nMin, sci.nMax, sci.nPage, sci.nPos, sci.nTrackPos);
			}
			return TRUE;
		}
		else if ( pSCNotification->nmhdr.code == SCN_GET_SCROLLBAR  )
		{
			if ( pSCNotification->wParam != 0 )
			{
				LPSCROLLINFO lpsi = (LPSCROLLINFO)(pSCNotification->wParam);
				if (pSCNotification->ch == SB_VERT )
				{
					editor_.m_ScrollBarWnd->GetSkinScrollInfo(SB_VERT,lpsi,lpsi->fMask);
				}
				else
				{
					editor_.m_ScrollBarWnd->GetSkinScrollInfo(SB_HORZ,lpsi,lpsi->fMask);
				}

			}
		}
		else
		{
			switch(pSCNotification->nmhdr.code)
			{
			case SCN_CHARADDED:
				cout << " SC edit add char succees!\n"; break;

			case SCN_MARGINCLICK : cout << " SC edit margin click succees! " << '\n'; break;
			case SCN_DOUBLECLICK : cout << " SC edit double click succees! " << '\n';break; 
			case  SCN_LBUTTONUP : 
				m_SelectRange.cpMin = pSCNotification->x;
				m_SelectRange.cpMax = pSCNotification->y;
				if ( !ProgramEditor::step_running_flag && !ProgramEditor::continue_running_flag )
				{
					ShowExecuteLineNO( editor_.GetCurrentLineNumber(), 0, editor_.GetCurrentCol() );
				}
				break; 
			}
			return TRUE;
		}
 
	}

	return CDialogEx::OnNotify(wParam, lParam, pResult);
}

LRESULT ProgramDialog::OnMainButtonClicked( WPARAM wp, LPARAM lp)
{
	unsigned int btnid = (unsigned int)(wp);
	unsigned int menuid = (unsigned int)(lp);
	CString wstr;
	string str;
	int no	= editor_.GetCurrentLineNumber();
	int tlength  = editor_.sci_edit_call(SCI_LINELENGTH);
	int selStart = editor_.sci_edit_call(SCI_POSITIONFROMLINE, no);
	editor_.sci_edit_call(SCI_SETANCHOR,selStart);
	editor_.sci_edit_call(SCI_SETCURRENTPOS,selStart);
	int selEnd   = tlength + selStart;

	if ( m_SelectRange.cpMin != m_SelectRange.cpMax )
	{
		editor_.sci_edit_call(SCI_SETSEL,m_SelectRange.cpMin,m_SelectRange.cpMax);
 	}
   
	if ( MWM.get_userdata().nGrade < 2 )
	{
		MSD.ShowInfo(_T("没有编辑程序文件的权限，请以管理员身份登录系统"),_T("提示"),this,5);
		return 0;
	}

	switch (btnid) 
	{
	case 0:
		{
			std::string lineText = editor_.GetLineText();
			//trim(lineText);
			if ( lineText.empty() )
			{
				MSD.ShowInfo(_T("请选中需要编辑的指令！"));
				break;
			}
			else
			{
				int ret = LSCINFO->parse_command( lineText.c_str(), parse_result );
				parse_result->nRow = editor_.GetCurrentLineNumber();

				if ( ret )
				{
					GetParent()->SendMessage( WM_ACTIVATE_WINDOW, winInstructionEdit, (LPARAM)parse_result );
				}
				else
				{
					ceInfo.maxWin = false;
					ceInfo.lineCmd = CString( Ansi2Unicode(lineText).c_str() );
					GetParent()->SendMessage(WM_ACTIVATE_WINDOW,winEditCommand,(LPARAM)&ceInfo);
				}
			}
		}
		break;
	case 1:	
// 		get_select_text();
		if ( ProgramEditor::is_running_flag )
		{
 			if ( MWM.get_robot_state().iGroupStatus == GrpHold )
				SMC.setCmdInfo(CMD_CONTINUE);
 
			std::cout << "程序窗口—— 继续 按钮" << std::endl;
		}
		else
		{
 			switch (menuid)
			{
			case 0:  editor_.set_execute_start();			break;
			case 1:  		
				printf("continue run\n");
				MWM.set_programrun_mode(prAUTO);
				m_bRunProgramClicked = true;
				break;
			case 2: 		
				printf("step run\n"); 
				MWM.set_programrun_mode(prSTEP);
 				m_bRunProgramClicked = true;
				break;
			default:break;
			}
		}

 		break;
	case 2:  
		if ( ProgramEditor::is_running_flag )
		{
			m_bRunProgramClicked = true;
		}
		else
			GetParent()->SendMessage(WM_ACTIVATE_WINDOW,winInstructionDLG,0);		
		break;
	case 3:
		{
			switch (menuid)
			{
			case 0:  // new empty line
				editor_.sci_edit_call(SCI_NEWLINE);
				break;
			case 1:  //删除
				editor_.DeleteCurrLine();
				break;
			case 2:  //删除整行
				editor_.DeleteLineText(editor_.GetCurrentLineNumber());
				break;
			case 3:  //粘贴
				// 			if(clipBoard==_T("")) break;
				// 			editor_.InsertBeforeLineTextW(editor_.GetCurrentLineNumber(),clipBoard);
				// 
				editor_.sci_edit_call(SCI_PASTE); 
				break;
			case 4:  //复制
				//clipBoard = editor_.GetLineTextW();
				editor_.sci_edit_call(SCI_LINECOPY); 
				break;
			case 5:  //剪切
				// 			clipBoard = editor_.GetLineTextW();
				// 			editor_.DeleteLineText(editor_.GetCurrentLineNumber());
				editor_.sci_edit_call(SCI_LINECUT); 
				break;		
			default:break;
			}
		}

		break;
	case 4:  
		if ( ProgramEditor::is_running_flag )
		{
 			if ( MWM.get_robot_state().iGroupStatus != GrpHold )
 				SMC.setCmdInfo(CMD_HALT);

			std::cout << "程序窗口—— 暂停 按钮" << std::endl;
		}
 		else
		{
			GetParent()->SendMessage(WM_SAVE_PROJECT,0,0);
			std::cout << "程序窗口——保存 按钮" << std::endl;
 		}
		break;
	case 5:
		{
			if ( ProgramEditor::is_running_flag )
			{
				m_bRunProgramClicked = false;
 				on_free_script();
 				std::cout << "程序窗口—— 终止 按钮" << std::endl;
			} 
			else
			{
				switch (menuid)
				{
				case 0:	
					{
						std::string lineText = editor_.GetLineText();
 						FKEY->activate_keyboard(GetSafeHwnd(),Ansi2Unicode(lineText).c_str(),_T("程序"));
					}
					break;
				case 1:  editor_.sci_edit_call(SCI_TAB);     break;
				case 2:  editor_.sci_edit_call(SCI_BACKTAB); break;					
				case 3: AfxMessageBox(_T("查找"));		break;
				case 4:	append_comment(true); break;
				case 5:	append_comment(false); break;
				default:break;
				}
			}
		}
 		break;
	default:break;

	}
	if ( m_SelectRange.cpMin != m_SelectRange.cpMax )
	{
		editor_.sci_edit_call(SCI_SETEMPTYSELECTION,editor_.sci_edit_call(SCI_GETCURRENTPOS));
		m_SelectRange.cpMin = m_SelectRange.cpMax = 0;
	}
 // 	editor_.sci_edit_call(SCI_SETEMPTYSELECTION,editor_.sci_edit_call(SCI_GETCURRENTPOS));
	return 1;
}

LRESULT ProgramDialog::OnKeyboardEnter(WPARAM wp,LPARAM lp)
{
	//CString str_result = *((CString *)wp);	
	//CreatPRGFile(str_result);
	CString str_result = *((CString *)wp);
	//InsertAfterLineText(0,str_result);
	str_result += _T("\r\n");
	ReplaceLineText(0,str_result);
	return 1;
}

void ProgramDialog::show_edit_win( BOOL bShow /* = TRUE */,  LPARAM lParam )
{
	if ( !m_pwndDlgEditCommand )
		return;

	GUI::Rectangle rcpos;
	rcpos.left   = m_rcPrgEdit.left;
	rcpos.top    = m_rcPrgEdit.top;
	rcpos.right  = m_rcPrgEdit.right;
	rcpos.bottom = m_rcPrgEdit.bottom;

	if ( bShow )
	{
		if ( lParam )
		{
			CommandEditInfo cmdEditInfo = *(CommandEditInfo*)lParam;

			if ( cmdEditInfo.maxWin )
			{
				rcpos.bottom = m_rcPrgEdit.bottom - m_rcCmdEdit.Height()-2;
				m_pwndDlgEditCommand->MoveWindow(&m_rcCmdEdit);
			}
			else
			{
				int nHeightEditWin = m_rcCmdEdit.Height()/2;
				rcpos.bottom = m_rcPrgEdit.bottom - nHeightEditWin - 8;
				CRect rcNewPos = m_rcCmdEdit;
				rcNewPos.top = m_rcCmdEdit.top + nHeightEditWin - 8;
				m_pwndDlgEditCommand->MoveWindow(&rcNewPos);
			}

			m_pwndDlgEditCommand->set_edit_text(cmdEditInfo.lineCmd);
			m_pwndDlgEditCommand->ShowWindow(SW_SHOW);

			editor_.sci_edit_call(SCI_SETHSCROLLBAR, FALSE, 0); 
		}

	}
	else
	{
		editor_.sci_edit_call(SCI_SETHSCROLLBAR, TRUE, 0); 
		m_pwndDlgEditCommand->ShowWindow(SW_HIDE);
	}

	editor_.SetPosition(rcpos);

 
	// 	editor_.InvalidateAll();
	// 	editor_.m_ScrollBarWnd->Invalidate();
}

void ProgramDialog::load_script(const char* fn )
{
	editor_.LoadFile(fn); 
	file_name_.SetText(CString(fn));
}

// editType [ 0 : replace, 1 : before add, 2: after add, 3 :del ]
void ProgramDialog::modify_command_from_wizards( LPCTSTR text, int nEditType, bool fromSimpleEdit,  bool newCreate, bool bParenthesisSelect )
{
	int nCurrRow = parse_result->nRow;
	if ( newCreate )
	{
		nCurrRow = editor_.GetCurrentLineNumber();
	}
	if ( nCurrRow < 0 )
	{
		return;
	}
	string strLocal = "local";
	string newparam = "";
	size_t NoteFound;
	std::string lineText = editor_.GetLineText(nCurrRow);
	bool isEmptyLine = false;
	char pbuf[512] = {'\0'};
	if ( lineText.empty() )
		isEmptyLine = true;

	if ( fromSimpleEdit || newCreate )
	{
		CString newText = CString(text);
		if ( newCreate )
		{
			if ( newText.Find(_T("repeat")) != -1 )
			{
				CString addStr = _T("cnt = 0 \r\n");
				addStr = addStr + newText;
				addStr.Replace(_T("( ... )"),_T("( cnt > 5  )"));
				if ( bParenthesisSelect )
				{
					get_select_text(pbuf,true);
					CString strBrace = _T("\r\n");
					strBrace = strBrace + CString(pbuf) + strBrace + _T("cnt = cnt + 1 ")+ strBrace;
					addStr.Replace(_T("..."),strBrace);
				}
				newText = addStr;
			}
			else
			{

				newText.Replace(_T("( ... )"),_T("(  )"));
				if ( bParenthesisSelect )
				{
					get_select_text(pbuf,true);
					CString strBrace = _T("\r\n");
					strBrace = strBrace + CString(pbuf) + strBrace;
					newText.Replace(_T("..."),strBrace);
				}
				else
					newText.Replace(_T("..."),_T("\r\n"));	
			}
		}
 
		lineText = Unicode2Ansi(newText.GetBuffer(0));
	}
	else
	{
		string strLineCmd = Unicode2Ansi( CString(text).GetBuffer(0) );
		RegParseResult new_result(false);
		int ret = LSCINFO->parse_command( strLineCmd.c_str(), &new_result );
		if ( ret != 1 || new_result.sct != parse_result->sct )
		{
			return;
		}
		string newNote = "";
		if ( new_result.nCmdType == 0 ) // local ap1 = APOS() -- define var
		{
			// 增删local
			NoteFound = strLineCmd.find(strLocal.c_str());
			bool addLocal = false;
			bool delLocal = false;
			if ( NoteFound != string::npos ) // new have local
			{ 
				NoteFound = lineText.find(strLocal.c_str());
				if ( NoteFound == string::npos )
					addLocal = true; // add local
			}
			else // new no local
			{
				NoteFound = lineText.find(strLocal.c_str());
				if ( NoteFound != string::npos )
					delLocal = true;
			}	
			if ( addLocal )
				newparam = strLocal + " " + strLineCmd.substr(new_result.nParamStart, new_result.nParamEnd-new_result.nParamStart); // npos, len
			else
				newparam = strLineCmd.substr(new_result.nParamStart, new_result.nParamEnd-new_result.nParamStart); // npos, len

			lineText = lineText.erase( parse_result->nParamStart, parse_result->nParamEnd - parse_result->nParamStart );
			lineText = lineText.insert( parse_result->nParamStart, newparam.c_str(), newparam.length() );

			NoteFound = strLineCmd.find_last_of("--"); // 新注释
			if ( NoteFound != string::npos)
				newNote = strLineCmd.substr(NoteFound-1,strLineCmd.length()-NoteFound+1);

			if ( !newNote.empty() )
			{
				NoteFound = lineText.find_last_of("--");
				if ( NoteFound != string::npos)
				{
					lineText.replace(NoteFound-1,lineText.length()-NoteFound+1,newNote.c_str());
				}
				else
					lineText += newNote;
			}

			if( delLocal )
			{
				NoteFound = lineText.find(strLocal.c_str());
				if ( NoteFound != string::npos )
					lineText = lineText.erase( NoteFound, strLocal.length() );
			}
 		}
		else // PTP(ap1) or din1:Set(true)
		{
			newNote  = strLineCmd.substr(new_result.nParamEnd+1); // 可能新添加注释
			string newparam = strLineCmd.substr(new_result.nParamStart, new_result.nParamEnd-new_result.nParamStart); // npos, len
			string newcmd   = strLineCmd.substr(new_result.nCmdStart,   new_result.nCmdEnd-new_result.nCmdStart);

			lineText = lineText.erase( parse_result->nParamStart, parse_result->nParamEnd - parse_result->nParamStart );
			lineText = lineText.insert( parse_result->nParamStart, newparam.c_str(), newparam.length() );

			if ( parse_result->nCmdType == 2 ) // [ dout1:Set ]
			{
				lineText = lineText.erase( parse_result->nCmdStart, parse_result->nCmdEnd - parse_result->nCmdStart );
				lineText = lineText.insert( parse_result->nCmdStart, newcmd.c_str(), newcmd.length() );
			}

			if ( !newNote.empty() )
			{
				NoteFound = lineText.find_last_of("--");
				if ( NoteFound != string::npos)
				{
					lineText.replace(parse_result->nParamEnd+1,lineText.length()-parse_result->nParamEnd,newNote.c_str());
				}
				else
					lineText += newNote;
			}

		}

	}

	switch (nEditType)
	{
	case 0:
 		editor_.ReplaceLineText( nCurrRow, lineText.c_str() ); 
		break; 
	case 1:
		editor_.sci_edit_call(SCI_NEWLINE);
		editor_.InsertBeforeLineText( nCurrRow, lineText.c_str() ); 
		break;
	case 2: 
		//editor_.sci_edit_call(SCI_NEWLINE);
		nCurrRow = editor_.GetCurrentLineNumber();
  		
		if ( !isEmptyLine )
		{
			editor_.InsertAfterLineText( nCurrRow, "\r\n" ); 
			editor_.InsertAfterLineText( nCurrRow+1, lineText.c_str() ); 
			editor_.HighlightLine(nCurrRow+1);
		}
		else
		{
			if ( lineText.find_last_of("\r\n") == string::npos )
			{
			  	lineText += "\r\n";
			}
			editor_.ReplaceLineText( nCurrRow, lineText.c_str() );  
		}
		//editor_.sci_edit_call(SCI_LINETRANSPOSE);
		break;
	case 3: editor_.DeleteLineText(nCurrRow); break;
	default: break;
	}


	cout <<" new cmd = " << lineText << endl;
}

void ProgramDialog::get_select_text(char* text, bool delText)
{
	if ( !text || m_SelectRange.cpMin == m_SelectRange.cpMax )
 		return;

  	printf(" sel [ %d %d] \n",m_SelectRange.cpMin, m_SelectRange.cpMax ); 
	int text_length = editor_.GetRange(m_SelectRange.cpMin, m_SelectRange.cpMax,text);
	cout << text << endl;
	string spaceChar("");
	if ( delText )
	{
		editor_.sci_edit_call( SCI_DELETERANGE, m_SelectRange.cpMin, m_SelectRange.cpMax-m_SelectRange.cpMin );
   	}

	m_SelectRange.cpMin = m_SelectRange.cpMax = 0;
}

void ProgramDialog::append_comment( bool isAppend )
{
	int lineNo =editor_.GetCurrentLineNumber();
	 
	string noteStr = "--";
	string strCmd = editor_.GetLineText(lineNo);
	string regString = "^\\s*(--)(.*)$";
	regex regExp(regString);
	smatch msPart;  
	bool bfindNote = false;

	if( regex_match( strCmd, msPart, regExp ) ) 
	{
		 bfindNote = true; 
	}
 
	if ( isAppend )
	{
		if( !bfindNote ) 
		{
 			regString = "^\\s*(.*)$";
			regExp = regex(regString);
			if( regex_match( strCmd, msPart, regExp ) )
			{
				int npos = msPart[1].first-strCmd.begin();
				strCmd.insert(npos,noteStr.c_str());
			}
			editor_.ReplaceLineText( lineNo, strCmd.c_str() );
		}
	}
	else // cancle comments
	{
		if ( bfindNote )
		{
 			strCmd.erase(msPart[1].first,msPart[1].second);
			editor_.ReplaceLineText( lineNo, strCmd.c_str() );
		}
	}

}