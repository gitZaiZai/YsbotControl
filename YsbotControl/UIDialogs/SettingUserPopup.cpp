#include "stdafx.h"
#include "SettingUserPopup.h"
#include "../colourcode.h"
#include "../Angle.h"
#include "../stringconvert.h"
#include "../WorldModel.h"
#include "state_dialog.h"
#include "SqlDatabase.h"
#include "../resource.h"
#include <regex>

BEGIN_MESSAGE_MAP(CSettingUserPopup, DialogType)
	ON_BN_CLICKED(IDC_SETTING_POPUP_BUTTON, BtnCancel)
	ON_BN_CLICKED(IDC_SETTING_POPUP_BUTTON+1, BtnOK)
END_MESSAGE_MAP()

CSettingUserPopup::CSettingUserPopup() 
{
	m_nParentWinType = popLoginUser;
	m_nPrimaryKeyEdit = 0;
}

CSettingUserPopup::~CSettingUserPopup() 
{
	font_.DeleteObject();
}
  
BOOL CSettingUserPopup::OnInitDialog() 
{
	font_.CreateFont(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("Arial"));

	CRect rect;
	GetParent()->GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetWindowRect(&rect);
 	int x0 = rect.left + rect.Width() / 5;
// 	int y0 = rect.top +	rect.Height() / 2 - 150;
	int y0 = rect.top;
	int width0 = (int)(rect.Width() * 0.6);
	int height0 = rect.Height() - 10;
	SetWindowPos(NULL, x0, y0, width0, height0, SWP_NOZORDER);

	/** ���ñ���ɫ, �ڲ���ϵͳ�Ĳ�ͬ��������£��Ի������ű䣬��������������һ���Է����ű�**/
	SetBackgroundColor(RGB(239, 239, 239));
	title_.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_NOTIFY,
		CRect(0, 0, width0, 30), this, IDC_SETTING_POPUP);
	title_.SetBkBmp(_T(".\\res\\01.bmp"));
	title_.Draw(_T("��������"));
// 	rect.SetRectEmpty(); // �����϶���Ϣ��ʾ�Ի���
	title_.set_custom_font(TRUE);
	title_.SetArea(rect);
 
	CRect clientRect;
	GetClientRect(&clientRect);
	int widthBtn  = 150;
	int heightBtn = 50;
	int halfWidthClient = clientRect.Width()/2;
	button_[0].Create(_T(""), WS_CHILD | WS_VISIBLE, CRect( CPoint(halfWidthClient-widthBtn, clientRect.bottom-heightBtn-10), CSize(widthBtn,heightBtn) ), this, IDC_SETTING_POPUP_BUTTON);
	button_[0].SetFaceColor(kColor[8]);
 	button_[0].SetMyIcon( _T(".\\res\\cancel.ico")  );

	button_[1].Create(_T(""), WS_CHILD | WS_VISIBLE, CRect( CPoint(halfWidthClient,clientRect.bottom-heightBtn-10), CSize(widthBtn,heightBtn) ), this,	IDC_SETTING_POPUP_BUTTON+1);
	button_[1].SetFaceColor(kColor[8]);
	button_[1].SetMyIcon( _T(".\\res\\checkmark.ico")  );
    
	//////////////////////////////////////////////////////////////////////////
	const LPCTSTR strTitle[] = 
	{
         _T("�û�"),
		 _T("����"),
		 _T("ȷ��"),
         _T("����"),
		 _T("����"),
		 _T("����Ա"),

	};

	for ( int i = 0; i < ARRAY_SIZE(textLabel); ++i )
	{
		textLabel[i].Create(strTitle[i],WS_CHILD|WS_VISIBLE, CRect( CPoint(10, 50 * ( i + 1 ) ), CSize(60,30) ),this, IDC_SETTING_POPUP_TEXT+i);
 		textLabel[i].SetFont(22);
 	}

	for ( int i=0; i<ARRAY_SIZE(editBox); i++ )
	{
 		editBox[i].Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_RIGHT | (i==0 ? 0:ES_PASSWORD),
			CRect( CPoint(halfWidthClient, 50 * ( i + 1 ) ), CSize(halfWidthClient-5,30) ),this, IDC_SETTING_POPUP_EDIT+i,TRUE );
		editBox[i].SetFont(&font_);
		editBox[i].SetAllowPopupNested(TRUE);
  	}
	for ( int i=0; i<ARRAY_SIZE(paraCombbox); i++ )
	{
		paraCombbox[i].SetSort(false);
		paraCombbox[i].Create( WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED |CBS_HASSTRINGS,
			CRect( CPoint(halfWidthClient, 50 * ( i + 4 ) ), CSize(halfWidthClient-5,170) ),this, IDC_SETTING_POPUP_COMBBOX+i );
		paraCombbox[i].set_custom_message(FALSE);
		paraCombbox[i].SetShowImage(FALSE);
		paraCombbox[i].SetAllowPopupNested(TRUE);
	}	
	
	CString strItem;
	for ( int i = 0; i < 16; ++i )
	{
		strItem.Format(_T("%d"),i+1);
		paraCombbox[0].AddString(strItem,i+1);
  	}
	paraCombbox[1].AddString(_T("���ģ����壩"));
	paraCombbox[0].SetCurSel(0);
	paraCombbox[1].SetCurSel(0);
	paraCombbox[1].EnableWindow(FALSE);

	// 	admin_check.Create(_T(""),WS_CHILD|WS_VISIBLE|WS_GROUP ,CRect( CPoint(halfWidthClient, 50 * 6 ),CSize(30,32)),this,IDC_SETTING_POPUP_CHECKBOX); 
	// 	admin_check.loadBmp(_T("res/unchecked.bmp"),_T("res/checked.bmp"),_T("res/disablecheck.bmp"),_T("res/disableUncheck.bmp"));


	admin_check.Create(_T(""),WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX,
		CRect( CPoint(halfWidthClient, 50 * 6 ),CSize(30,32)),this,IDC_SETTING_POPUP_CHECKBOX);

	admin_check.Init(IDB_PNG_UNCHECK, IDB_PNG_CHECKED, IDB_PNG_DISABLECHECK, IDB_PNG_DISABLEUNCHECK );  
	admin_check.SetBackColor(RGB(0, 200, 200));  
	admin_check.SetFontType(16, L"����");  
	admin_check.SetTextColor(RGB(0, 200, 0), RGB(100, 100, 0), TRUE);  
	 
	return TRUE;
}

