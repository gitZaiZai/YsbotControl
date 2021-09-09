#pragma once

#include "GroupFrame.h"
#include "DIO_module.h" 
#include "DIOSetting.h"
 
class CIoRegion : public CGroupFrame
{
public:
	CIoRegion(void);
	virtual ~CIoRegion(void);
	BOOL Create(CString caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd=NULL, UINT nID=65535);
	int creat_io_board ();
    void scroll_window (int nYAmount );

	//刷新IO状态,YsbotControlDlg定时器调用
	void Updata_IOState();
 	//取消所有强制
	void CleanForceValue();
 	//筛选显示IO类型，ioType:"0"-all,"1"-in,"2"-out
	int ShowIOType(int ioType);
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
	CDIOGroup* DIO_Group[3];
	CLIOGroup* LIO_Group;
	int m_nTotalByteNum;

	int calc_byte_num ( int ntype );
	void ShowLIO();
	 
protected:

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnDIOStnClicked(WPARAM wParam,LPARAM lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

private:
	enum StateID 
	{
		STATE_OFF = 0,
		STATE_ON = 1,
		STATE_FORCE_OFF = 2,
		STATE_FORCE_ON = 3	
	};

	CFont font_; 
	CDIO DIO_Unit;
	CDIOSetting *DIO_Setting;

	int mccX_WordSize;	
	int mccY_WordSize;	
	int mccX_Num;	
	int mccY_Num;
	int mccLocal_input_size;
	bool mcc_is_connected;
	int IN_num;
	int OUT_num;

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

	void GetLocalIO();
	void GetGPIO(); 
	void GetCardIO();                //获取Motion card IO 状态
	void GetFieldBusIO();           //获取FieldBus IO 状态  
 	void Reflash_Dout();
	void UpdateSurface();

};

