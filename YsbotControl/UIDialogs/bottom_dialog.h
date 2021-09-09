#pragma once

#include "DialogType.h"
#include "WindowButtonMenu.h"
#include "popup_menu.h"
#include "../BasicControls/buttons.h"

class BottomDialog : public DialogType {
public:
	BottomDialog(PopupMenu *);
	virtual ~BottomDialog();
 
	void SetCaption(int id1, CString caption);  // ���ð�ť����
	void Return(void);                          // ���ذ�ť
	void SetCaptionScheme(int id1,HWND hwnd);             // ���ð�ť����

	void set_response_window(HWND hwnd) { response_win = hwnd; }
	HWND get_response_window() { return response_win; }
	void reset_button_cation ( bool bRunning );
	void update_button_text( int iWin, int iButton, CString strText );

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()
// 	afx_msg void Clicked();                     // ��ť�������

	afx_msg void OnButtonClicked(UINT id);
 
private:
 	BasicButton button_[6];
	BasicButton return_;

	void test_something(void);
	void update_window (bool b);

	HWND response_win;
	ChildWindowButtonMenu* win_info;

	PopupMenu *popup_menu_;
	ChilidWindowKind m_curr_win;
	CFont font_;
};