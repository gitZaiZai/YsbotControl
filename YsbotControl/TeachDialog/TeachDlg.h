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


//----------�ڲ��ӿ������-----------

//��λʾ��������
class TeachDialog :public DialogType
{
public:
	TeachDialog();
	virtual ~TeachDialog();
	//���ñ�������Ϊ�����б��ѡ����,ѡ�з������б����ţ����Ʋ����б��з���-1��
	int SelectVariableName( CString varName, ScriptVarSetT vSet, ScriptVarT vTyp, BOOL bRefresh=TRUE );
 
//  	//ˢ��λ�����ݣ�����������ʱ������
// 	void update_Teach_window();
	//��ʼ���������ο�����ϵ�������������б�
	void InitVariableCombo();
	//���������Ƿ�ֻ��
	void SetReadOnly(BOOL bReadOnly);
 	//����ѡ���ʾ���˶��岹��ʽ����
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

	//���ÿؼ��е�λ������
	void SetPositionData(const PosInfo& cp);
	//��ȡ�ؼ��е�λ������
	bool GetPositionData(PosInfo& cp );
	//��ʾ��ؽ�λ��
	void set_axis_pos( const PosInfo& ap );
	//��ʾ�ѿ�������λ��
	void set_cart_pos( const PosInfo& cp );
 
	void set_variable ();
	void teach_pos_var ();
	void show_window_type (int nType = -1);

};

extern TeachDialog *teach_Dialog;

 