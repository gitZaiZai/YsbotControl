#pragma once


#include "../IO Dialog/GroupFrame.h"
#include "../BasicControls/PngButton.h"
#include "../BasicControls/labels.h"
#define IDC_TEACHDLG_MOVETYPE 4128 //4128-4130
#define IDB_TEACHDLG_MOVETYPE 4131//4131-4132

enum movetype{ MovePTP=0, MoveLine=1 };

//运动类型选择框
class CMoveType: public CGroupFrame
{
public:
	CMoveType();
	virtual ~CMoveType();

	BOOL Create(CString caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd=NULL, UINT nID=65535);
	void SetMoveType(movetype mTpye);
	void Enable (bool b);
	unsigned int GetMoveType();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBtnClicked(unsigned int btnID);
	DECLARE_MESSAGE_MAP()

private:
	BasicLabel label_[2];
	CPngButton btnMoveTpye[2];
	 
	unsigned int _nID;

};