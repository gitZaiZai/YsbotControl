// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef directory_operate_h
#define directory_operate_h 

#include <windows.h>  
#include <TChar.h>  
#include <string>  
#include <map>  
#include <vector>  
#include <algorithm>  



#ifdef UNICODE  
#ifndef TSTRING  
#define TSTRING std::wstring  
#endif  
#else  
#ifndef TSTRING  
#define TSTRING std::string  
#endif  
#endif  

#ifndef _WIN32_WCE  

enum FileDeviceType  // The type of disk partition    
{   
	DISK_FIXED,    
	DISK_REMOVABLE,  
	CDROM  
};  
#endif  

class DirectoryOperate  
{  
public:  
	//-------------------------------------------------------------------------------------  
	//Description:  
	//  Get directory's size  
	//  
	//Parameters:  
	//  strPath: [in] The path of directory  
	//-------------------------------------------------------------------------------------  
	DWORD GetDirectorySize(const TSTRING &strPath);  

	//-------------------------------------------------------------------------------------  
	//Description:  
	//  Check that the directory exists  
	//  
	//Parameters:  
	//  strPath: [in] The path of directory  
	//-------------------------------------------------------------------------------------  
	BOOL CheckDirectoryExist(const TSTRING &strPath);  

	//-------------------------------------------------------------------------------------  
	//Description:  
	//  Copy no empty directory to another directory  
	//  
	//Parameters:  
	//  strDestinationPath: [in] The path of destination directory  
	//  strSourcePath:      [in] The path of source directory  
	//-------------------------------------------------------------------------------------  
	BOOL CopyDirectory(const TSTRING &strDestinationPath,const TSTRING &strSourcePath);  

	//-------------------------------------------------------------------------------------  
	//Description:  
	//  Create the directory serial as to the path. For explame: If the path is "harddisk//test//temp",  
	//it will create the directory: "harddisk","test",and "temp".  
	//  
	//Parameters:  
	//  strPath[in]:   The path of the directory to be created  
	//--------------------------------------------------------------------------------------  
	void CreateDirectorySerial(const TSTRING &strPath);  

	//-------------------------------------------------------------------------------------  
	//Description:  
	//  Delete the no empty directory  
	//  
	//Parameters:  
	//  strPath: [in] The path of directory  
	//--------------------------------------------------------------------------------------  
	void DeleteDirectory(const TSTRING &strPath);  

	void DeleteSpecFile(const TSTRING &strPath, const TSTRING &fileType);  

	//-------------------------------------------------------------------------------------  
	//Description:  
	//  Get the current runing path including '/' in last  
	//  
	//Parameters:  
	//  pszPath: [out] Get the current path  
	//  ulSize: [in] The size of the buffer. For example: szPath[MAX_NUMBER],the size if MAX_NUMBER.  
	//  
	//-------------------------------------------------------------------------------------  
	TCHAR *GetCurrentDirectory(TCHAR *pszPath, ULONG ulSize);  

	//---------------------------------------------------------------------  
	//Description:  
	//  Find the file in the directory  
	//  
	//Parameters:  
	//  DirectoryPath:[in] The path of directory.  
	//  SuffixName:  [in] To find the name of the suffix .if SuffixName is like TEXT("file.exe"),  
	//   will find the path of all "file.exe".  
	//  if SuffixName is like TEXT("*.exe"), will find the path of all file's Suffix is TEXT("exe").  
	//  m_vFilePathList: [Out] Storage the path of file which to be found  
	//---------------------------------------------------------------------  
	BOOL FindFileFromDirectory(const TSTRING &DirectoryPath,const TSTRING &SuffixName,std::vector<TSTRING> &vFilePathList);  

#ifndef _WIN32_WCE  
	//---------------------------------------------------------------------  
	//Description:  
	// Get disk partition  
	//  
	//Parameters:  
	//  mDiskParition: [Out] Store disk partition'type  
	//  
	//---------------------------------------------------------------------  
#endif  

protected:  
	//---------------------------------------------------------------------------  
	//Description:  
	//  Find the string in the source string ,and return the position.  
	//  
	//Parameter:  
	//  szSource:[in]   
	//      The source string  
	//  szFind:[in]  
	//      The find string  
	//  iBeginPos:[in]  
	//      The begin finding position  
	//  
	//Return Values:  
	//  If it -1 , couldn't find the string.  
	//  Others , it's the position of the first character of szFind in the source string  
	//  
	//---------------------------------------------------------------------------  
	int FindString(const TCHAR *szSource, const TCHAR *szFind,const int iBeginPos);  

	//---------------------------------------------------------------------  
	//Description:  
	// Check the suffix of file name  
	//  
	//Parameters:  
	//  FileName: [in] The file name to check  
	//  SuffixName: [in] Special suffix name  
	//  
	//Return Value:  
	// TRUE -- The file is suitable  
	// FALSE -- The file is not suitable  
	//---------------------------------------------------------------------  
	BOOL CheckFileSuffix(const TSTRING &FileName,const TSTRING &SuffixName);  


public:  
	DirectoryOperate();  
	virtual ~DirectoryOperate();  
}; 

#endif