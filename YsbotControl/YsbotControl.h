
// YsbotControl.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#define REG_MSG (_T("{7510EF00-BADA-48de-A6CE-5FBC817616DD}"))
// CYsbotControlApp:
// See YsbotControl.cpp for the implementation of this class
//

class CYsbotControlApp : public CWinApp
{
public:
	CYsbotControlApp();
	HINSTANCE scintilla_;

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;

private:
	HACCEL	m_hHotKey;
	HWND	m_hWndAcquireKeyInput;
	int		m_nAcquiredKeyNum;
	BYTE	m_byAcquiredKey[256];
	BOOL    is_redirect;
	// Methods
public:
	BOOL	AcquireKeyInput(HWND hWnd, const BYTE *pbyVirtualKey);
	void	ReleaseKeyInput();
	// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	BOOL VerifyPermission();

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

};

extern UINT WM_ACTIVE_MSG;
extern CYsbotControlApp theApp;