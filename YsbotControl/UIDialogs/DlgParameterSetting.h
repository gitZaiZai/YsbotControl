#pragma once

#include "DialogType.h"
#include "../YsibotsException.h"
#include "../GridCtrl_src/TreeGridCtrl.h"
#include "../NewCellTypes/GridURLCell.h"
#include "../NewCellTypes/GridCellCombo.h"
#include "../NewCellTypes/GridCellCheck.h"
#include "../NewCellTypes/GridCellNumeric.h"
#include "../NewCellTypes/GridCellDateTime.h"
#include "../NewCellTypes/GridCellComposite.h"
#include <vector>
#include "../GroupPropertiesReadWriter.h"

class DlgParameterSetting : public DialogType
{
public:
	DlgParameterSetting( const ConfigReader&, HBITMAP hBmpScrollBar = NULL, 
		HBITMAP hBmpPlus  = NULL, 
		HBITMAP hBmpMinus = NULL );
	virtual ~DlgParameterSetting(void);

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnClickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnKeyboardEnter(WPARAM,LPARAM); 
	afx_msg LRESULT OnMainButtonClicked(WPARAM,LPARAM); 
	afx_msg void OnTreeBeginEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTreeEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg LRESULT OnResizeGrid(WPARAM,LPARAM);

private:
	CImageList image_list_;
	int     joint_icon;
	int     motion_icon;
	HTREEITEM* parameter_type;
    GroupPropertiesReadWriter* group_read_writer;
	CTreeGridCtrl m_Grid;

	HBITMAP	m_hBmpScrollBar;
	HBITMAP	m_hBmpPlus;				 
	HBITMAP	m_hBmpMinus;

	std::vector<GTV_HTREENODE> hItem_list;
	GroupProperties group_prop;

	CCellID       m_CellEdit;

	BOOL create_tree_grid() throw (InvalidConfigurationException);
	BOOL initialize_tree_grid();
	void insert_axis_parameter();
	void insert_motion_parameter();
	void update_tree_parameter();
	void parse_item_string(int row);
 };

