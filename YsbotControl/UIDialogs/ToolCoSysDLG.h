
#pragma once
#include "DialogType.h"
#include "CToolCoSysSetting.h"
#include "CToolMethod.h"
#include "CToolSettingGuide.h"
#include "CRefCoSysSelect.h"
#include "CToolPicture.h"
#include "../LuaScript/ScriptVariableType.h"

#define IDD_ToolCoSysDLG       4650
    

class ToolCoSysDLG :public DialogType
{
public:
	ToolCoSysDLG(int iWinID);
	virtual ~ToolCoSysDLG();

	int SelectToolName(CString RefName,BOOL bRefresh=TRUE);
	CString GetCurToolName(){return curToolName;};
	void InitCombobox();
 
	void SaveCurToolData();
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnMainButtonClicked(WPARAM,LPARAM); 
	afx_msg LRESULT OnSelchangeTool(WPARAM Wparam,LPARAM Lparam);;
 	afx_msg LRESULT OnMethodChange(WPARAM Wparam,LPARAM Lparam);	
	virtual int OnActivateChildWin(LPARAM);

private:
	CToolCoSysSetting toolSetting;
	CToolMethod       toolMethod; 
	CRefCoSysSelect   toolSelect; // share with user coordinate setting dialog.
	CToolSettingGuide toolGuide;
	CToolPicture	  toolPicture;

	CString	curToolName;
	CartRef curr_tool;
 	 
	int m_iStep;
	int m_iMethodType;

	CString	curRefName;
	VariableKey m_currRef;
	int		m_iWindowID;

	static const CString strToolSetupBtn[6];
	static const CString strToolProcessBtn[6];
	static const CString strToolConfirmBtn[6];

	void SetToolCoSysData(const CartRef* ctool );
	void GetToolCoSysData(CartRef& ctool );
	void update_tool_data( int selID );
	void update_show_ui( int iStep );
	void reset();
	void save_calc_result ();
};

extern ToolCoSysDLG *toolCoSys_DLG;