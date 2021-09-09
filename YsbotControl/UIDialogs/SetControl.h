#pragma once

#include "../IO Dialog/GroupFrame.h"
#include "../BasicControls/IconComboBox.h"
#include "../BasicControls/Label.h"
#include "../BasicControls/buttons.h"
#include "../IO Dialog/StaticTransparent.h"
#include "../Keyboard/PopEdit.h"
#include "../ConfigReader.h"

#define IDC_SETTING_CONTRO               6030 
#define IDC_SETTING_CONTROL_EDIT         6031 // 6034 

class CSetControl :	public CGroupFrame
{
public:
	CSetControl(const ConfigReader& vread);
	virtual ~CSetControl(void);

	BOOL Create(CString caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd=NULL, UINT nID=65535);

protected:

	DECLARE_MESSAGE_MAP()

private:
 
	CStaticTran titleLabel[6];
	CpopEdit    editBox[4];
	 
	unsigned int m_nID;
	CFont font_;
	std::string ip_address_controler;
	std::string ip_address_teachpendant;
};

