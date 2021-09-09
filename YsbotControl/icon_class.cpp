#include "stdafx.h"
#include "icon_class.h"

IMPLEMENT_DYNAMIC(StaticIcon, CStatic)

BEGIN_MESSAGE_MAP(StaticIcon, CStatic)
	ON_WM_CREATE()
END_MESSAGE_MAP()

StaticIcon::StaticIcon() {
	dc_ = nullptr;
	bgcolor = RGB(0,0,0);
}

StaticIcon::~StaticIcon() {

}

int StaticIcon::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	GetClientRect(&rect_);

	return 0;
}

// ����ͼƬ
void StaticIcon::SetBmp(int image) {
	HBITMAP bmp;
	bmp = (HBITMAP)::LoadBitmap(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(image));
	SetBitmap(bmp);
}

// ʹ��˫������ʾͼƬ
// void StaticIcon::ShowBmp(int image) {
// 	// ����ͼƬ
// 	if (bitmap_.LoadBitmap(image)) 
// 	{
// 		dc_ = GetDC();
// 		// �����ڴ�DC
// 		if (mem_dc_.CreateCompatibleDC(dc_)) 
// 		{
//  			mem_dc_.SelectObject(&bitmap_);
// 
// 			dc_->StretchBlt(rect_.left, rect_.top, rect_.Width(), rect_.Height(),
// 				&mem_dc_, rect_.left, rect_.top, rect_.Width(), rect_.Height(),
// 				SRCCOPY);
// 			mem_dc_.DeleteDC();
// 		}	
// 		bitmap_.DeleteObject();
// 		ReleaseDC(dc_);
// 	}
//  }

// ʹ��˫������ʾͼƬ
void StaticIcon::ShowBmp(int image) {
	// ����ͼƬ
// 	if (bitmap_.LoadBitmap(image)) 
// 	{
// 		dc_ = GetDC();
// 		// �����ڴ�DC
// 		if (mem_dc_.CreateCompatibleDC(dc_)) 
// 		{
// 			// 			CBrush brush1;
// 			// 			brush1.CreateSolidBrush(bgcolor);
// 			//  			mem_dc_.FillRect(&rect_, &brush1);
// 			// 			brush1.DeleteObject();
// 
// 			mem_dc_.SelectObject(&bitmap_);
// 
// 			dc_->StretchBlt(rect_.left, rect_.top, rect_.Width(), rect_.Height(),
// 				&mem_dc_, rect_.left, rect_.top, rect_.Width(), rect_.Height(),
// 				SRCCOPY);
// 			mem_dc_.DeleteDC();
// 		}	
// 		bitmap_.DeleteObject();
// 		ReleaseDC(dc_);
// 	}

	dc_ = this->GetDC();
	CBitmap bitmapTmp;
	bitmapTmp.LoadBitmap(image); //����λͼ
	CDC memDc;
	memDc.CreateCompatibleDC(dc_);    //��������λͼDC
	memDc.SelectObject(&bitmapTmp);   //�����豸����
	BITMAP bitmap;
	ZeroMemory(&bitmap,sizeof(bitmapTmp));
	bitmapTmp.GetBitmap(&bitmap);  //�õ�����λͼ��Ϣ
 	BOOL ret = dc_->StretchBlt(0,0,rect_.Width(), rect_.Height(),&memDc,0,0,bitmap.bmWidth,bitmap.bmHeight,SRCCOPY);//��ʾλͼ 
	ReleaseDC(dc_);
}