#pragma once
 
#include "../IO Dialog/StaticTransparent.h"
#include "../IO Dialog/GroupFrame.h"
 
#include "../BasicControls/IconComboBox.h"
#include "../BasicControls/Label.h"
 

#define IDC_UserCoSys_RefSelect 4620	//4620-4621
#define WM_SelChangeUserRef 4600

//变量选择框
class CRefCoSysSelect : public CGroupFrame
{
public:
	CRefCoSysSelect(bool isTool = false);
	virtual ~CRefCoSysSelect();
	BOOL Create(CString caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd=NULL, UINT nID=65535);
	int AddString(LPCTSTR lpszString, DWORD dwData = 0 ) {return ref_.AddString(lpszString, dwData);};
	void AddStringList(CString* StrArray,unsigned int ArraySize);
	//设置变量名称为下拉列表的选中项,选中返回在列表的序号，名称不在列表中返回-1。
	int SelectRefName(CString str);
	//h获取当前下拉列表选中的选择变量名称
	CString GetSelRefName();
	void ResetContent() {ref_.ResetContent();};
	CIconComboBox ref_;
	void enable_window(BOOL);
 
protected:
 	afx_msg void OnSelchangeRef();
	DECLARE_MESSAGE_MAP()

private:
	CStaticTran label_;
	CFont font_;
	unsigned int _nID;

	bool is_tool;
};