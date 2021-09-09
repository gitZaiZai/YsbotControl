#include "stdafx.h"
#include "atlimage.h"
#include "PngButton.h"


IMPLEMENT_DYNAMIC(CPngButton, CStatic)

BEGIN_MESSAGE_MAP(CPngButton, CStatic)
/*	ON_WM_CREATE()*/
	ON_WM_CTLCOLOR()
	//ON_WM_PAINT()
	ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)  
END_MESSAGE_MAP()

CPngButton::CPngButton()
{
	m_bmp[0]=NULL;
	m_bmp[1]=NULL;
	m_bmp[2]=NULL;
	m_bmp[3]=NULL;
	m_bEnable  = TRUE;
	m_bChecked = FALSE;
}

CPngButton::~CPngButton()
{
	for ( int i = 0; i < 4; ++i )
	{
		if ( m_bmp[i] )
		{
			DeleteObject(m_bmp[i]);
			m_bmp[i]=NULL;
		}
	}
}


BOOL CPngButton::Create( CString Caption,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CStatic::Create(Caption, dwStyle|SS_NOTIFY|SS_BITMAP|SS_CENTERIMAGE, rect, pParentWnd, nID);
	OnInitialize();
	return TRUE;
}

void CPngButton:: OnInitialize()
{
	//InitImage(_T("res/unchecked.png"),_T("res/checked.png"),_T("res/disablecheck.png"));
	SetBitmap(m_bmp[bkImg]);
}

HBRUSH CPngButton::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr =CStatic::OnCtlColor(pDC, pWnd, nCtlColor);

	//CTLCOLOR_STATIC为 CStatic
	//CTLCOLOR_EDIT
	//CTLCOLOR_LISTBOX
	//CTLCOLOR_BTN
	//CTLCOLOR_DLG  等等

	//if ( nCtlColor == CTLCOLOR_STATIC )
	//{
	//	//static HBRUSH hbrStatic = ::CreateSolidBrush(RGB(255, 0, 255));
	//	//COLOR是你想设置的背景颜色 此处必须为静态变量，否则不能实现
	//	pDC->SetTextColor(RGB(255, 0, 255));
	//	pDC->SetBkMode(TRANSPARENT);
	//	return hbr ; //返回该刷
	//}
	return hbr;
}

BOOL CPngButton::OnClicked()  
{  
	if ( !m_bEnable )
	{
		return FALSE;
	}

	if (!m_bChecked) 
		m_bChecked = TRUE;  
	else 
		m_bChecked = FALSE;  
	SetChecked(m_bChecked);

// 	std::cout << "checkBox = " << (int)m_bChecked << '\n';

	return FALSE;  
}  

BOOL CPngButton::GetChecked()
{
	return m_bChecked;
}

void CPngButton::SetChecked(BOOL bChecked)
{
	if ( !m_bEnable )
 		return;

 	m_bChecked=bChecked;
	if ( IsWindowEnabled() )
	{
		if (m_bChecked)
			SetBitmap(m_bmp[1]);
		else
			SetBitmap(m_bmp[0]);
	}
	else
	{
		if (m_bChecked)
			SetBitmap(m_bmp[2]);
		else
			SetBitmap(m_bmp[3]);

	}

	Invalidate();
}

void CPngButton::EnableButton( BOOL b )
{
	m_bEnable = b;
	if ( b )
	{
		SetChecked(m_bChecked);
	}
	else
	{
		if ( m_bChecked )
		{
			SetBitmap(m_bmp[chkdisImg]);
		}
		else
			SetBitmap(m_bmp[disImg]);
	}
}

void CPngButton::InitImage(HBITMAP nBkImg, HBITMAP nCkImg, HBITMAP nDisableImg, HBITMAP nCkDisableImg)
{
	m_bmp[bkImg] =nBkImg;
	m_bmp[chkImg] =nCkImg;
	m_bmp[disImg] =nDisableImg;
	m_bmp[chkdisImg] =nCkDisableImg;
	SetBitmap(m_bmp[0]);
}


