#pragma once
#include "../IO Dialog/StaticTransparent.h"
#include "../GroupProperties.h"
#include "../Keyboard/digital_keyboard.h"
#include "../stringconvert.h"
#include "../Keyboard/PopEdit.h"
//#include "full_qwerty.h"

//#define MotorTypeListString _T("Dummy,delta-A2,Maxsine,Nidec,Sanyo-R1,Sanyo-R2,Yaskawa")
#define MotorTypeListString _T("Dummy,Maxsine,Nidec,")


class CMechRow : public CStaticTran
{
	DECLARE_DYNAMIC(CMechRow)
public:
	virtual ~CMechRow();
	CMechRow();
//	virtual void Initialize(LPCTSTR lpszText,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	BOOL Create(LPCTSTR lpszText,DWORD dwStyle=WS_CHILD|WS_VISIBLE, const RECT& rect=CRect(1,1,600,30), CWnd* pParentWnd=NULL, UINT nID=65535);
	void SetRowFont(int fontCB_Height=0,int fontEdit_Height=0,CString fontStr=_T("arial"));//设置字体高度,字体
	
	void SetMotorTypeList(CString str);
 	void SetRowData(const MECH_PARAM RowMP);
	MECH_PARAM GetRowData(void);
	void SetKB( DigitalKeyboard* kb);

	CString RowStr;
	CStatic RowTitle;
	CComboBox EncoderDir;
	CComboBox Reverse;
	CpopEdit RPM;
	CpopEdit PPR;
	CpopEdit GearRatio;
	CpopEdit HighLimit;
	CpopEdit Low_Limit;
	CComboBox MotorType;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnEditClicked(UINT ctlID);
	DECLARE_MESSAGE_MAP()


private:
	int RowCtlID;
	int RowHeight;
	int ColWidth;
	int TitleWidth;
	CRect RowRect;
	CFont font_edit,font_combobox,font_title;


	//MECH_PARAM rowMP;
	MECH_PARAM mp;
	 

};

//////////////////////////////////////////////////////////////////////////

class CMechGrid : public CStaticTran
{
	DECLARE_DYNAMIC(CMechGrid)
public:
	virtual ~CMechGrid();
	CMechGrid();
	//	virtual void Initialize(LPCTSTR lpszText,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	BOOL Create(GroupProperties *GP,LPCTSTR lpszText,DWORD dwStyle=WS_CHILD|WS_VISIBLE, const RECT& rect=CRect(1,1,600,30), CWnd* pParentWnd=NULL, UINT nID=65535);
	void SetGridFont(int fontCB_Height=0,int fontEdit_Height=0,CString fontStr=_T("arial"));//设置字体高度,字体
	void SaveMechGridData();
	void RecoverMechGridData();
	void SetKB(DigitalKeyboard* kb);

	CMechRow *mRow;
	CStatic title[9];

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()


private:
	int GridCtlID;
	int GridHeight;
	int GridWidth;
	int RowHeight;
	int ColWidth;
	int TitleWidth;
	MECH_PARAM mp;
	CFont font_left,font_top;
	CRect GridRect;
	GroupProperties *gp;

// 	DigitalKeyboard *KB_;

	CString ListName[12];
	void StringToArray();
	int CheckIndexOfArray(CString* Ptr,unsigned int Num,CString str);
	void NameToID();
	void IDtoName();

};