void CSettingUserPopup::BtnCancel()
{
	ShowWindow(SW_HIDE);
}

void CSettingUserPopup::BtnOK()
{
	CString userName = _T("");
	CString userPwod = _T("");
	editBox[0].GetWindowTextW(userName);
	editBox[1].GetWindowTextW(userPwod);

	UserData userdata;
	bool bSuccessLogin = false;
	if ( m_nParentWinType == popLoginUser )
	{
	     if ( userName == _T("Administrator")  )
	     {
			 if( userPwod.Compare(_T("ys")) == 0 )
			 {
				 userdata.administrator = true;
				 userdata.nGrade  = 16;
				 userdata.nUserID = 0;
				 bSuccessLogin = true;
			 }
			 else
			 {
			     MSD.ShowInfo(_T("���벻��ȷ"));
			 }
	     } 
		 else//  ���ݲ��ҵ�ǰ�û���Ϣ������ƥ��
		 {
			 sql::Record rcord(SQL.getFieldSet(TBLUser));
			 rcord.setInteger(5,1);
			 rcord.setString(1,Unicode2Ansi(userName.GetBuffer(0)).c_str());
			 rcord.setString(2,Unicode2Ansi(userPwod.GetBuffer(0)).c_str());

			 int nIndex = SQL.check_equal(TBLUser,&rcord);
			 if ( nIndex >= 0 )
			 {
				 sql::Record* record = SQL.getTable(TBLUser)->getRecordByKeyId(nIndex);
				 userdata.nGrade = (int)record->getValue(SQL.getFieldSet(TBLUser)->getByIndex(3)->getName())->asInteger();
				 userdata.administrator = (int)record->getValue(SQL.getFieldSet(TBLUser)->getByIndex(4)->getName())->asBool();
				 userdata.nUserID = (int)record->getValue(SQL.getFieldSet(TBLUser)->getByIndex(0)->getName())->asInteger();
				 bSuccessLogin = true;
			 } 
			 else
			 {
				 MSD.ShowInfo(nIndex == -1 ? _T("�û�������"):_T("�������"),1); 
			 }
		 }

		 if ( bSuccessLogin )
		 {
			 userdata.nState  = 1; 
			 sprintf(userdata.strUserName,"%s",Unicode2Ansi(userName.GetBuffer(0)).c_str());
			 MWM.set_userdata( userdata );
			 for ( int i = 0; i < 2; ++i )
			 {
				 editBox[i].SetWindowText(_T(""));
			 }
			 ShowWindow(SW_HIDE);
			 ::SendMessage( m_hwndResponse, WM_POPUPBTN_YESNO, (WPARAM)&userdata, m_nParentWinType );
		 }
	}
	else if ( m_nParentWinType == popCreateUser )
	{
 		int nRet = get_input_userdata(&userdata);
		if ( nRet == 0 )
		{
			sql::Record rcord(SQL.getFieldSet(TBLUser));
			rcord.setString(1, userdata.strUserName );
			rcord.setString(2, userdata.strPassWord );
			rcord.setInteger(3,userdata.nGrade);
			rcord.setBool(4,userdata.administrator);
			rcord.setInteger(5,1);
			int nReturn = SQL.insert_record(TBLUser,&rcord);
			if ( nReturn >= 0 )
			{
				MSD.ShowInfo(_T("�����û��ɹ���"));
				ShowWindow(SW_HIDE);
				 
				userdata.nUserID = SQL.find_primary_key(TBLUser,&rcord);
				::SendMessage( m_hwndResponse, WM_POPUPBTN_YESNO, (WPARAM)&userdata, m_nParentWinType );
 			}
			else
			{ 
				if ( nReturn == -1 )
				{
					MSD.ShowInfo(_T("�û��������ã�"));
 				}
				else
					MSD.ShowInfo(_T("�����û�ʧ�ܣ�"));
			}
		}
	}
	else if ( m_nParentWinType == popEditUser )
	{
		int nRet = get_input_userdata(&userdata);
		if ( nRet == 0 )
		{
			sql::Record* record = SQL.getTable(TBLUser)->getRecordByKeyId(m_nPrimaryKeyEdit);
			if ( record )
			{
				record->setString(1, userdata.strUserName );
				record->setString(2, userdata.strPassWord );
				int nReturn = SQL.modify_record(TBLUser,record);
				if ( nReturn >= 0 )
				{
					MSD.ShowInfo(_T("�û���Ϣ�޸ĳɹ���"));
					ShowWindow(SW_HIDE);
 				}
				else
				{ 
					if ( nReturn == -1 )
					{
						MSD.ShowInfo(_T("�û������ڣ�"));
					}
					else
					{
						MSD.ShowInfo(_T("�û���Ϣ�޸�ʧ�ܣ�"));
					}
 				}			
			}
			else
			{
				MSD.ShowInfo(_T("�û������ڣ�"));
			}
 		}
	}
	else
	{
		;
	}
}

