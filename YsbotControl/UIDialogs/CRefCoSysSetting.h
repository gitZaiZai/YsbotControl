#pragma once
 
#include "../IO Dialog/StaticTransparent.h"
#include "../IO Dialog/GroupFrame.h"
 
#include "../Keyboard/PopEdit.h"
 
#include "../BasicControls/IconComboBox.h"
#include "../BasicControls/Label.h"
#include "../BasicControls/labels.h" 

#define IDC_UserCoSys_RefSetting 4601 //4601-4610
#define WM_SelChangeBaseRef 4601
 

class CRefCoSysSetting : public CGroupFrame
{
public:
	CRefCoSysSetting();
	virtual ~CRefCoSysSetting();

	BOOL Create(CString caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd=NULL, UINT nID=65535);
	void SetData(double* JArray,unsigned int JNum=6);
	void GetData(double* JArray,unsigned int JNum=6);
	void ResetContent() {base_ref.ResetContent();};
	int AddString(LPCTSTR lpszString, DWORD dwData = 0) {return base_ref.AddString(lpszString,dwData);};
	void AddStringList(CString* StrArray,unsigned int ArraySize);
	//���ñ�������Ϊ�����б��ѡ����,ѡ�з������б����ţ����Ʋ����б��з���-1��
	int SelectBaseRefName(CString str);
	//h��ȡ��ǰ�����б�ѡ�е�ѡ���������
	CString GetCurBaseRefName();
	void SetReadOnly(BOOL bReadOnly);
	CIconComboBox base_ref;

	void clear_show ();

protected:
 	afx_msg void OnSelchangeBaseRef();
	DECLARE_MESSAGE_MAP()

private:
	CStaticTran label_joint[8];
	CpopEditDouble edit_joint[6];
	CFont font_;
	unsigned int _nID;
	BasicLabel     strUnit[6];

};