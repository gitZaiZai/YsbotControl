#include "stdafx.h"
#include "buttons.h"

IMPLEMENT_DYNAMIC(BasicButton, CMFCButton)

BEGIN_MESSAGE_MAP(BasicButton, CMFCButton)
END_MESSAGE_MAP()

BasicButton::BasicButton() 
{
	font_.CreateFont(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("ËÎÌו"));
}

CFont * BasicButton::GetMyFont() 
{
	return &font_;
}

void BasicButton::SetMyIcon( CString icon ) 
{
	HICON my_icon =	(HICON)::LoadImage(NULL, icon, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
	SetIcon(my_icon);
	Invalidate(FALSE);
	return;
}