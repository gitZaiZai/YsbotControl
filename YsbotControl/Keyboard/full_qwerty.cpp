#include "stdafx.h"
#include "full_qwerty.h"
#include <regex>      // 正则表达式

BEGIN_MESSAGE_MAP(FullQwerty, DialogType)
	ON_COMMAND_RANGE(IDC_FULL_KEY, IDC_FULL_KEY + 55, KeyPress)
	ON_MESSAGE(WM_EDIT_MESSAGE, RuleChecking)
END_MESSAGE_MAP()

FullQwerty::FullQwerty() {
	font_.CreateFont(22, 0, 0, 0, FW_HEAVY, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS,
		DRAFT_QUALITY, FIXED_PITCH | FF_MODERN, _T("黑体"));
	original_value_ = _T("");
	legal_value_ = _T("");

	m_hwndResponse = NULL;
	m_lParamData = NULL;
}

void FullQwerty::DoDataExchange(CDataExchange* pDX) {

}

// void FullQwerty::OnCancel() {
// 	DestroyWindow();
// }
// 
// void FullQwerty::PostNcDestroy() {
// 	CDialogEx::PostNcDestroy();
// 	if (NULL != this) {
// 		delete this;
// 	}
// }

// 初始化
BOOL FullQwerty::OnInitDialog() {
	DialogType::OnInitDialog();

	// 根据父窗口位置计算居中位置
	CRect rect;
	GetParent()->GetWindowRect(&rect);
	int width0 = 710;
	int height0 = 290;
	// 全键盘几乎占满编辑区，所以不再计算相对坐标，直接指定
	SetWindowPos(NULL, 30, 220, width0, height0, SWP_HIDEWINDOW);
	// 设置背景色
	// 在操作系统的不同风格设置下，对话框会跟着变，所以在这里设置一下以防跟着变
	SetBackgroundColor(RGB(239, 239, 239));
	title_.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_NOTIFY,
		CRect(0, 0, width0, 30), this, IDC_FULL_QWERTY_TITLE);
	title_.SetBkBmp(_T("res\\keyboard_title.bmp"));
	title_.SetArea(rect);
	// 创建数值框
	value_.Create(WS_CHILD | WS_VISIBLE, CRect(4, 34, width0 - 8, 66), this,
		IDC_FULL_QWERTY_VALUE);
	value_.ModifyStyleEx(0, WS_EX_DLGMODALFRAME | WS_EX_STATICEDGE,
		SWP_DRAWFRAME);
	value_.SetFont(&font_);
	SetRulePlan(KBRule_varName);
	CreateButton();

	return TRUE;
}

// 创建按钮
void FullQwerty::CreateButton() {
	int x0 = 0;
	int y0 = 30;
	int width = 0;
	int height = 40;
	int row0 = -1;
	for (int i = 0; i != 56; i++) {
		if (row0 != kFullQwerty[i].row) {
			x0 = 2;
			y0 += height + 2;
			row0++;
		} else {
			x0 += width;
		}
		width = kFullQwerty[i].weight * 5;
		button_[i].Create(kFullQwerty[i].code[0], WS_CHILD | WS_VISIBLE,
			CRect(x0, y0, x0 + width, y0 + height),	this, IDC_FULL_KEY + i);
		button_[i].set_data(kFullQwerty[i]);
		button_[i].SetMyFont(20);
		// 设置按键的背景色，否则会受操作系统影响
		button_[i].SetFaceColor(RGB(239, 239, 239));
		// 设置显示图标
		if (kFullQwerty[i].icon.GetLength() != 0) {
			button_[i].SetMyIcon(kFullQwerty[i].icon);
		}
	}
	return;
}

