#include "stdafx.h"
#include "bottom_dialog.h"
#include "../DigitalInput.h"
#include "../DigitalOutput.h"
#include "../stringconvert.h"
#include "../variable_manager.h"
#include "../WorldModel.h"
#include "../resource.h"

BottomDialog::BottomDialog(PopupMenu * pMenuDlg):popup_menu_(pMenuDlg)
{
	win_info = new ChildWindowButtonMenu();

}

BottomDialog::~BottomDialog() 
{
	delete win_info;
// 	popup_menu_->DestroyWindow();
// 	delete popup_menu_;
	font_.DeleteObject();
}

BEGIN_MESSAGE_MAP(BottomDialog, DialogType)
 	ON_COMMAND_RANGE( BOTTOM_DIALOG_BUTTON1, BOTTOM_DIALOG_RETURN, OnButtonClicked )
END_MESSAGE_MAP()

void BottomDialog::DoDataExchange(CDataExchange* pDX) {

}
 
BOOL BottomDialog::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	//
	SetBackgroundColor(kColor[8]);
 	CRect clientRect;
	if ( GetParent()->GetDlgItem(IDC_BOX_BOTTONS)->GetSafeHwnd() )
	{
		GetParent()->GetDlgItem(IDC_BOX_BOTTONS)->GetClientRect(&clientRect);
		SetWindowPos(NULL, clientRect.left, clientRect.top, clientRect.Width(), clientRect.Height(), SWP_NOZORDER);
	}

	font_.CreateFont(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("宋体"));

	int width = 120;
	int space = 4;
	int x0 = clientRect.left;
	int y0 = clientRect.top;
	int x1 = x0 + width;
	int y1 = 59;
	for (unsigned i = 0; i != 6; i++)
	{
		button_[i].Create(_T(""), WS_CHILD | WS_VISIBLE,
			CRect(x0, y0, x1, y1), this, BOTTOM_DIALOG_BUTTON1 + i);
		button_[i].SetFaceColor(kColor[8]);
		button_[i].SetFont(&font_);
		x0 = x1 + space;
		x1 = x0 + width;
	}
	return_.Create(_T(""), WS_CHILD | WS_VISIBLE, CRect(x0, y0, x0 + 49, y1), this,
		BOTTOM_DIALOG_RETURN);
	return_.SetFaceColor(kColor[8]);
	return_.SetFont(return_.GetMyFont());
	return_.SetIcon(  AfxGetApp()->LoadIcon(IDI_UNDO)  );
	//HBITMAP hbitmap=(HBITMAP)::LoadImage(::AfxGetInstanceHandle(),_T("res//undo.bmp"),IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	//return_.SetBitmap(hbitmap);
	SetCaptionScheme(1,NULL);

// 	popup_menu_ = new PopupMenu();
// 	popup_menu_->CreateDialogFromTemplate(this, FALSE, WS_POPUP | SS_OWNERDRAW);
 
	return FALSE;
}

// void BottomDialog::Clicked() {
// 	for(unsigned i = 0; i != 6; i++) {
// 		if (button_[i].GetState() != 0) {
// 			button_[i].SetTextColor(kColor[1]);
// 			button_[i].SetFaceColor(kColor[11]);
// 			Go(i+1);    // 调用相应按钮指向的功能
// 		} else {
// 			button_[i].SetFaceColor(kColor[8]);
// 			button_[i].SetTextColor(kColor[0]);
// 		}
// 	}
// 	RedrawWindow();
// }

void BottomDialog::test_something()
{
	// 	PosInfo pidest;
	// 	pidest.set_axis_deg(-10,45,90,0,45,0);
	// 	ScriptVariables::getInstance()->creat_variable(svAXISPOS,"ap1",(&pidest)); 
// 	DigitalInput din;
// 	din.port = 100;
// 	din.input_value = 1;
// 	din.wait_time = 2345;
// 	ScriptVariables::getInstance()->creat_variable(svDIGITALINPUT,"din1",(&din)); 
// 
// 	DigitalOutput dout;
// 	dout.port = 500;
// 	dout.output_value = 1;
// 	dout.pulse_time = 6000;
// 	ScriptVariables::getInstance()->creat_variable(svDIGITALOUTPUT,"dout1",(&dout)); 

}

// 执行指定序号按钮的功能，一般是由Clicked()调用

// 设置按钮文字
void BottomDialog::SetCaption(int id1, CString caption) {
	id1--;
	if (id1 >=0 && id1 <= 6) {
		button_[id1].SetWindowText(caption);
	}	
}