void CSettingUserPopup::activate_settingUserPopup( HWND hwnd, UserPopupType winType, const UserData& udata  )
{
	ShowWindow( SW_SHOW );
	show_child_controls( winType );
	m_hwndResponse = hwnd;
	m_nParentWinType = winType;

	if ( winType == popLoginUser )
	{
		title_.set_text(_T("��¼�û�"));
		editBox[0].SetText( Ansi2Unicode( udata.strUserName ).c_str() );
		editBox[1].SetText(_T("")); // clear password
	}
	else if ( winType == popEditUser )
	{
		title_.set_text(_T("�༭�û�"));
		m_nPrimaryKeyEdit = udata.nUserID;

		UserData userdataTemp;
 		sql::Record* record = SQL.getTable(TBLUser)->getRecordByKeyId(udata.nUserID);
		sql::string strUserName = record->getValue(SQL.getFieldSet(TBLUser)->getByIndex(1)->getName())->asString();
		sql::string strPassword = record->getValue(SQL.getFieldSet(TBLUser)->getByIndex(2)->getName())->asString();
		sprintf(userdataTemp.strUserName,"%s",strUserName.c_str() );
		sprintf(userdataTemp.strPassWord,"%s",strPassword.c_str() );
 		userdataTemp.nGrade = (int)record->getValue(SQL.getFieldSet(TBLUser)->getByIndex(3)->getName())->asInteger();
		userdataTemp.administrator = (int)record->getValue(SQL.getFieldSet(TBLUser)->getByIndex(4)->getName())->asBool();

		editBox[0].SetText( Ansi2Unicode( userdataTemp.strUserName ).c_str() );
		editBox[1].SetText(_T("************")); // clear password
		editBox[2].SetText(_T("************")); // clear password

		CString strGrade;
		strGrade.Format(_T("%d"),userdataTemp.nGrade);
		paraCombbox[0].SelectString(0,strGrade); // clear password
		admin_check.SetChecked(userdataTemp.administrator ? TRUE : FALSE );


 	}
	else if ( winType == popCreateUser )
	{
		title_.set_text(_T("�����û�"));
		editBox[0].SetText(_T(""));
		editBox[1].SetText(_T("")); // clear password
		editBox[2].SetText(_T("")); // clear password
		paraCombbox[0].SetCurSel(0);
		admin_check.SetChecked(FALSE);
 	}
	else
	{
		title_.set_text(_T("ɾ���û�"));

	}
 

	BringWindowToTop();
}