// 按键响应
void FullQwerty::KeyPress(UINT number) {
	int id0 = number - IDC_FULL_KEY;
	CString str;
	value_.GetWindowText(str);
	CString key;
	button_[id0].GetWindowText(key);
	// 根据输入的不同字符进行判断处理
	switch (button_[id0].data().type) {
	case 11:    // 退格
		::SendMessage(value_, WM_CHAR, VK_BACK, 0);
		break;
	case 12:    // 取消
		value_.SetWindowText(_T(""));
		pointer_ = nullptr;
		if ( m_hwndResponse )
		{
			::SendMessage(m_hwndResponse, WM_KEYBOARD_CANCLE,0,0 );
		}
		ShowWindow(SW_HIDE);
		break;
	case 13:    // 重置
		value_.SetWindowText(_T(""));
		value_.SetFocus();
		break;
		// 确定，用消息把数值传递出去，隐藏窗口
	case 14: 
		{
			int iret = 0;
			if (pointer_type_ == 0) 
			{
				// 指针类型为0时以消息的形式返回
				// 			GetParent()->SendMessage(FULL_QWERTY_RESULT, 0, (LPARAM)(&str));
				if ( m_hwndResponse )
				{
					iret = ::SendMessage(m_hwndResponse,WM_KEYBOARD_ENTER, (WPARAM)(&str), m_lParamData );
				}
			} 
			else
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

			if ( iret )
			{
				ShowWindow(SW_HIDE);
			}
		}

		return;
	case 16: {  // 上档键
		CString str0;
		button_[0].GetWindowText(str0);
			int id1 = str0 == kFullQwerty[0].code[0] ? 1 : 0;
			for (int i = 0; i != 56; i++) {
				button_[i].SetWindowText(kFullQwerty[i].code[id1]);
			}
		}
		break;
	case 17: {  // 大小写键
	  	CString str0;
  		button_[14].GetWindowText(str0);
	  	int id1 = str0 == kFullQwerty[14].code[0] ? 1 : 0;
	  	for (int i = 0; i != 56; i++) {
	  		if (kFullQwerty[i].type != 1)
	  			continue;
	  		button_[i].SetWindowText(kFullQwerty[i].code[id1]);
	  	}
		}
		break;
	default:
		{
			wchar_t a1 = key[0];
			::SendMessage(value_, WM_CHAR, a1, 0);  // 直接向编辑框发送字符
			if ( key == _T("(") )
			{
				key = _T(")");
 				::SendMessage(value_, WM_CHAR, key[0], 0);  // 直接向编辑框发送字符
			}
		}

	}
	// 使用正则表达式检查是否符合规则
	CString str2;
	value_.GetWindowText(str2);
	std::string str1 = (CStringA)str2;
	std::regex re(rule_);
	bool regmat = regex_match(str1, re);
	if ( !regmat && button_[id0].data().type != 11) 
	{
		// 不符合规则时恢复成原来的字符
		value_.SetWindowText(str);
		value_.SetSel(str.GetLength(), str.GetLength() + 1, FALSE);
	}
	value_.SetFocus();
	return;
}

void FullQwerty::SetOriginalValue(CString str, int type0, void *p0) {
	value_.SetWindowText(str);
	original_value_ = str;
	title_.set_text(str);
	legal_value_ = str;
	// 把编辑框内的光标移到字符末尾
	value_.SetSel(str.GetLength(), str.GetLength() + 1, FALSE);
	value_.SetFocus();
	pointer_ = p0;
	pointer_type_ = type0;
	ShowWindow(SW_SHOW);
}

LRESULT FullQwerty::RuleChecking(WPARAM wParam, LPARAM lParam) {
	CString str0;
	value_.GetWindowText(str0);
	if (wParam == 0) {
		value_.SetWindowText(legal_value_);
		value_.SetSel(legal_value_.GetLength(), legal_value_.GetLength() + 1,
			FALSE);
	} else {
		legal_value_ = str0;
	}

	return 0;
}

BOOL FullQwerty::RuleChecking(CString str) {
	BOOL result = FALSE;
	return result;
}

// 使用默认的规则方案
void FullQwerty::SetRulePlan(unsigned short number) {
	switch (number) {
	// 变量与文件名的命名规则：
	// -- 必须以字母开头，后面可以是字母、数字与下划线；
	// -- 总长度不能超过20，字母不分大小写。
	case 0://变量名
		rule_ = "|([a-z]|[A-Z])\\w{0,19}";
		value_.SetLimitText(20);
		break;
	case 1://程序编辑
		rule_ = ".*";
		value_.SetLimitText(50);
		break;
	default:
		;
	}
	value_.set_rule(rule_);
	return;
}

void FullQwerty::activate_keyboard( HWND hwnd, LPCTSTR strvar, LPCTSTR strname, LPARAM pCell, DWORD sel )
{
	m_hwndResponse = hwnd;
 	 
	m_lParamData = pCell;

	value_.SetWindowText(strvar);
	value_.SetFocus();
	value_.SetSel(sel);
	legal_value_ = strvar;
	original_value_ = strvar;
	title_.set_text(strname);
	pointer_type_ = 0;
	ShowWindow(SW_SHOW);
	BringWindowToTop();
}