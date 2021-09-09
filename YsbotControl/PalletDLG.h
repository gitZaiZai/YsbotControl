
#pragma once
#include "UIDialogs/DialogType.h"
#include "IO Dialog/StaticTransparent.h"
#include "IO Dialog/GroupFrame.h"
#include <vector>

#define IDD_PalletDLG 4650




class PalletDLG :public DialogType
{
public:
	PalletDLG();
	virtual ~PalletDLG();
	//BOOL CreateModeDlg(CString caption, CRect rect, BOOL mode = TRUE,
	//	CWnd *parent = NULL);	


protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnMainButtonClicked(WPARAM,LPARAM); 

public:



private:


};
extern PalletDLG *pallet_DLG;