// Copyright 2016, ��˼��Ϣ�Ƽ��������ι�˾
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - ��˼�������˶�����ϵͳ

#pragma once
#include "PosInfo.h"
#include "UIDialogs/DialogType.h"

#include "BasicControls/labels.h"
#include "PoseTransform.h"
#include "BasicControls/IconComboBox.h"
#include "BasicControls/Label.h"
#include "LuaScript/ScriptVariableType.h"
#include "GridCtrl_src/TreeGridCtrl.h"
#include "YsibotsException.h"
#include "BasicControls/BtnST.h"
#include "BasicControls/CGroupBox.h"

class LocationDialog : public DialogType {
public:
	LocationDialog();
	virtual ~LocationDialog();
	 
	// ��������ϵ
	// ��1������������ϵ���ͣ���2����������������
	void SetType();                // ������������
	// ���ñ������
	void SetData(double *data);
	void SetSpeed(double speed);                        // �����ٶ�
	void SetMode(int mode);                             // ����ģʽ
	void SetJogSpeed(double speed);                     // ���õ㶯�ٶ�
	void set_curr_tool(); // for test
	void set_redraw_lable (BOOL b) { m_reDrawLable = b;  }

	const CartRef& get_curr_tool() { return curr_jog_tool; }

	void update_location_window(void);
	void get_cartref_data ( int nIndex,ScriptVarSetT* varset );
	void get_tool_data ( int nIndex, ScriptVarSetT* varset  );

	enum { SHOW_SERVO = 0, SHOW_JOINT = 1, SHOW_WORLD = 2, SHOW_WORK = 3 };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual int OnActivateChildWin(LPARAM);

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnMainButtonClicked(WPARAM,LPARAM); 
	afx_msg LRESULT OnUpdateLocationCommbox(WPARAM,LPARAM); 
// 	afx_msg void OnSelchangeCommboTool(); 
// 	afx_msg void OnSelchangeCommboUserCoord(); 
	afx_msg void OnSelchangeCommboTool(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnSelchangeCommboUserCoord(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnPowerButtonClicked(UINT id);

private:
	static LPCTSTR cart_label[8][2]; //= { { _T("X"), _T(" ����") }, { _T("Y"), _T(" ����") }, { _T("Z"), _T(" ����") },
	                                  //        { _T("A"), _T(" ��")   }, { _T("B"), _T(" ��")   }, { _T("C"), _T(" ��") } } ;
	//PoseTransform ptrans;
	enum { POWER_OFF, POWER_ON };

	BasicLabel title1_;
	BasicLabel title2_;
	BasicLabel title3_;
	BasicLabel title4_;
	BasicLabel title5_;
	BasicLabel title6_;

	CIconComboBox robot_name_;
 	CIconComboBox coordinate_;
	CIconComboBox tool_;
 
	CLabel robot_name_label;
	CLabel     m_act_speed;
	CLabel     m_robot_mode;
	CLabel     m_jog_speed;

	CartRef curr_jog_cart_ref;
	CartRef curr_jog_tool;
	int     show_coordinate_type;
	int     grid_row_count;
	int     m_iWidthGrid;
	int     m_iHeightGrid;

 	BOOL    m_reDrawLable;

	CTreeGridCtrl m_Grid;
	CButtonST     m_BtnServoPower[2];
	HICON         m_IconPower[2];
	CGroupBox     m_GroupBoxMotor;

 	void show_axis_pos ( const PosInfo& );
	void show_cart_pos ( const PosInfo&, const CartRef*, const CartPos* );
	void show_servo_pos ();

	void init_commbox();
	void add_tool(const char* toolname,DWORD dwData = 0);
	void delete_tool(const char* toolname);

	void add_user_coordinate(const char* usercoord,DWORD dwData = 0);
	void delete_user_coordinate(const char* usercoord);

	void set_speed_percent(double);
	BOOL create_tree_grid() throw (InvalidConfigurationException);
	BOOL initialize_tree_grid();
	void update_pos_grid_text (); 
	void update_pos_grid_data (double *data); 


public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

extern LocationDialog *location_dialog;