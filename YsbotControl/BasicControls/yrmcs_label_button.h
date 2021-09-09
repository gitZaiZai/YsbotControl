// Copyright 2016, 亚思信息科技有限公司
// All rights reserved.
//
// Author: letuspray@163.com (Lu Wei)
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统
//
// 这是标签形式的按钮类头文件

#pragma once

#define ICON_NUMBER 4  // 图标数量

// 标签形式按钮类
class LabelButton : public CMFCButton {
public:
	LabelButton(void);
	~LabelButton(void);

	// 字体设置
	// 输入：height 字体高度，width 字体宽度（为0时表示按照高度来自动设置宽度）
	//   name 字体名称，weigth 字体磅数，italic 斜体，underline 下划线。
	void SetMyFont(int height = 18, int width = 0, CString name = _T("黑体"),
		int weight = FW_NORMAL, BYTE italic = FALSE, BYTE underline = FALSE);
	inline void set_state(int state) {states_ = state;}      // 设置状态值
	inline int states(void) {return states_;}                // 读取状态值
	void SetCaption(short id1, CString caption);             // 设置标题
	CString GetCaption(short id1);                           // 获取标题
	// 设置图标序列
	// 输入：number 序号；file 图标文件名；width 宽度；height 高度
	void SetIcons(short number, CString file, int width, int height);
	// 通过序号设置当前显示内容
	void SetLable(short number);

protected:

private:
	CFont *font_;                      // 字体
	int states_;                       // 状态值
	HICON icons_[ICON_NUMBER];         // 图标系列
	CString caption_[ICON_NUMBER];     // 文字
};