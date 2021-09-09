#include "stdafx.h"
#include "math.h"
#include "DIO_module.h"
#include "GroupFrame.h"

#define InLeft 15
#define InTop 30
#define IOWidth 45
#define IOHeight 45
#define OutLeft 15
//#define OutTop InTop+InByteSize*(IOHeight+15);
//#define space_width	(GroupWidth-IOWidth*8)/8;
#define space_height 15


IMPLEMENT_DYNAMIC(CDIO, CStaticTran)

	BEGIN_MESSAGE_MAP(CDIO, CStaticTran)
		ON_WM_CREATE()
		ON_WM_CTLCOLOR()
	END_MESSAGE_MAP()

 CDIO::~CDIO()
 {

 }

 CDIO::CDIO()
 {
	 StateValue=0;
	 Value=false;
	 Forced=false;
	 ForceValue=false;
	 DisplayValue=false;
 }

 int CDIO::OnCreate(LPCREATESTRUCT lpCreateStruct) 
 {
	return 0;
	
 }

void CDIO::SetDIOText(CString text)
{
	DIOLabel.SetWindowTextW(text);
	DIOText=text;	
}

void  CDIO::SetDIOFont(int fontHeight,CString fontStr)
{
	DIOLabel.SetFont(fontHeight,fontStr);
}

int CDIO::GetCurrentState() 
{
	int SValue;
	if (Forced)
	{
		if (ForceValue)			
			SValue=0x03;
		else
			SValue=0x02;
	}
	else
	{
		if (Value)		
			SValue=0x01;
		else
			SValue=0x00;
	}
	
	return SValue;
}

void CDIO::SetDIOBitmap(HBITMAP bmp)
{
	DIOImage.SetBitmap(bmp);
}

void CDIO::ShowDIOBitmap(int IOState)
{
	int sValue=GetCurrentState();
	if (StateValue==sValue) return;  //如果状态图像不需要更换，则不刷新图片
	DIOImage.SetBitmap(DIObmp[sValue]);
	StateValue=sValue;
}

