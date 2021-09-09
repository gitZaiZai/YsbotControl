#include "stdafx.h"
#include "edit_plus.h"
#include <regex>

IMPLEMENT_DYNAMIC(EditPlus, CEdit)

BEGIN_MESSAGE_MAP(EditPlus, CEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()

EditPlus::EditPlus() {
	return;
}

void EditPlus::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) {
	CEdit::OnChar(nChar, nRepCnt, nFlags);
	if (RuleChecking() == false)
		::SendMessage(GetParent()->GetSafeHwnd(), WM_EDIT_MESSAGE, 0, 0);
	else
		::SendMessage(GetParent()->GetSafeHwnd(), WM_EDIT_MESSAGE, 1, 0);
	return;
}

bool EditPlus::RuleChecking() {
	CString str;
	GetWindowText(str);
	std::string str1 = (CStringA)str;
	std::regex re(rule_);
	return regex_match(str1, re);
}
