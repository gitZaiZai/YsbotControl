#pragma once

#include "DialogType.h"
#include "../BasicControls/IconComboBox.h"
#include "../BasicControls/Label.h"
#include "../IO Dialog/StaticTransparent.h" 
#include "../GridCtrl_src/TreeGridCtrl.h" 
#include "../ConfigReader.h"
#include "JournalData.h"

#define JOURNAL_DIALOG_GRID    4801
#define JOURNAL_DIALOG_TIPS    4802
#define JOURNAL_DIALOG_COMBBOX 4803

class JournalDLG : public DialogType 
{
public:
	JournalDLG(const ConfigReader& vr,HBITMAP hBmpScrollBar = NULL, 
		HBITMAP hBmpPlus  = NULL, 
		HBITMAP hBmpMinus = NULL );
	virtual ~JournalDLG();

	// insert journal to grid and database.
	void insert_journal_information (const JournalData& jdata, bool insertDatabase = true);

protected:
	virtual BOOL OnInitDialog();
 	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnMainButtonClicked(WPARAM,LPARAM); 
	afx_msg void OnCbnSelGroupType();
	afx_msg void OnClickGrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnResizeGrid(WPARAM,LPARAM);
	virtual int OnActivateChildWin(LPARAM);

private:
	enum { SHOW_ALL, SHOW_ERROR, SHOW_WARNING, SHOW_MESSAGE, SHOW_OTHERS };
	CFont font_;
	CGridCtrl m_Grid;
 	HBITMAP m_hBmpScrollBar;
	HBITMAP m_hBmpPlus;
	HBITMAP m_hBmpMinus;
	GV_ITEM m_Item;

	CImageList image_list_;
	int     overlay_icon ;
	int     message_icon; 
	int     warning_icon; 
	int     error_icon;
	int     none_icon;

	CIconComboBox group_combbox;
	CStaticTran m_title;
	 
	CLabel m_labelCount[4];

	CEdit   m_tipEdit;
	UINT m_columFitTypeGrid;

	int m_ShowJournalType;
	
	sql::time timeSystemBoot;
	sql::time timeCurrentLastRecord;

	unsigned int nMessageCount;
	unsigned int nWarningCount;
	unsigned int nErrorCount;

	BOOL create_tree_grid();

	void show_journal_according_type (int nType, int nFindDir = 0);
	void show_journal_count(int,int,int);
	void UpdateSurface();

	void show_journal_history(int);
	void make_where_condition (char* whereCondition, char* sortCondition, int nType, int nFindDir);
	void insert_journal_record ( sql::Record*, bool insertDatabase = true);

};

extern JournalDLG *journal_dialog;