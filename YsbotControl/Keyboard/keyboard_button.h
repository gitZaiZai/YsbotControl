// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: letuspray@163.com (Lu Wei)
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统
//
// 键盘按钮类头文件

#pragma once

struct KeyData {
	CString code[2];  // 要显示的字符，包含普通状态与上档键状态
	int type;         // 分类，0是符号类，1是字母类，2是正负号类，3是小数点
	int state;        // 状态，0是普通状态，1是上档键状态
	int row;          // 第几行
	int weight;       // 宽度所占的权重
	CString icon;     // 指向的图标文件
};

class KeyboardButton : public CMFCButton {
public:
	KeyboardButton(void);
	~KeyboardButton(void);

	// 字体设置
	// 输入：height 字体高度，width 字体宽度（为0时表示按照高度来自动设置宽度）
	//   name 字体名称，weigth 字体磅数，italic 斜体，underline 下划线。
	void SetMyFont(int height = 18, int width = 0, CString name = _T("黑体"),
		int weight = FW_HEAVY, BYTE italic = FALSE, BYTE underline = FALSE);
	void SetMyIcon(CString icon);                     // 图标设置
	inline KeyData data(void) {return data_;}
	inline void set_data(KeyData data) {data_ = data;}

protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

private:
	CFont *font_;      // 字体
	KeyData data_;
};