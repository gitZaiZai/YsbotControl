// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: letuspray@163.com (Lu Wei)
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统
//
// 数字键盘类头文件

#pragma once

#include "../UIDialogs/DialogType.h"
#include "keyboard_button.h"
#include "redbud_caption.h"
#include "../Singleton.h"
#include "edit_plus.h"

#define KBRule_varName 0
#define KBRule_Program 1
#define KBRule_All 2

#define IDD_FULL_QWERTY 5011
#define IDC_FULL_QWERTY_TITLE 5012
#define IDC_FULL_QWERTY_VALUE 5013
#define IDC_FULL_KEY 5021
#define FULL_QWERTY_RESULT 5016


class CCellID;

const KeyData kFullQwerty[] = {
	// 第一排
	_T("`"), _T("~"), 0, 0, 0, 10, _T(""),
	_T("1"), _T("!"), 0, 0, 0, 10, _T(""),
	_T("2"), _T("@"), 0, 0, 0, 10, _T(""),
	_T("3"), _T("#"), 0, 0, 0, 10, _T(""),
	_T("4"), _T("$"), 0, 0, 0, 10, _T(""),
	_T("5"), _T("%"), 0, 0, 0, 10, _T(""),
	_T("6"), _T("^"), 0, 0, 0, 10, _T(""),
	_T("7"), _T("&&"), 0, 0, 0, 10, _T(""),  // &比较特殊，两个才会显示一个
	_T("8"), _T("*"), 0, 0, 0, 10, _T(""),
	_T("9"), _T("["), 0, 0, 0, 10, _T(""),
	_T("0"), _T("]"), 0, 0, 0, 10, _T(""),
	_T("-"), _T("_"), 0, 0, 0, 10, _T(""),
	_T("="), _T("+"), 0, 0, 0, 10, _T(""),
	_T("\\"), _T("|"), 0, 0, 0, 10, _T(""),
	// 第二排
	_T("q"), _T("Q"), 1, 0, 1, 10, _T(""),
	_T("w"), _T("W"), 1, 0, 1, 10, _T(""),
	_T("e"), _T("E"), 1, 0, 1, 10, _T(""),
	_T("r"), _T("R"), 1, 0, 1, 10, _T(""),
	_T("t"), _T("T"), 1, 0, 1, 10, _T(""),
	_T("y"), _T("Y"), 1, 0, 1, 10, _T(""),
	_T("u"), _T("U"), 1, 0, 1, 10, _T(""),
	_T("i"), _T("I"), 1, 0, 1, 10, _T(""),
	_T("o"), _T("O"), 1, 0, 1, 10, _T(""),
	_T("p"), _T("P"), 1, 0, 1, 10, _T(""),
	_T("("), _T("{"), 0, 0, 1, 10, _T(""),
	_T(")"), _T("}"), 0, 0, 1, 10, _T(""),
	_T(""), _T(""), 11, 0, 1, 20, _T("res\\backup.ico"),
	// 第三排
	_T("Ctrl"), _T("Ctrl"), 0, 0, 2, 15, _T(""),
	_T("Caps"), _T("Caps"), 17, 0, 2, 15, _T(""),
	_T("a"), _T("A"), 1, 0, 2, 10, _T(""),
	_T("s"), _T("S"), 1, 0, 2, 10, _T(""),
	_T("d"), _T("D"), 1, 0, 2, 10, _T(""),
	_T("f"), _T("F"), 1, 0, 2, 10, _T(""),
	_T("g"), _T("G"), 1, 0, 2, 10, _T(""),
	_T("h"), _T("H"), 1, 0, 2, 10, _T(""),
	_T("j"), _T("J"), 1, 0, 2, 10, _T(""),
	_T("k"), _T("K"), 1, 0, 2, 10, _T(""),
	_T("l"), _T("L"), 1, 0, 2, 10, _T(""),
	_T(";"), _T(":"), 0, 0, 2, 10, _T(""),
	_T("'"), _T("\""), 0, 0, 2, 10, _T(""),
	// 第四排
	_T("Alt"), _T("Alt"), 0, 0, 3, 15, _T(""),
	_T(""), _T(""), 16, 0, 3, 15, _T("res\\shift_key.ico"),
	_T(""), _T(""), 13, 0, 3, 10, _T("res\\clear.ico"),
	_T("z"), _T("Z"), 1, 0, 3, 10, _T(""),
	_T("x"), _T("X"), 1, 0, 3, 10, _T(""),
	_T("c"), _T("C"), 1, 0, 3, 10, _T(""),
	_T("v"), _T("V"), 1, 0, 3, 10, _T(""),
	_T("b"), _T("B"), 1, 0, 3, 10, _T(""),
	_T("n"), _T("N"), 1, 0, 3, 10, _T(""),
	_T("m"), _T("M"), 1, 0, 3, 10, _T(""),
	_T(","), _T("<"), 0, 0, 3, 10, _T(""),
	_T("."), _T(">"), 0, 0, 3, 10, _T(""),
	_T("/"), _T("?"), 0, 0, 3, 10, _T(""),
	// 第五排
	_T(""), _T(""), 12, 0, 4, 30, _T("res\\cancel.ico"),
	_T(" "), _T(" "), 0, 0, 4, 80, _T(""),
	_T(""), _T(""), 14, 0, 4, 30, _T("res\\checkmark.ico"),
};

// 全键盘类
class FullQwerty : public DialogType {
public:
	enum {IDD = IDD_FULL_QWERTY};
	FullQwerty();
	void SetOriginalValue(CString str, int type0 = 0, void *p0 = nullptr);
	inline void set_rule(CString rule) {rule_ = CStringA(rule);}
	void SetRulePlan(unsigned short number);
	void SetArea(CRect rect){title_.SetArea(rect);};
	void activate_keyboard( HWND hwnd, LPCTSTR strvar, LPCTSTR strname, LPARAM pCell = NULL, DWORD sel = -1 );
	 
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	BOOL OnInitDialog(void);
// 	afx_msg void OnCancel(void);
// 	afx_msg void PostNcDestroy(void);
	afx_msg void KeyPress(UINT number);

	// 规则检查。接收来自数值框传来的检测结果，并根据结果做出相应提示
	afx_msg LRESULT RuleChecking(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

private:
	CFont font_;                     // 字体一
	RedbudCaption title_;            // 标题栏
	KeyboardButton button_[56];      // 键盘按钮
	EditPlus value_;                    // 内容框
	CString original_value_;         // 原始值
	std::string rule_;               // 字符规则（正则表达式用）
	void *pointer_;                  // 用户传入的指针
	int pointer_type_;               // 用户传入的打针类型

	void CreateButton();
	BOOL RuleChecking(CString str);  // 规则检查（使用正则表达式）
	CString legal_value_;            // 合法值

	HWND m_hwndResponse;
	 
	LPARAM m_lParamData;
};

#define FKEY Singleton<FullQwerty>::Instance()