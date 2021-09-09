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

// ͨ��ģ�崴���Ի���
BOOL DialogType::CreateDialogFromTemplate( CWnd *parent, BOOL mode,	DWORD style, LPCSTR caption, DWORD style_ex) 
{
		// ����nothrow��ʧ��ʱ����NULL�����򷵻ص����쳣
		BYTE *temp = new(std::nothrow) BYTE[1024];
		if (NULL == temp) return FALSE;
		LPDLGTEMPLATE dlg_temp = (LPDLGTEMPLATE)temp;

		// �Ի���λ�ô�С�ڶԻ����OnInitDialog�����ڽ��о�ȷ����
		dlg_temp->style = style;              // ���
		dlg_temp->dwExtendedStyle = style_ex; // ��չ���
		// �ؼ�������ֵ�������0��Ӧ�������Ӧ��DLGITEMTEMPLATE�����򱨴�
		dlg_temp->cdit = 0;
		// ���µ�λ������������С���߼���λ��ʵ��ʹ���������н��о�ȷ���ã�
		// �����ڶԻ�����OnInitDialog��ʹ��SetWindowPOS��MoveWindow�Ⱥ���
		dlg_temp->x = 10;                     // ���ϽǺ�����
		dlg_temp->y = 10;                     // ���Ͻ�������
		dlg_temp->cx = 200;                   // ���
		dlg_temp->cy = 200;                   // �߶�
		// �Ի���䳤����
		LPWORD lpw = (LPWORD)(dlg_temp + 1);
		*lpw++ = 0x0000;                      // û�в˵�
		*lpw++ = 0x0000;                      // ʹ��ϵͳĬ����
		// ����
		int n = MultiByteToWideChar(CP_ACP, 0, caption, -1, (LPWSTR)lpw, 50);
		lpw += n;

		// ע��ģ̬���ģ̬�����𣬳��˱������ڴ�����ʽ�ϵĲ�֮ͬ�⣬
		// �û��ڵ���ǰ����ú�Ĵ�����һ������ο��������
		BOOL result = FALSE;
		if (mode == TRUE) {
			// ����ģ̬�Ի���
			result = InitModalIndirect(dlg_temp, parent);
			DoModal();
		} else {
			// ���ɷ�ģ̬�Ի���
			result = CreateIndirect(dlg_temp, parent);
		}
		// �ͷſռ�
		if (NULL != temp) {
			delete[] temp;
			temp = NULL;
		}

		return result;
}
