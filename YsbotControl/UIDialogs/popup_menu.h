#pragma once
#include "../BasicControls/static_button.h"
#include "DialogType.h"

#define IDD_POPUP_MENU 5201
#define IDC_POPUP_MENU_ITEM 5211

#define MAX_MENUITEM_NUMBER 15
#define MENUITEM_WIDTH 120
// 弹出式菜单。创建时请不要使用带有边框的风格
class PopupMenu : public DialogType {
public:
	enum {IDD = IDD_POPUP_MENU};
	PopupMenu();
	~PopupMenu();
	// 创建菜单项，用户创建菜单对话框后必须使用此函数创建菜单项
	// 输入：菜单项ID增量，菜单项目字符串，以逗号隔开（只能用英文逗号）
	// 示例：CreateMenuItem(10, _T("删除,粘贴,复制,剪切,重命名,新建"));
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
	afx_msg void ItemClick(UINT number);      // 菜单项点击响应
	afx_msg void OnKillFocus(CWnd* pNewWnd);  // 失去焦点时
	afx_msg void OnSetFocus(CWnd* pOldWnd);   // 获得焦点时
	DECLARE_MESSAGE_MAP()

private:
	CFont font_;             // 字体
	StaticButton item_[MAX_MENUITEM_NUMBER];   // 菜单项
	CString item_name_[MAX_MENUITEM_NUMBER];   // 菜单项名字
	int item_number_;        // 记录项目数量
	BOOL draw_begin_;        // 控制菜单项绘制
	int parent_menu_id_;            // 用户输入的菜单项ID值
	HWND m_hRespondWin;
	CRect m_rcButton;
};