#pragma once
 
#include "../UIDialogs/DialogType.h"
#include <vector>
#include "../ConfigReader.h"
#include "../SkinScrollBar/skinscrollbar.h"
#include "../SkinScrollBar/SkinComboBox.h"
#include "../SkinScrollBar/skinscrollwnd.h" 
#include "IoRegion.h"

class CSettingIODlg : public DialogType 
{
public:
	CSettingIODlg(const ConfigReader&, HBITMAP hBmpScrollBar = NULL, 
		HBITMAP hBmpPlus  = NULL, 
		HBITMAP hBmpMinus = NULL);
	virtual ~CSettingIODlg();


	void update_io_status ();
	int get_virtual_input( const int deviceID, const int port, bool* val );
	bool is_forced_output( const int deviceID, const int port );


protected:
	virtual BOOL OnInitDialog();
  
	DECLARE_MESSAGE_MAP()
	 
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	 
	afx_msg LRESULT OnMainButtonClicked(WPARAM,LPARAM); 
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	
private:
 	HBITMAP	m_hBmpScrollBar;
	HBITMAP	m_hBmpPlus;				 
	HBITMAP	m_hBmpMinus;
	long    m_YCharHeigth;
 
	CIoRegion*      m_wndIoRegion;
	CSkinScrollBar* m_wndScrollBar;
	int m_iBarWidth;
	CRect m_rectIoRegion;
	bool m_bUseCustomScrollBar;
 	void on_custom_scroll ( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	void filiter_io_show ( int );
	void show_local_io ();
	void clear_force_value ();

};

extern CSettingIODlg *m_pwndDlgIO;