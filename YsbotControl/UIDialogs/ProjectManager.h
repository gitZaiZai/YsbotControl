#pragma  once
//#include "ColumnTreeCtrl.h"
#include "io.h"
#include "DialogType.h"
#include <vector>
#include "../GridCtrl_src/TreeGridCtrl.h"
#include "../YsibotsException.h"

#define prgSuffixStr _T(".prg")
#define varSuffixStr _T(".var")
#define userProgramDir _T("UserPrograms\\")

// 工程（程序）管理窗口类
class ProjectDialog : public DialogType {
public:
	ProjectDialog(const char*);
	virtual ~ProjectDialog();

	void getFiles( CString path, std::vector<CString>& files ,std::vector<CTime>& ftime); 
	CString GetSelectedFileName();
	void load_project( LPCTSTR strProjcet, BOOL initLoad = FALSE );
	void save_project( LPCTSTR strProjcet );
	void CreatNewProject(){ CreatFileQwerty();};
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()
//	virtual void OnOK(); 
//// 	virtual void OnCancel();
//	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
//	afx_msg void OnPaint();
	afx_msg LRESULT OnMainButtonClicked(WPARAM,LPARAM); 
	afx_msg LRESULT OnResizeGrid(WPARAM,LPARAM);
	afx_msg LRESULT OnKeyboardEnter(WPARAM Wparam,LPARAM Lparam);
	afx_msg void OnSetFocus(CWnd* pOldWnd);

private:
	std::vector<CString> fileNames;
	std::vector<CTime> fileTime;
	std::string home_path;
	CString Program_path;
	int QwertyResultType;
	BOOL treeGrid_Ready;

	CString currProjectName;

	BOOL create_tree_grid() throw (InvalidConfigurationException);
	BOOL initialize_tree_grid();
	BOOL tree_grid_putdata();
	void getPRGFiles();
	void CreatFileQwerty();
	BOOL CreatPRGFile(CString newNameStr);
	void CopyFileQwerty();
	BOOL CopyPRGFile(CString newNameStr);
	void RenameFileQwerty();
	BOOL RenamePRGFile(CString newNameStr);
	BOOL DeletePRGFile();

	int  save_application_name_to_database(LPCTSTR);

public:
	 
	CImageList image_list_;
	int system_icon ;
	int global_icon ;
	int program_icon ;
	int overlay_icon ;

	//std::vector<std::string> files;  

	HBITMAP m_hBmpScrollBar;
	HBITMAP m_hBmpPlus;
	HBITMAP m_hBmpMinus;
	CTreeGridCtrl m_Grid;
	GTV_HTREENODE hItemSystem;

	GTV_HTREENODE	hItemGlobal;
	GTV_HTREENODE	hItemProgram;

	int m_nFixCols ;
	int m_nFixRows ;
	int m_nCols ;
	int m_nRows ;

	
};

extern ProjectDialog*   m_pwndDlgProject;