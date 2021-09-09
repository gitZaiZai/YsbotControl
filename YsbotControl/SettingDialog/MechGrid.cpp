#pragma once
#include "stdafx.h"
#include "MechGrid.h"


//CMechRow class
CMechRow::CMechRow()
{
	RowHeight=26;
	ColWidth=80;
	TitleWidth=40;

	VERIFY(font_edit.CreateFont(
		20,                // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		DEFAULT_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("arial")));   
	VERIFY(font_combobox.CreateFont(
		18,                // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		DEFAULT_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("arial")));  
	VERIFY(font_title.CreateFont(
		26,                // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		DEFAULT_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("arial")));  
}

CMechRow::~CMechRow()
{
	font_title.DeleteObject();
	font_edit.DeleteObject();
	font_combobox.DeleteObject();
}

IMPLEMENT_DYNAMIC(CMechRow, CStaticTran)

	BEGIN_MESSAGE_MAP(CMechRow, CStaticTran)
		ON_WM_CREATE()
		ON_WM_CTLCOLOR()
		//ON_CONTROL_RANGE(ON_BN_CLICKED,)
	END_MESSAGE_MAP()

BOOL CMechRow::Create(LPCTSTR lpszText,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	RowStr=lpszText;
	RowCtlID=nID;
	RowRect=rect;
	RowRect.bottom=rect.top+RowHeight;
	ColWidth=(rect.right-rect.left-TitleWidth)/8;
	ColWidth=ColWidth<40?40:ColWidth;
	

	CStatic::Create(_T(""), dwStyle,RowRect, pParentWnd, nID);
	//Initialize(lpszText,dwStyle,rect,pParentWnd,nID);
	return TRUE;
}

int CMechRow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{		
 
	int i=0;
	int x0=TitleWidth;
	RowTitle.Create(RowStr,WS_CHILD|WS_VISIBLE|WS_BORDER|SS_CENTER|SS_CENTERIMAGE ,
		CRect(0,0,x0,RowHeight),this,RowCtlID+i);
	
	RowTitle.SetFont(&font_edit);

	Reverse.Create(WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST,
	CRect(x0+ColWidth*i++,0,x0+ColWidth*i,RowHeight),this,RowCtlID+i);
	Reverse.SetFont(&font_combobox);
 
	EncoderDir.Create(WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST,
  		CRect(x0+ColWidth*i++,0,x0+ColWidth*i,RowHeight),this,RowCtlID+i);
  	EncoderDir.SetFont(&font_combobox);
  	RPM.Create(WS_CHILD|WS_VISIBLE|WS_BORDER,
  		CRect(x0+ColWidth*i++,0,x0+ColWidth*i,RowHeight),this,RowCtlID+i);
  	RPM.SetFont(&font_edit);
  	
  	PPR.Create(WS_CHILD|WS_VISIBLE|WS_BORDER,
  		CRect(x0+ColWidth*i++,0,x0+ColWidth*i,RowHeight),this,RowCtlID+i);
  	PPR.SetFont(&font_edit);
  
  	GearRatio.Create(WS_CHILD|WS_VISIBLE|WS_BORDER,
  		CRect(x0+ColWidth*i++,0,x0+ColWidth*i,RowHeight),this,RowCtlID+i);
  	GearRatio.SetFont(&font_edit);
  
  	HighLimit.Create(WS_CHILD|WS_VISIBLE|WS_BORDER,
  		CRect(x0+ColWidth*i++,0,x0+ColWidth*i,RowHeight),this,RowCtlID+i);
  	HighLimit.SetFont(&font_edit);
  
  	Low_Limit.Create(WS_CHILD|WS_VISIBLE|WS_BORDER,
  		CRect(x0+ColWidth*i++,0,x0+ColWidth*i,RowHeight),this,RowCtlID+i);
  	Low_Limit.SetFont(&font_edit);
  
  	MotorType.Create(WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST,
  		CRect(x0+ColWidth*i++,0,RowRect.Width(),RowHeight),this,RowCtlID+i);
  	MotorType.SetFont(&font_combobox);
    
	return 0;
}

//void Initialize(LPCTSTR lpszText,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
//{
//
//}

HBRUSH CMechRow::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr =CStatic::OnCtlColor(pDC, pWnd, nCtlColor);

	//CTLCOLOR_STATIC为 CStatic
	//CTLCOLOR_EDIT
	//CTLCOLOR_LISTBOX
	//CTLCOLOR_BTN
	//CTLCOLOR_DLG  等等

	if ( nCtlColor == CTLCOLOR_STATIC )
	{
		static HBRUSH hbrStatic = ::CreateSolidBrush(RGB(220,220,220));
		//COLOR是你想设置的背景颜色 此处必须为静态变量，否则不能实现
		pDC->SetTextColor(RGB(0, 0, 0));
		pDC->SetBkMode(TRANSPARENT);
		return hbrStatic ; //返回该刷
	}

		//if(pWnd->GetDlgCtrlID()==RowCtlID){
		//	pDC->SetTextColor(RGB(0, 0, 0));
		//	pDC->SetBkColor(RGB(210,210,210));
		////如果有多个STATIC需要设置背景色则将代码添加在下面
		//}


	return hbr;
}

void  CMechRow::SetRowFont(int fontCB_Height,int fontEdit_Height,CString fontStr)
{
	
}

void  CMechRow::SetMotorTypeList(CString str)
{
	Reverse.ResetContent();
	Reverse.AddString(_T("否"));
	Reverse.AddString(_T("是"));
	EncoderDir.ResetContent();
	EncoderDir.AddString(_T("否"));
	EncoderDir.AddString(_T("是"));
	MotorType.ResetContent();
	CString str0 =str.Find(_T(","), str.GetLength() - 1) == -1 ? _T(",") : _T("");
	str += str0;
	// 读取电机类型名并添加到控件
	int num=0;
	CString tempstr=str;
	int j=tempstr.Remove(',');
	CString *TypeList;
	TypeList=new CString[j] ;
	for (int i = 0; i < j; i++) {
		// 读取电机类型名字
		int site = str.Find(_T(","));
		if (site == -1)
			break;
		num++;
		TypeList[i] = str.SpanExcluding(_T(","));  // 截取第一个逗号前的字符串
		str.Delete(0, site + 1);                     // 删除第一个逗号前的字符串
		MotorType.AddString(TypeList[i]);
	}
	delete[] TypeList;
}

void  CMechRow::SetRowData(const MECH_PARAM RowMP)
{
 	mp=RowMP;
 	if (mp.encoder_dir){
		EncoderDir.SetCurSel(1);}
	else{
		EncoderDir.SetCurSel(0);}
	if (mp.reverse){
		Reverse.SetCurSel(1);}
	else{
		Reverse.SetCurSel(0);}
	CString str;

	str.Format(_T("%ld"),mp.rpm);
	RPM.SetWindowTextW(str);
	str.Format(_T("%ld"),mp.ppr);
	PPR.SetWindowTextW(str);

	str.Format(_T("%.4f"),mp.gear_ratio);
	GearRatio.SetWindowTextW(str);
	str.Format(_T("%.4f"),mp.high_limit);
	HighLimit.SetWindowTextW(str);
	str.Format(_T("%.4f"),mp.low_limit);
	Low_Limit.SetWindowTextW(str);

	MotorType.SetCurSel(mp.motor_type);

 
}

MECH_PARAM  CMechRow::GetRowData(void)
{
	mp.encoder_dir=EncoderDir.GetCurSel();
	mp.reverse=Reverse.GetCurSel()?TRUE:FALSE;
	CString str;
	PPR.GetWindowTextW(str);
	mp.ppr=_ttoi(str);
	RPM.GetWindowTextW(str);
	mp.rpm=_ttoi(str);
	GearRatio.GetWindowTextW(str);
	mp.gear_ratio=_ttof(str);
	HighLimit.GetWindowTextW(str);
	mp.high_limit=_ttof(str);
	Low_Limit.GetWindowTextW(str);
	mp.low_limit=_ttof(str);
	mp.motor_type=MotorType.GetCurSel();
	
	return mp;
}

// void CMechRow::SetKB(DigitalKeyboard* kb)
// {
// 	KB_=kb;
// 	RPM.SetKB(KB_);
// 	PPR.SetKB(KB_);
// 	GearRatio.SetKB(KB_);
// 	HighLimit.SetKB(KB_);
// 	Low_Limit.SetKB(KB_);
// };





//MechGrid class
CMechGrid::CMechGrid()
{
	RowHeight=26;
	ColWidth=80;
	TitleWidth=40;
}

CMechGrid::~CMechGrid()
{
	delete[] mRow;
}

IMPLEMENT_DYNAMIC(CMechGrid, CStaticTran)

	BEGIN_MESSAGE_MAP(CMechGrid, CStaticTran)
		ON_WM_CREATE()
		ON_WM_CTLCOLOR()
	END_MESSAGE_MAP()

BOOL CMechGrid::Create(GroupProperties *GP,LPCTSTR lpszText,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	GridCtlID=nID;
	GridRect=rect;
	ColWidth=(rect.right-rect.left-TitleWidth)/8;
	ColWidth=ColWidth<40?40:ColWidth;
	gp=GP;
	GridRect.bottom=(gp->axis_num+1)*RowHeight+GridRect.top;
  	CStatic::Create(_T(""), dwStyle,GridRect, pParentWnd, nID);
	//Initialize(lpszText,dwStyle,rect,pParentWnd,nID);
 
	return TRUE;
}

int CMechGrid::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{		 
	
	VERIFY(font_top.CreateFont(
		16,                // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		DEFAULT_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("arial")));  

	CString strArray[9]={_T("轴"),_T("运动反向"),_T("编码器反向 "),_T("额定转速")
		,_T("每转脉冲量"),_T("齿轮比"),_T("正限位"),_T("负限位"),_T("伺服型号")};
	title[0].Create(strArray[0],WS_CHILD|WS_VISIBLE|WS_BORDER|SS_CENTER|SS_CENTERIMAGE,
		CRect(0,0,TitleWidth,RowHeight),this);
	title[0].SetFont(&font_top);
	int x0=TitleWidth;
	int x1;
	for (int i=1;i<9;i++)
	{
		x1=x0+ColWidth;
		if(i==8) x1=GridRect.Width();
		title[i].Create(strArray[i],WS_CHILD|WS_VISIBLE|WS_BORDER|SS_CENTER|SS_CENTERIMAGE,
			CRect(x0,0,x1,RowHeight),this);
		x0+=ColWidth; 
		title[i].SetFont(&font_top);
	}

	StringToArray();
	NameToID();
	int num=gp->axis_num;
	mRow=new CMechRow[num];
	char chr[5];
	CString str;
	for (int i=1;i<num+1;i++)
	{
		sprintf(chr,"J%d",i);
		str=chr;
		mRow[i-1].Create(str,WS_CHILD|WS_VISIBLE,
			CRect(0,RowHeight*i,GridRect.Width(),RowHeight*(i+1)),this,GridCtlID+1+i*10);		
		mRow[i-1].SetMotorTypeList(MotorTypeListString);
		//		mRow[i-1].SetMotorTypeList(_T("delta-A2,Sanyo-R1,Sankyo,dummy"));
		mRow[i-1].SetRowData(gp->mp[i-1]);
	}

// #ifdef _DEBUG
// 
// #endif
	
 	return 0;
}

	//void Initialize(LPCTSTR lpszText,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
	//{
	//
	//}

HBRUSH CMechGrid::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
	{
		HBRUSH hbr =CStatic::OnCtlColor(pDC, pWnd, nCtlColor);

		//CTLCOLOR_STATIC为 CStatic
		//CTLCOLOR_EDIT
		//CTLCOLOR_LISTBOX
		//CTLCOLOR_BTN
		//CTLCOLOR_DLG  等等

		if ( nCtlColor == CTLCOLOR_STATIC )
		{
			static HBRUSH hbrStatic = ::CreateSolidBrush(RGB(220, 220, 220));
			//COLOR是你想设置的背景颜色 此处必须为静态变量，否则不能实现
			pDC->SetTextColor(RGB(0, 0, 0));
			pDC->SetBkMode(TRANSPARENT);
			return hbrStatic ; //返回该刷
		}

		return hbr;
	}

void  CMechGrid::SetGridFont(int fontCB_Height,int fontEdit_Height,CString fontStr)
	{

	}

void CMechGrid::SaveMechGridData()
{
	for (unsigned int i=0;i<gp->axis_num;i++)
	{
		gp->mp[i]=mRow[i].GetRowData();		
	}
	IDtoName();
}

void CMechGrid::RecoverMechGridData()
{
	for (unsigned int i=0;i<gp->axis_num;i++)
	{
		mRow[i].SetRowData(gp->mp[i]);
	}	
}


void CMechGrid::StringToArray()
{
	CString str=MotorTypeListString;
	CString str0 =str.Find(_T(","), str.GetLength() - 1) == -1 ? _T(",") : _T("");
	str += str0;
	int num=0;
	CString tempstr=str;
	int j=tempstr.Remove(',');
	for (int i = 0; i < j; i++) 
	{	// 读取电机类型名字
		int site = str.Find(_T(","));
		if (site == -1)
			break;
		num++;
		ListName[i] = str.SpanExcluding(_T(","));  // 截取第一个逗号前的字符串
		str.Delete(0, site + 1);                     // 删除第一个逗号前的字符串
	}
}

int CMechGrid::CheckIndexOfArray(CString* Ptr,unsigned int Num,CString str)
{
	int ret=-1;
	for (unsigned int i=0;i<Num;i++)
	{
		if(*(Ptr+i)==str) 
			return i;
	}
	return ret;
}

void CMechGrid::NameToID()
{
	std::wstring str;
	int ret;
	for(unsigned int i=0;i<gp->axis_num;i++)
	{
		str=Ansi2Unicode(gp->servoName[i]);
		ret=CheckIndexOfArray(ListName,12,str.c_str());
		if (ret>=0)
			gp->mp[i].motor_type=ret;
		else
			gp->mp[i].motor_type=0;		
	}
}

void CMechGrid::IDtoName()
{
	char chr[200];
	std::string str;
	for(unsigned int i=0;i<gp->axis_num;i++)
	{
		memset(chr,0,sizeof(chr)/sizeof(char));
		str=Unicode2Ansi((std::wstring)ListName[gp->mp[i].motor_type]);
		strcpy_s(chr,str.c_str());
		strcpy_s(gp->servoName[i],chr);
		/*gp->servoName[i]=chr;	*/
	}
}

// void CMechGrid::SetKB(DigitalKeyboard* kb)
// {
// 	KB_=kb;
// 	for(unsigned int i=0;i<gp->axis_num;i++)
// 	{
// 		mRow[i].SetKB(KB_);
// 	}
// }

