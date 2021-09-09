#include "stdafx.h"
#include "CRefCoSysSelect.h"

#pragma region Class CRefCoSysSelect
//CRefCoSysSelect Frame
BEGIN_MESSAGE_MAP(CRefCoSysSelect, CGroupFrame)
	ON_WM_CREATE()
	ON_CBN_SELCHANGE(IDC_UserCoSys_RefSelect+1, &CRefCoSysSelect::OnSelchangeRef)

END_MESSAGE_MAP()

CRefCoSysSelect::CRefCoSysSelect(bool isTool): is_tool(isTool) 
{

}

CRefCoSysSelect::~CRefCoSysSelect() 
{

}

BOOL CRefCoSysSelect::Create(CString caption,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CStatic::Create(caption, dwStyle|SS_NOTIFY, rect, pParentWnd, nID);
	_nID=nID;
	CString strlable;
	if ( is_tool )
	{
		strlable = _T("选择工具变量");
	}
	else
		strlable = _T("选择用户坐标变量");
	label_.Create(strlable,WS_CHILD|WS_VISIBLE,CRect(10,38,200,70),this);
	label_.SetFont(20);

	ref_.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED |CBS_HASSTRINGS,CRect(250,30,600,260),this,_nID+1);
	ref_.set_custom_message(FALSE);
	// WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED |CBS_HASSTRINGS
	return TRUE;
}
  
void CRefCoSysSelect::OnSelchangeRef()
{
	int index=ref_.GetCurSel();
	//CString* str;
	//variable_Combobox.GetLBText(index,*str);
	GetParent()->SendMessage(WM_SelChangeUserRef,(WPARAM)index);
}

void CRefCoSysSelect::AddStringList(CString* StrArray,unsigned int ArraySize)
{
	for (unsigned int i=0;i<ArraySize;i++)
	{
		ref_.AddString(*(StrArray+i));
	}
}

int CRefCoSysSelect::SelectRefName(CString str)
{
	if(str==_T("")) return -1;
	if (CB_ERR==ref_.FindString(-1,str))
	{
		return -1;
	}
	return ref_.SelectString(-1,str);

}

CString CRefCoSysSelect::GetSelRefName()
{
	CString str;
	ref_.GetWindowTextW(str) ;
	return str;
}

void CRefCoSysSelect::enable_window(BOOL b)
{
	EnableWindow(b);
	ref_.EnableWindow(b);
}