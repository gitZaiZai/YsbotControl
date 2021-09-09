#pragma once
#include "../UIDialogs/DialogType.h"
#include "../PosInfo.h"
#include "../CmdInfo.h"
#include <map>
#include "../BasicControls/IconComboBox.h"
#include "../LuaScript/ScriptVariableType.h"

#include "../TeachDialog/JointData.h"
#include "../TeachDialog/PoseData.h"
#include "../TeachDialog/CoSysInfo.h"
#include "../TeachDialog/HelpFrame.h"
#include "../TeachDialog/MoveType.h"
#include "../TeachDialog/VariableSelect.h"

#define IDD_TEACHDIALOG 4100
#define IDC_TEACHDLG_JOINT 4101		//4101-4109
#define IDC_TEACHDLG_POSE 4110		//4110-4119

#define IDC_TEACHDLG_VARIABLE 4120 //4120-4121
#define IDC_VARIABLE_COMBOBOX 4121


#define IDC_TEACHDLG_HELPER 4133//4133-4134


//----------内部子框类结束-----------

//点位示教主窗口
class TeachDialog :public DialogType
{
public:
	TeachDialog();
	virtual ~TeachDialog();
	//设置变量名称为下拉列表的选中项,选中返回在列表的序号，名称不在列表中返回-1。
	int SelectVariableName( CString varName, ScriptVarSetT vSet, ScriptVarT vTyp, BOOL bRefresh=TRUE );
 
//  	//刷新位置数据，可以在主定时器调用
// 	void update_Teach_window();
	//初始化变量、参考坐标系，工具手下拉列表
	void InitVariableCombo();
	//设置数据是否只读
	void SetReadOnly(BOOL bReadOnly);
 	//返回选择的示教运动插补方式（）
	unsigned int GetMoveType() { return moveType_frame.GetMoveType(); }

	void  set_prog_cartref( ScriptVarSetT vt, CString strname );
	void set_prog_toolref(  ScriptVarSetT vt, CString strname );
	void set_jog_cartref(  ScriptVarSetT vt, CString strname );
	void set_jog_toolref(  ScriptVarSetT vt, CString strname );
	virtual int OnActivateChildWin(LPARAM);

	void active_from_variable_dlg (VariableKey* pVarKey);

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
 
	DECLARE_MESSAGE_MAP()	
	afx_msg LRESULT OnMainButtonClicked(WPARAM Wparam,LPARAM Lparam); 
	afx_msg LRESULT OnSelchangeVariable(WPARAM Wparam,LPARAM Lparam);
	afx_msg LRESULT OnChangeMoveType(WPARAM Wparam,LPARAM Lparam);
	
private:
	CVariableSelect variable_Select;
	CJointData jointData_frame;
	CPoseData poseData_frame;
	CCoSysInfo teacCoSys_frame, posCoSys_frame;
	CMoveType moveType_frame;
	CHelpFrame help_frame;
  
	CartRef curr_cart_ref;
	CartRef curr_tool;
	ScriptVarT showVariableType;
    CmdInfo curr_teach_cmd;
	PosInfo last_pos;

	//设置控件中的位置数据
	void SetPositionData(const PosInfo& cp);
	//获取控件中的位置数据
	bool GetPositionData(PosInfo& cp );
	//显示轴关节位置
	void set_axis_pos( const PosInfo& ap );
	//显示笛卡尔坐标位置
	void set_cart_pos( const PosInfo& cp );
 
	void set_variable ();
	void teach_pos_var ();
	void show_window_type (int nType = -1);

};

extern TeachDialog *teach_Dialog;

 