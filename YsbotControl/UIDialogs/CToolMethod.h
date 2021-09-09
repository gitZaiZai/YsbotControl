#pragma once

#include "../IO Dialog/GroupFrame.h"
#include "../BasicControls/Label.h"
#include "../BasicControls/labels.h"
#include "../BasicControls/PngRadio.h"
#include "../BasicControls/CGroupBox.h"

#define IDC_ToolCoSys_ChooseMethod  4680	//4623-4631
#define TOOLMETHOD_RADIO_BUTTON_1   4681
#define TOOLMETHOD_RADIO_BUTTON_2   4682
#define TOOLMETHOD_RADIO_BUTTON_3   4683
#define WM_CHANGE_TOOLMETHOD        4684

class CToolMethod : public CGroupFrame
{
public:
	CToolMethod();
	virtual ~CToolMethod();

	BOOL Create(CString caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd=NULL, UINT nID=65535);
	void reset();

protected:
 	afx_msg void OnRadioButtonClicked(UINT id);


	DECLARE_MESSAGE_MAP()

private:
	unsigned int _nID;
	CFont font_;
	BasicLabel  toolTitle[2];
 
	std::vector<CPngRadio*> radio_method_list; 
 
	int			m_nRadioType;
};