#include "stdafx.h"    
  
#include "gdiplus.h"    
#pragma comment( lib, "gdiplus.lib" )    
using namespace Gdiplus; 

bool LoadImageFromResourse(CImage* pImg, UINT nImgID, LPCTSTR lpImgType)  
{  
	if (pImg == NULL)  
	{  
		return FALSE;  
	}  
	pImg->Destroy();  

	//查找资源    
	HRSRC hRsrc = ::FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(nImgID), lpImgType);  
	if (hRsrc == NULL)  
	{  
		return false;  
	}  

	//加载资源    
	HGLOBAL hImgData = ::LoadResource(AfxGetResourceHandle(), hRsrc);  
	if (hImgData == NULL)  
	{  
		::FreeResource(hImgData);  
		return false;  
	}  


	LPVOID lpVoid = ::LockResource(hImgData);                           //锁定内存中指定资源    
	LPSTREAM pStream = NULL;  
	DWORD dwSize = ::SizeofResource(AfxGetResourceHandle(), hRsrc);  
	HGLOBAL hNew = ::GlobalAlloc(GHND, dwSize);  
	LPBYTE lpByte = (LPBYTE)::GlobalLock(hNew);  
	::memcpy(lpByte, lpVoid, dwSize);  
	::GlobalUnlock(hNew);                                              //解除资源锁定    

	HRESULT ht = ::CreateStreamOnHGlobal(hNew, TRUE, &pStream);  
	if (ht != S_OK)  
	{  
		GlobalFree(hNew);  
	}  
	else  
	{  
		//加载图片    
		pImg->Load(pStream);  
		GlobalFree(hNew);  
	}  

	//释放资源    
	::FreeResource(hImgData);  
	return true;  
}  

bool LoadPicture(CImage& bmp, UINT nImgID, LPCTSTR lpImgType)           //含Alpha通道的图片处理成CImage    
{  
	LoadImageFromResourse(&bmp, nImgID, lpImgType);                 //加载图片资源    

	if (bmp.IsNull())  
	{  
		return false;  
	}  
	if (bmp.GetBPP() == 32)                                              //确认该图片包含Alpha通道    
	{  
		for (int i = 0; i < bmp.GetWidth(); i++)  
		{  
			for (int j = 0; j < bmp.GetHeight(); j++)  
			{  
				byte* pByte = (byte*)bmp.GetPixelAddress(i, j);  
				pByte[0] = pByte[0] * pByte[3] / 255;  
				pByte[1] = pByte[1] * pByte[3] / 255;  
				pByte[2] = pByte[2] * pByte[3] / 255;  
			}  
		}  
	}  

	return true;  
}  