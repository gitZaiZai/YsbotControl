
//���ڴ�������ģ��worldmodel֮���ڴ����˴���

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

	//���µײ����ڰ�ť,buttomDLG����
	void BottomDLG_BntClicked(int ID);

	//ˢ��IO״̬,YsbotControlDlg��ʱ������
	void Updata_IOState();

	//ȡ������ǿ��
	void CleanForceValue();

	//ɸѡ��ʾIO���ͣ�ioType:"0"-all,"1"-in,"2"-out
	void ShowIOType(int ioType);

	//��������
	int Din(const int port, bool* val);
	//�������
	int Dout(const int port, const bool val);

	//Remote_IO
	//��������
	int Mcc_in(const int port, bool* val);
	//�������
	int Mcc_out(const int port, const bool val);

	//GP_IO
	//��������
	int GP_in(const int port, bool* val);
	//�������
	int GP_out(const int port, const bool val);

	int LIO_in(const int port, bool* val);
	//�������
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
	//void BtnFun(int BtnID);			// ����ͨ����ťָ��Ĺ���	
	////void ShowStatechk( );			//ˢ��ǿ��״̬ѡ��
	void GetLocalIO();
	void GetGPIO(); 
	void GetCardIO();                //��ȡMotion card IO ״̬
	void GetFieldBusIO();           //��ȡFieldBus IO ״̬  
	//void GetInOutState();           //��ȡIO ״̬
	//void ProcessIO();               //���� IO �ź�
	void Reflash_Dout();
	void ShowLIO();
	//void ShowIOstate();             //��ʾIO ״̬
	//void SetStatebmp(int bmpID,int State_value);
	void on_system_scroll ( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void on_custom_scroll ( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
 	CSkinScrollBar* m_wndScrollBar;

	bool m_bUseCustomScrollBar;

};
extern InoutDialog *Inout_Dialog;