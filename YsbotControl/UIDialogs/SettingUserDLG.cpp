#include "stdafx.h"
#include "SettingUserDLG.h"
#include "../colourcode.h"
#include "../WorldModel.h"
#include "../MotionControl.h"
#include "../LuaScript/ScriptVariables.h"
#include "../stringconvert.h" 
#include "state_dialog.h"


using namespace std;

CSettingUserDLG*	  m_pwndDlgSettingUser = NULL;

 
BEGIN_MESSAGE_MAP(CSettingUserDLG, DialogType)
	ON_MESSAGE(WM_BTN_USER_LOGIN, &OnButtonLogin )	
	ON_MESSAGE(WM_BTN_USER_MANAGE, &OnButtonManage )	
	ON_MESSAGE( WM_POPUPBTN_YESNO, &OnButtonPopupYesNo )	

END_MESSAGE_MAP()

CSettingUserDLG::CSettingUserDLG( const ConfigReader& vr,
	HBITMAP hBmpScrollBar, 
	HBITMAP hBmpPlus, 
	HBITMAP hBmpMinus ) : m_hBmpScrollBar(hBmpScrollBar),
	m_hBmpPlus(hBmpPlus), m_hBmpMinus(hBmpMinus)
{
	for ( int i = 0; i < ARRAY_SIZE(pSetBoard); ++i )
	{
		pSetBoard[i] = NULL;
	}
	pSetUserPopup = NULL;
 
	pSetBoard[0] = new CSetLogin();
	pSetBoard[1] = new CSetSystem();
	pSetBoard[2] = new CSetControl(vr);
	pSetBoard[3] = new CSetDisplay(vr);
	pSetBoard[4] = new CSetUserManage(m_hBmpScrollBar,m_hBmpPlus,m_hBmpMinus);
}

CSettingUserDLG:: ~CSettingUserDLG()
{
	for ( int i = 0; i < ARRAY_SIZE(pSetBoard); ++i )
	{
		delete pSetBoard[i];
		pSetBoard[i] = NULL;
	}

	if ( pSetUserPopup && pSetUserPopup->GetSafeHwnd() ) 
	{
		pSetUserPopup->DestroyWindow();
		delete pSetUserPopup;
		pSetUserPopup = NULL;
	}
 
}
 
LRESULT CSettingUserDLG::OnMainButtonClicked( WPARAM wp, LPARAM lp)
{
	int btnid  = (int)(wp);
	int menuid = (int)(lp);

	UserData currUserData;
 
	switch(btnid)
	{
	case 0: update_ui_status(winPageSetting);	break;
	case 1:	update_ui_status(winPageUserManage);	break;
	case 2:		break;
	case 3:     break;
	case 4: 	break;
	case 5:		break;
	default:
		break;
	}
 
	return 1;
}

BOOL CSettingUserDLG::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	CRect rect;
	GetClientRect(&rect);

	if ( GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetSafeHwnd() )
	{
		GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetClientRect(&clientRect);
		rect = clientRect;
		SetWindowPos(NULL,clientRect.left, clientRect.top, clientRect.Width(), clientRect.Height(), SWP_NOZORDER);
	}

	SetBackgroundColor(kColor[8], TRUE);
 
	CRect rectBoard;
	for ( int i = 0; i < ARRAY_SIZE(pSetBoard); ++i )
	{
		if ( i == ARRAY_SIZE(pSetBoard) - 1 )
 			rectBoard = rect;
 		else
			rectBoard = CRect( CPoint(i%2*(rect.Width()/2+4), (int)(i/2)*( rect.Height()/2+3) ), CSize( rect.Width()/2,rect.Height()/2) );

 		pSetBoard[i]->Create( _T(""), WS_CHILD| SS_NOTIFY, rectBoard, this );
		pSetBoard[i]->SetFont(20,_T("ËÎÌו"));
	}

	pSetUserPopup = new CSettingUserPopup();
 	pSetUserPopup->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE,this);

	return FALSE;
}

int CSettingUserDLG::OnActivateChildWin( LPARAM lp )
{
	update_ui_status(winPageSetting);
	((CSetUserManage*)(pSetBoard[4]))->load_user_data(); 
    UpdateSurface();

	return 1;
}

void CSettingUserDLG::UpdateSurface()
{
 	CRect (rc);
// 	GetWindowRect(rc);
// 	RedrawWindow();

	for ( int i = 0; i < ARRAY_SIZE(pSetBoard); ++i )
	{
		pSetBoard[i]->Invalidate();
		pSetBoard[i]->RedrawWindow();
	}

	ScreenToClient(rc);
	InvalidateRect(rc,TRUE);
	UpdateWindow();

}

LRESULT CSettingUserDLG::OnButtonLogin(WPARAM wp, LPARAM lp)
{
	UserData logUser;
	CString strName = *(CString*)(wp);
 	sprintf(logUser.strUserName,"%s",Unicode2Ansi(strName.GetBuffer(0)).c_str());
	 
	pSetUserPopup->activate_settingUserPopup( GetSafeHwnd(), popLoginUser, logUser );

	return 0;
}

LRESULT CSettingUserDLG::OnButtonManage(WPARAM wp, LPARAM lp)
{
	UserData user;
	user.nUserID = (int) wp;
	
	if ( (UserPopupType)(lp) < popCount ) // edit or creat
	{
		pSetUserPopup->activate_settingUserPopup( GetSafeHwnd(), (UserPopupType)(lp), user );
	}
	else // delete user
	{
		((CSetLogin*)(pSetBoard[0]))->del_user_combbox(user.nUserID);
	}

	return 0;
}


LRESULT CSettingUserDLG::OnButtonPopupYesNo( WPARAM wp, LPARAM lp)
{
	if ( (UserPopupType)(lp) == popLoginUser )
	{
		pSetBoard[0]->SendMessage( WM_POPUPBTN_YESNO, wp, lp ); 
 	}
	else if (  (UserPopupType)(lp) == popEditUser )
	{
		//pSetBoard[0]->SendMessage( WM_POPUPBTN_YESNO, wp, lp ); 
 	}
	else if ( (UserPopupType)(lp) == popCreateUser )
	{
		pSetBoard[4]->SendMessage( WM_POPUPBTN_YESNO, wp, lp ); 

		UserData user = *((UserData*)(wp));
		((CSetLogin*)(pSetBoard[0]))->add_user_combbox(user);

 	}
 
	return 1;
}

void CSettingUserDLG::update_ui_status( int nWin )
{
	BOOL bShowUserManage = (nWin == winPageUserManage);
	pSetBoard[4]->ShowWindow(bShowUserManage);
	for ( int i = 0; i < 4; ++i )
	{
		pSetBoard[i]->ShowWindow(!bShowUserManage);
	}
}

void CSettingUserDLG::update_userdlg_show()
{
	((CSetSystem*)(pSetBoard[1]))->update_ui_status();

}