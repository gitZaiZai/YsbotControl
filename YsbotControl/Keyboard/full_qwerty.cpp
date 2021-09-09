#include "stdafx.h"
#include "full_qwerty.h"
#include <regex>      // ������ʽ

BEGIN_MESSAGE_MAP(FullQwerty, DialogType)
	ON_COMMAND_RANGE(IDC_FULL_KEY, IDC_FULL_KEY + 55, KeyPress)
	ON_MESSAGE(WM_EDIT_MESSAGE, RuleChecking)
END_MESSAGE_MAP()

FullQwerty::FullQwerty() {
	font_.CreateFont(22, 0, 0, 0, FW_HEAVY, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS,
		DRAFT_QUALITY, FIXED_PITCH | FF_MODERN, _T("����"));
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

// ��ʼ��
BOOL FullQwerty::OnInitDialog() {
	DialogType::OnInitDialog();

	// ���ݸ�����λ�ü������λ��
	CRect rect;
	GetParent()->GetWindowRect(&rect);
	int width0 = 710;
	int height0 = 290;
	// ȫ���̼���ռ���༭�������Բ��ټ���������ֱ꣬��ָ��
	SetWindowPos(NULL, 30, 220, width0, height0, SWP_HIDEWINDOW);
	// ���ñ���ɫ
	// �ڲ���ϵͳ�Ĳ�ͬ��������£��Ի������ű䣬��������������һ���Է����ű�
	SetBackgroundColor(RGB(239, 239, 239));
	title_.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_NOTIFY,
		CRect(0, 0, width0, 30), this, IDC_FULL_QWERTY_TITLE);
	title_.SetBkBmp(_T("res\\keyboard_title.bmp"));
	title_.SetArea(rect);
	// ������ֵ��
	value_.Create(WS_CHILD | WS_VISIBLE, CRect(4, 34, width0 - 8, 66), this,
		IDC_FULL_QWERTY_VALUE);
	value_.ModifyStyleEx(0, WS_EX_DLGMODALFRAME | WS_EX_STATICEDGE,
		SWP_DRAWFRAME);
	value_.SetFont(&font_);
	SetRulePlan(KBRule_varName);
	CreateButton();

	return TRUE;
}

// ������ť
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
		// ���ð����ı���ɫ��������ܲ���ϵͳӰ��
		button_[i].SetFaceColor(RGB(239, 239, 239));
		// ������ʾͼ��
		if (kFullQwerty[i].icon.GetLength() != 0) {
			button_[i].SetMyIcon(kFullQwerty[i].icon);
		}
	}
	return;
}

// ������Ӧ
void FullQwerty::KeyPress(UINT number) {
	int id0 = number - IDC_FULL_KEY;
	CString str;
	value_.GetWindowText(str);
	CString key;
	button_[id0].GetWindowText(key);
	// ��������Ĳ�ͬ�ַ������жϴ���
	switch (button_[id0].data().type) {
	case 11:    // �˸�
		::SendMessage(value_, WM_CHAR, VK_BACK, 0);
		break;
	case 12:    // ȡ��
		value_.SetWindowText(_T(""));
		pointer_ = nullptr;
		if ( m_hwndResponse )
		{
			::SendMessage(m_hwndResponse, WM_KEYBOARD_CANCLE,0,0 );
		}
		ShowWindow(SW_HIDE);
		break;
	case 13:    // ����
		value_.SetWindowText(_T(""));
		value_.SetFocus();
		break;
		// ȷ��������Ϣ����ֵ���ݳ�ȥ�����ش���
	case 14: 
		{
			int iret = 0;
			if (pointer_type_ == 0) 
			{
				// ָ������Ϊ0ʱ����Ϣ����ʽ����
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
	case 16: {  // �ϵ���
		CString str0;
		button_[0].GetWindowText(str0);
			int id1 = str0 == kFullQwerty[0].code[0] ? 1 : 0;
			for (int i = 0; i != 56; i++) {
				button_[i].SetWindowText(kFullQwerty[i].code[id1]);
			}
		}
		break;
	case 17: {  // ��Сд��
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
			::SendMessage(value_, WM_CHAR, a1, 0);  // ֱ����༭�����ַ�
			if ( key == _T("(") )
			{
				key = _T(")");
 				::SendMessage(value_, WM_CHAR, key[0], 0);  // ֱ����༭�����ַ�
			}
		}

	}
	// ʹ��������ʽ����Ƿ���Ϲ���
	CString str2;
	value_.GetWindowText(str2);
	std::string str1 = (CStringA)str2;
	std::regex re(rule_);
	bool regmat = regex_match(str1, re);
	if ( !regmat && button_[id0].data().type != 11) 
	{
		// �����Ϲ���ʱ�ָ���ԭ�����ַ�
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
	// �ѱ༭���ڵĹ���Ƶ��ַ�ĩβ
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

// ʹ��Ĭ�ϵĹ��򷽰�
void FullQwerty::SetRulePlan(unsigned short number) {
	switch (number) {
	// �������ļ�������������
	// -- ��������ĸ��ͷ�������������ĸ���������»��ߣ�
	// -- �ܳ��Ȳ��ܳ���20����ĸ���ִ�Сд��
	case 0://������
		rule_ = "|([a-z]|[A-Z])\\w{0,19}";
		value_.SetLimitText(20);
		break;
	case 1://����༭
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