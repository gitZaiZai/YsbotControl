#include "StdAfx.h"
#include "SetLogin.h"
#include "../colourcode.h"
#include "UserData.h"
#include "state_dialog.h"
#include "../WorldModel.h"
#include "../stringconvert.h"
#include "SettingUserPopup.h"
#include "SqlDatabase.h"

#define DEBUG_LOGIN_USER  (1)

BEGIN_MESSAGE_MAP(CSetLogin, CGroupFrame)
	ON_BN_CLICKED( IDC_SETTING_LOGIN_BTN, OnButtonClicked )
	ON_MESSAGE( WM_POPUPBTN_YESNO, &OnButtonPopup )	

END_MESSAGE_MAP()

CSetLogin::CSetLogin(void)
{
	caption_default = _T("µÇÂ¼");
}
 
CSetLogin::~CSetLogin(void)
{
	font_.DeleteObject();
}

BOOL CSetLogin::Create(CString caption,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CStatic::Create( caption.IsEmpty() ? caption_default : caption, dwStyle|SS_NOTIFY, rect, pParentWnd, nID );
 
	m_nID = nID;
 
	font_.CreateFont(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("Arial"));

	LOGFONT lf;
 	font_.GetLogFont(&lf);

	CRect clientRect;
	GetClientRect(&clientRect);
	int widthBtn  = 150;
	int heightBtn = 50;
	int halfWidthClient = clientRect.Width()/2-5;

	labels[0].Create(_T("ÓÃ»§"),WS_CHILD|WS_VISIBLE,CRect( CPoint(10,35), CSize(50,30) ),this);
	labels[0].SetFont(22,_T("ËÎÌå"));
	labels[1].Create(_T("È¨ÏÞ"),WS_CHILD|WS_VISIBLE,CRect( CPoint(10,100), CSize(50,30) ),this);
	labels[1].SetFont(22,_T("ËÎÌå"));
 
	CRect rectPos( CPoint(halfWidthClient,30), CSize(halfWidthClient,170) );
	user_combbox.SetSort(false);
	user_combbox.SetShowImage(FALSE);
	user_combbox.Create( WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED |CBS_HASSTRINGS,rectPos,this, IDC_SETTING_LOGIN_COMBBOX );
	user_combbox.set_custom_message(FALSE);
    user_combbox.AddString(_T("Administrator"));
	sql::string strValName = "";
	sql::integer gradeVal = 0;
	sql::integer nKey = 0 ;
	CString levelStr;

	SQL.getTable(TBLUser)->open();
	for (int index = 0; index < SQL.getTable(TBLUser)->recordCount(); index++)
	{
		if ( sql::Record* record = SQL.getTable(TBLUser)->getRecord(index) )
		{
			strValName = record->getValue(SQL.getFieldSet(TBLUser)->getByIndex(1)->getName())->asString();
			nKey =  record->getValue(SQL.getFieldSet(TBLUser)->getByIndex(0)->getName())->asInteger();
 			user_combbox.AddString( Ansi2Unicode(strValName).c_str(), (DWORD)nKey );
 		}
	}

	user_combbox.SetCurSel(0);

 	userGradeNum.Create( _T(""), WS_CHILD | WS_VISIBLE | DT_RIGHT | SS_CENTERIMAGE,CRect( CPoint(300,100), CSize(50,30) ), this );
	userGradeNum.SetFont(lf);
  	userGradeNum.SetBkColor(kColor[8]);
	

	btn_login.Create(_T("µÇÂ¼"), WS_CHILD | WS_VISIBLE,CRect( CPoint(halfWidthClient,150), CSize(halfWidthClient,46) ), this, IDC_SETTING_LOGIN_BTN );
	btn_login.SetFont(&font_);
	btn_login.SetFaceColor(kColor[8]);

	// for test
#if  DEBUG_LOGIN_USER
	UserData logUser;
	logUser.nUserID = 0;
	logUser.nState = 1;
	logUser.nGrade = 16;
	logUser.administrator = true;
	sprintf(logUser.strUserName,"%s","Administrator");
	userGradeNum.SetText(_T("16"));
	MWM.set_userdata( logUser );  
#endif 
	

	return TRUE;
}

void CSetLogin::OnButtonClicked()
{
	CString btnString;
	btn_login.GetWindowText(btnString);
	// ×¢ÏúµÇÂ¼
	if ( btnString.Compare(_T("×¢Ïú")) == 0 )
	{
		UserData logUser;
		logUser.nUserID = 0;
		logUser.nState = 0;
		logUser.nGrade = 1;
		logUser.administrator = false;
		sprintf(logUser.strUserName,"%s","Administrator");
		MWM.set_userdata( logUser );
		btn_login.SetWindowText(_T("µÇÂ¼"));
		user_combbox.EnableWindow(TRUE);
		set_grade( 1 );

		return;
	}

	// µÇÂ¼²Ù×÷
	CString strName  = _T("");
	int nIndex = user_combbox.GetCurSel();
	if ( nIndex < 0  )
	{
		strName = _T("Administrator");
		MSD.ShowInfo(_T("log system with Administrator."));
 	}
	else
		user_combbox.GetLBText( nIndex, strName );

	GetParent()->SendMessage( WM_BTN_USER_LOGIN, (WPARAM) &strName );

	//logUser.strUserName
	cout << "login system\n";
}

LRESULT CSetLogin::OnButtonPopup(WPARAM wp, LPARAM lp)
{
	UserData logUser  = *(UserData*)(wp);
  
	if ( logUser.nState == 1 ) // if login success
	{
		set_grade( logUser.nGrade );
		btn_login.SetWindowText( _T("×¢Ïú") );
 		CString strName = Ansi2Unicode(logUser.strUserName).c_str();
		user_combbox.SelectString( 0, strName );
		user_combbox.EnableWindow(FALSE);
	}
	 

	return 0;
}

void CSetLogin::set_grade(int ng)
{
	CString strGrade  = _T("");
	strGrade.Format(_T("%d"),ng);
	userGradeNum.SetText(strGrade);

}

void CSetLogin::add_user_combbox(const UserData& user)
{
 	CString userName = Ansi2Unicode(user.strUserName).c_str();
	
	user_combbox.AddString( userName, (DWORD)user.nUserID );
}

void CSetLogin::del_user_combbox(int nKeyPri)
{
	DWORD nkey = 0;
	for ( int i = 0; i < user_combbox.GetCount(); ++i )
	{
		nkey = user_combbox.GetItemData(i);
		if ( nkey == nKeyPri )
		{
			user_combbox.DeleteString(i);
			return;
 		}
	}
}