#pragma once

#include "../IO Dialog/GroupFrame.h"
#include "../BasicControls/Label.h"
#include "../LuaScript/ScriptVariableType.h"

#define IDC_TEACHDLG_TEACHINFO 4122 //4122-4124
#define IDC_TEACHDLG_POSINFO 4125 //4125-4127


#define WM_SelChangeRef 4134
#define WM_SelChangeTool 4135

//²Î¿¼ÏµÏÔÊ¾¿ò
class CCoSysInfo: public CGroupFrame
{
public:
	CCoSysInfo();
	virtual ~CCoSysInfo();

	BOOL Create(CString caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd=NULL, UINT nID=65535);
 
	void set_cartref(ScriptVarSetT vt,CString crefStr);
	void set_toolref(ScriptVarSetT vt,CString toolStr);
	void get_cartref(int* pIcon,CString* pCartRef);
	void get_toolref(int* pIcon,CString* pCartRef);


protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
 
	DECLARE_MESSAGE_MAP()

private:
	CStaticTran label_[2];
    int widthIcon, heightIcon;

	unsigned int _nID;
	CFont font_;

	CLabel ref_cosys_label;
	CLabel tool_cosys_label;
 
	UINT get_icon_id ( ScriptVarSetT vt);
};