#include "stdafx.h"
#include "PalletDLG.h"

PalletDLG *pallet_DLG;


////PalletDLG class
BEGIN_MESSAGE_MAP(PalletDLG, DialogType)


END_MESSAGE_MAP()

PalletDLG::PalletDLG()
{

}
PalletDLG:: ~PalletDLG()
{

}

void PalletDLG::DoDataExchange(CDataExchange* pDX) 
{

}

LRESULT PalletDLG::OnMainButtonClicked( WPARAM wp, LPARAM lp)
{
	int btnid  = (int)(wp);
	int menuid = (int)(lp);

	switch(btnid)
	{
	case 0:
		MessageBox(_T("test pallet dialog"));
		break;
	case 1:

		break;
	case 2:

		break;
	case 3:

		break;
	case 4:

		break;
	case 5:

		break;

	default:
		break;
	}
	return 1;
}

BOOL PalletDLG::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	SetWindowPos(NULL,g_x0 + 22, 95, 724, 439, SWP_NOZORDER);
	//	SetBackgroundColor(kColor[8], TRUE);

	//³õÊ¼»¯¿Ø¼þ


	return TRUE;
}