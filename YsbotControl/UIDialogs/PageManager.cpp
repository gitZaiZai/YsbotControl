// PageManager.cpp : implementation file
//

#include "stdafx.h"
#include "PageManager.h"
#include "../program_dialog.h"
#include "WindowButtonMenu.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageManager

CPageManager::CPageManager()
{
	m_ppPages = new CWnd*[MAX_PAGE_NUM];
	ClearAllPages();

	m_lParam = 0;
}

CPageManager::~CPageManager()
{
	delete [] m_ppPages;
}


BEGIN_MESSAGE_MAP(CPageManager, CStatic)
	//{{AFX_MSG_MAP(CPageManager)
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

void CPageManager::ClearAllPages()
{
	m_nCurSelPage = -1;
	m_uPageNum = 0;
	::ZeroMemory(m_ppPages, sizeof(CWnd*) * MAX_PAGE_NUM);
}


// CPageManager Helpers

void CPageManager::SizePage(CWnd* pwndPage)
{
	if( GetSafeHwnd() && pwndPage->GetSafeHwnd() )
	{
		// move & re-size the window to exactly the same with the manager's
		CRect  rect;
		GetWindowRect(&rect);
// 		if ( rect.top != 0 )
// 		{
// 			rect.OffsetRect(0,24);
// 		}
		GetParent()->ScreenToClient(&rect);
		pwndPage->MoveWindow(&rect);
	}
}


// CPageManager Methods

UINT CPageManager::InsertPage(CWnd* pwndPage, UINT nIDTemplate)
{
	ASSERT( GetParent() );

	if( pwndPage )
	{
		if( !pwndPage->GetSafeHwnd() )
		{
			if( !pwndPage->IsKindOf(RUNTIME_CLASS(CDialog)) ||
				!((CDialog*)pwndPage)->Create(nIDTemplate, GetParent()) )
				return -1;
		}

		// store in the list
		m_ppPages[m_uPageNum++] = pwndPage;

		// in case the manager has been created
		SizePage(pwndPage);

		// firstly hide window
		pwndPage->ShowWindow(SW_HIDE);
	}

	return m_uPageNum;
}

UINT CPageManager::InsertPage(int nIndex, CWnd* pwndPage, UINT nIDTemplate)
{
	ASSERT( GetParent() );

	if( pwndPage )
	{
		if( !pwndPage->GetSafeHwnd() )
		{
			if( !pwndPage->IsKindOf(RUNTIME_CLASS(CDialog)) ||
				!((CDialog*)pwndPage)->Create(nIDTemplate, GetParent()) )
				return -1;
		}

		// store in the list
		m_ppPages[nIndex] = pwndPage;
		m_uPageNum++;
		// in case the manager has been created

		SizePage(pwndPage);	 

		// firstly hide window
		pwndPage->ShowWindow(SW_HIDE);
	}

	return m_uPageNum;
}

