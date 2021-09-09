#pragma once
#include "PosInfo.h"
#include "UIDialogs/DialogType.h"
#include "BasicControls/comboboxes.h"
#include "BasicControls/grids.h"
#include "BasicControls/labels.h"
#include "SkinScrollBar/ListCtrlEx.h"
#include "PoseTransform.h"
#include "SQLiteDB.h"

#define LOG_DIALOG_GRID 4800

class LogDialog : public DialogType {
public:
	LogDialog();
	virtual ~LogDialog();

	// ��������ϵ
	// ��1������������ϵ���ͣ���2����������������
	void SetType();                // ������������
	// ���ñ������
	void SetData(double *data);

	void update_location_window(void);
	void updata_grid_fromDB(unsigned int logType = 0);

	enum { SHOW_SERVO = 0, SHOW_JOINT = 1, SHOW_WORLD = 2, SHOW_WORK = 3 };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnMainButtonClicked(WPARAM,LPARAM); 
	afx_msg void OnItemchangedList(NMHDR* pNMHDR,LRESULT* pResult);
	afx_msg void OnActivate(UINT, CWnd*, BOOL);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//afx_msg v OnItemchangedList(WPARAM pNMHDR,LPARAM pResult);
private:

 	CFont font_;
	CListCtrlEx grid_;
	BasicLabel title1_;
	BasicCombobox robot_name_;
	BOOL ctlReady;
	unsigned int filter_;

};

extern LogDialog *log_dialog;