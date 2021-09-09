//////////Ã∆è™è™è™è™
#pragma once
#include "DialogType.h"
#include "../IO Dialog/StaticTransparent.h"
#include "../IO Dialog/GroupFrame.h"
#include "AreaInformation.h"
#include "AreaGeneral.h"
#include "AreaDimension.h"
#include "AreaIllustration.h"
#include "../LuaScript/ScriptVariableType.h"
#include "../DigitalInput.h"
#include "../DigitalOutput.h"
#include <vector>

#define  MAX_AREA_NUM  (10)

class ScanAreaList 
{
public:
	std::vector<SecurArea*> areas;   
	std::vector<VariableKey*> keys;   
	ScanAreaList(int n);
	~ScanAreaList();
	void active( VariableKey, SecurArea );
	void deactive( VariableKey );
	void clear();
	int findAreaID( const VariableKey& );
	int findDeactiveAreaID();

};

class SecurityAreaDLG :public DialogType
{
public:
	SecurityAreaDLG();
	virtual ~SecurityAreaDLG();

	void scan_security_area();

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnMainButtonClicked(WPARAM,LPARAM); 
	afx_msg LRESULT OnSelchangeshape(WPARAM Wparam,LPARAM Lparam);
	afx_msg LRESULT OnSelchangetype(WPARAM Wparam,LPARAM Lparam);
	afx_msg LRESULT OnSelchangeAreaName(WPARAM Wparam,LPARAM Lparam);
	virtual int OnActivateChildWin(LPARAM);

private:
	CAreaInformation  AreaInformation;
	CAreaGeneral AreaGeneral;
	CAreaDimension AreaDimension;
	CAreaIllustration AreaIllustration;
	SecurArea m_area;
	VariableKey m_currAreaKey;
	ScanAreaList* areaScanList;
 	int m_shape;
	int m_type;
	bool m_bEnableScanArea;
	
	DigitalInput m_din;
	DigitalOutput m_dout;

	void update_area_dlg( int selID );
	void InitCombobox();
	void InitAreaActiveAndStateVar();

	void save_area();
	unsigned int get_acitive_area_num();
};

extern SecurityAreaDLG *SecurityArea_DLG;