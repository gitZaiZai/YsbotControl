#pragma once

#include "../IO Dialog/StaticTransparent.h"
#include "../IO Dialog/GroupFrame.h"

#include "../Keyboard/PopEdit.h"

#include "../BasicControls/IconComboBox.h"
#include "../BasicControls/Label.h"
#include "../BasicControls/labels.h"


#define IDC_ToolCoSys_RefSetting 4651  
 

class CToolCoSysSetting : public CGroupFrame
{
public:
	CToolCoSysSetting();
	virtual ~CToolCoSysSetting();

	BOOL Create(CString caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd=NULL, UINT nID=65535);
	void SetData(double* JArray,unsigned int JNum=6);
	void GetData(double* JArray,unsigned int JNum=6);
	 
 	void AddStringList(CString* StrArray,unsigned int ArraySize);
	//设置变量名称为下拉列表的选中项,选中返回在列表的序号，名称不在列表中返回-1。
	 
	void SetReadOnly(BOOL bReadOnly);
	void clear_show ();

protected:
  	DECLARE_MESSAGE_MAP()

private:
	CStaticTran    label_joint[6];
	CpopEditDouble edit_joint[6];
	BasicLabel     strUnit[6];

	CFont          font_;
	unsigned int   _nID;
};