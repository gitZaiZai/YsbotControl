
//请在创建世界模型worldmodel之后在创建此窗口

#pragma once
//#include "StaticImage.h"
// #include "Edit_class.h"
// #include "ComboxBox_class.h"
#include "StaticTransparent.h"
#include "DIO_module.h"
#include "DIOSetting.h"
#include "../UIDialogs/DialogType.h"
#include <vector>
#include "../ConfigReader.h"
#include "../SkinScrollBar/skinscrollbar.h"
#include "../SkinScrollBar/SkinComboBox.h"
#include "../SkinScrollBar/skinscrollwnd.h" 

class InoutDialog : public DialogType {
public:
	InoutDialog(const ConfigReader&, HBITMAP hBmpScrollBar = NULL, 
		HBITMAP hBmpPlus  = NULL, 
		HBITMAP hBmpMinus = NULL);
	virtual ~InoutDialog();
 

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK(); 
// 	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg LRESULT OnDIOStnClicked(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnMainButtonClicked(WPARAM,LPARAM); 
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);


public:

	//按下底部窗口按钮,buttomDLG调用
	void BottomDLG_BntClicked(int ID);

	//刷新IO状态,YsbotControlDlg定时器调用
	void Updata_IOState();

	//取消所有强制
	void CleanForceValue();

	//筛选显示IO类型，ioType:"0"-all,"1"-in,"2"-out
	void ShowIOType(int ioType);

	//数字输入
	int Din(const int port, bool* val);
	//数字输出
	int Dout(const int port, const bool val);

	//Remote_IO
	//数字输入
	int Mcc_in(const int port, bool* val);
	//数字输出
	int Mcc_out(const int port, const bool val);

	//GP_IO
	//数字输入
	int GP_in(const int port, bool* val);
	//数字输出
	int GP_out(const int port, const bool val);

	int LIO_in(const int port, bool* val);
	//数字输出
	int LIO_out(const int port, const bool val);

	int get_virtual_input( const int deviceID, const int port, bool* val );
	bool is_forced_output( const int deviceID, const int port );

private:

	enum StateID 
	{
		STATE_OFF = 0,
		STATE_ON = 1,
		STATE_FORCE_OFF = 2,
		STATE_FORCE_ON = 3	
	};
 
	CDIO DIO_Unit;
	CDIOGroup *DIO_Group[3];
	CDIOSetting *DIO_Setting;
	CLIOGroup LIO_Group;

	bool is_MWM_enable;
	int X_WordSize;
	int MX_WordSize;
	int Y_WordSize;
	int MY_WordSize;
	int X_Num;
	int MX_Num;
	int Y_Num;
	int MY_Num;
	bool FeildBus_is_connected;
	int X_totalSize;
	int Y_totalSize;
	int mccX_WordSize;	
	int mccY_WordSize;	
	int mccX_Num;	
	int mccY_Num;
	int mccLocal_input_size;
	bool mcc_is_connected;
	int IN_num;
	int OUT_num;

	HBITMAP	m_hBmpScrollBar;
	HBITMAP	m_hBmpPlus;				 
	HBITMAP	m_hBmpMinus;
	long    m_YCharHeigth;
	///////////////////////////////////////////////////////////////
	//void BtnChlChecked(int BtnID);
	//void BtnFun(int BtnID);			// 调用通道按钮指向的功能	
	////void ShowStatechk( );			//刷新强制状态选择
	void GetLocalIO();
	void GetGPIO(); 
	void GetCardIO();                //获取Motion card IO 状态
	void GetFieldBusIO();           //获取FieldBus IO 状态  
	//void GetInOutState();           //获取IO 状态
	//void ProcessIO();               //处理 IO 信号
	void Reflash_Dout();
	void ShowLIO();
	//void ShowIOstate();             //显示IO 状态
	//void SetStatebmp(int bmpID,int State_value);
	void on_system_scroll ( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void on_custom_scroll ( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
 	CSkinScrollBar* m_wndScrollBar;

	bool m_bUseCustomScrollBar;

};
extern InoutDialog *Inout_Dialog;