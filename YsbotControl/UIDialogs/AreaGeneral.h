

//////////Ã∆è™è™è™è™

#pragma once


#include "../IO Dialog/StaticTransparent.h"
#include "../IO Dialog/GroupFrame.h"
#include "../BasicControls/IconComboBox.h"
#include "../BasicControls/PngButton.h"
#include "SecurArea.h"

//using namespace std;

#define IDC_SecurArea_General 5020

#define IDC_General_COMBOX_1 5021


#define WM_SELAREASHAPE 5026
#define WM_SELAREATYPE 5027


class CAreaGeneral:public CGroupFrame
{
public:
	CAreaGeneral();
	virtual ~CAreaGeneral();
	 
	BOOL Create(CString caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd=NULL, UINT nID=65535);

	void get_general_setting( SecurArea* );
	void update_general_show( const SecurArea& );

protected:

	DECLARE_MESSAGE_MAP()
 	//afx_msg void OnCbnSelchange();
	afx_msg void OnCbnSelShape();
	afx_msg void OnCbnSelType();

private:
	unsigned int _nID;
	CFont font_;
	CStaticTran AreaLabel[7];
	CString textStr[7]; 
	CIconComboBox Area_cmb[5];
	CPngButton emgc_check;
	CPngButton Auto_active;
	//CButton NextPage;




	// void AdditionalSetting();
	//void BasicSetting();
};