void CPngButton::InitImage(CString nBkPath, CString nCkPath, CString nDisablePath,  CString nCkDisablePath)
{
	CImage image[4];
	image[bkImg].Load(nBkPath);
	if (!image[bkImg].IsNull())
		m_bmp[bkImg] =image[0].Detach();

	image[chkImg].Load(nCkPath);
	if (!image[chkImg].IsNull())
		m_bmp[chkImg] =image[1].Detach();

	image[disImg].Load(nDisablePath);
	if (!image[disImg].IsNull())
		m_bmp[disImg] =image[2].Detach();

	image[chkdisImg].Load(nCkDisablePath);
	if (!image[chkdisImg].IsNull())
		m_bmp[chkdisImg] =image[3].Detach();
	SetBitmap(m_bmp[bkImg]);

}

void CPngButton::loadBmp(CString nBkPath, CString nCkPath, CString nDisablePath, CString nCkDisablePath)
{
 
	// Load bitmap In
	m_bmp[bkImg] = (HBITMAP)::LoadImage(::AfxGetInstanceHandle(), nBkPath,	IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	
	m_bmp[chkImg] = (HBITMAP)::LoadImage(::AfxGetInstanceHandle(), nCkPath,	IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	m_bmp[disImg] = (HBITMAP)::LoadImage(::AfxGetInstanceHandle(), nDisablePath,	IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    m_bmp[chkdisImg] = (HBITMAP)::LoadImage(::AfxGetInstanceHandle(), nCkDisablePath,	IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	SetBitmap(m_bmp[bkImg]);

}


void CPngButton::InitImage(UINT nID_BkImg,UINT nID_CkImgg, UINT nID_DisImg, UINT nID_CkDisImg )
{
	CImage image[4];
	LoadImageFromIDResource(&image[0],nID_BkImg,_T("PNG"));
	LoadImageFromIDResource(&image[1],nID_CkImgg,_T("PNG"));
	LoadImageFromIDResource(&image[2],nID_DisImg,_T("PNG"));
	LoadImageFromIDResource(&image[3],nID_CkDisImg,_T("PNG"));
	m_bmp[bkImg] =image[0].Detach();
	m_bmp[chkImg] =image[1].Detach();
	m_bmp[disImg] =image[2].Detach();
	m_bmp[chkdisImg] =image[3].Detach();
	SetBitmap(m_bmp[0]);

}

BOOL CPngButton::LoadImageFromIDResource(CImage *pImage, UINT nResID,LPCTSTR lpTyp)
{	
	if ( pImage == NULL) 		
		return false;	
	pImage->Destroy();	
	// 查找资源	
	HRSRC hRsrc = ::FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(nResID), lpTyp);	
	if (hRsrc == NULL) 		
		return false;	
	// 加载资源	
	HGLOBAL hImgData = ::LoadResource(AfxGetResourceHandle(), hRsrc);	
	if (hImgData == NULL)	
	{		
		::FreeResource(hImgData);		
		return false;	
	}	
	// 锁定内存中的指定资源	
	LPVOID lpVoid = ::LockResource(hImgData);	
	LPSTREAM pStream = NULL;	
	DWORD dwSize = ::SizeofResource(AfxGetResourceHandle(), hRsrc);	
	HGLOBAL hNew = ::GlobalAlloc(GHND, dwSize);	
	LPBYTE lpByte = (LPBYTE)::GlobalLock(hNew);	
	::memcpy(lpByte, lpVoid, dwSize);	
	// 解除内存中的指定资源	
	::GlobalUnlock(hNew);	
	// 从指定内存创建流对象	
	HRESULT ht = ::CreateStreamOnHGlobal(hNew, TRUE, &pStream);	
	if ( ht != S_OK )	
	{		
		GlobalFree(hNew);	
	}	
	else	
	{		
		// 加载图片		
		pImage->Load(pStream);		
		GlobalFree(hNew);	
	}	
	// 释放资源	
	::FreeResource(hImgData);	
	return true;
}

void CPngButton::OnPaint() 
{
     CStatic::OnPaint();
	 
}