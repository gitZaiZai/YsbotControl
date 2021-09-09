#pragma once
#include "../BasicControls/static_button.h"
#include "DialogType.h"

#define IDD_POPUP_MENU 5201
#define IDC_POPUP_MENU_ITEM 5211

#define MAX_MENUITEM_NUMBER 15
#define MENUITEM_WIDTH 120
// ����ʽ�˵�������ʱ�벻Ҫʹ�ô��б߿�ķ��
class PopupMenu : public DialogType {
public:
	enum {IDD = IDD_POPUP_MENU};
	PopupMenu();
	~PopupMenu();
	// �����˵���û������˵��Ի�������ʹ�ô˺��������˵���
	// ���룺�˵���ID�������˵���Ŀ�ַ������Զ��Ÿ�����ֻ����Ӣ�Ķ��ţ�
	// ʾ����CreateMenuItem(10, _T("ɾ��,ճ��,����,����,������,�½�"));
	void CreateMenuItem(int id, CString str);
	void active_menu(HWND hRespondWin, int primary_btn_id );
	void active_menu(HWND hRespondWin, int primary_btn_id, int menu_count, const LPCTSTR* menu_text );
	void set_rect_button(LPRECT);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	BOOL OnInitDialog(void);
// 	afx_msg void OnCancel(void);
// 	afx_msg void PostNcDestroy(void);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void ItemClick(UINT number);      // �˵�������Ӧ
	afx_msg void OnKillFocus(CWnd* pNewWnd);  // ʧȥ����ʱ
	afx_msg void OnSetFocus(CWnd* pOldWnd);   // ��ý���ʱ
	DECLARE_MESSAGE_MAP()

private:
	CFont font_;             // ����
	StaticButton item_[MAX_MENUITEM_NUMBER];   // �˵���
	CString item_name_[MAX_MENUITEM_NUMBER];   // �˵�������
	int item_number_;        // ��¼��Ŀ����
	BOOL draw_begin_;        // ���Ʋ˵������
	int parent_menu_id_;            // �û�����Ĳ˵���IDֵ
	HWND m_hRespondWin;
	CRect m_rcButton;
};