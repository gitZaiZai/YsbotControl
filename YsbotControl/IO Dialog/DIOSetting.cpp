// Copyright 2016, ��˼��Ϣ�Ƽ��������ι�˾
// All right reserved.

#include "stdafx.h"
#include "DIOSetting.h"
#include "../resource.h"

BEGIN_MESSAGE_MAP(CDIOSetting, DialogType)
	ON_BN_CLICKED(IDC_DIO_SETTING_BUTTON, BtnCancel)
	ON_BN_CLICKED(IDC_DIO_SETTING_BUTTON+2, BtnOK)
END_MESSAGE_MAP()

CDIOSetting::CDIOSetting() {

}

CDIOSetting::~CDIOSetting() {
}

void CDIOSetting::DoDataExchange(CDataExchange* pDX) {

}

// void CDIOSetting::OnCancel() {
// 	DestroyWindow();
// }
// 
// void CDIOSetting::PostNcDestroy() {
// 	CDialogEx::PostNcDestroy();
// 	if (NULL != this) {
// 		delete this;
// 	}
// }

// ��ʼ��
BOOL CDIOSetting::OnInitDialog() 
{
	DialogType::OnInitDialog();
	// ���ݸ�����λ�ü������λ��
	IOPtr=nullptr;
	CRect rect;
	GetParent()->GetWindowRect(&rect);
	int x0 = rect.left + rect.Width() / 2 - 150;
	int y0 = rect.top +	rect.Height() / 2 - 150;
	int width0 = 200;
	int height0 = 220;
	SetWindowPos(NULL, x0, y0, width0, height0, SWP_NOZORDER);
	
	// ���ñ���ɫ
	// �ڲ���ϵͳ�Ĳ�ͬ��������£��Ի������ű䣬��������������һ���Է����ű�
	SetBackgroundColor(RGB(239, 239, 239));
	title_.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_NOTIFY,
		CRect(0, 0, width0, 30), this, IDC_DIO_SETTING_TITLE);
	title_.SetBkBmp(_T(".\\res\\01.bmp"));
	title_.Draw(_T("DI:1"));
	title_.SetArea(rect);

#ifdef _DEBUG
	test_ = 0;