void CSettingUserPopup::close_settingUserPopup()
{
	m_hwndResponse = NULL;

	 

	ShowWindow(SW_HIDE);

}

void CSettingUserPopup::show_child_controls(UserPopupType uType)
{
	 struct{
		 int nItemID;
		 BOOL bStatusAtUserPopupWin[popCount];
	 } 
	 ui_item_status[] = {
		 // -------              ------    ------- ----------
	     // ITEM ID              LOGIN          EDIT  CREAT   
		 //------------          --------------------------
		 IDC_SETTING_POPUP_EDIT,       1,         0,       1,
		 IDC_SETTING_POPUP_EDIT+1,     1,         1,       1,
		 IDC_SETTING_POPUP_EDIT+2,     0,         1,       1,
		 IDC_SETTING_POPUP_COMBBOX,    0,         0,       1,
		 IDC_SETTING_POPUP_COMBBOX+1,  0,         0,       1,
		 IDC_SETTING_POPUP_CHECKBOX,   0,         0,       1,
		 IDC_SETTING_POPUP_TEXT,       1,         1,       1,
		 IDC_SETTING_POPUP_TEXT+1,     1,         1,       1,
		 IDC_SETTING_POPUP_TEXT+2,     0,         1,       1,
		 IDC_SETTING_POPUP_TEXT+3,     0,         1,       1,
		 IDC_SETTING_POPUP_TEXT+4,     0,         1,       1,
		 IDC_SETTING_POPUP_TEXT+5,     0,         1,       1,

	 };

	 for ( int i = 0; i < ARRAY_SIZE(ui_item_status); ++i )
	 {
		 CWnd* pwnd = GetDlgItem(ui_item_status[i].nItemID);
		 if ( pwnd->GetSafeHwnd() )
		 {
			 if ( uType == popLoginUser )
			 {
				 if ( ui_item_status[i].bStatusAtUserPopupWin[uType] )
				 {
					 pwnd->EnableWindow( ui_item_status[i].bStatusAtUserPopupWin[uType] );
				 }
				 pwnd->ShowWindow( ui_item_status[i].bStatusAtUserPopupWin[uType] );
		     }
			 else
			 {
				 pwnd->EnableWindow( ui_item_status[i].bStatusAtUserPopupWin[uType] );
				 pwnd->ShowWindow( SW_SHOW );
 			 }
 		 }
	 }

	  
}
 
int CSettingUserPopup::get_input_userdata(UserData* pUser)
{
	if ( !pUser )
 		return -4;

 	CString strname = _T("");
	CString strpw1 = _T("");
	CString strpw2 = _T("");
	editBox[0].GetWindowText(strname);
	editBox[1].GetWindowText(strpw1);
	editBox[2].GetWindowText(strpw2);

	std::string chName = Unicode2Ansi(strname.GetBuffer(0)).c_str();
	std::string chpwd1 = Unicode2Ansi(strpw1.GetBuffer(0)).c_str();
	 
	std::regex reRule("\\w{1,16}");
	
	if ( chpwd1.length() > 16 || chName.length() > 16 )
	{
		MSD.ShowInfo(_T("�û���������ַ�����Ϊ16"),1);
 		return -5;
	}

	if( !regex_match(chName,reRule) )
	{
 		MSD.ShowInfo(_T("�û�������ȷ��"),1);
		return -1;
	}

 	if( !regex_match(chpwd1,reRule) )
	{
		MSD.ShowInfo(_T("���벻��ȷ��"),1);
		return -2;
	}

	if ( strpw1.Compare(strpw2) != 0 )
	{
		MSD.ShowInfo(_T("�����������벻һ�£�"),1);
		return -3;
 	}
 
 	pUser->nGrade = paraCombbox[0].GetItemData( paraCombbox[0].GetCurSel() );
	pUser->administrator = admin_check.GetChecked() ? true : false;
	pUser->nState = 1;
	sprintf( pUser->strUserName,"%s", chName.c_str() );
	sprintf( pUser->strPassWord,"%s", chpwd1.c_str() );

	return 0;

}