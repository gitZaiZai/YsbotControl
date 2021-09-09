#pragma once
#include "StaticBmp_Btn.h"
#include "StaticTransparent.h"
#include "GroupFrame.h"


//#define IDD_DIO_MODULE 5201

enum DIOType
{
	DIO_Unknow= 0,
	DIO_Input= 1,
	DIO_Output= 2

};

enum DIOStateValue
{
	DIO_Off=0,
	DIO_On=1,
	DIO_ForceOff=2,
	DIO_ForceOn=3
};

struct DIOInfo
{
	CString GroupName;
	bool Value;
	bool Forced;
	bool ForceValue;
	bool DisplayValue;
	int Type;
	int Index;
	bool Writeable;
};

struct DIOGroupInfo
{
	CString GroupName;
	int InNum;
	int InIndex;
	int OutNum;
	int OutIndex;
};


class CDIO: public CStaticTran
{
	DECLARE_DYNAMIC(CDIO)
public:
	virtual ~CDIO();
	CDIO();
	//��ȡ֮ǰIO״ֵ̬
	int GetStateValue() {return StateValue;};
	 //���㵱ǰIO״ֵ̬
	int GetCurrentState();
	//��ȡIO���ͣ�1���룬2���
	//int GetDIOType() {return Type;};
	////����IO���ͣ�1���룬2���
	//void SetDIOType(int type) {Type=type;};
	////��ȡIO���
	//int GetDIOIndex() {return Index;};
	////����IO���
	//void SetDIOIndex(int index) {Index=index;};
	////��ȡIO�����ı�
	CString GetDIOText() {return DIOText;};
	//����IO�����ı�
	void SetDIOText(CString text);

	DIOInfo& GetDIOinfo() {return DIOinfo_;};
	//����IO״ָ̬ʾͼ
	void SetDIOBitmap(HBITMAP bmp);
	//ˢ����ʾIO״ָ̬ʾͼ��IOStateΪǿ��ָ��ֵ��0��1��2��3��
	//IOStateС��0��IO��ǰֵˢ��
	void ShowDIOBitmap(int IOState=-1);
	//
	void SetDIOFont(int fontHeight,CString fontStr);
	//
	BOOL Create( DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID,DIOInfo IOinfo);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()

	virtual void Initialize(const RECT& rect, CWnd* pParentWnd, UINT nID);
private:
	CStaticTran DIOLabel;
	CStaticBmp_Btn DIOImage;
	int StateValue;
	CString DIOText;
	int Type;
	int Index;
	bool Writeable;
	HBITMAP DIObmp[4];
	DIOInfo DIOinfo_;
public:
	bool Value;
	bool Forced;
	bool ForceValue;
	bool DisplayValue;


};



class CDIOGroup :public CGroupFrame
{
	DECLARE_DYNAMIC(CDIOGroup)
public:
	virtual ~CDIOGroup();
	CDIOGroup(int widthLeft = 0);

	BOOL Create(LPCTSTR lpszText,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
		const DIOGroupInfo GInfo,UINT nID=65535);
	void SetDIOFont(int FontHeight);//��������߶�
	void ShowDIOBitmap();//ˢ��DIO״̬ͼƬ
	void CleanForceValue(int ioType=0,int index=-1);//���ǿ��ֵ;ioType:"0"-clean all,"1"-in,"2"-out;index:"<0"-all,"<num"-index
	void ShowIOType(int ioType);//ɸѡ��ʾIO���ͣ�ioType:"0"-all,"1"-in,"2"-out
	void GetMyRect(LPRECT lpRect){GetWindowRect(lpRect);};
	void SetMyPos(LPRECT lpRect,BOOL bRepaint=TRUE){ MoveWindow(lpRect,bRepaint);}
	virtual void OnInit(){;};
	//BOOL ShowWindow(BOOL bobCmdShow){return CStaticTran::ShowWindow(bobCmdShow);}
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

	CString GroupCaption;
	CStaticTran DIOGroupCaption;
	CStaticTran DIOSplitLine;

	DIOGroupInfo GroupInfo;
	int InByteSize;
	int OutByteSize;
	CRect m_GroupRect;
	int m_widthLeft;
public:
	CDIO *Din;
	CDIO *Dout;

};



class CLIOGroup :public CGroupFrame
{
	DECLARE_DYNAMIC(CLIOGroup)
public:
	virtual ~CLIOGroup();
	CLIOGroup();

	BOOL Create(LPCTSTR lpszText,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
		const DIOGroupInfo GInfo,UINT nID=65535);
	void SetDIOFont(int FontHeight);//��������߶�
	void ShowDIOBitmap();//ˢ��DIO״̬ͼƬ
	void CleanForceValue(int ioType=0,int index=-1);//���ǿ��ֵ;ioType:"0"-clean all,"1"-in,"2"-out;index:"<0"-all,"<num"-index
	void ShowIOType(int ioType);//ɸѡ��ʾIO���ͣ�ioType:"0"-all,"1"-in,"2"-out
	void GetMyRect(LPRECT lpRect){GetWindowRect(lpRect);};
	void SetMyPos(LPRECT lpRect,BOOL bRepaint=TRUE){ MoveWindow(lpRect,bRepaint);}
	virtual void OnInit(){;};
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

	CString GroupCaption;
	CStaticTran DIOGroupCaption;
	CStaticTran DIOSplitLine;
	DIOGroupInfo GroupInfo;
	int InByteSize;
	int OutByteSize;
	CRect m_GroupRect;

public:
	CDIO Din[40];
	CDIO Dout[16];

};