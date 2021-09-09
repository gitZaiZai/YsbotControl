// EditPopupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "YsbotControl.h"
#include "EditPopupDlg.h"
#include "afxdialogex.h"


// CEditPopupDlg dialog
#define IDC_EDIT					1000

IMPLEMENT_DYNAMIC(CEditPopupDlg, CDialogEx)

CEditPopupDlg::CEditPopupDlg(): CDialogEx()
{

}

CEditPopupDlg::~CEditPopupDlg()
{
}

void CEditPopupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT, m_ctlEdit);
	DDX_Control(pDX, IDOK, m_ctlOk);
	DDX_Control(pDX, IDCANCEL, m_ctlCancel);
}


BEGIN_MESSAGE_MAP(CEditPopupDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, OnOk)
END_MESSAGE_MAP()


// CEditPopupDlg message handlers
int CEditPopupDlg::DoModal
	(
	CWnd*				pParent,
	CRect				editRect,
	CString				text,
	DWORD				sel
	)
{
#pragma pack(push)
#pragma pack(2)
	struct tDialogTemplate
	{
		DLGTEMPLATE			dlgTemplate;
		WORD				dlgMenuArray[1];
		WORD				dlgClassArray[1];
		WORD				dlgTitleArray[1];
		DLGITEMTEMPLATE		editTemplate;
		WORD				editClassArray[2];
		WORD				editTitleArray[1];
		WORD				editDataArray[1];
		WORD				_reserved1;
		DLGITEMTEMPLATE		okTemplate;
		WORD				okClassArray[2];
		WORD				okTitleArray[1];
		WORD				okDataArray[1];
		WORD				_reserved2;
		DLGITEMTEMPLATE		cancelTemplate;
		WORD				cancelClassArray[2];
		WORD				cancelTitleArray[1];
		WORD				cancelDataArray[1];
	};
#pragma pack(pop)

	tDialogTemplate			dtl;

	memset(&dtl, 0, sizeof(dtl));

	dtl.dlgTemplate.style			= WS_POPUP | WS_DLGFRAME;
	dtl.dlgTemplate.dwExtendedStyle	= 0;
	dtl.dlgTemplate.cdit			= 3;
	dtl.dlgTemplate.x				= 0;
	dtl.dlgTemplate.y				= 0;
	dtl.dlgTemplate.cx				= 1;
	dtl.dlgTemplate.cy				= 1;

	dtl.editTemplate.style			= WS_VISIBLE | WS_BORDER | WS_TABSTOP;
	dtl.editTemplate.dwExtendedStyle= 0;
	dtl.editTemplate.x				= 0;
	dtl.editTemplate.y				= 0;
	dtl.editTemplate.cx				= 1;
	dtl.editTemplate.cy				= 1;
	dtl.editTemplate.id				= IDC_EDIT;
	dtl.editClassArray[0]			= 0xFFFF;
	dtl.editClassArray[1]			= 0x0081;	// Edit

	dtl.okTemplate.style			= WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP;
	dtl.okTemplate.dwExtendedStyle	= 0;
	dtl.okTemplate.x				= 0;
	dtl.okTemplate.y				= 0;
	dtl.okTemplate.cx				= 1;
	dtl.okTemplate.cy				= 1;
	dtl.okTemplate.id				= IDOK;
	dtl.okClassArray[0]				= 0xFFFF;
	dtl.okClassArray[1]				= 0x0080;	// Button

	dtl.cancelTemplate.style		= WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP;
	dtl.cancelTemplate.dwExtendedStyle= 0;
	dtl.cancelTemplate.x			= 0;
	dtl.cancelTemplate.y			= 0;
	dtl.cancelTemplate.cx			= 1;
	dtl.cancelTemplate.cy			= 1;
	dtl.cancelTemplate.id			= IDCANCEL;
	dtl.cancelClassArray[0]			= 0xFFFF;
	dtl.cancelClassArray[1]			= 0x0080;	// Button

	BOOL result = InitModalIndirect(&dtl, pParent);
	ASSERT(result);

	m_editRect = editRect;
	m_text = text;
	m_sel = sel;

	return DoModal();
}


BOOL CEditPopupDlg::OnInitDialog() 
{
	CDialogEx::OnInitDialog();

	const int			dlgBorderX = GetSystemMetrics(SM_CXDLGFRAME);
	const int			dlgBorderY = GetSystemMetrics(SM_CYDLGFRAME);
	const int			buttonWidth  = 50;
	const int			buttonHeight = 16;
	const int			border = 8;
	int					width;

	//
	// Here is where we size and position our dialog.  (We do it here rather
	// than above because the units are pixels here.)
	//
	// We want the edit box for this dialog to be the same size and position
	// as the CEditPopup control's box.
	//

	width = buttonWidth*2 + border;
	if (m_editRect.Width() > width)
		width = m_editRect.Width();

	SetWindowPos(&wndTop,
  		m_editRect.left - border - dlgBorderX + 200,
		m_editRect.top - border - dlgBorderY + 50,
		width + border*2 + dlgBorderX*2,
		m_editRect.Height() + buttonHeight + border*3 + dlgBorderY*2,
		SWP_NOZORDER | SWP_SHOWWINDOW);

	m_ctlEdit.SetWindowPos(&wndTop,
		border,
		border,
		m_editRect.Width(),
		m_editRect.Height(),
		SWP_NOZORDER | SWP_SHOWWINDOW);

	m_ctlCancel.SetWindowText(_T("取消"));
	m_ctlCancel.SetWindowPos(&wndTop,
		border,
		m_editRect.Height() + border*2,
		buttonWidth,
		buttonHeight,
		SWP_NOZORDER | SWP_SHOWWINDOW);

	m_ctlOk.SetWindowText(_T("确认"));
	m_ctlOk.SetWindowPos(&wndTop,
		border*2 + buttonWidth,
		m_editRect.Height() + border*2,
		buttonWidth,
		buttonHeight,
		SWP_NOZORDER | SWP_SHOWWINDOW);

	//
	// Set text and selection.
	//
	m_ctlEdit.SetFocus();
	m_ctlEdit.SetWindowText(m_text);
	m_ctlEdit.SetSel(m_sel);

	return FALSE;  // We set the focus
}


void CEditPopupDlg::OnOk()
{
	m_ctlEdit.GetWindowText(result);

	EndDialog(IDOK);
}