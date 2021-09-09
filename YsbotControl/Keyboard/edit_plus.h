#pragma once


class EditPlus : public CEdit {
	DECLARE_DYNAMIC(EditPlus)
public:
	EditPlus(void);

	inline void set_rule(std::string rule) {rule_ = rule;}

protected:
	bool RuleChecking(void);

	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	DECLARE_MESSAGE_MAP()

private:
	std::string rule_;  // 正则表达式规则
};