#include "stdafx.h"
#include "StaticBmp_Btn.h"
#include <iostream>

IMPLEMENT_DYNAMIC(CStaticBmp_Btn, CStatic)

	BEGIN_MESSAGE_MAP(CStaticBmp_Btn, CStatic)
		//ON_WM_CREATE()
		ON_CONTROL_REFLECT(STN_CLICKED, &CStaticBmp_Btn::OnStnClicked)
	END_MESSAGE_MAP()


	void CStaticBmp_Btn::OnStnClicked()
	{
		// TODO: 在此添加控件通知处理程序代码
	/*	char chr[5];
		CString str;
		sprintf(chr,"%d",DIOIndex);
		str=chr;*/
		int sm;
		sm=::SendMessage(this->GetParent()->GetParent()->GetParent()->GetSafeHwnd(),
			DIO_StnClicked,DIOType,(LPARAM)GetParent());
		std::cout<< " IOdialog CStaticBmp_Btn: " << sm <<"\n" ;
	}

	BOOL CStaticBmp_Btn::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,int IOType,int IOIndex)
	{
		CStatic::Create(_T(""), dwStyle, rect, pParentWnd);
		DIOType=IOType;
		DIOIndex=IOIndex;
		return TRUE;
	}