CWnd* CPageManager::SelectPage(int nIndex, LPARAM lp)
{
	CWnd*  pWnd = NULL;

	if(nIndex >= 0 )//&& nIndex < (int) GetPageCount()
	{
		pWnd = m_ppPages[nIndex];

		if(nIndex != m_nCurSelPage)
		{
// 			if(m_nCurSelPage >= 0)
// 			{
// 				m_ppPages[m_nCurSelPage]->ShowWindow(SW_HIDE);
// 			}
// 
// //  			BOOL ret = m_ppPages[nIndex]->SetWindowPos(
// //  				NULL, //m_pwndZOrderAfter,	// Z-order
// //  				0, 0, 0, 0, // not used
// //  				SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW // show window |SWP_SHOWWINDOW
// //  				);
//    			m_ppPages[nIndex]->ShowWindow(SW_SHOW);

			if ( nIndex == winEditCommand )
			{
				if ( m_nCurSelPage >= 0 && m_nCurSelPage != winProgram )
				{
					ASSERT(m_ppPages[m_nCurSelPage]);
					m_ppPages[m_nCurSelPage]->ShowWindow(SW_HIDE);

					m_ppPages[winProgram]->ShowWindow(SW_SHOW);
				}

				program_dialog->show_edit_win( TRUE, lp ); // lp = big or small edit win.
			}
			else
			{
				if( m_nCurSelPage >= 0 && m_ppPages[m_nCurSelPage] )
				{
					if ( m_nCurSelPage == winEditCommand )
						program_dialog->show_edit_win( FALSE );
					else
						m_ppPages[m_nCurSelPage]->ShowWindow(SW_HIDE);
				}

				BOOL ret = m_ppPages[nIndex]->SetWindowPos(
				   	m_pwndZOrderAfter, //m_pwndZOrderAfter,	// Z-order
				   	0, 0, 0, 0, // not used
				   	SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW // show window |SWP_SHOWWINDOW
				   	);
				//m_ppPages[nIndex]->ShowWindow(SW_SHOW);
			}

			m_ppPages[nIndex]->SetFocus();
			m_nCurSelPage = nIndex;
			m_lParam = lp;
		}
	}
	else
	{
		if(m_nCurSelPage >= 0)
		{
			m_ppPages[m_nCurSelPage]->ShowWindow(SW_HIDE);
			m_nCurSelPage = nIndex;
		}
	}

	return pWnd;
}

void CPageManager::BringToTopShow(int nIndex)
{
	BOOL ret = m_ppPages[nIndex]->SetWindowPos(
		&wndTopMost,	// Z-order
		0, 0, 800, 600, // not used
		SWP_SHOWWINDOW // show window
		);
	m_ppPages[nIndex]->SetFocus();

	m_nCurSelPage = nIndex;
}

CWnd* CPageManager::GetPage(int nIndex) const
{
	return
		(nIndex >= 0 && nIndex < MAX_PAGE_NUM) ?
		m_ppPages[nIndex]:
	NULL;
}

int	CPageManager::GetPageIndex(const CWnd* pwndPage) const
{
	for(int i = 0; i < (int) GetPageCount(); ++i)
	{
		if( m_ppPages[i] == pwndPage )
		{
			return i;
		}
	}

	return -1;
}

CWnd* CPageManager::GetCurSelPage() const
{
	return (m_nCurSelPage >= 0) ? m_ppPages[m_nCurSelPage]: NULL;
}

int	CPageManager::GetCurSelPageIndex(void) const
{
	return m_nCurSelPage;
}

LPARAM CPageManager::GetCurSelPageLParam(void) const
{
	return m_lParam;
}

UINT CPageManager::GetPageCount() const
{
	return m_uPageNum;
}

void CPageManager::EnableAllChildWindow(int nIndex, BOOL bEnable)
{
	CWnd	*pwndFirst, *pwndLast, *pwndChild;

	if(nIndex >= 0 && nIndex < (int) GetPageCount())
	{
		if( (pwndChild = m_ppPages[nIndex]->GetWindow(GW_CHILD)) != NULL )
		{
			pwndFirst = pwndChild->GetWindow(GW_HWNDFIRST);
			pwndLast  = pwndChild->GetWindow(GW_HWNDLAST);

			for(
				pwndChild = pwndFirst;
				pwndChild != pwndLast; 
			pwndChild = pwndChild->GetWindow(GW_HWNDNEXT)
				)
			{
				pwndChild->EnableWindow(bEnable);
			}

			pwndLast->EnableWindow(bEnable);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPageManager message handlers

void CPageManager::PreSubclassWindow() 
{
	m_pwndZOrderAfter = NULL;
	CStatic::PreSubclassWindow();
}

void CPageManager::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);

	// re-size all pages
	if( GetPageCount() > 0 )
	{
		for(int i = 0; i < MAX_PAGE_NUM; ++i)
			SizePage(m_ppPages[i]);
	}
}

void CPageManager::OnSetFocus(CWnd* pOldWnd)
{
	CStatic::OnSetFocus(pOldWnd);

	// refer focus to currently selected page
	if( GetPageCount() > 0 )
		GetCurSelPage()->SetFocus();
}