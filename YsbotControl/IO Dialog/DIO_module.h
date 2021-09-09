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
	//获取之前IO状态值
	int GetStateValue() {return StateValue;};
	 //计算当前IO状态值
	int GetCurrentState();
	//获取IO类型；1输入，2输出
	//int GetDIOType() {return Type;};
	////设置IO类型；1输入，2输出
	//void SetDIOType(int type) {Type=type;};
	////获取IO序号
	//int GetDIOIndex() {return Index;};
	////设置IO序号
	//void SetDIOIndex(int index) {Index=index;};
	////获取IO标题文本
	CString GetDIOText() {return DIOText;};
	//设置IO标题文本
	void SetDIOText(CString text);

	DIOInfo& GetDIOinfo() {return DIOinfo_;};
	//设置IO状态指示图
	void SetDIOBitmap(HBITMAP bmp);
	//刷新显示IO状态指示图，IOState为强制指定值（0，1，2，3）
	//IOState小于0则按IO当前值刷新
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
	void SetDIOFont(int FontHeight);//设置字体高度
	void ShowDIOBitmap();//刷新DIO状态图片
	void CleanForceValue(int ioType=0,int index=-1);//清除强制值;ioType:"0"-clean all,"1"-in,"2"-out;index:"<0"-all,"<num"-index
	void ShowIOType(int ioType);//筛选显示IO类型；ioType:"0"-all,"1"-in,"2"-out
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
	void SetDIOFont(int FontHeight);//设置字体高度
	void ShowDIOBitmap();//刷新DIO状态图片
	void CleanForceValue(int ioType=0,int index=-1);//清除强制值;ioType:"0"-clean all,"1"-in,"2"-out;index:"<0"-all,"<num"-index
	void ShowIOType(int ioType);//筛选显示IO类型；ioType:"0"-all,"1"-in,"2"-out
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