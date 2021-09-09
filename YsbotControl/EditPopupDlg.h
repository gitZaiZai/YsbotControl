#pragma once


// CEditPopupDlg dialog

class CEditPopupDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEditPopupDlg)

public:
	CEditPopupDlg();   // standard constructor
	virtual ~CEditPopupDlg();
	int DoModal(CWnd* pParent, CRect editRect, CString text, DWORD sel);

	CString				result;		// Edit box string, when IDOK returned

// Dialog Data
 	enum { IDD = IDD_EDITPOPUPDLG };

private:

	virtual int DoModal() { return CDialogEx::DoModal(); }
	CEdit				m_ctlEdit;
	CButton				m_ctlOk;
	CButton				m_ctlCancel;
	afx_msg void OnOk();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	CRect				m_editRect;		// Screen location of edit box
	CString				m_text;			// Initial text and selection
	DWORD				m_sel;
};
