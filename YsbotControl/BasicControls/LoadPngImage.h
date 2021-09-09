#ifndef load_png_image_h
#define load_png_image_h 

#include <atlimage.h>    
 
// 按钮的状态    
// enum  
// {  
// 	CTRL_NOFOCUS = 0x01,            //普通    
// 	CTRL_FOCUS,                     //mousemove    
// 	CTRL_SELECTED,                  //buttondown    
// 	CTRL_DISABLE,                   //无效    
// };  
// 
// 图片形式    
// enum  
// {  
// 	BTN_IMG_1 = 1,                  //    
// 	BTN_IMG_3 = 3,                  //3分图（1个图片内有3小图，下同）    
// 	BTN_IMG_4 = 4,                  //4分图    
// };  
// 
// 按钮类型    
// enum  
// {  
// 	BTN_TYPE_NORMAL = 0x10,         //普通BTN    
// 	BTN_TYPE_MENU,                  //菜单类型的BTN    
// 	BTN_TYPE_STATIC,                //静态类型的BTN    
// };  

//从资源里面加载位图    
bool LoadImageFromResourse(CImage* pImg, UINT nImgID, LPCTSTR lpImgType);  
bool LoadPicture(CImage& bmp, UINT nImgID, LPCTSTR lpImgType = _T("PNG"));          //含Alpha通道的图片处理成CImage 

#endif