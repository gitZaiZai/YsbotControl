#include "stdafx.h"
#include "dialog_template.h"

CDlgTemplate::CDlgTemplate(int buffer_length) {
	buffer = new WORD[buffer_length];
	if ((!buffer) || (DLG_TEMPLATE_BUFFER_MAX_LENGTH > buffer_length)) {
		exit(0);
	}
}

CDlgTemplate::~CDlgTemplate() {
	if (buffer) {
		delete [] buffer;
		buffer = NULL;
	}
}

DLGTEMPLATE * CDlgTemplate::CreateTemplate(DWORD style, CRect &rect, 
	CString caption, DWORD style_ex) {
		WORD *temp = buffer;
		DLGTEMPLATE * dlg_temp = (DLGTEMPLATE *)temp;
		// 对话框模板
		dlg_temp->style = style;
		dlg_temp->dwExtendedStyle = style_ex;
		dlg_temp->cdit = 0;
		dlg_temp->cx = rect.Width();
		dlg_temp->cy = rect.Height();
		dlg_temp->x = (short)rect.left;
		dlg_temp->y = (short)rect.top;
		temp = (WORD *)(dlg_temp + 1);
		// 菜单
		*temp++=0;
		// class
		*temp++=0;
		// caption
 		CStringW strWide = CT2CW(caption); // 将T字符转换成宽字符
		wcscpy((WCHAR *)temp, strWide); 
// 		wcscpy_s((WCHAR *)temp, wcslen(caption.GetBuffer(0)), caption); // 1
		temp += caption.GetLength() + 1;
		// font

		return dlg_temp;
}