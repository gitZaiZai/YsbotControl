// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: letuspray@163.com (Lu Wei)
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统
//
// 数字键盘类实现文件

#include "stdafx.h"
#include "digital_keyboard.h"
#include <regex>      // 正则表达式
#include "xsleep.h"
#include <iostream>
#include <string>
#include <iostream>
#include <sstream>

BEGIN_MESSAGE_MAP(DigitalKeyboard, DialogType)
	ON_WM_CTLCOLOR()
	ON_COMMAND_RANGE(IDC_DIGITAL_KEY, IDC_DIGITAL_KEY + 15, KeyPress)
	ON_MESSAGE(WM_EDIT_MESSAGE, RuleChecking)

END_MESSAGE_MAP()

DigitalKeyboard::DigitalKeyboard() 
{
	font_.CreateFont(22, 0, 0, 0, FW_HEAVY, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS,
		DRAFT_QUALITY, FIXED_PITCH | FF_MODERN, _T("黑体"));
	original_value_ = _T("0");
	min_value_ = -100000;
	max_value_ = 100000;
	decimals_ = 0;
	// 初始化正则表达式规则，默认只能输入0，因此在使用本工具时，
	// 都应该进行数值范围与小数位（为0时自然判断为整数）的设置
	rule_ = "[0]";
	pointer_ = NULL;
	pointer_type_ = 0;
	m_hwndResponse = NULL;
	m_lParamData = NULL;
	create_rule(-1000000,1000000,3); // 默认是浮点数
}

DigitalKeyboard::~DigitalKeyboard() 
{
	;
}

void DigitalKeyboard::DoDataExchange(CDataExchange* pDX) {

}

// void DigitalKeyboard::OnCancel() {
//   DestroyWindow();
// }
// 
// void DigitalKeyboard::PostNcDestroy() {
//   CDialogEx::PostNcDestroy();
//   if (NULL != this) {
//     delete this;
//   }
// }

// 初始化
BOOL DigitalKeyboard::OnInitDialog() 
{
	DialogType::OnInitDialog();
	// 根据父窗口位置计算居中位置
	CRect rect;
	GetParent()->GetWindowRect(&rect);
	int x0 = rect.left + rect.Width() / 2 - 150;
	int y0 = rect.top +	rect.Height() / 3;
	int width0 = 300;
	int height0 = 300;
  SetWindowPos(NULL, x0, y0, width0, height0, SWP_HIDEWINDOW); //liurundan
  // 设置背景色
  // 在操作系统的不同风格设置下，对话框会跟着变，所以在这里设置一下以防跟着变
  SetBackgroundColor(RGB(239, 239, 239));
  title_.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_NOTIFY,
    CRect(0, 0, width0, 30), this, IDC_DIGITAL_KEYBOARD_TITLE);
  title_.SetBkBmp(_T("res\\keyboard_title.bmp"));
	title_.SetArea(rect);
	// 创建数值框
	value_.Create(WS_CHILD | WS_VISIBLE | ES_CENTER,
		CRect(4, 34, 290, 66), this, IDC_DIGITAL_KEYBOARD_VALUE);
	value_.ModifyStyleEx(0, WS_EX_DLGMODALFRAME | WS_EX_STATICEDGE,
		SWP_DRAWFRAME);
	value_.SetFont(&font_);
	// 创建数值范围框
	range_.Create(_T(""), WS_CHILD | WS_VISIBLE |	SS_CENTER | SS_OWNERDRAW,
		CRect(10, 73, 290, 90),	this, 4002);
	range_.SetMyFont();
	// 创建键盘按钮
	CreateButton();
	value_.SetFocus();
  return TRUE;
}

HBRUSH DigitalKeyboard::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	// 下面的代码让只读状态的编辑框背景色仍然为白色
	if (pWnd->GetDlgCtrlID() == IDC_DIGITAL_KEYBOARD_VALUE) {
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(0, 0, 0));
		HBRUSH back_hbr = CreateSolidBrush(RGB(255, 255, 255));
		return back_hbr;
	}
	return hbr;
}

