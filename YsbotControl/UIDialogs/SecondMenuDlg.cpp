#include "stdafx.h"
#include "SecondMenuDlg.h"
#include "../colourcode.h"
#include "../Angle.h"
#include "../control_id.h"
#include "../resource.h"
#include "../WorldModel.h"


CSecondMenuDlg::CSecondMenuDlg( )
{
    second_button_list.clear();
}

CSecondMenuDlg::~CSecondMenuDlg() 
{
	for ( unsigned int i =0; i < second_button_list.size(); ++i )
	{
		SAFE_DELETE(second_button_list[i]);
	}
	second_button_list.clear();
}

BEGIN_MESSAGE_MAP(CSecondMenuDlg, DialogType)
	ON_COMMAND_RANGE( IDC_FIRSTMENU_BTN1, IDC_FIRSTMENU_BTN7, &CSecondMenuDlg::OnSecondButtonClicked )

END_MESSAGE_MAP()

void CSecondMenuDlg::DoDataExchange(CDataExchange* pDX) 
{

}

BOOL CSecondMenuDlg::OnInitDialog() 
{
	CDialogEx::OnInitDialog();

 	if ( GetParent()->GetDlgItem(IDC_BOX_SECONDMENU)->GetSafeHwnd() )
	{
		GetParent()->GetDlgItem(IDC_BOX_SECONDMENU)->GetClientRect(&clientRect);
	}

	SetBackgroundColor( kColor[0], TRUE);

	creat_second_button();

	return FALSE;
}

void CSecondMenuDlg::creat_second_button()
{
	int nRow = 4;
	int nCol = 6;
	int iSpaceY = 10;
	int iSpaceX = 10;
	int xsize = (int)((clientRect.Width()-(nCol+1)*iSpaceX)/nCol);
	int ysize = (int)(clientRect.Height()/nRow - iSpaceY);

//  
// 	CRect rctBtn;
// 	rctBtn.SetRectEmpty();
//     for (unsigned int i = 0; i < main_button_list.size(); ++i )
//     {
// 		int nLine = (int)(i/nCol);
// 		if ( nLine == 0 )
// 		{
// 			rctBtn = CRect(CPoint( iSpaceX + (xsize+iSpaceX)*i, iSpaceY),CSize(xsize,ysize));
//  		}
//         else
// 		{
// 			rctBtn = CRect(CPoint( iSpaceX + (xsize+iSpaceX)*(i%nCol), iSpaceY + (ysize+iSpaceY)*nLine),CSize(xsize,ysize));
// 		    
// 		}
// 		main_button_list[i] = new CButtonST();
// 		main_button_list[i]->Create( _T(""), WS_VISIBLE| WS_CHILD | WS_TABSTOP, rctBtn,this, IDC_FIRSTMENU_BTN1+i );
// 		main_button_list[i]->SetDefaultColors();
// 		//main_button_list[i]->SetIcon(IDI_MAIN_HOME_BTN,(int)BTNST_AUTO_DARKER);//
// 		main_button_list[i]->DrawTransparent(TRUE);
// 		main_button_list[i]->DrawBorder(FALSE);
// 		main_button_list[i]->SetFlat(FALSE);
// 		
// 		CString msg;
// 		msg.Format(_T("btn _%d"),i);
// 		main_button_list[i]->SetWindowText(msg);
//     }
}
 
void CSecondMenuDlg::OnSecondButtonClicked(UINT id) 
{
	if ( MWM.is_popup_window_active() )
		return;

	unsigned int bnt_index = id - IDC_FIRSTMENU_BTN1;
	printf("\n main btn %d clicked ! \n",bnt_index);
	 
}