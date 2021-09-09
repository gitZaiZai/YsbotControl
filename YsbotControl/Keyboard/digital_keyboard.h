// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: letuspray@163.com (Lu Wei)
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统
//
// 数字键盘类头文件

#pragma once

#include "redbud_caption.h"
#include "keyboard_button.h"
#include "static_plus.h"
#include "../UIDialogs/DialogType.h"
#include "../Singleton.h"
#include "edit_plus.h"

#define IDD_DIGITAL_KEYBOARD 5101
#define IDC_DIGITAL_KEYBOARD_TITLE 5102
#define IDC_DIGITAL_KEY 5111
#define IDC_DIGITAL_KEYBOARD_VALUE 5103
#define DIGITAL_KEYBOARD_RESULT 5104

const KeyData kDigitalKeyboard[] = {
	// 第一排
	_T("7"), _T("7"), 0, 0, 0, 10, _T(""),
	_T("8"), _T("8"), 0, 0, 0, 10, _T(""),
	_T("9"), _T("9"), 0, 0, 0, 10, _T(""),
	_T(""), _T(""), 11, 0, 0, 10, _T("res\\backup.ico"),
	// 第二排
	_T("4"), _T("4"), 0, 0, 0, 10, _T(""),
	_T("5"), _T("5"), 0, 0, 0, 10, _T(""),
	_T("6"), _T("6"), 0, 0, 0, 10, _T(""),
	_T(""), _T(""), 12, 0, 0, 10, _T("res\\cancel.ico"),
	// 第三排
	_T("1"), _T("1"), 0, 0, 0, 10, _T(""),
	_T("2"), _T("2"), 0, 0, 0, 10, _T(""),
	_T("3"), _T("3"), 0, 0, 0, 10, _T(""),
	_T(""), _T(""), 13, 0, 0, 10, _T("res\\clear.ico"),
	// 第四排
	_T("0"), _T("0"), 0, 0, 0, 10, _T(""),
	_T(""), _T(""), 2, 0, 0, 10, _T("res\\sign.ico"),
	_T("."), _T("."), 3, 0, 0, 10, _T(""),
	_T(""), _T(""), 14, 0, 0, 10, _T("res\\checkmark.ico"),
};

// 数字键盘类
class DigitalKeyboard : public DialogType {
public:
  DigitalKeyboard(void);
  ~DigitalKeyboard(void);
  enum {IDD = IDD_DIGITAL_KEYBOARD};
	inline CString value(void) {
		CString str;
		value_.GetWindowText(str);
		return str;
	}
	inline CString original_value(void) {return title_.text();}
	void set_original_value(CString str,int type0 = 0, void *p0 = nullptr); 
	
	void activate_keyboard( HWND hwnd, LPCTSTR strvar, LPCTSTR strname, LPARAM pCell = NULL, DWORD sel = -1 );

	// 设置数值范围与小数位数
	void SetValueRange(double min0, double max0, short decimals);
	void SetArea(CRect rect){title_.SetArea(rect);};
protected:
  virtual void DoDataExchange(CDataExchange* pDX);
  BOOL OnInitDialog(void);
//   afx_msg void OnCancel(void);
//   afx_msg void PostNcDestroy(void);
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg void KeyPress(UINT number);
  // 规则检查。接收来自数值框传来的检测结果，并根据结果做出相应提示
  afx_msg LRESULT RuleChecking(WPARAM wParam, LPARAM lParam);

  DECLARE_MESSAGE_MAP()

private:
	CFont font_;                 // 字体一
    RedbudCaption title_;        // 标题栏
	KeyboardButton button_[16];  // 键盘按钮
	EditPlus value_;                // 数值框
	StaticPlus range_;           // 数值范围显示框
	CString original_value_;     // 原始数值
	double max_value_;           // 最大数值
	double min_value_;           // 最小数值
	unsigned decimals_;          // 小数位数
	std::string rule_;        // 字符规则（正则表达式用）
	void *pointer_;                  // 用户传入的指针
	int pointer_type_;               // 用户传入的打针类型

	void CreateButton();

	void close_keyboard();
	void create_rule(double min0, double max0, short decimals);
	CString strRange;

	HWND m_hwndResponse;
	LPARAM m_lParamData;



};

#define DKEY Singleton<DigitalKeyboard>::Instance()