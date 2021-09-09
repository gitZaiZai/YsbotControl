#pragma once

#include "../IO Dialog/GroupFrame.h"
#include "../BasicControls/IconComboBox.h"
#include "../BasicControls/Label.h"
#include "../BasicControls/buttons.h"
#include "../IO Dialog/StaticTransparent.h"
#include "../Time.h"

#define IDC_SETTING_SYSTEM_COMBBOX         6020 

class CSetSystem :	public CGroupFrame
{
public:
	CSetSystem(void);
	virtual ~CSetSystem(void);
	BOOL Create(CString caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd=NULL, UINT nID=65535);

	void update_ui_status();

protected:

	DECLARE_MESSAGE_MAP()

private:
	CIconComboBox language_combbox;
	CStaticTran titleLabel[4];
	CLabel contentLabel[3];
	 
	unsigned int m_nID;
	CFont font_;
	CTime  timeBegin;
	Ysbot::Time checktime;
 	BOOL CheckLegalityMachine(const CString& RUN_ID);

	void CalcMachineID(CString& hardwareID);
	CString m_strMachineID;
};

