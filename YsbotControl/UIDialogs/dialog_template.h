#pragma once

#define DLG_TEMPLATE_BUFFER_MAX_LENGTH 1024

// �Ի�����Դģ����
class CDlgTemplate {
public:
	CDlgTemplate(int buffer_length = DLG_TEMPLATE_BUFFER_MAX_LENGTH);
	virtual ~CDlgTemplate();
private:
	WORD *buffer;
public:
	DLGTEMPLATE * CreateTemplate(DWORD style, CRect &rect, CString caption,
		DWORD style_ex = 0);
};