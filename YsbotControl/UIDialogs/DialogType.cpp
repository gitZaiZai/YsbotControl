// DialogType.cpp : implementation file
//

#include "stdafx.h"
#include "DialogType.h"
#include "afxdialogex.h"


// DialogType dialog

IMPLEMENT_DYNAMIC(DialogType, CDialogEx)

DialogType::DialogType(): CDialogEx()
{

}
DialogType::DialogType(UINT uResourceID, CWnd* pParent): CDialogEx(uResourceID, pParent)
{

}
DialogType::DialogType(LPCTSTR pszResourceID, CWnd* pParent): CDialogEx(pszResourceID, pParent)
{

}
DialogType::~DialogType()
{
}

void DialogType::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DialogType, CDialogEx)
	ON_MESSAGE( WM_BUTTON_PRESS,    &DialogType::OnMainButtonClicked )
	ON_MESSAGE( WM_KEYBOARD_ENTER,  &DialogType::OnKeyboardEnter     )
	ON_MESSAGE( WM_KEYBOARD_CANCLE, &DialogType::OnKeyboardCancle    )
	ON_MESSAGE( WM_RESIZE_GRID,     &DialogType::OnResizeGrid        )
END_MESSAGE_MAP()

BOOL DialogType::CreateModeDlg(CString caption, CRect rect, BOOL mode,	CWnd *parent, DWORD style /*=WS_POPUP | WS_CHILD | WS_BORDER*/) 
{
		BOOL result = FALSE;
		DLGTEMPLATE *temp = NULL;
  		temp = dlg_temp.CreateTemplate(style, rect,caption);
 		if (mode) {
			result = InitModalIndirect(temp, parent);
		} else {
			result = CreateIndirect(temp, parent);
		}
 
		return result;
}

// DialogType message handlers

// 通过模板创建对话框
BOOL DialogType::CreateDialogFromTemplate( CWnd *parent, BOOL mode,	DWORD style, LPCSTR caption, DWORD style_ex) 
{
		// 加入nothrow，失败时返回NULL，否则返回的是异常
		BYTE *temp = new(std::nothrow) BYTE[1024];
		if (NULL == temp) return FALSE;
		LPDLGTEMPLATE dlg_temp = (LPDLGTEMPLATE)temp;

		// 对话框。位置大小在对话框的OnInitDialog函数内进行精确设置
		dlg_temp->style = style;              // 风格
		dlg_temp->dwExtendedStyle = style_ex; // 扩展风格
		// 控件数（此值如果大于0，应该添加相应的DLGITEMTEMPLATE，否则报错）
		dlg_temp->cdit = 0;
		// 以下单位是相对于字体大小的逻辑单位，实际使用中请自行进行精确设置，
		// 比如在对话框中OnInitDialog中使用SetWindowPOS、MoveWindow等函数
		dlg_temp->x = 10;                     // 左上角横坐标
		dlg_temp->y = 10;                     // 左上角纵坐标
		dlg_temp->cx = 200;                   // 宽度
		dlg_temp->cy = 200;                   // 高度
		// 对话框变长数组
		LPWORD lpw = (LPWORD)(dlg_temp + 1);
		*lpw++ = 0x0000;                      // 没有菜单
		*lpw++ = 0x0000;                      // 使用系统默认类
		// 标题
		int n = MultiByteToWideChar(CP_ACP, 0, caption, -1, (LPWSTR)lpw, 50);
		lpw += n;

		// 注意模态与非模态的区别，除了本函数在创建方式上的不同之外，
		// 用户在调用前与调用后的处理都不一样，请参考相关资料
		BOOL result = FALSE;
		if (mode == TRUE) {
			// 生成模态对话框
			result = InitModalIndirect(dlg_temp, parent);
			DoModal();
		} else {
			// 生成非模态对话框
			result = CreateIndirect(dlg_temp, parent);
		}
		// 释放空间
		if (NULL != temp) {
			delete[] temp;
			temp = NULL;
		}

		return result;
}