void DigitalKeyboard::KeyPress(UINT number) {
	std::regex re(rule_);
	int id0 = number - IDC_DIGITAL_KEY;
	CString str;
	value_.GetWindowText(str);
	CString key;
	button_[id0].GetWindowText(key);
	// 根据输入的不同字符进行判断处理
	switch (button_[id0].data().type) {
	case 2:  // 输入负号，与其它字符不同，负号是加在最左边
		{
			if (str.Find(_T("-")) != -1)
				break;
			// 先把光标移到最左边，输入完成后再移到最右边
			value_.SetSel(0, 0, FALSE);
			::SendMessage(value_, WM_CHAR, '-', 0);
			value_.SetSel(str.GetLength() + 1, str.GetLength() + 2, FALSE);
		}
		break;
	case 11: // 退格
		::SendMessage(value_, WM_CHAR, VK_BACK, 0);
		break;
	case 12: // 取消，恢复原始数值，隐藏窗口
		value_.SetWindowText(_T(""));
		pointer_ = nullptr;
		if ( m_hwndResponse )
		{
			::SendMessage(m_hwndResponse, WM_KEYBOARD_CANCLE,0,0 );
		}
		ShowWindow(SW_HIDE);
		break;
	case 13: // 复位，恢复原始数值，不隐藏窗口
		value_.SetWindowText(_T(""));
		value_.SetFocus();
		break;
	case 14: // 确定，用消息把数值传递出去，隐藏窗口
		{
			CString str2;
			value_.GetWindowText(str2);
			std::string str1 = (CStringA)str2;
			std::regex re(rule_);
		if (regex_match(str1, re) == false || _ttof(str2) < min_value_ ||
			_ttof(str2) > max_value_) 
			break;  // 不符合规格即跳出
		str1 = (CStringA)str;
		if (pointer_type_ == 0) 
		{
			// 指针类型为0时以消息的形式返回
// 			GetParent()->SendMessage(DIGITAL_KEYBOARD_RESULT, 0, (LPARAM)(&str)); // liurundan
			if ( m_hwndResponse )
			{
				::SendMessage(m_hwndResponse,WM_KEYBOARD_ENTER, (WPARAM)(&str), m_lParamData );
			}
		}
		else if( pointer_ )
		{
  			switch (pointer_type_) 
			{
			case 1: {
				CEdit *edit0 = (CEdit *)pointer_;
				edit0->SetWindowText(str);
				pointer_ = nullptr;
							}
							break;
			case 2:
				break;
			default:
				;
			}
		}
	}
	close_keyboard();
	return;
	default:
		{
			wchar_t a1 = key[0];
			::SendMessage(value_, WM_CHAR, a1, 0);  // 直接向编辑框发送字符
		}
		break;
	}
	// 使用正则表达式检查是否符合规则
	value_.SetFocus();
	return;
}

void DigitalKeyboard::CreateButton() {
	int x0 = 4;
	int y0 = 48;
	int width = 70;
	int height = 47;
	for (int i = 0; i != 16; i++) {
		if (i % 4 == 0) {
			x0 = 4;
			y0 += height + 2;
		} else {
			x0 += width + 2;
		}
		button_[i].Create(kDigitalKeyboard[i].code[0], WS_CHILD | WS_VISIBLE,
			CRect(x0, y0, x0 + width, y0 + height),	this, IDC_DIGITAL_KEY + i);
		button_[i].set_data(kDigitalKeyboard[i]);
		button_[i].SetMyFont(20);
		// 设置按键的背景色，否则会受操作系统影响
		button_[i].SetFaceColor(RGB(239, 239, 239));
		// 设置显示图标
		if (kDigitalKeyboard[i].icon.GetLength() != 0) {
			button_[i].SetMyIcon(kDigitalKeyboard[i].icon);
		}
	}
	return;
}
 
// 设置输入数值范围，显示出来并设置相关正则表达式规则
void DigitalKeyboard::create_rule(double min0, double max0, short decimals)
{
	CString d1;
	d1.Format(_T("%d"), decimals);
	CString c1;
	c1.Format(_T("%-16." + d1 + "f"), min0);
	CString c2;
	c2.Format(_T("%16." + d1+ "f"), max0);
	c1 = c1 + _T("-") + c2;
	min_value_ = min0;
	max_value_ = max0;
	decimals_ = decimals;
	// 负号的处理
	std::string sign0 = "";
	if (min0 < 0) {
		sign0 = "[-]{";
		sign0 += max0 < 0 ? "1" : "0";
		sign0 += ",1}";
	}
	// 小数部分的处理
	std::string fraction = "";
	if (decimals >= 1) {
		std::stringstream s1;
		s1 << decimals;
		s1 >> fraction;
		rule_ = sign0 + "[0-9]*[\\.]{0,1}[0-9]{0," + fraction + "}";
	} else {
		rule_ = sign0 + "[0-9]*";
	}
	strRange = c1;
}

void DigitalKeyboard::SetValueRange( double min0, double max0, short decimals ) 
{
	create_rule(min0,max0,decimals);
	range_.SetWindowText(strRange);

	value_.set_rule(rule_);
}

void DigitalKeyboard::set_original_value( CString str, int type0, void *p0 ) 
{
	value_.SetWindowText(str);
	original_value_ = str;
	title_.set_text(str);
	pointer_ = p0;
	pointer_type_ = type0;
	ShowWindow(SW_SHOW);
	BringWindowToTop();
}

void DigitalKeyboard::activate_keyboard( HWND hwnd, LPCTSTR strvar, LPCTSTR strname, LPARAM pCell, DWORD sel )
{
	m_hwndResponse = hwnd;
	m_lParamData   = pCell;


	value_.SetWindowText(strvar);
	original_value_ = strvar;
	title_.set_text(strname);
 	pointer_type_ = 0;

	value_.SetSel(sel);
// 	value_.SetSel(-1<<16); // all selecte.
	value_.SetFocus();
	ShowWindow(SW_SHOW);
	BringWindowToTop();
}

void DigitalKeyboard::close_keyboard()
{
	m_hwndResponse = NULL;
	 
	pointer_type_  = 0;
	pointer_       = nullptr;

	ShowWindow(SW_HIDE);

}

LRESULT DigitalKeyboard::RuleChecking(WPARAM wParam, LPARAM lParam) {
	CString str0;
	value_.GetWindowText(str0);
	if (wParam == 0 || _ttof(str0) < min_value_ || _ttof(str0) > max_value_) {
		range_.set_text_color(RGB(255, 0, 0));
		RedrawWindow();
	} else {
		// 符合规范时颜色恢复正常
		range_.set_text_color(0);
		RedrawWindow();
	}
	return 0;
}