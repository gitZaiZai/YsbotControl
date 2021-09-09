#pragma once
#define DIO_StnClicked WM_USER+1000

class CStaticBmp_Btn :public CStatic
{
	DECLARE_DYNAMIC(CStaticBmp_Btn)

protected:
	//afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()


private:

	int DIOType;
	int DIOIndex;

public:
	afx_msg void OnStnClicked();
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,int IOType=0,int IOIndex=0);


};