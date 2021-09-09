#pragma once
#include "DialogType.h"
#include "../PosInfo.h" 
#include "CRefCoSysSelect.h"

#include "CRefCoSysSetting.h"
#include "CRefChooseMethod.h"
#include "CToolCoSysVal.h"
#include "CRefSettingGuide.h"
#include "CRefPosPic.h" 

#include "../LuaScript/ScriptVariableType.h"
 
class UserCoSysDLG :public DialogType
{
public:
	UserCoSysDLG(int iWinID);
	virtual ~UserCoSysDLG();
	int SelectRefName(CString RefName,BOOL bRefresh=TRUE);
	CString GetCurRefName(){return curRefName;};
	void InitCombobox();
// 	void update_UserCoSys_window();
	void SaveCurRefData();
	void update_refData( int selID );
	void update_toolData(const CartRef* refsys );
	void reset();

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
 	afx_msg LRESULT OnMainButtonClicked(WPARAM,LPARAM); 
	afx_msg LRESULT OnSelchangeUserRef(WPARAM Wparam,LPARAM Lparam);
	afx_msg LRESULT OnSelchangeBaseRef(WPARAM Wparam,LPARAM Lparam);	
	afx_msg LRESULT OnSelchangeRefStep(WPARAM Wparam,LPARAM Lparam);	
	afx_msg LRESULT OnMethodChange(WPARAM Wparam,LPARAM Lparam);	

	virtual int OnActivateChildWin(LPARAM);

private:
	CRefCoSysSelect  refCoSysSelect;
	CRefCoSysSetting refCoSysSetting;
	CRefChooseMethod refChooseMethod;
	CToolCoSysVal    toolCoSysVal;
	CRefSettingGuide refGuide;
	CRefPosPic       refPicture;
	int m_iStep;
	int m_iMethodType;

 	CString	curRefName;
	CartRef curr_cart_ref;
	CartRef curr_tool;
	int		m_iWindowID;

	static const CString strSetupBtn[6];
	static const CString strProcessBtn[6];
	static const CString strConfirmBtn[6];

	VariableKey m_currRef;

	//设置控件中的参考坐标系数据
	void SetUserCoSysData(const CartRef* cref );
	//获取控件中的参考坐标系数据
	void GetUserCoSysData(CartRef& cref );

	void SetToolCoSysData(const CartRef* cref );

	void GetToolCoSysData(CartRef& cref );

	void update_show_ui( int iStep );
	void save_calc_result ();

};

extern UserCoSysDLG *userCoSys_DLG;