void BottomDialog::SetCaptionScheme(int id1, HWND hwnd) 
{
    if ( id1 < 0 )
 		return;

	CString strTmp = _T("");

	for (unsigned i = 0; i != NUM_CHILD_BUTTON; i++) 
	{
		// 让不使用的按钮失效
		strTmp = CString(win_info->get_button_caption((ChilidWindowKind)id1,i));
		if (strTmp == _T("")) 
		{
			button_[i].EnableWindow(FALSE);
		}
		else 
		{
			button_[i].EnableWindow(TRUE);
		}
		// 让按钮颜色回到默认值
		button_[i].SetFaceColor(kColor[8]);
		button_[i].SetTextColor(kColor[0]);
		button_[i].SetWindowText(strTmp);
	}
	if ( hwnd != NULL )
	{
		set_response_window(hwnd);
	}
	m_curr_win = (ChilidWindowKind)id1;
}

void BottomDialog::update_button_text( int iWin, int iButton, CString strText )
{
	win_info->update_button_text((ChilidWindowKind)iWin,iButton,strText);
	if ( strText == _T("") ) 
	{
		button_[iButton].EnableWindow(FALSE);
	}
	else
	{
		if ( !button_[iButton].IsWindowEnabled() )
			button_[iButton].EnableWindow(TRUE);
 	}

	button_[iButton].SetWindowText(strText);
}

// 返回按钮
void BottomDialog::Return(void) 
{
// 	CYsbotControlDlg* pMainDlg = (CYsbotControlDlg*)AfxGetMainWnd();
// 	ProgramDialog* pEditProgram = pMainDlg->left_dialog_->get_program_window();
// 
// 	pEditProgram->save_script("UserPrograms/4.lua");
// 
// 	ScriptVariables::getInstance()->write_var_at("UserPrograms/test.var");
//  	PostQuitMessage(0); // sometime can't call ondestroy function.
}

void BottomDialog::OnButtonClicked(UINT id)
{
	if ( MWM.is_popup_window_active() )
		return;

	unsigned int bnt_index = id - BOTTOM_DIALOG_BUTTON1;
	if ( bnt_index < NUM_CHILD_BUTTON )
	{
		int menu_cnt = win_info->GetMenuCount( m_curr_win, bnt_index );
		if ( menu_cnt > 0 )
		{
			CRect posrc(0,0,0,0);
			GetDlgItem(id)->GetWindowRect(&posrc);
			popup_menu_->set_rect_button(&posrc);
			popup_menu_->active_menu( response_win, bnt_index, menu_cnt, win_info->get_menu_text(m_curr_win,bnt_index) );
 		}
		else
		    ::SendMessage( response_win, WM_BUTTON_PRESS, bnt_index, -1 );
	}
 	else
	{
		GetParent()->SendMessage( WM_ACTIVATE_WINDOW, winLastWindow ); // [return] bottom
	}

// 	if ( bnt_index < NUM_BUTTON )
// 	{
//  		::SendMessage( response_win, WM_BUTTOM_PRESS, bnt_index, 0 );
//  	}
// 	else // Return 
// 	{
// 		CYsbotControlDlg* pMainDlg = (CYsbotControlDlg*)AfxGetMainWnd();
// 		ProgramDialog* pEditProgram = pMainDlg->left_dialog_->get_program_window();
// 
// 		pEditProgram->save_script("UserPrograms/4.lua");
// 		ScriptVariables::getInstance()->write_var_at("UserPrograms/test.var");
// 		//  	PostQuitMessage(0); // sometime can't call ondestroy function.
// 		close_program = true;
// 	}

}

// Notice: this fuction is called cycly in main timer.
void BottomDialog::reset_button_cation( bool bRunning )
{
	static int status = 0;

	if ( status == 0 )
	{
		if ( bRunning )
		{
			status = 1;
			update_window(true); // show button text when run program.
		}
	}
	else if ( status == 1 )
	{
		if ( !bRunning )
		{
			status = 0;
			update_window(false);//show button text when normal status.
		}
	}
	
	if ( bRunning && m_curr_win == winProgram )
	{
		if ( MWM.get_robot_state().iGroupStatus == GrpHold )
		{
			button_[5].EnableWindow(FALSE);
			button_[4].EnableWindow(FALSE);
			button_[1].EnableWindow(TRUE);
		}
		else
		{
			button_[5].EnableWindow(TRUE);
			button_[4].EnableWindow(TRUE);
			button_[1].EnableWindow(FALSE);
 		}
	}
}

void BottomDialog::update_window( bool b )
{
	CString strTmp;
	win_info->reset_button_table(b);
	SetCaptionScheme( m_curr_win, NULL );
}