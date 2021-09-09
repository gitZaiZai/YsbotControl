
// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "stdafx.h"
#include "ProjectManager.h"
#include "../colourcode.h"
#include "../Keyboard/full_qwerty.h" 
#include "state_dialog.h"
#include "../program_dialog.h"
#include "../variable_manager.h"
#include "../LuaScript/ScriptVariables.h"
#include "../stringconvert.h"
#include "InfoBox.h"
#include "../WorldModel.h"
#include "SqlDatabase.h"

#define VAL_NAME_APPLICATION            ("Application")

ProjectDialog*   m_pwndDlgProject;

BEGIN_MESSAGE_MAP(ProjectDialog, DialogType)
	//ON_WM_ERASEBKGND()
	//ON_WM_PAINT()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

ProjectDialog::ProjectDialog(const char* strpath) 
{
	home_path = strpath;
	Program_path = CString( home_path.c_str() )+_T("\UserPrograms\\");
	currProjectName = _T("Test");
	treeGrid_Ready=FALSE;
}

ProjectDialog::~ProjectDialog() {
	image_list_.DeleteImageList();
	fileTime.clear();
	fileNames.clear();
	//files.clear();
}

void ProjectDialog::DoDataExchange(CDataExchange* pDX) {

}

void ProjectDialog::OnSetFocus(CWnd* pOldWnd)
{
	if (treeGrid_Ready)
	{
		tree_grid_putdata();
	}

}


LRESULT ProjectDialog::OnResizeGrid(WPARAM wp,LPARAM lp)
{
 	static UINT lastType = 0;
	static UINT i = 0;
	UINT nType = UINT(wp);
	// re-size all pages
	if ( nType == 1 )
	{
		if ( lastType != nType )
		{
			m_Grid.ExpandColumn(1,250,FALSE);
			lastType = nType;
			printf(" N %d dialog on size message %d\n",i++,nType);
		}
	}
	else
	{
		if ( lastType != nType )
		{
			m_Grid.ExpandColumn(1,250,TRUE);
			lastType = nType;
			printf(" N %d dialog on size message %d\n",i++,nType);
		}
	}
	return 1;
}
 
BOOL ProjectDialog::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	//SetWindowPos(NULL, g_x0 + 22, 95, 730, 439, SWP_NOZORDER);
	if ( GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetSafeHwnd() )
	{
		GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetWindowRect(&clientRect);
		SetWindowPos(NULL,clientRect.left, clientRect.top, clientRect.Width(), clientRect.Height(), SWP_NOZORDER);
	}
	SetBackgroundColor(kColor[8]);
	ModifyStyleEx(WS_EX_STATICEDGE, WS_EX_CLIENTEDGE, SWP_DRAWFRAME);
 
	create_tree_grid();

	getPRGFiles();
	tree_grid_putdata();

	CString strGlobalVariable = Program_path+_T("Global") + varSuffixStr;
	CFileFind finder;
	if( !finder.FindFile(strGlobalVariable) ) 
	{
		MSD.ShowInfo(_T("无法找到全局变量文件！"),2);
	}
	LVAR->read_var_from( NULL, SVAR_SYSTEM );
	LVAR->read_var_from( Unicode2Ansi(strGlobalVariable.GetBuffer(0)).c_str(), SVAR_GLOBAL );
	variable_dialog->update_variable_grid(SVAR_SYSTEM);
	variable_dialog->update_variable_grid(SVAR_GLOBAL);
 	
	std::string name = "App1";
	SQL.get("Application",name);
	currProjectName = Ansi2Unicode(name).c_str();
// 	sql::Record strValRecord(SQL.getFieldSet(TBLValText));
// 	strValRecord.setString(1, "Application" );
// 	int nkey = SQL.find_primary_key(TBLValText,&strValRecord);
// 	if ( nkey < 0 )
// 	{
// 		 currProjectName = _T("App1");
// 	}
// 	else
// 	{
// 		sql::Record* record = SQL.getTable(TBLValText)->getRecordByKeyId(nkey);
// 		sql::string strval = record->getValue(SQL.getFieldSet(TBLValText)->getByIndex(2)->getName())->asString();
// 		currProjectName = Ansi2Unicode(strval).c_str();
// 		 
// 	}
 



 	load_project( currProjectName, TRUE );

	return TRUE;
}

