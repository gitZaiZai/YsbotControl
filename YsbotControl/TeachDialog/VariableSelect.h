#pragma once

#include "../IO Dialog/GroupFrame.h"
#include "../BasicControls/IconComboBox.h"
#include "../LuaScript/ScriptVariableType.h"
#include "../BasicControls/labels.h"
#define IDC_TEACHDLG_VARIABLE 4120 //4120-4121
#define WM_SelChangeVariable 4133

//����ѡ���
class CVariableSelect : public CGroupFrame
{
public:
	CVariableSelect();
	virtual ~CVariableSelect();

	BOOL Create(CString caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd=NULL, UINT nID=65535);	
	void ResetContent() {variable_Combobox.ResetContent();};
	int AddString(LPCTSTR lpszString, DWORD dwData = 0 ) {return variable_Combobox.AddString(lpszString,dwData);};
	void AddStringList(CString* StrArray,unsigned int ArraySize);
	//���ñ�������Ϊ�����б��ѡ����,ѡ�з������б����ţ����Ʋ����б��з���-1��
	int SelectVariableName(CString str);
	int FindStringExact(int nAfer,CString str) { return variable_Combobox.FindStringExact(nAfer,str); }
	//h��ȡ��ǰ�����б�ѡ�е�ѡ���������
	CString GetSeLVariableName();

	void select_variable_index(int idx);
	int GetCurSel() { return variable_Combobox.GetCurSel(); }
	DWORD_PTR GetItemData(int nitem) { return variable_Combobox.GetItemData(nitem); }
	
	void get_curr_set_type(ScriptVarSetT* varset,ScriptVarT* svt);
	void get_set_type (int nIndex, ScriptVarSetT* varset,ScriptVarT* svt);
	void EnableWindow(BOOL bEnable /* = TRUE */) { variable_Combobox.EnableWindow(bEnable); }

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelchangeVariable(NMHDR *pNotifyStruct, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

private:
	BasicLabel label_;
	CIconComboBox variable_Combobox;
	CFont font_;
	unsigned int _nID;
};