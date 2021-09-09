// Copyright 2016, ��˼��Ϣ�Ƽ��������ι�˾
// All right reserved.
//
// Author: letuspray@163.com (Lu Wei)
//
// Yasi Robot Motion Control System - ��˼�������˶�����ϵͳ
//
// ���ּ�����ʵ���ļ�

#include "stdafx.h"
#include "digital_keyboard.h"
#include <regex>      // ������ʽ
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
		DRAFT_QUALITY, FIXED_PITCH | FF_MODERN, _T("����"));
	original_value_ = _T("0");
	min_value_ = -100000;
	max_value_ = 100000;
	decimals_ = 0;
	// ��ʼ��������ʽ����Ĭ��ֻ������0�������ʹ�ñ�����ʱ��
	// ��Ӧ�ý�����ֵ��Χ��С��λ��Ϊ0ʱ��Ȼ�ж�Ϊ������������
	rule_ = "[0]";
	pointer_ = NULL;
	pointer_type_ = 0;
	m_hwndResponse = NULL;
	m_lParamData = NULL;
	create_rule(-1000000,1000000,3); // Ĭ���Ǹ�����
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

// ��ʼ��
BOOL DigitalKeyboard::OnInitDialog() 
{
	DialogType::OnInitDialog();
	// ���ݸ�����λ�ü������λ��
	CRect rect;
	GetParent()->GetWindowRect(&rect);
	int x0 = rect.left + rect.Width() / 2 - 150;
	int y0 = rect.top +	rect.Height() / 3;
	int width0 = 300;
	int height0 = 300;
  SetWindowPos(NULL, x0, y0, width0, height0, SWP_HIDEWINDOW); //liurundan
  // ���ñ���ɫ
  // �ڲ���ϵͳ�Ĳ�ͬ��������£��Ի������ű䣬��������������һ���Է����ű�
  SetBackgroundColor(RGB(239, 239, 239));
  title_.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_NOTIFY,
    CRect(0, 0, width0, 30), this, IDC_DIGITAL_KEYBOARD_TITLE);
  title_.SetBkBmp(_T("res\\keyboard_title.bmp"));
	title_.SetArea(rect);
	// ������ֵ��
	value_.Create(WS_CHILD | WS_VISIBLE | ES_CENTER,
		CRect(4, 34, 290, 66), this, IDC_DIGITAL_KEYBOARD_VALUE);
	value_.ModifyStyleEx(0, WS_EX_DLGMODALFRAME | WS_EX_STATICEDGE,
		SWP_DRAWFRAME);
	value_.SetFont(&font_);
	// ������ֵ��Χ��
	range_.Create(_T(""), WS_CHILD | WS_VISIBLE |	SS_CENTER | SS_OWNERDRAW,
		CRect(10, 73, 290, 90),	this, 4002);
	range_.SetMyFont();
	// �������̰�ť
	CreateButton();
	value_.SetFocus();
  return TRUE;
}

HBRUSH DigitalKeyboard::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	// ����Ĵ�����ֻ��״̬�ı༭�򱳾�ɫ��ȻΪ��ɫ
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
	// ��������Ĳ�ͬ�ַ������жϴ���
	switch (button_[id0].data().type) {
	case 2:  // ���븺�ţ��������ַ���ͬ�������Ǽ��������
		{
			if (str.Find(_T("-")) != -1)
				break;
			// �Ȱѹ���Ƶ�����ߣ�������ɺ����Ƶ����ұ�
			value_.SetSel(0, 0, FALSE);
			::SendMessage(value_, WM_CHAR, '-', 0);
			value_.SetSel(str.GetLength() + 1, str.GetLength() + 2, FALSE);
		}
		break;
	case 11: // �˸�
		::SendMessage(value_, WM_CHAR, VK_BACK, 0);
		break;
	case 12: // ȡ�����ָ�ԭʼ��ֵ�����ش���
		value_.SetWindowText(_T(""));
		pointer_ = nullptr;
		if ( m_hwndResponse )
		{
			::SendMessage(m_hwndResponse, WM_KEYBOARD_CANCLE,0,0 );
		}
		ShowWindow(SW_HIDE);
		break;
	case 13: // ��λ���ָ�ԭʼ��ֵ�������ش���
		value_.SetWindowText(_T(""));
		value_.SetFocus();
		break;
	case 14: // ȷ��������Ϣ����ֵ���ݳ�ȥ�����ش���
		{
			CString str2;
			value_.GetWindowText(str2);
			std::string str1 = (CStringA)str2;
			std::regex re(rule_);
		if (regex_match(str1, re) == false || _ttof(str2) < min_value_ ||
			_ttof(str2) > max_value_) 
			break;  // �����Ϲ������
		str1 = (CStringA)str;
		if (pointer_type_ == 0) 
		{
			// ָ������Ϊ0ʱ����Ϣ����ʽ����
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
			::SendMessage(value_, WM_CHAR, a1, 0);  // ֱ����༭�����ַ�
		}
		break;
	}
	// ʹ��������ʽ����Ƿ���Ϲ���
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
		// ���ð����ı���ɫ��������ܲ���ϵͳӰ��
		button_[i].SetFaceColor(RGB(239, 239, 239));
		// ������ʾͼ��
		if (kDigitalKeyboard[i].icon.GetLength() != 0) {
			button_[i].SetMyIcon(kDigitalKeyboard[i].icon);
		}
	}
	return;
}
 
// ����������ֵ��Χ����ʾ�������������������ʽ����
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
	// ���ŵĴ���
	std::string sign0 = "";
	if (min0 < 0) {
		sign0 = "[-]{";
		sign0 += max0 < 0 ? "1" : "0";
		sign0 += ",1}";
	}
	// С�����ֵĴ���
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
		// ���Ϲ淶ʱ��ɫ�ָ�����
		range_.set_text_color(0);
		RedrawWindow();
	}
	return 0;
}