void ProjectDialog::getFiles( CString path, std::vector<CString>& files ,std::vector<CTime>& ftime )  
{  
	if (path == (""))
	{
		return;
	}
	else
	{
		if (path.Right(1) != "\\")                        //使用的是多字节字符集
		{
			path += "\\";
		}
		path += _T("*");
		path += prgSuffixStr;                               //后缀 prg
	}
	CFileFind finder;
	CString strPath;
	CString strName;
	BOOL bWorking = finder.FindFile(path);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		strPath = finder.GetFilePath();
		strName=finder.GetFileName();
		if (finder.IsDirectory() && !finder.IsDots()){ 
			//getFiles(strPath,files,fileTime);                                        //递归调用
			}
		else if (!finder.IsDirectory() && !finder.IsDots())
		{
			files.push_back(strName);
			CTime ft;
			finder.GetCreationTime(ft);
			fileTime.push_back(ft);

		}
	}

}  

BOOL ProjectDialog::create_tree_grid() throw (InvalidConfigurationException)
{
	image_list_.Create (16, 16, ILC_COLOR32 | ILC_MASK, 5, 1);

	program_icon = image_list_.Add((HICON)::LoadImageW(AfxGetInstanceHandle(),
		_T(".\\res\\variable_project.ico"),IMAGE_ICON,0,0,LR_DEFAULTSIZE|LR_LOADFROMFILE));
	overlay_icon = image_list_.Add((HICON)::LoadImageW(AfxGetInstanceHandle(),
		_T(".\\res\\OverlayTrans.ico"),IMAGE_ICON,0,0,LR_DEFAULTSIZE|LR_LOADFROMFILE));

	image_list_.SetOverlayImage(overlay_icon, 1);

	HBITMAP hbmp=(HBITMAP)::LoadImageW(AfxGetInstanceHandle(),_T(".\\res\\scroll_bar.BMP")
		,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	if ( hbmp )
	{
		m_hBmpScrollBar=hbmp;
	}
	else
	{
		throw InvalidConfigurationException("project_dialog_error_skin_bar:");
	}

	hbmp=(HBITMAP)::LoadImageW(AfxGetInstanceHandle(),_T(".\\res\\plus.BMP")
		,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	if ( hbmp )
	{
		m_hBmpPlus=hbmp;
	}
	else
	{
		throw InvalidConfigurationException("project_dialog_error_read_bmp:");
	}

	hbmp=(HBITMAP)::LoadImageW(AfxGetInstanceHandle(),_T(".\\res\\minus.BMP")
		,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	if ( hbmp )
	{
		m_hBmpMinus=hbmp;
	}
	else
	{
		throw InvalidConfigurationException("project_dialog_error_read_bmp:");
	}

	BOOL ret = m_Grid.Create(
		CRect(0, 0, clientRect.Width(),clientRect.Height()), 
		this,
		VARIABLE_DIALOG_TREE, WS_VISIBLE | WS_CHILD,
		m_hBmpScrollBar, m_hBmpPlus, m_hBmpMinus, SKIN_WINDOW_VARIABLE,35 );

	if ( ret )
	{
		m_Grid.SetImageList(&image_list_);
		m_Grid.SetColumnResize(FALSE);
		m_Grid.SetRowResize(FALSE);
		// 		m_Grid.EnableSmoothScroll(TRUE);

		m_Grid.EnableDragAndDrop(FALSE);
		m_Grid.SetBkColor(RGB(252, 252, 252));
		m_Grid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(252, 252, 252)); // 0XE0

		LOGFONT lf;
		memset(&lf, 0, sizeof(LOGFONT));
		lf.lfHeight = 22;
		wcsncpy_s(lf.lfFaceName, _T("Arial"), LF_FACESIZE-1);
		CFont font;
		font.CreateFontIndirect(&lf);
		m_Grid.SendMessage(WM_SETFONT,(WPARAM)(font.GetSafeHandle()));
		font.DeleteObject();

		m_Grid.SetEditable(FALSE);
		m_Grid.SetListMode(TRUE);

		m_Grid.SetGridLines(GVL_BOTH);
		//m_Grid.SetGridLineColor(RGB(60,64,60));
		m_Grid.SetGridLineColor(RGB(210,210,210));
		
		m_Grid.SetFixedRowSelection(TRUE);
		m_Grid.SetSingleRowSelection(TRUE);
		m_Grid.SetFixedColumnSelection(FALSE);

		initialize_tree_grid();
		m_Grid.GetCell(0, 0)->SetDrawSortArrow(FALSE); 
		m_Grid.GetCell(0, 1)->SetDrawSortArrow(FALSE); 
	}
	return ret;
}

BOOL ProjectDialog::initialize_tree_grid()
{
	UpdateData();

	m_Grid.SetAutoSizeStyle();

	m_nFixCols = 0;
	m_nFixRows = 1;
	m_nCols = 2;
	m_nRows = 1;

	TRY 
	{
		//m_Grid.SetRowCount(m_nRows);
		// 		m_Grid.SetColumnCount(m_nCols);
		m_Grid.SetFixedRowCount(m_nFixRows);
		m_Grid.SetFixedColumnCount(m_nFixCols);
	}
	CATCH (CMemoryException, e)
	{
		e->ReportError();
		return FALSE;
	}
	END_CATCH

	m_Grid.SetDefCellHeight(33);
	m_Grid.SetRedraw(FALSE);
	//m_Grid.SetRowHeight(0,33);

	// 	m_Grid.SetTreeColumnCellTypeID(CT_CHECKBOX);	

	m_Grid.InsertColumn(_T("项目"),DT_LEFT|DT_TOP);
	m_Grid.InsertColumn(_T("修改时间"),DT_LEFT|DT_TOP);

	m_Grid.SetColumnWidth(0, 400);
	m_Grid.SetColumnWidth(1, 287);// liurundan

 	m_Grid.ExpandColumnsToFit(); // liurundan 

	m_Grid.SetRedraw(TRUE);

	m_Grid.SetTreeColumnCellTypeID(CT_DEFAULT);	

	GV_ITEM Item;
	Item.mask = GVIF_TEXT | GVIF_IMAGE | GVIF_FORMAT | GVIF_STATE;
	Item.nFormat = DT_LEFT | DT_BOTTOM;
	Item.nState  = GVIS_READONLY;

	m_Grid.SetHeaderSort(TRUE);

	// Set the default compare function for all rows
	m_Grid.SetCompareFunction(CGridCtrl::pfnCellTextCompare);

	// Set the Text compare function at indivisual column 0		   

	//m_Grid.GetDefaultCell(FALSE,FALSE)->SetFormat(DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_END_ELLIPSIS);
	//m_Grid.GetDefaultCell(TRUE, FALSE)->SetFormat(DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_END_ELLIPSIS);

	//insert_system_var();
	//insert_global_var();
	m_Grid.SetShowSrollBar(SB_HORZ,FALSE); // liurundan.

	UpdateData(FALSE);
	treeGrid_Ready=TRUE;

	return TRUE;
}

BOOL ProjectDialog::tree_grid_putdata()
{
	CString FileTimeStr;
	GV_ITEM Item;
	GTV_HTREENODE hItem;
	UpdateData();
	if (!treeGrid_Ready) {return FALSE;}
	m_Grid.DeleteAllItems();
	initialize_tree_grid();
	getPRGFiles();

	Item.mask = GVIF_TEXT | GVIF_IMAGE | GVIF_FORMAT | GVIF_STATE;
	Item.nFormat = DT_LEFT | DT_BOTTOM;
	Item.nState  = GVIS_READONLY;

	for (unsigned int i=0;i<fileTime.size();i++)
	{
		Item.strText = fileNames[i].Left(fileNames[i].GetLength()-4);//cut right 4 char
		Item.iImage  = overlay_icon;
		hItem = m_Grid.InsertItem(&Item);		
		FileTimeStr=fileTime[i].Format(_T("%Y-%m-%d %H:%M:%S"));
		//m_Grid.SetItemState(m_Grid.FindRowIndex(hItem),1,GVIS_READONLY);	
		//m_Grid.SetItemData(m_Grid.FindRowIndex(hItem),1,(LPARAM)&FileTimeStr);
		m_Grid.SetCellText(m_Grid.FindRowIndex(hItem),1,FileTimeStr);

	}
	
	UpdateData(FALSE);
	return TRUE;
}

LRESULT ProjectDialog::OnMainButtonClicked( WPARAM wp, LPARAM lp)
{
	int btnid  = (int)(wp);
	int menuid = (int)(lp);

	//int curr_row = -1;
	int nGradeUser = MWM.get_userdata().nGrade;
	if ( btnid != 1 && nGradeUser <= 1 )
	{
		MSD.ShowInfo(_T("没有管理程序文件的权限，请以管理员身份登录系统"),_T("提示"),this,5);
		return 0;
	} 

	switch( btnid )
	{
	case 0:		CreatFileQwerty();		break;
	case 1:		load_project( GetSelectedFileName() ); 		break;
	case 2:		CopyFileQwerty();		break;
	case 3:		RenameFileQwerty();		break;
	case 4:		DeletePRGFile();		break;
	case 5:	    printf("5 program\n");	break;
	default:	break;
	}
	return 1;
}

LRESULT ProjectDialog::OnKeyboardEnter(WPARAM Wparam,LPARAM Lparam)
{
	CString *pstr=(CString*)Wparam;
	switch (QwertyResultType)
	{
		case 1:
			return CreatPRGFile(*pstr);
			break;
		case 2:
			return CopyPRGFile(*pstr);
			break;
		case 3:
			return RenamePRGFile(*pstr);
			break;
		default:
			;
	}
	return 0;
}

void ProjectDialog::getPRGFiles()
{
	fileNames.clear();
	fileTime.clear();
	getFiles( Program_path, fileNames,fileTime);
}

CString ProjectDialog::GetSelectedFileName()
{
	CString retStr=_T("");
	CCellID CID;
	CID=m_Grid.GetFocusCell();
	int SelectedIndex=CID.row;
	if (SelectedIndex>=0)
	{
		retStr=m_Grid.GetItemText(SelectedIndex,0);		
	}
	return retStr;
}

void ProjectDialog::CreatFileQwerty()
{
	QwertyResultType=1;
 
	FKEY->SetArea(CRect(22, 95, 740, 535));
	FKEY->activate_keyboard(GetSafeHwnd(),_T("NewProgram"),_T("CreatNewProgram"));
}

BOOL ProjectDialog::CreatPRGFile(CString newNameStr)
{
	CString NewFileStr,VarName;
	CString numStr;
	int num=1;
	NewFileStr=Program_path;
	NewFileStr+=newNameStr;
	VarName=NewFileStr;
	NewFileStr+=prgSuffixStr;
	CFileFind finder;
	if(finder.FindFile(NewFileStr))
	{	
		finder.Close();
		CreatFileQwerty();
		MSD.ShowInfo(_T("文件名已经存在"),_T(""),this,4);
		return 0;
	}
	else
	{
		CFile newPRGFile(NewFileStr, CFile::modeWrite|CFile::modeCreate);  
// 		string firstcmd = "--NOP--\r\n";
//  		newPRGFile.Write(firstcmd.c_str(),firstcmd.length());
		newPRGFile.Close();
	}
	
	VarName+=varSuffixStr;
	CFile newVarFile(VarName, CFile::modeWrite|CFile::modeCreate);  
	newVarFile.Close();
	finder.Close();
	tree_grid_putdata();
	CString msg;
	msg.Format(_T("新建程序%s"),newNameStr);
	MSD.ShowInfo(msg,JTMESSAGE);

	load_project(newNameStr);

	return TRUE;
}

void ProjectDialog::CopyFileQwerty()
{
	CString FileStr,PrgName;
	QwertyResultType=2;
	FileStr=Program_path+ GetSelectedFileName()+prgSuffixStr;
	CFileFind finder;
	if (finder.FindFile(FileStr))
	{	
 		FKEY->SetArea(CRect(22, 95, 740, 535));
		//Qwerty_->SetOriginalValue(GetSelectedFileName()+_T("_copy"),2,this);
		FKEY->activate_keyboard(GetSafeHwnd(),GetSelectedFileName()+_T("_copy"),_T("CopyProgram"));
	}
	finder.Close();
}

BOOL ProjectDialog::CopyPRGFile(CString newNameStr)
{
	CString FileStr,newFileStr,VarName;
	BOOL retPrg=0;
	BOOL retVar=0;
	//copy prg file
	FileStr=Program_path+ GetSelectedFileName()+prgSuffixStr;
	newFileStr=Program_path+ newNameStr+prgSuffixStr;
	VarName=Program_path+ newNameStr;
	CFileFind finder;
	int num=1;
	CString numStr;
	if (finder.FindFile(FileStr))
	{	
		if(finder.FindFile(newFileStr))
		{
			MSD.ShowInfo(_T("文件名已经存在"),_T(""),this,4);
			finder.Close();
			return 0;
		}
		else
		{
			retPrg=CopyFile(FileStr,newFileStr,FALSE);
		}	
	}

	///copy VAR file
	FileStr=Program_path+ GetSelectedFileName()+varSuffixStr;
	VarName+=varSuffixStr;
	if (finder.FindFile(FileStr))
	{	
		retVar=CopyFile(FileStr,VarName,TRUE);
	}
	else
	{
		CFile newVarFile(VarName, CFile::modeWrite|CFile::modeCreate);  
		newVarFile.Close();
	}
	tree_grid_putdata();
	finder.Close();

	CString msg;
	msg.Format(_T("复制程序%s"),newNameStr);
	MSD.ShowInfo(msg,JTMESSAGE);

 	load_project(newNameStr);

	return TRUE;
}

void ProjectDialog::RenameFileQwerty()
{
	CString FileStr,PrgName;
	QwertyResultType=3;
	FileStr=Program_path+ GetSelectedFileName()+prgSuffixStr;
	CFileFind finder;
	if (finder.FindFile(FileStr))
	{	
 		FKEY->SetArea(CRect(22, 95, 740, 535));
		//Qwerty_->SetOriginalValue(GetSelectedFileName(),2,this);
		FKEY->activate_keyboard(GetSafeHwnd(),GetSelectedFileName(),_T("RenameProgram"));
	}
	finder.Close();
}

BOOL ProjectDialog::RenamePRGFile(CString newNameStr)
{
	CString FileStr,newFileStr,VarName,PrgName;
	BOOL retPrg=0;
	BOOL retVar=0;
	//rename prg file
	newFileStr=newNameStr;///test
	FileStr=Program_path+ GetSelectedFileName()+prgSuffixStr;
	PrgName=Program_path+ newFileStr+prgSuffixStr;
	VarName=Program_path+ newFileStr+varSuffixStr;
	CFileFind finder;
	int num=1;
	CString numStr;
	if (finder.FindFile(FileStr))
	{	
		if (finder.FindFile(PrgName))
		{
 			MSD.ShowInfo(_T("文件名已经存在"),_T(""),this,4);
			finder.Close();
			return 0;
		}
		else
		{			
			retPrg=MoveFile(FileStr,PrgName);
		}
		
	}

	///rename VAR file
	FileStr=Program_path+ GetSelectedFileName()+varSuffixStr;
	if (finder.FindFile(FileStr))
	{	
		retVar=MoveFile(FileStr,VarName);
	}
	else
	{
		CFile newVarFile(VarName, CFile::modeWrite|CFile::modeCreate);  
		newVarFile.Close();
	}
	CString msg;
	msg.Format(_T("重命名程序%s"),newNameStr);
	MSD.ShowInfo(msg,JTMESSAGE);

 	tree_grid_putdata();
	finder.Close();
	return TRUE;

}

BOOL ProjectDialog::DeletePRGFile()
{
	CString prgName=GetSelectedFileName();
	if (_T("")==prgName )
	{
		return 0;
	}
	if (prgName==currProjectName)
	{
		MessageBox(_T("项目 ‘")+prgName+_T("’ 加载中，不能删除！"),_T("警告！ "),MB_OK|MB_ICONWARNING);
		return 0;
	} 
	else
	{
		if(IDOK!=MessageBox(_T("是否删除项目: ‘")+prgName+_T("’ ？"),_T("注意！ "),MB_OKCANCEL|MB_ICONWARNING))
		{
			return 0;
		}
	}

	CString FileStr;
	FileStr=Program_path+ prgName+prgSuffixStr;
	BOOL retPrg=DeleteFileW(FileStr);
	FileStr=Program_path+ prgName+varSuffixStr;
	BOOL retVar=DeleteFileW(FileStr);
	tree_grid_putdata();

	CString msg;
	msg.Format(_T("删除程序%s"),prgName);
	MSD.ShowInfo(msg,JTMESSAGE);

 	return retPrg&retVar;
}

//////////////////////////////////////////////////////////////////////////

void ProjectDialog::load_project( LPCTSTR strProjcet, BOOL initLoad )
{
	if ( strProjcet == NULL || CString(strProjcet).IsEmpty() )
 		return;
 
 	if ( initLoad == FALSE && currProjectName != strProjcet )
	{
		// pop message box hints save old project.
		save_project(currProjectName);
	}
 
	CString strProgram  = Program_path+CString(strProjcet) + prgSuffixStr;
	CString strVariable = Program_path+CString(strProjcet) + varSuffixStr;
	CFileFind finder;
	if(!finder.FindFile(strProgram)) 
	{
		MSD.ShowInfo(_T("工程不存在"),2);
		return;
	}

	MSD.SetProjectName(strProjcet);

	program_dialog->load_script( Unicode2Ansi(strProgram.GetBuffer(0)).c_str() );
 
	LVAR->read_var_from( Unicode2Ansi(strVariable.GetBuffer(0)).c_str() );
	variable_dialog->update_variable_grid(SVAR_PROGRAM);
	
	if ( GetParent()->GetSafeHwnd() && !initLoad )
	{
		GetParent()->SendMessage( WM_ACTIVATE_WINDOW, winProgram, 0 );
	}
	currProjectName = strProjcet;
	program_dialog->SetPrgName(strProjcet);
  
	CString strmsg;
	strmsg.Format(_T("加载 %s 程序文件"),currProjectName);
	MSD.ShowInfo(strmsg);

	//////////////////////////////////////////////////////////////////////////
	save_application_name_to_database(strProjcet);

}

void ProjectDialog::save_project( LPCTSTR strProjcet )
{
	if ( strProjcet == NULL )
	{
		strProjcet = currProjectName;
	}

	CString strProgram  = Program_path+CString(strProjcet) + prgSuffixStr;
	CString strVariable = Program_path+CString(strProjcet) + varSuffixStr;
	CString strGlobalVariable = Program_path+_T("Global")+ varSuffixStr;
	program_dialog->save_script( Unicode2Ansi(strProgram.GetBuffer(0)).c_str() );
	ScriptVariables::getInstance()->write_var_at( Unicode2Ansi(strVariable.GetBuffer(0)).c_str(), SVAR_PROGRAM );

	ScriptVariables::getInstance()->write_var_at( Unicode2Ansi(strGlobalVariable.GetBuffer(0)).c_str(), SVAR_GLOBAL );
 
	save_application_name_to_database(strProjcet);

 	CString strmsg;
	strmsg.Format(_T("保存 %s 程序文件"),currProjectName);
	MSD.ShowInfo(strmsg);

}

int ProjectDialog::save_application_name_to_database( LPCTSTR strname )
{
	//////////////////////////////////////////////////////////////////////////
// 	sql::Record strValRecord(SQL.getFieldSet(TBLValText));
// 	strValRecord.setString(1, "Application" );
// 	int nkey = SQL.find_primary_key(TBLValText,&strValRecord);
// 	int nRet = -1;
// 	if ( nkey < 0 )
// 	{
// 		strValRecord.setString(2, Unicode2Ansi(strname).c_str() );
// 		strValRecord.setString(3, "lastApplictionUse" );
// 		strValRecord.setInteger(4, 1 );
// 		nRet = SQL.insert_record(TBLValText,&strValRecord);
// 	}
// 	else
// 	{
// 		sql::Record* record = SQL.getTable(TBLValText)->getRecordByKeyId(nkey);
// 		record->setString(2,Unicode2Ansi(strname).c_str());
// 		nRet = SQL.modify_record(TBLValText,record);
// 	}
// 	return nRet;

	SQL.set(VAL_NAME_APPLICATION,Unicode2Ansi(strname).c_str());



	return 0;
}







