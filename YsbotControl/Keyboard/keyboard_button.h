// Copyright 2016, ��˼��Ϣ�Ƽ��������ι�˾
// All right reserved.
//
// Author: letuspray@163.com (Lu Wei)
//
// Yasi Robot Motion Control System - ��˼�������˶�����ϵͳ
//
// ���̰�ť��ͷ�ļ�

#pragma once

struct KeyData {
	CString code[2];  // Ҫ��ʾ���ַ���������ͨ״̬���ϵ���״̬
	int type;         // ���࣬0�Ƿ����࣬1����ĸ�࣬2���������࣬3��С����
	int state;        // ״̬��0����ͨ״̬��1���ϵ���״̬
	int row;          // �ڼ���
	int weight;       // �����ռ��Ȩ��
	CString icon;     // ָ���ͼ���ļ�
};

class KeyboardButton : public CMFCButton {
public:
	KeyboardButton(void);
	~KeyboardButton(void);

	// ��������
	// ���룺height ����߶ȣ�width �����ȣ�Ϊ0ʱ��ʾ���ո߶����Զ����ÿ�ȣ�
	//   name �������ƣ�weigth ���������italic б�壬underline �»��ߡ�
	void SetMyFont(int height = 18, int width = 0, CString name = _T("����"),
		int weight = FW_HEAVY, BYTE italic = FALSE, BYTE underline = FALSE);
	void SetMyIcon(CString icon);                     // ͼ������
	inline KeyData data(void) {return data_;}
	inline void set_data(KeyData data) {data_ = data;}

protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

private:
	CFont *font_;      // ����
	KeyData data_;
};