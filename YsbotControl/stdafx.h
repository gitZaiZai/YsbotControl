
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
//#define new new(_CLIENT_BLOCK,__FILE__,__LINE__)

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

#pragma warning (disable:4290 4996)
 
#include <iostream>
#include <string>
#include <fstream>

#include "control_id.h"
// #include "hotkey_id.h"
// #include "colourcode.h"
// #include "common_file_tools.h"
// #include "labels.h"
// #include "buttons.h"
// #include "button_class.h"
// #include "icon_class.h"
// #include "grids.h"
// #include "ColumnTreeCtrl.h"
// #include "comboboxes.h"
// #include "program_editor.h"

// #include "dialog_template.h"
// #include "state_dialog.h"
// #include "left_dialog.h"
// #include "right_dialog.h"
// #include "bottom_dialog.h"
// #include "program_dialog.h"
// #include "location_manager.h"
// #include "variable_manager.h"
#include "Logger.h"
 
#define  RUNSIG      0X01
#define  ERRORSIG    0X02
#define  MOTIONSIG   0X04
#define  PROCESSSIG  0X08
#define  BUZZERSIG   0X80
#define  LIGHTALL    0X0F
  
#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


extern int g_x0;
extern Logger logout;