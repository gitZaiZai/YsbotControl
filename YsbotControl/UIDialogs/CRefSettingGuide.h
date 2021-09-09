
#pragma once
 
#include "../IO Dialog/StaticTransparent.h"
#include "../IO Dialog/GroupFrame.h"
#include "../BasicControls/PngCheckBox.h"
#include "../Keyboard/PopEdit.h"
 
#include "../BasicControls/PngButton.h"
 
#include "../BasicControls/IconComboBox.h"
#include "../BasicControls/Label.h"
#include "../BasicControls/labels.h"
#include "../BasicControls/PngRadio.h"
#include "../CartPos.h" 
 
#define IDC_UserCoSys_SettingGuid 4621	//4623-4631
#define REFGUIDE_TEACH_BUTTON     4622	//4623-4631
#define REFGUIDE_CHECK_BUTTON     4623	//4623-4631
#define REFGUIDE_EDIT_1           4624	//4623-4631
#define REFGUIDE_RADIO_BUTTON_1   4627
#define REFGUIDE_RADIO_BUTTON_2   4628
#define REFGUIDE_RADIO_BUTTON_3   4629

#define WM_SelChangeRefStep       4602

struct TeachPoint {
	Vec3D pos;
	int   type;
	bool  reverse;
};

class CRefSettingGuide : public CGroupFrame
{
public:
	CRefSettingGuide();
	virtual ~CRefSettingGuide();

	BOOL Create(CString caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd=NULL, UINT nID=65535);
	void SetData(double x,double y,double z);
	void GetData(double* JArray);
	void SetReadOnly(BOOL bReadOnly);
	void update_region_show( int iMethod, int iStep );
	void reset_all();

	void calc_reference_system ();
	const CartRef& get_refsys() { return m_refSysResult; } 

protected:
 	afx_msg void OnPosRadioClicked(unsigned int btnID);
	afx_msg void OnPosRevClicked();
	afx_msg void OnPosTeachClicked();
	afx_msg LRESULT OnKeyboardEnter(WPARAM,LPARAM); 

	DECLARE_MESSAGE_MAP()

private:
	unsigned int	_nID;
	CFont			font_;
	CStaticTran		label_btn[4];
	CpopEditDouble	pos_edit[3];
 	CPngButton		check_reverse;
	CButton			btn_teach;	
	TeachPoint      teach_point[3];
	BasicLabel      strUnit[3];

	std::vector<CPngRadio*> radio_pos_type;
	      
	int m_nPosType;
	int m_iMethod;
	int m_iStep;
	CartRef  m_refSysResult;

	void show_check_radio(BOOL, int diableRadioID = -1 );
	void change_radio_description(BOOL bAxis);
	void clear_show_info();
	void show_pos_info(int posID);

	void calc_coord_aixs_plane();
	void calc_coord_axes ();
	void calc_coord_orgpoint ();
};