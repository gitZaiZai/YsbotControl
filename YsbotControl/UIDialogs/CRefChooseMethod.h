#pragma once
 
#include "../IO Dialog/GroupFrame.h"
#include "../BasicControls/Label.h"
#include "../BasicControls/labels.h"
#include "../BasicControls/PngRadio.h"
#include "../BasicControls/CGroupBox.h"

#define IDC_UserCoSys_ChooseMethod 4640	//4623-4631
#define REFMETHOD_RADIO_BUTTON_1   4641
#define REFMETHOD_RADIO_BUTTON_2   4642
#define REFMETHOD_RADIO_BUTTON_3   4643
#define WM_CHANGE_CALIBRATIONUSER  4644

class CRefChooseMethod : public CGroupFrame
{
public:
	CRefChooseMethod();
	virtual ~CRefChooseMethod();

	BOOL Create(CString caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd=NULL, UINT nID=65535);
    void reset();

protected:
 	afx_msg void OnRadioButtonClicked(UINT id);


	DECLARE_MESSAGE_MAP()

private:
	unsigned int _nID;
	CFont font_;
	BasicLabel  toolTitle;
 	CLabel      toolLabel;
	std::vector<CPngRadio*> radio_method_list; 
	CGroupBox	m_GroupBox;
	CGroupBox	m_GroupBoxRadio;
	int			m_nRadioType;
};