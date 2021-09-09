#pragma once



class CPngButton : public CStatic
{
	DECLARE_DYNAMIC(CPngButton)
public:
	virtual ~CPngButton();
	CPngButton();

	BOOL Create( CString Caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID=65535);
	BOOL GetChecked();
	void SetChecked(BOOL bChecked = TRUE);  
	void InitImage(HBITMAP nBkImg, HBITMAP nCkImg, HBITMAP nDisableImg, HBITMAP nCkDisableImg); 
	void InitImage(CString nBkPath, CString nCkPath, CString nDisablePath, CString nCkDisablePath);
	void InitImage(UINT nID_BkImg,UINT nID_CkImgg, UINT nID_DisImg, UINT nID_CkDisImg);
	void loadBmp(CString nBkPath, CString nCkPath, CString nDisablePath, CString nCkDisablePath);

	void EnableButton(BOOL b);

	virtual void OnInitialize();
protected:
	BOOL m_bChecked;
	BOOL m_bEnable;

	HBITMAP m_bmp[4];

	enum img_index
	{
		bkImg=0,
		chkImg=1,
		disImg=2,
		chkdisImg=3,
	};

	//afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnClicked();  
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()

	
private:
	BOOL LoadImageFromIDResource(CImage *pImage, UINT nResID,LPCTSTR lpTyp);

};

