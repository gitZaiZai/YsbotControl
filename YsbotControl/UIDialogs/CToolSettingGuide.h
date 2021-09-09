
#pragma once

#include "../IO Dialog/StaticTransparent.h"
#include "../IO Dialog/GroupFrame.h"
#include "../BasicControls/PngCheckBox.h"
#include "../Keyboard/PopEdit.h"

#include "../BasicControls/PngButton.h"

#include "../BasicControls/IconComboBox.h"
#include "../BasicControls/Label.h"
#include "../BasicControls/PngRadio.h"
#include "../CartPos.h" 

#include "../BasicControls/labels.h"
#include "../BasicControls/PngButton.h"
#include "../BasicControls/IconComboBox.h"

#define IDC_ToolCoSys_SettingGuid  4663	 
#define TOOLGUIDE_TEACH_BUTTON     4664	 
#define TOOLGUIDE_EDIT_1           4665	  
#define TOOLGUIDE_EDIT_2           4666	 
#define TOOLGUIDE_EDIT_3           4667	  
#define TOOLGUIDE_EDIT_4           4668	 
#define TOOLGUIDE_EDIT_5           4669	 
#define TOOLGUIDE_EDIT_6           4670	 

#define TOOLGUIDE_COMBO_1          4671	 
#define TOOLGUIDE_COMBO_2          4672

#define TOOLGUIDE_CHECK_1          4673	 
#define TOOLGUIDE_CHECK_2          4674


struct TeachToolPoint {
	CartPos tpos;
	int     zAssignTo;
	int     xAssignTo;
	bool    zReverse;
	bool    xReverse;
};

class CToolSettingGuide : public CGroupFrame
{
public:
	CToolSettingGuide();
	virtual ~CToolSettingGuide();

	BOOL Create(CString caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd=NULL, UINT nID=65535);
	void SetData(double x,double y,double z);
	void GetData(double* JArray);
	void SetReadOnly(BOOL bReadOnly);
	void update_guide( int iMethod, int iStep );
	void reset_all();
	void calc_reference_system ();
	const CartRef& get_refsys() { return m_toolResult; } 

protected:
 	afx_msg void OnPosTeachClicked();

	afx_msg LRESULT OnKeyboardEnter(WPARAM,LPARAM); 
	afx_msg void OnSelChangeCombo1();
	afx_msg void OnSelChangeCombo2();

	afx_msg void OnCheckClicked(UINT id);

	DECLARE_MESSAGE_MAP()

private:
	void show_check_combo(BOOL);
 	void clear_show_info();
	void show_pos_info(int posID);
	void get_desired_orientation (Matrix& matAng);
	 
	void calc_one_point_xyz ();
	void calc_four_point_xyz ();
	void calc_one_point_6D ();

	unsigned int	_nID;
	CFont			font_;
	CString			strEditTitle[6];
	BasicLabel      strUnit[6];
	CStaticTran		label_btn[6];
	CpopEditDouble	pos_edit[6];
	CButton			btn_teach;	
	TeachToolPoint  teach_result[4];

	int m_nPosType;
	int m_iMethod;
	int m_iStep;
	CartRef  m_refSysResult;

	BasicLabel    strTitle[3];
	CPngButton	  checkReverse[2];
	CLabel        axisLabel[2];

	CIconComboBox axisCombo[2];
 

	CartRef  m_toolResult;

};
