#include "stdafx.h"
#include "../TeachDialog/VariableSelect.h"
#include "../colourcode.h"

BEGIN_MESSAGE_MAP(CVariableSelect, CGroupFrame)
	ON_WM_CREATE()
	//ON_CBN_SELCHANGE(IDC_TEACHDLG_VARIABLE+1, &CVariableSelect::OnSelchangeVariable )	
	ON_NOTIFY( WM_CUSTOMCOMBOX_SELCHANGED, IDC_TEACHDLG_VARIABLE+1, &CVariableSelect::OnSelchangeVariable)

END_MESSAGE_MAP()

CVariableSelect::CVariableSelect() 
{

}

CVariableSelect::~CVariableSelect() 
{

}

BOOL CVariableSelect::Create(CString caption,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	_nID=nID;
	CStatic::Create(caption, dwStyle|SS_NOTIFY, rect, pParentWnd, nID);
	Initialize(rect, pParentWnd, nID);		
	label_.Create(_T("Ñ¡ÔñÎ»ÖÃ"),WS_CHILD|WS_VISIBLE,CRect(45,40,200,70),this);
	label_.SetAlignment(1);
	label_.Initialize(kColor[0]);
	label_.SetFontType(20,350,_T("ËÎÌå"));
	variable_Combobox.Create(WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|CBS_NOINTEGRALHEIGHT|WS_VSCROLL|CBS_OWNERDRAWFIXED |CBS_HASSTRINGS,
		CRect(300,30,700,275),this,_nID+1);
	 
	variable_Combobox.ShowScrollBar(2);
	variable_Combobox.BringWindowToTop();
	CIconComboBox::set_DropDownHight(variable_Combobox,6);

	return TRUE;
}

int CVariableSelect::OnCreate(LPCREATESTRUCT lpCreateStruct)
{	
	return 0;
}

void CVariableSelect::OnSelchangeVariable(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_COMBOXVIEW* pItem = (NM_COMBOXVIEW*) pNotifyStruct;
	*pResult = 0; 
	int index = pItem->currSelIndex;
	unsigned int idFrom = pItem->hdr.idFrom;

	//CString* str;
	//variable_Combobox.GetLBText(index,*str);
	CWnd* pWnd = GetParent();
 	if ( pWnd != NULL && ::IsWindow(pWnd->m_hWnd) )
	{
		pWnd->SendMessage(WM_SelChangeVariable,(WPARAM)index);
	}
}

void CVariableSelect::AddStringList(CString* StrArray,unsigned int ArraySize)
{
	for (unsigned int i=0;i<ArraySize;i++)
	{
		variable_Combobox.AddString(*(StrArray+i));
	}
}

int CVariableSelect::SelectVariableName(CString str)
{
	if(str==_T("")) return -1;
	if (CB_ERR==variable_Combobox.FindStringExact(-1,str))
	{
		return -1;
	}
	return variable_Combobox.SelectString(-1,str);

}

CString CVariableSelect::GetSeLVariableName()
{
	CString str = _T("");
	variable_Combobox.GetWindowTextW(str) ;
	return str;
}

void CVariableSelect::select_variable_index(int idx)
{
	variable_Combobox.SetCurSel(idx);
}

void CVariableSelect::get_curr_set_type(ScriptVarSetT* varset,ScriptVarT* svt)
{
 	int nItem = GetCurSel();
	if (nItem == -1) 
		return;
	get_set_type(nItem,varset,svt);
}

void CVariableSelect::get_set_type( int nIndex, ScriptVarSetT* varset,ScriptVarT* svt )
{
	if ( nIndex < 0 || nIndex > variable_Combobox.GetCount() )
		return;

	DWORD dwData = GetItemData(nIndex);
	ScriptVarSetT vset = SVAR_PROGRAM;
	ScriptVarT    vtype = svAXISPOS;

	vset  = ScriptVarSetT(HIBYTE(dwData));
	vtype = ScriptVarT(LOBYTE(dwData));

	*varset = vset;
	*svt    = vtype;
}