void CDIO:: Initialize(const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	//创建标题
	CString str,IndexStr;
	char IndexChr[5];
	sprintf(IndexChr,"%d",Index);
	IndexStr=IndexChr;
	if(Type==1)
		str="DI:";
	else{
		if(Type==2)
			str="DO:";
		else
			str="D?:";}
	str+=IndexStr;
	DIOLabel.Create(str,WS_CHILD | WS_VISIBLE | SS_CENTER,CRect(0,0,rect.right-rect.left,20),this);
	////创建图标
	DWORD ss_style=0;
	if (Writeable) ss_style=SS_NOTIFY;
	DIOImage.Create(WS_CHILD | WS_VISIBLE |SS_BITMAP |ss_style,
		CRect(2,20,26,44),this,Type,Index);
	DIObmp[0] = (HBITMAP)::LoadImageW(AfxGetInstanceHandle(),_T(".\\res\\OFF.BMP")
		,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	DIObmp[1] = (HBITMAP)::LoadImageW(AfxGetInstanceHandle(),_T(".\\res\\On.BMP")
		,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	DIObmp[2] = (HBITMAP)::LoadImageW(AfxGetInstanceHandle(),_T(".\\res\\OFF_force.BMP")
		,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	DIObmp[3] = (HBITMAP)::LoadImageW(AfxGetInstanceHandle(),_T(".\\res\\On_force.BMP")
		,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);

	DIOImage.SetBitmap(DIObmp[0]);
}

BOOL CDIO::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID,DIOInfo IOinfo)
{
	CStatic::Create(_T(""), dwStyle, rect, pParentWnd, nID);
	DIOinfo_=IOinfo;
	Type=IOinfo.Type;
	Index=IOinfo.Index;
	Writeable=IOinfo.Writeable;
	Initialize(rect, pParentWnd, nID);
	return TRUE;
}

HBRUSH CDIO::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr =CStatic::OnCtlColor(pDC, pWnd, nCtlColor);

	//CTLCOLOR_STATIC为 CStatic
	//CTLCOLOR_EDIT
	//CTLCOLOR_LISTBOX
	//CTLCOLOR_BTN
	//CTLCOLOR_DLG  等等

	if ( nCtlColor == CTLCOLOR_STATIC )
	{
		//static HBRUSH hbrStatic = ::CreateSolidBrush(RGB(255, 0, 255));
		//COLOR是你想设置的背景颜色 此处必须为静态变量，否则不能实现
		pDC->SetTextColor(RGB(255, 0, 255));
		pDC->SetBkMode(TRANSPARENT);
		return hbr ; //返回该刷
	}
	return hbr;
}





// class CDIOGoup
IMPLEMENT_DYNAMIC(CDIOGroup, CGroupFrame)

	BEGIN_MESSAGE_MAP(CDIOGroup, CGroupFrame)
		ON_WM_CREATE()
	END_MESSAGE_MAP()

	
	BOOL CDIOGroup::Create(LPCTSTR lpszText,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, DIOGroupInfo GInfo,UINT nID)
	{
		GroupCaption=lpszText;
		GroupInfo=GInfo;
		InByteSize=static_cast<int>( ceil( static_cast<double>(GroupInfo.InNum) / 8.0 ) );
		OutByteSize=static_cast<int>( ceil( static_cast<double>(GroupInfo.OutNum) / 8.0 ) );
		CRect Prect;
		pParentWnd->GetClientRect(Prect);
		int Pwidth=Prect.Width();
		Prect.left=rect.left;
		Prect.right=Pwidth - m_widthLeft;
		Prect.top=rect.top;
		Prect.bottom=rect.top+40+(InByteSize+OutByteSize)*(IOHeight+space_height);	
		m_GroupRect=Prect;
		CStatic::Create(lpszText, dwStyle, Prect, pParentWnd, nID);

		if(0==InByteSize+OutByteSize) {ShowWindow(SW_HIDE);}
		//Initialize(rect, pParentWnd, nID);
		return TRUE;
	}

	CDIOGroup::CDIOGroup(int widthLeft):m_widthLeft(widthLeft)
	{

	}

	CDIOGroup::~CDIOGroup()
	{
		delete[] Din;
		delete[] Dout;
	}

	int CDIOGroup::OnCreate(LPCREATESTRUCT lpCreateStruct) 
	{		
		//CRect GroupRect;
		//this->GetClientRect(GroupRect);
		int GroupWidth=m_GroupRect.Width();

		Din=new CDIO[GroupInfo.InNum];
		Dout=new CDIO[GroupInfo.OutNum];
		
		//int InLeft=15;
		//int InTop=30;
		//int IOWidth=45;
		//int IOHeight=45;
		//int OutLeft=InLeft;
		int OutTop=InTop+InByteSize*(IOHeight+space_height);
		int space_width=(GroupWidth-IOWidth*8)/8;
		//int space_height=15;

		DIOInfo IOinfo;
		//创建输入模块
		int x0,y0;
		x0 = InLeft;
		y0 = -25;
		for (int i=0;i<GroupInfo.InNum;i++)
		{
			if (i % 8 == 0) {
				x0 = InLeft;
				y0 += IOHeight + space_height;
			} else {
				x0 += IOWidth + space_width;
			}
			/*Din[i].Create(WS_CHILD | WS_VISIBLE | SS_CENTER,CRect(x0,y0,x0+IOWidth,y0+IOHeight),
			this,65535,1,GroupInfo.InIndex+i);*/
			
			IOinfo.Type=DIO_Input;
			IOinfo.Index=i;
			IOinfo.GroupName=GroupCaption;
			Din[i].Create(WS_CHILD | WS_VISIBLE | SS_CENTER,CRect(x0,y0,x0+IOWidth,y0+IOHeight),
				this,65535,IOinfo);
		}

		DIOSplitLine.Create(_T(""),WS_CHILD | WS_VISIBLE |WS_BORDER,CRect(4,OutTop+1,GroupWidth-8,OutTop+2),this);
		//创建输出模块
		x0 = OutLeft;
		y0 = OutTop-45;	
		for (int i=0;i<GroupInfo.OutNum;i++)
		{
			if (i % 8 == 0) {
				x0 = OutLeft;
				y0 += IOHeight + space_height;
			} else {
				x0 += IOWidth + space_width;
			}

			IOinfo.Type=DIO_Output;
			IOinfo.Index=i;
			IOinfo.GroupName=GroupCaption;
			Dout[i].Create(WS_CHILD | WS_VISIBLE | SS_CENTER,CRect(x0,y0,x0+IOWidth,y0+IOHeight),
				this,65535,IOinfo);
		}


		OnInit();
		return 0;
	}

	void  CDIOGroup::SetDIOFont(int fontHeight)
	{
		DIOGroupCaption.SetFont(fontHeight,_T("宋体"));
	}

	void CDIOGroup::ShowDIOBitmap()
	{

		for (int i=0;i<GroupInfo.InNum;i++)
		{
			Din[i].ShowDIOBitmap();
		}
		for (int i=0;i<GroupInfo.OutNum;i++)
		{
			Dout[i].ShowDIOBitmap();
		}
		//CWindowDC dc(this);
		//DrawGroupLine(&dc,RGB(1,1,1),10,10,200,20);
	}

	void CDIOGroup::ShowIOType(int ioType)
	{
		//CRect GroupRect;

		int GroupWidth=m_GroupRect.Width();
		int InByte=static_cast<int>( ceil( static_cast<double>(GroupInfo.InIndex) / 8.0 ) );
		int OutByte=static_cast<int>( ceil( static_cast<double>(GroupInfo.OutIndex) / 8.0 ) );
		CRect Prect;

		int OutTop=InTop+InByteSize*(IOHeight+space_height);
		int space_width=(GroupWidth-IOWidth*8)/8;
		//int space_height=15;

		//重设输入模块位置
		int x0,y0;
		switch(ioType)
		{
			case 0:
				Prect=m_GroupRect;
				Prect.bottom=m_GroupRect.top+40+(InByteSize+OutByteSize)*(IOHeight+space_height);
				MoveWindow(&Prect);

				//重设输入模块位置
				x0 = InLeft;
				y0 = -25;
				for (int i=0;i<GroupInfo.InNum;i++)
				{
					if (i % 8 == 0) {
						x0 = InLeft;
						y0 += IOHeight + space_height;
					} else {
						x0 += IOWidth + space_width;
					}
					Din[i].MoveWindow(x0,y0,IOWidth,IOHeight);
					Din[i].ShowWindow(SW_SHOW);
				}
				DIOSplitLine.ShowWindow(SW_SHOW);
				//重设输出模块位置
				x0 = OutLeft;
				y0 = OutTop-45;	
				for (int i=0;i<GroupInfo.OutNum;i++)
				{
					if (i % 8 == 0) {
						x0 = OutLeft;
						y0 += IOHeight + space_height;
					} else {
						x0 += IOWidth + space_width;
					}
					Dout[i].MoveWindow(x0,y0,IOWidth,IOHeight);
					Dout[i].ShowWindow(SW_SHOW);
				}
				ShowWindow(SW_SHOW);
				break;
			case 1:
				Prect=m_GroupRect;
				Prect.top=m_GroupRect.top-OutByte*(IOHeight+space_height);
				Prect.bottom=Prect.top+40+(InByteSize)*(IOHeight+space_height);
				MoveWindow(Prect.left,Prect.top,Prect.Width(),Prect.Height());

				//重设输入模块位置
				x0 = InLeft;
				y0 = -25;
				for (int i=0;i<GroupInfo.InNum;i++)
				{
					if (i % 8 == 0) {
						x0 = InLeft;
						y0 += IOHeight + space_height;
					} else {
						x0 += IOWidth + space_width;
					}
					Din[i].MoveWindow(x0,y0,IOWidth,IOHeight);
					Din[i].ShowWindow(SW_SHOW);
				}
				for (int i=0;i<GroupInfo.OutNum;i++)
				{					
					Dout[i].ShowWindow(SW_HIDE);
				}
				ShowWindow(SW_SHOW);
				DIOSplitLine.ShowWindow(SW_HIDE);
				break;
			case 2:
				Prect=m_GroupRect;
				Prect.top=m_GroupRect.top-InByte*(IOHeight+space_height);
				Prect.bottom=Prect.top+40+(OutByteSize)*(IOHeight+space_height);
				MoveWindow(Prect.left,Prect.top,Prect.Width(),Prect.Height());

				//重设输入模块位置
				x0 = InLeft;
				y0 = -25;
				for (int i=0;i<GroupInfo.InNum;i++)
				{			
					Din[i].ShowWindow(SW_HIDE);
				}
				for (int i=0;i<GroupInfo.OutNum;i++)
				{
					if (i % 8 == 0) {
						x0 = InLeft;
						y0 += IOHeight + space_height;
					} else {
						x0 += IOWidth + space_width;
					}
					Dout[i].MoveWindow(x0,y0,IOWidth,IOHeight);
					Dout[i].ShowWindow(SW_SHOW);
				}
				ShowWindow(SW_SHOW);
				DIOSplitLine.ShowWindow(SW_HIDE);
				break;
			default:
				for (int i=0;i<GroupInfo.InNum;i++)
				{			
					Din[i].ShowWindow(SW_HIDE);
				}
				for (int i=0;i<GroupInfo.OutNum;i++)
				{
					Dout[i].ShowWindow(SW_SHOW);
				}
				ShowWindow(SW_HIDE);
				DIOSplitLine.ShowWindow(SW_HIDE);
				break;
		}
		if(0==InByteSize+OutByteSize) {ShowWindow(SW_HIDE);}
		Invalidate();
		

	}

	void  CDIOGroup::CleanForceValue(int ioType,int index)
	{
		switch(ioType)
		{
			case 0:			//清除全部强制
				for(int i=0;i<GroupInfo.InNum;i++)
				{
					Din[i].Forced=0;
					Din[i].ForceValue=0;
					Din[i].ShowDIOBitmap();
				}		
				for(int i=0;i<GroupInfo.OutNum;i++)
				{
					Dout[i].Forced=0;
					Dout[i].ForceValue=0;
					Dout[i].ShowDIOBitmap();
				}
				break;
			case 1:		//清除输入强制
				if (index<0)
				{	
					for(int i=0;i<GroupInfo.InNum;i++)
					{
						Din[i].Forced=0;
						Din[i].ForceValue=0;
						Din[i].ShowDIOBitmap();
					}	
				}
				else if(index<GroupInfo.InNum)
				{
					Din[index].Forced=0;
					Din[index].ForceValue=0;
					Din[index].ShowDIOBitmap();
				}
				break;
			case 2:		//清除输出强制
				if (index<0)
				{
					for(int i=0;i<GroupInfo.OutNum;i++)
					{
						Dout[i].Forced=0;
						Dout[i].ForceValue=0;
						Dout[i].ShowDIOBitmap();
					}
				}
				else if (index<GroupInfo.OutNum)
				{
					Dout[index].Forced=0;
					Dout[index].ForceValue=0;
					Dout[index].ShowDIOBitmap();
				}
				break;
			default:
				break;		

		}
		
	}



////CLIOGroup class
	IMPLEMENT_DYNAMIC(CLIOGroup, CGroupFrame)

		BEGIN_MESSAGE_MAP(CLIOGroup, CGroupFrame)
			ON_WM_CREATE()
		END_MESSAGE_MAP()

	CLIOGroup::CLIOGroup()
	{

	}

	CLIOGroup::~CLIOGroup()
	{

	}

	BOOL CLIOGroup::Create(LPCTSTR lpszText,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, DIOGroupInfo GInfo,UINT nID)
	{
		GroupCaption=lpszText;
		GroupInfo=GInfo;
		InByteSize=static_cast<int>( ceil( static_cast<double>(GroupInfo.InNum) / 8.0 ) );
		OutByteSize=static_cast<int>( ceil( static_cast<double>(GroupInfo.OutNum) / 8.0 ) );

		CRect Prect;
		pParentWnd->GetClientRect(Prect);
		int Pwidth=Prect.Width();
		Prect.left=rect.left;
		Prect.right=Pwidth;
		Prect.top=rect.top;
		Prect.bottom=rect.top+40+(InByteSize+OutByteSize)*(IOHeight+space_height);
		m_GroupRect=Prect;
		CStatic::Create(lpszText, dwStyle, Prect, pParentWnd, nID);
		//Initialize(rect, pParentWnd, nID);
		return TRUE;
	}

	int CLIOGroup::OnCreate(LPCREATESTRUCT lpCreateStruct) 
	{		

		int GroupWidth=m_GroupRect.Width();
		
		int OutTop=InTop+InByteSize*(IOHeight+space_height);
		int space_width=(GroupWidth-IOWidth*8)/8;

		DIOInfo IOinfo;
		CString str;
		//创建报警输入
		int x0,y0;
		x0 = InLeft;
		y0 = -25;
		for (int i=0;i<GroupInfo.InNum;i++)
		{
			if (i % 8 == 0) {
				x0 = InLeft;
				y0 += IOHeight + space_height;
			} else {
				x0 += IOWidth + space_width;
			}			
			IOinfo.Type=DIO_Input;
			IOinfo.Index=i;
			IOinfo.GroupName=GroupCaption;
			//IOinfo.Writeable=false;
			Din[i].Create(WS_CHILD | WS_VISIBLE | SS_CENTER,CRect(x0,y0,x0+IOWidth,y0+IOHeight),
				this,65535,IOinfo);
			if (i>=0 && i<8)
			{
				str.Format(_T("Alm%d"),i);
				Din[i].SetDIOText(str);
			}

			if (i>=8 && i<16)
			{
				str.Format(_T("Home%d"),i-8);
				Din[i].SetDIOText(str);
			}

			if (i>=16 && i<24)
			{
				str.Format(_T("OT+%d"),i-16);
				Din[i].SetDIOText(str);
			}

			if (i>=24 && i<32)
			{
				str.Format(_T("OT-%d"),i-24);
				Din[i].SetDIOText(str);
			}

			if (i>=32 && i<40)
			{
				str.Format(_T("ES%d"),i-32);
				Din[i].SetDIOText(str);
			}
		
		}

		DIOSplitLine.Create(_T(""),WS_CHILD | WS_VISIBLE |WS_BORDER,CRect(4,OutTop+1,GroupWidth-8,OutTop+2),this);

		//创建输出模块
		x0 = OutLeft;
		y0 = OutTop-45;	
		for (int i=0;i<GroupInfo.OutNum;i++)
		{
			if (i % 8 == 0) {
				x0 = OutLeft;
				y0 += IOHeight + space_height;
			} else {
				x0 += IOWidth + space_width;
			}
			/*Dout[i].Create(WS_CHILD | WS_VISIBLE | SS_CENTER,CRect(x0,y0,x0+IOWidth,y0+IOHeight),
			this,65535,2,GroupInfo.OutIndex+i);*/
			IOinfo.Type=DIO_Output;
			IOinfo.Index=i;
			IOinfo.GroupName=GroupCaption;
			//IOinfo.Writeable=false;
			Dout[i].Create(WS_CHILD | WS_VISIBLE | SS_CENTER,CRect(x0,y0,x0+IOWidth,y0+IOHeight),
				this,65535,IOinfo);
			if (i>=0 && i<8)
			{
				str.Format(_T("SvOn%d"),i);
				Dout[i].SetDIOText(str);
			}

			if (i>=8 && i<16)
			{
				str.Format(_T("Prdy%d"),i-8);
				Dout[i].SetDIOText(str);
			}
		}

		return 0;
	}


	void  CLIOGroup::SetDIOFont(int fontHeight)
	{
		DIOGroupCaption.SetFont(fontHeight,_T("宋体"));
	}

	void CLIOGroup::ShowDIOBitmap()
	{

		for (int i=0;i<GroupInfo.InNum;i++)
		{
			Din[i].ShowDIOBitmap();
		}
		for (int i=0;i<GroupInfo.OutNum;i++)
		{
			Dout[i].ShowDIOBitmap();
		}

	}

	void CLIOGroup::ShowIOType(int ioType)
	{
		
		int GroupWidth=m_GroupRect.Width();
		int InByte=static_cast<int>( ceil( static_cast<double>(GroupInfo.InIndex) / 8.0 ) );
		int OutByte=static_cast<int>( ceil( static_cast<double>(GroupInfo.OutIndex) / 8.0 ) );
		CRect Prect;

		int OutTop=InTop+InByteSize*(IOHeight+space_height);
		int space_width=(GroupWidth-IOWidth*8)/8;
		//int space_height=15;

		//重设输入模块位置
		int x0,y0;
		switch(ioType)
		{
			case 0:
				Prect=m_GroupRect;
				Prect.bottom=m_GroupRect.top+40+(InByteSize+OutByteSize)*(IOHeight+space_height);
				MoveWindow(&Prect);

				//重设输入模块位置
				x0 = InLeft;
				y0 = -25;
				for (int i=0;i<GroupInfo.InNum;i++)
				{
					if (i % 8 == 0) {
						x0 = InLeft;
						y0 += IOHeight + space_height;
					} else {
						x0 += IOWidth + space_width;
					}
					Din[i].MoveWindow(x0,y0,IOWidth,IOHeight);
					Din[i].ShowWindow(SW_SHOW);
				}
				//重设输出模块位置
				x0 = OutLeft;
				y0 = OutTop-45;	
				for (int i=0;i<GroupInfo.OutNum;i++)
				{
					if (i % 8 == 0) {
						x0 = OutLeft;
						y0 += IOHeight + space_height;
					} else {
						x0 += IOWidth + space_width;
					}
					Dout[i].MoveWindow(x0,y0,IOWidth,IOHeight);
					Dout[i].ShowWindow(SW_SHOW);
				}
				ShowWindow(SW_SHOW);
				DIOSplitLine.ShowWindow(SW_SHOW);
				break;
			case 1:
				Prect=m_GroupRect;
				Prect.top=m_GroupRect.top-OutByte*(IOHeight+space_height);
				Prect.bottom=Prect.top+40+(InByteSize)*(IOHeight+space_height);
				MoveWindow(Prect.left,Prect.top,Prect.Width(),Prect.Height());

				//重设输入模块位置
				x0 = InLeft;
				y0 = -25;
				for (int i=0;i<GroupInfo.InNum;i++)
				{
					if (i % 8 == 0) {
						x0 = InLeft;
						y0 += IOHeight + space_height;
					} else {
						x0 += IOWidth + space_width;
					}
					Din[i].MoveWindow(x0,y0,IOWidth,IOHeight);
					Din[i].ShowWindow(SW_SHOW);
				}
				for (int i=0;i<GroupInfo.OutNum;i++)
				{					
					Dout[i].ShowWindow(SW_HIDE);
				}
				ShowWindow(SW_SHOW);
				DIOSplitLine.ShowWindow(SW_HIDE);
				break;
			case 2:
				Prect=m_GroupRect;
				Prect.top=m_GroupRect.top-InByte*(IOHeight+space_height);
				Prect.bottom=Prect.top+40+(OutByteSize)*(IOHeight+space_height);
				MoveWindow(Prect.left,Prect.top,Prect.Width(),Prect.Height());

				//重设输入模块位置
				x0 = InLeft;
				y0 = -25;
				for (int i=0;i<GroupInfo.InNum;i++)
				{			
					Din[i].ShowWindow(SW_HIDE);
				}
				for (int i=0;i<GroupInfo.OutNum;i++)
				{
					if (i % 8 == 0) {
						x0 = InLeft;
						y0 += IOHeight + space_height;
					} else {
						x0 += IOWidth + space_width;
					}
					Dout[i].MoveWindow(x0,y0,IOWidth,IOHeight);
					Dout[i].ShowWindow(SW_SHOW);
				}
				ShowWindow(SW_SHOW);
				DIOSplitLine.ShowWindow(SW_HIDE);
				break;
			default:
				for (int i=0;i<GroupInfo.InNum;i++)
				{			
					Din[i].ShowWindow(SW_HIDE);
				}
				for (int i=0;i<GroupInfo.OutNum;i++)
				{
					Dout[i].ShowWindow(SW_SHOW);
				}
				ShowWindow(SW_HIDE);
				DIOSplitLine.ShowWindow(SW_HIDE);
				break;
		}
		Invalidate();

	}

	void  CLIOGroup::CleanForceValue(int ioType,int index)
	{
		switch(ioType)
		{
			case 0:			//清除全部强制
				for(int i=0;i<GroupInfo.InNum;i++)
				{
					Din[i].Forced=0;
					Din[i].ForceValue=0;
					Din[i].ShowDIOBitmap();
				}		
				for(int i=0;i<GroupInfo.OutNum;i++)
				{
					Dout[i].Forced=0;
					Dout[i].ForceValue=0;
					Dout[i].ShowDIOBitmap();
				}
				break;
			case 1:		//清除输入强制
				if (index<0)
				{	
					for(int i=0;i<GroupInfo.InNum;i++)
					{
						Din[i].Forced=0;
						Din[i].ForceValue=0;
						Din[i].ShowDIOBitmap();
					}	
				}
				else if(index<GroupInfo.InNum)
				{
					Din[index].Forced=0;
					Din[index].ForceValue=0;
					Din[index].ShowDIOBitmap();
				}
				break;
			case 2:		//清除输出强制
				if (index<0)
				{
					for(int i=0;i<GroupInfo.OutNum;i++)
					{
						Dout[i].Forced=0;
						Dout[i].ForceValue=0;
						Dout[i].ShowDIOBitmap();
					}
				}
				else if (index<GroupInfo.OutNum)
				{
					Dout[index].Forced=0;
					Dout[index].ForceValue=0;
					Dout[index].ShowDIOBitmap();
				}
				break;
			default:
				break;		

		}
	}