//	SetTimer(1, 8000, NULL);
#endif
	// ������ť
	x0 = 5;
	y0 = 140;
	int width = 60;
	int height = 35;
	CString key_text[] = {
		 _T("��"), _T("?"), _T("��")
	};
	for (unsigned i = 0; i < 3; i++) {
		if (i % 4 == 0) {
			x0 = 4;
			y0 += height + 2;
		} else {
			x0 += width + 3;
		}
		button_[i].Create(key_text[i], WS_CHILD | WS_VISIBLE,
			CRect(x0, y0, x0 + width, y0+height),
			this, IDC_DIO_SETTING_BUTTON + i);
		button_[i].SetMyFont(26,0,_T("����"));
		// ���ð����ı���ɫ��������ܲ���ϵͳӰ��
		button_[i].SetFaceColor(RGB(239, 239, 239));

	}
	// ��ɾ������ȷ�ϼ�����������ɫ
	button_[0].SetTextColor(RGB(206, 49, 49));
	button_[2].SetTextColor(RGB(94, 151, 24));
	// ˢ������������������ʾ����ɫ
	GetDlgItem(IDC_DIO_SETTING_BUTTON + 0)->GetWindowRect(rect);
	ScreenToClient(rect);
	InvalidateRect(rect);
	GetDlgItem(IDC_DIO_SETTING_BUTTON + 2)->GetWindowRect(rect);
	ScreenToClient(rect);
	InvalidateRect(rect);

	int label_x0=20;
	int label_y0=40;
	int label_width=80;
	int label_height=30;

	//������̬��ǩ
	Label_[0].Create(_T("��ǰֵ"),WS_CHILD | WS_VISIBLE ,
		CRect(label_x0,label_y0,label_x0+label_width,label_y0+label_height),this);
	label_y0+=40;
	Label_[1].Create(_T("����ֵ"),WS_CHILD | WS_VISIBLE ,
		CRect(label_x0,label_y0,label_x0+label_width,label_y0+label_height),this);
	label_y0+=50;
	Label_[2].Create(_T("ǿ��ʹ�ܣ�"),WS_CHILD | WS_VISIBLE ,
		CRect(label_x0,label_y0,label_x0+label_width,label_y0+label_height),this);
	for ( int i = 0; i < 3; ++i )
	{
		Label_[i].SetFont(18,_T("����"));
	}

	int checkbox_x0=120;
	int checkbox_y0=75;
	int checkbox_width=15;
	int checkbox_height=15;
	checkbox_[0].Create(_T(""),WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX,
		CRect(checkbox_x0,checkbox_y0,checkbox_x0+checkbox_width,checkbox_y0+checkbox_height),
		this,IDC_DIO_SETTING_CHECKBOX);
	checkbox_y0+=45;
	checkbox_[1].Create(_T(""),WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX,
		CRect(checkbox_x0,checkbox_y0,checkbox_x0+checkbox_width,checkbox_y0+checkbox_height),
		this,IDC_DIO_SETTING_CHECKBOX+1);

	for ( int i= 0; i < 2; ++i )
	{
 		checkbox_[i].Init(IDB_PNG_UNCHECK, IDB_PNG_CHECKED, IDB_PNG_DISABLECHECK, IDB_PNG_DISABLEUNCHECK );  
		checkbox_[i].SetBackColor(RGB(0, 200, 200));  
		checkbox_[i].SetFontType(16, L"����");  
		checkbox_[i].SetTextColor(RGB(0, 200, 0), RGB(100, 100, 0), TRUE);  
	}

	//����IO״̬ͼƬ
	bmp_Off = (HBITMAP)::LoadImageW(AfxGetInstanceHandle(),_T(".\\res\\Off.BMP")
		,IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	bmp_On = (HBITMAP)::LoadImageW(AfxGetInstanceHandle(),_T(".\\res\\On.BMP")
		,IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	bmp_OffForce = (HBITMAP)::LoadImageW(AfxGetInstanceHandle(),_T(".\\res\\Off_Force.BMP")
		,IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	bmp_OnForce = (HBITMAP)::LoadImageW(AfxGetInstanceHandle(),_T(".\\res\\On_Force.BMP")
		,IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_CREATEDIBSECTION);

	StateBmp.Create(_T(""),WS_CHILD |WS_VISIBLE|SS_BITMAP |SS_RIGHT,CRect(123,35,162,72),this);
	StateBmp.SetBitmap(bmp_Off);
	
	return TRUE;
}

void CDIOSetting::BtnCancel()
{
 	IOPtr=nullptr;
	this->ShowWindow(false);
}

void CDIOSetting::BtnOK()
{
//	int NewIOState;
	if(IOPtr)
	{
		if(checkbox_[0].GetChecked()) 
			IOPtr->ForceValue=true;
		else 
			IOPtr->ForceValue=false;
		if(checkbox_[1].GetChecked()) 
			IOPtr->Forced=true;
		else 
			IOPtr->Forced=false;
		IOPtr->ShowDIOBitmap();
		IOPtr=nullptr;
	}	
	this->ShowWindow(false);
}

//������Ҫ���õ�IOģ��ָ�룬��������ֵ����
void CDIOSetting::SetHostPointer(CDIO* Ptr)
{
	char chr[5];
	CString str1,str2;
	IOPtr=Ptr;
	if (!IOPtr)
	{
		return;
	}
	sprintf(chr,"%d",IOPtr->GetDIOinfo().Index);
	str2=chr;
	str1=IOPtr->GetDIOinfo().GroupName;
	if (IOPtr->GetDIOinfo().Type==1)
	{
		str1+=_T("DI:");
		Label_[2].SetWindowTextW(_T("�������룿"));
	}
	else if(IOPtr->GetDIOinfo().Type==2)
	{
		str1+=_T("DO:");
		Label_[2].SetWindowTextW(_T("ǿ�������"));
	}
	else
	{
		str1=_T("DI0");
		Label_[2].SetWindowTextW(_T("ǿ��ʹ�ܣ�"));
	}
	str1+=str2;
	title_.Draw(str1);
	this->ShowWindow(true);
	ShowIOState();
	if(IOPtr->ForceValue) 
		checkbox_[0].SetChecked(1);
	else 
		checkbox_[0].SetChecked(0);
	if(IOPtr->Forced) 
		checkbox_[1].SetChecked(1);
	else
		checkbox_[1].SetChecked(0);

	UpdateSurface();
}

void CDIOSetting::UpdateSurface()
{
	CRect (rc);
	GetWindowRect(rc);
	RedrawWindow();

	GetParent()->ScreenToClient(rc);
	GetParent()->InvalidateRect(rc,TRUE);
	GetParent()->UpdateWindow();
}

//ˢ��IO״̬ͼƬ
void CDIOSetting::ShowIOState()
{
	if (IOPtr==nullptr) return;

	if (IOPtr->Forced)
	{	
		if (IOPtr->ForceValue)
			StateBmp.SetBitmap(bmp_OnForce);
		else		
			StateBmp.SetBitmap(bmp_OffForce);
	} 
	else
	{
		if (IOPtr->Value)
			StateBmp.SetBitmap(bmp_On);
		else		
			StateBmp.SetBitmap(bmp_Off);
	}
	
}

