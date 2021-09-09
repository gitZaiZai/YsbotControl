#include "stdafx.h"
#include "../TeachDialog/HelpFrame.h"

BEGIN_MESSAGE_MAP(CHelpFrame, CGroupFrame)
	ON_WM_CREATE()
END_MESSAGE_MAP()

CHelpFrame::CHelpFrame() {  }

CHelpFrame::~CHelpFrame() {	 }

BOOL CHelpFrame::Create(CString caption,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	_nID=nID;
	CStatic::Create(caption, dwStyle, rect, pParentWnd, nID);
	CImage image_[3];
	image_[0].Load(_T("res/Go_Btn.bmp"));
	image_[1].Load(_T("res/minus_btn.bmp"));
	image_[2].Load(_T("res/plus_btn.bmp"));
	picture_[0].Create(_T(""),WS_CHILD|WS_VISIBLE|SS_BITMAP,CRect(70,35,110,80),this);
	picture_[0].SetBitmap(image_[0].Detach());
	picture_[1].Create(_T(""),WS_CHILD|WS_VISIBLE|SS_BITMAP,CRect(140,40,180,80),this);
	picture_[1].SetBitmap(image_[1].Detach());
	picture_[2].Create(_T(""),WS_CHILD|WS_VISIBLE|SS_BITMAP,CRect(210,40,250,80),this);
	picture_[2].SetBitmap(image_[2].Detach());
	text_.Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_READONLY|ES_MULTILINE|ES_WANTRETURN,CRect(5,100,rect.right-rect.left-5,rect.bottom-rect.top-8),this,_nID+1);
	//text_.
	CString text_str=_T("����ײ���������ť���ұ߳��֡�Go����ʶ,���������˽�����ƶ�״̬");
	text_str+=_T("\r\n");
	text_str+=_T("\r\n");
	text_str+=_T("��ס��+����ť���ţ��ɿ��ƻ�е������Ŀ���λ");
	text_str+=_T("\r\n");
	text_str+=_T("\r\n");
	text_str+=_T("��ס��-����ť���ţ��ɿ��ƻ�е�ַ��س�ʼλ��");
	text_str+=_T("\r\n");
	text_str+=_T("\r\n");
	text_str+=_T("ע��:���ڵ����²�����<10%��");

	text_.SetWindowTextW(text_str);
	return TRUE;
}

int CHelpFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	return 0;
}