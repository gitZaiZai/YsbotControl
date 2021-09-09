#pragma once
 
#include "../IO Dialog/StaticTransparent.h"
#include "../IO Dialog/GroupFrame.h"
 
#include "../BasicControls/IconComboBox.h"
#include "../BasicControls/Label.h"
 

#define IDC_UserCoSys_RefSelect 4620	//4620-4621
#define WM_SelChangeUserRef 4600

//����ѡ���
class CRefCoSysSelect : public CGroupFrame
{
public:
	CRefCoSysSelect(bool isTool = false);
	virtual ~CRefCoSysSelect();
	BOOL Create(CString caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd=NULL, UINT nID=65535);
	int AddString(LPCTSTR lpszString, DWORD dwData = 0 ) {return ref_.AddString(lpszString, dwData);};
	void AddStringList(CString* StrArray,unsigned int ArraySize);
	//���ñ�������Ϊ�����б��ѡ����,ѡ�з������б����ţ����Ʋ����б��з���-1��
	int SelectRefName(CString str);
	//h��ȡ��ǰ�����б�ѡ�е�ѡ���������
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