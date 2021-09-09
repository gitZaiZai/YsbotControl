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

// 设置图片
void StaticIcon::SetBmp(int image) {
	HBITMAP bmp;
	bmp = (HBITMAP)::LoadBitmap(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(image));
	SetBitmap(bmp);
}

// 使用双缓存显示图片
// void StaticIcon::ShowBmp(int image) {
// 	// 加载图片
// 	if (bitmap_.LoadBitmap(image)) 
// 	{
// 		dc_ = GetDC();
// 		// 创建内存DC
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

// 使用双缓存显示图片
void StaticIcon::ShowBmp(int image) {
	// 加载图片
// 	if (bitmap_.LoadBitmap(image)) 
// 	{
// 		dc_ = GetDC();
// 		// 创建内存DC
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
	bitmapTmp.LoadBitmap(image); //加载位图
	CDC memDc;
	memDc.CreateCompatibleDC(dc_);    //创建兼容位图DC
	memDc.SelectObject(&bitmapTmp);   //载入设备环境
	BITMAP bitmap;
	ZeroMemory(&bitmap,sizeof(bitmapTmp));
	bitmapTmp.GetBitmap(&bitmap);  //得到加载位图信息
 	BOOL ret = dc_->StretchBlt(0,0,rect_.Width(), rect_.Height(),&memDc,0,0,bitmap.bmWidth,bitmap.bmHeight,SRCCOPY);//显示位图 
	ReleaseDC(dc_);
}