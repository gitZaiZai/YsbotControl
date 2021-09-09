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

	//������Դ    
	HRSRC hRsrc = ::FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(nImgID), lpImgType);  
	if (hRsrc == NULL)  
	{  
		return false;  
	}  

	//������Դ    
	HGLOBAL hImgData = ::LoadResource(AfxGetResourceHandle(), hRsrc);  
	if (hImgData == NULL)  
	{  
		::FreeResource(hImgData);  
		return false;  
	}  


	LPVOID lpVoid = ::LockResource(hImgData);                           //�����ڴ���ָ����Դ    
	LPSTREAM pStream = NULL;  
	DWORD dwSize = ::SizeofResource(AfxGetResourceHandle(), hRsrc);  
	HGLOBAL hNew = ::GlobalAlloc(GHND, dwSize);  
	LPBYTE lpByte = (LPBYTE)::GlobalLock(hNew);  
	::memcpy(lpByte, lpVoid, dwSize);  
	::GlobalUnlock(hNew);                                              //�����Դ����    

	HRESULT ht = ::CreateStreamOnHGlobal(hNew, TRUE, &pStream);  
	if (ht != S_OK)  
	{  
		GlobalFree(hNew);  
	}  
	else  
	{  
		//����ͼƬ    
		pImg->Load(pStream);  
		GlobalFree(hNew);  
	}  

	//�ͷ���Դ    
	::FreeResource(hImgData);  
	return true;  
}  

bool LoadPicture(CImage& bmp, UINT nImgID, LPCTSTR lpImgType)           //��Alphaͨ����ͼƬ�����CImage    
{  
	LoadImageFromResourse(&bmp, nImgID, lpImgType);                 //����ͼƬ��Դ    

	if (bmp.IsNull())  
	{  
		return false;  
	}  
	if (bmp.GetBPP() == 32)                                              //ȷ�ϸ�ͼƬ����Alphaͨ��    
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