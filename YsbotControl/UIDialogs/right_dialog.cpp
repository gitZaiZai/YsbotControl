#include "stdafx.h"
#include "right_dialog.h"
#include "../WorldModel.h"
#include "../resource.h"
#include "../control_id.h"
#include "../stringconvert.h"


CString str_jog_lable[][7] = { { _T("A1"), _T("A2"),_T("A3"), _T("A4"),_T("A5"),_T("A6"),_T("2nd") },
	                           { _T("X"),  _T("Y"), _T("Z"),  _T("A"), _T("B"), _T("C"),_T("2nd")  },
							   { _T("TX"), _T("TY"),_T("TZ"), _T("TA"),_T("TB"),_T("TC"),_T("2nd") },
							   { _T("RX"), _T("RY"),_T("RZ"), _T("RA"),_T("RB"),_T("RC"),_T("2nd") },
                               { _T("A7"), _T("A8"),_T(""),   _T(""),  _T(""),  _T(""),_T("2nd")   },
							   { _T("GO"), _T(""),_T(""),   _T(""),  _T(""),  _T(""),_T("2nd")     }};

RightDialog::RightDialog()  
{
	label_list.reserve(7);
	label_list.clear();
	labelType = -1;
}

RightDialog::~RightDialog() 
{
 	for (unsigned int i=0; i < label_list.size(); i++) 
	{
 		delete label_list[i];
	}
}

BEGIN_MESSAGE_MAP(RightDialog, DialogType)
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void RightDialog::DoDataExchange(CDataExchange* pDX) 
{

}


BOOL RightDialog::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
 	 
   	//SetWindowPos(NULL,m_RectPos.left, m_RectPos.top, m_RectPos.Width(), m_RectPos.Height(), SWP_NOZORDER);
	if ( GetParent()->GetDlgItem(IDC_BOX_RIGHT)->GetSafeHwnd() )
	{
		GetParent()->GetDlgItem(IDC_BOX_RIGHT)->GetClientRect(&clientRect);
		SetWindowPos( NULL, clientRect.left, clientRect.top, clientRect.Width(), clientRect.Height(), SWP_NOZORDER );
    }  

	//::SetWindowPos(right_dialog_->GetSafeHwnd(), NULL, 
	//	boxRect[BOX_RIGHT].left, boxRect[BOX_RIGHT].top, boxRect[BOX_RIGHT].Width(), boxRect[BOX_RIGHT].Height(), 
	//	SWP_SHOWWINDOW | SWP_NOZORDER );


  	SetBackgroundColor(kColor[2], TRUE);
	ModifyStyle(WS_CAPTION | WS_BORDER,0,SWP_DRAWFRAME); 

 	init_right_label();
 	
	return FALSE;
}

void RightDialog::init_right_label()
{
	CRect clientRect;
	//GetClientRect(&clientRect);
 
	if ( GetParent()->GetDlgItem(IDC_BOX_RIGHT)->GetSafeHwnd() )
	{
		GetParent()->GetDlgItem(IDC_BOX_RIGHT)->GetClientRect(&clientRect);
 	}
 	
	label_list.resize(7);

	const int totalHeight = clientRect.Height();
	const int cellHeight = (int)( (float)(clientRect.Height())/7.0+0.5);
	int lineSpace  = -1;
	int x0 = clientRect.left;
	int y0 = clientRect.top;
	int x1 = x0 + 47;
	int y1 = y0 + cellHeight;

	COLORREF m_crBackgroud = RGB(102, 102, 102);
	COLORREF m_crText      = RGB(255, 255, 255);

	for ( unsigned int i = 0; i < 7; i++ ) 
	{
		if ( i == 6 )
			rect2nd = CRect(x0,y0,x1,y1);

		label_list[i] = new CLabel();
		label_list[i]->Create( _T(""), WS_CHILD | WS_VISIBLE | DT_CENTER | SS_CENTERIMAGE,
			CRect(x0,y0,x1,y1), this, RIGHT_DIALOG_LABEL1+i );
		label_list[i]->SetBkColor(m_crBackgroud)
			.SetTextColor(m_crText)
			.SetText(_T(""));
		y0 = y1 + lineSpace;
		y1 = y0 + cellHeight;
		if ( i == 5 )
		{
			y1 += 4;
		}
    }
	label_list[6]->SetLabelIcon(IDI_2ND,40,60); // ????

	// test liurundan
	for ( int i= 0; i < 6; ++i )
	{
		label_list[i]->SetText(str_jog_lable[0][i]);
	}
	//label_list[6]->SetLabelBitmaps(IDI_2ND,44,68);
}
  
void RightDialog::update_window()
{
 	bool in_second_page = MWM.get_robot_state().in_second;
	bool in_teachmoving = MWM.get_robot_state().in_teachmoving;
	if (in_teachmoving)
	{
		if ( labelType != RIGHT_LABEL_TEACHMOVE )
		{
			labelType = RIGHT_LABEL_TEACHMOVE;
			for ( int i= 0; i < 6; ++i )
			{
				label_list[i]->SetText(str_jog_lable[RIGHT_LABEL_TEACHMOVE][i]);
			}
		}
	}
	else if ( in_second_page )
	{
 		if ( labelType != RIGHT_LABEL_SECOND )
		{
			labelType = RIGHT_LABEL_SECOND;
			for ( int i= 0; i < 6; ++i )
			{
				label_list[i]->SetText(str_jog_lable[RIGHT_LABEL_SECOND][i]);
			}
		}
	}
	else
	{
		// coordinate is joint not cart.
 		JogCoordT currjt = MWM.get_robot_state().coordinate;

		if ( labelType != (int)currjt )
		{
			labelType = (int)currjt;
			for ( int i= 0; i < 6; ++i )
			{
				label_list[i]->SetText(str_jog_lable[(int)currjt][i]);
			}
		}
  	}
}
 
bool RightDialog::IsCoveredByOtherWindow(HWND hWnd)
{
 	HWND deswnd = ::GetDesktopWindow();
	HWND topwnd = ::GetTopWindow(deswnd);
	HWND topwnd2 = ::GetForegroundWindow();
// 	CYsbotControlDlg *p = (CYsbotControlDlg *)GetParent();
// 
// 	HWND self = p->GetSafeHwnd(); 
// 
// 	TCHAR t[30];
// 	_itot_s((int)topwnd,t,30,10);
// 	TRACE(t);
// 	TRACE(_T("\n"));
// 
// 	cout << (long)topwnd2 << " : ************** :" << (long)self << '\n';

	return false;
}

void RightDialog::OnLButtonUp(UINT nFlags, CPoint point)
{
   	if ( rect2nd.PtInRect(point) )
	{
 		GetParent()->SendMessage(WM_2ND_CLICKED,0,0);
  	}
	 
	DialogType::OnLButtonUp(nFlags, point);
}
