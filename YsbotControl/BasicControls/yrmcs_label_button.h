// Copyright 2016, ��˼��Ϣ�Ƽ����޹�˾
// All rights reserved.
//
// Author: letuspray@163.com (Lu Wei)
//
// Yasi Robot Motion Control System - ��˼�������˶�����ϵͳ
//
// ���Ǳ�ǩ��ʽ�İ�ť��ͷ�ļ�

#pragma once

#define ICON_NUMBER 4  // ͼ������

// ��ǩ��ʽ��ť��
class LabelButton : public CMFCButton {
public:
	LabelButton(void);
	~LabelButton(void);

	// ��������
	// ���룺height ����߶ȣ�width �����ȣ�Ϊ0ʱ��ʾ���ո߶����Զ����ÿ�ȣ�
	//   name �������ƣ�weigth ���������italic б�壬underline �»��ߡ�
	void SetMyFont(int height = 18, int width = 0, CString name = _T("����"),
		int weight = FW_NORMAL, BYTE italic = FALSE, BYTE underline = FALSE);
	inline void set_state(int state) {states_ = state;}      // ����״ֵ̬
	inline int states(void) {return states_;}                // ��ȡ״ֵ̬
	void SetCaption(short id1, CString caption);             // ���ñ���
	CString GetCaption(short id1);                           // ��ȡ����
	// ����ͼ������
	// ���룺number ��ţ�file ͼ���ļ�����width ��ȣ�height �߶�
	void SetIcons(short number, CString file, int width, int height);
	// ͨ��������õ�ǰ��ʾ����
	void SetLable(short number);

protected:

private:
	CFont *font_;                      // ����
	int states_;                       // ״ֵ̬
	HICON icons_[ICON_NUMBER];         // ͼ��ϵ��
	CString caption_[ICON_NUMBER];     // ����
};