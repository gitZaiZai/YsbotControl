// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

// YsbotControl.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "YsbotControl.h"
#include "YsbotControlDlg.h"

#include "UIDialogs/SystemInformation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define REG_MSG (_T("{7510EF00-BADA-48de-A6CE-5FBC817616DD}"))
UINT WM_ACTIVE_MSG = ::RegisterWindowMessage(REG_MSG);
// CYsbotControlApp

BEGIN_MESSAGE_MAP(CYsbotControlApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CYsbotControlApp construction

CYsbotControlApp::CYsbotControlApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
	is_redirect = FALSE;
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CYsbotControlApp object

CYsbotControlApp theApp;

LRESULT CALLBACK GetMessageProc(int nCode, WPARAM wParam, LPARAM lParam)
{
// 	AFX_MAINTAIN_STATE(AfxGetStaticModuleState());
	LPMSG lpMsg = (LPMSG) lParam;

	if (AfxGetApp()->PreTranslateMessage(lpMsg))
	{
		lpMsg->message = WM_NULL;
		lpMsg->lParam = 0L;
		lpMsg->wParam = 0;
	}
	return 0;
}
// CYsbotControlApp initialization

BOOL CYsbotControlApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	 
	HANDLE m_hMutex = ::CreateMutex(NULL, FALSE, _T("{6668BB0A-DE0C-499d-8520-79653FF9B2EB}"));
	if ( GetLastError() == ERROR_ALREADY_EXISTS )
	{
// 		AfxMessageBox(_T("已经有一个实例正在运行中……"));
		CloseHandle(m_hMutex);
		m_hMutex = NULL;

		DWORD dwRecipients = BSM_APPLICATIONS;
		::BroadcastSystemMessage(BSF_NOHANG, &dwRecipients, WM_ACTIVE_MSG, 0, 0);

		return FALSE;
	}

	BOOL b = VerifyPermission();
	if ( !b )
 		return FALSE;

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

// 	scintilla_ = LoadLibrary(_T("SciLexer.DLL"));
// 	if (scintilla_ == NULL)
// 	{
// 		CString str("The Scintilla DLL could not be loaded.\r\nError loading Scintilla");
// 		AfxMessageBox(str);
// 		return FALSE;
// 	}
	Scintilla_RegisterClasses(AfxGetInstanceHandle());

	AfxEnableControlContainer();

// 	HHOOK hHook = ::SetWindowsHookExW(WH_GETMESSAGE,GetMessageProc,AfxGetInstanceHandle(),GetCurrentThreadId());
// 	ASSERT(hHook);
	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

#ifdef _DEBUG
	if ( !AllocConsole() )
	{
		//AfxMessageBox(_T("Failed to create the console!"), MB_ICONEXCLAMATION);
		TRACE(_T("\nFailed to create the console!\n"));
	}

	freopen("CONOUT$", "w+t", stdout);  
	freopen("CONOUT$", "w+t", stderr);  

	system("chcp 65001"); //控制台设置UTF8字符集 （使用SetConsoleCP(65001)设置无效，原因未知）
	CONSOLE_FONT_INFOEX info = { 0 }; // 以下设置字体
	info.cbSize = sizeof(info);
	info.dwFontSize.Y = 16; // leave X as zero
	info.FontWeight = FW_NORMAL;
	wcscpy(info.FaceName, L"Consolas");
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), NULL, &info);

	setlocale(LC_ALL,"chs");
	cout.imbue(locale("chs"));

#endif 

// 	CSystemInformation sysinfo;
// 	char strmsg[200] = {'\0'};
// 	sprintf(strmsg, " OS:%s\r\nVer:%s\r\nMAC:%s\r\nLocalIP:%s\r\n",
// 		sysinfo.m_strOS.c_str(),sysinfo.m_strVersion.c_str(), sysinfo.m_strMAC.c_str(),sysinfo.m_strIP.c_str());
// 	cout << strmsg << endl;

	CYsbotControlDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

#ifdef _DEBUG
	FreeConsole();
#endif 

	if (m_hMutex) 
	{
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
	}
// 	UnhookWindowsHookEx((HHOOK)hHook);
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CYsbotControlApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class
	Scintilla_ReleaseResources();
	Gdiplus::GdiplusShutdown(gdiplusToken);
	CMFCVisualManager::DestroyInstance(TRUE); //
	return CWinApp::ExitInstance();
}

BOOL CYsbotControlApp::VerifyPermission()
{
	CTime time = CTime::GetCurrentTime();
	CString current_date = time.Format(_T("%Y-%m-%d %H:%M:%S"));
	CString expiration_max_date = _T("2020-09-03 14:16:00");
	CString expiration_min_date = _T("2019-01-16 04:16:00");
	if (current_date > expiration_max_date || current_date < expiration_min_date)
	{
		AfxMessageBox(_T("软件启动故障！请联系软件供应商。"));
 		return FALSE;
	}
	return TRUE;
}

BOOL CYsbotControlApp::AcquireKeyInput(HWND hWnd, const BYTE *pbyVirtualKey)
{
	if( IsWindow(hWnd) )
	{
		int  nKeyNum = 0;

		while( pbyVirtualKey[nKeyNum] && nKeyNum < 256 )
		{
			m_byAcquiredKey[nKeyNum] = pbyVirtualKey[nKeyNum];
			nKeyNum++;
		}

		m_hWndAcquireKeyInput = hWnd;
		m_nAcquiredKeyNum = nKeyNum;

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CYsbotControlApp::ReleaseKeyInput()
{
	m_hWndAcquireKeyInput = NULL;
	m_nAcquiredKeyNum = 0;
}

BOOL CYsbotControlApp::PreTranslateMessage(MSG* pMsg)
{
	//
	// handles hot keys
	//
	if( TranslateAccelerator(m_pMainWnd->m_hWnd, m_hHotKey, pMsg) )
	{
		return TRUE;
	}

	//
	// redirects the key inputs to the window who acquires
	//

	if( m_hWndAcquireKeyInput && m_nAcquiredKeyNum > 0 )
	{
		if( (pMsg->hwnd != m_hWndAcquireKeyInput) && 
			(pMsg->message == WM_KEYDOWN    || pMsg->message == WM_KEYUP   ||
			pMsg->message == WM_SYSKEYDOWN || pMsg->message == WM_SYSKEYUP) )
		{
			if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
			{
				for(int i = 0; i < m_nAcquiredKeyNum; ++i)
				{
					// 需同时按下Alt，才强制变更接受窗口句柄
					if( m_byAcquiredKey[i] == (BYTE) pMsg->wParam && 
						( (::GetKeyState(VK_MENU) < 0) || (::GetKeyState(VK_SHIFT) < 0) || (::GetKeyState(VK_CONTROL) < 0) ) )   
					{
						pMsg->hwnd = m_hWndAcquireKeyInput;
						break;
					}
				}
			}
			else if ( pMsg->message == WM_KEYUP || pMsg->message == WM_SYSKEYUP )
			{
				// 当弹起Alt或指定字符时，才强制变更键盘消息接受窗口句柄 
				for(int i = 0; i < m_nAcquiredKeyNum; ++i)
				{
					if( m_byAcquiredKey[i] == (BYTE) pMsg->wParam ||
						( (::GetKeyState(VK_MENU) > 0) || (::GetKeyState(VK_SHIFT) > 0) || (::GetKeyState(VK_CONTROL) > 0) )   )  
					{
						pMsg->hwnd = m_hWndAcquireKeyInput;
						break;
					}
				}
			}
		}
	}
	return CWinApp::PreTranslateMessage(pMsg);
}
