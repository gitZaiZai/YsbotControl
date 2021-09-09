#include "stdafx.h"  
#include "DirectoryOperate.h"  


DirectoryOperate::DirectoryOperate()  
{  

}  
DirectoryOperate::~DirectoryOperate()  
{  
}  

TCHAR *DirectoryOperate::GetCurrentDirectory(TCHAR *pszPath, ULONG ulSize)  
{  
	memset(pszPath, 0, sizeof(TCHAR) * ulSize);  

	TCHAR szBuf[MAX_PATH] = {0};  
	GetModuleFileName(NULL,szBuf,sizeof(szBuf)/sizeof(TCHAR));  

	int ulCount = _tcslen(szBuf);  

	while(--ulCount >= 0)  
	{  
		if(szBuf[ulCount] == TEXT('\\'))  
		{  
			break;  
		}  
		else  
		{  
			continue;  
		}  
	}  

	if(ulSize > (DWORD)ulCount)  
	{  
		_tcsncpy(pszPath,szBuf,(ulCount + 1));  
	}  

	return pszPath;  
}  

void DirectoryOperate::CreateDirectorySerial(const TSTRING &strPath)  
{  

	TSTRING SourcePath = strPath;  

	if(strPath.size() >= MAX_PATH)  
		return;  

	if(SourcePath[SourcePath.size() -1 ] == TEXT('\\'))  
	{  
		SourcePath[SourcePath.size() -1 ] = TEXT('\0');  
	}  

	if(CheckDirectoryExist(strPath))  
		return;  

	TSTRING::size_type idx = SourcePath.find(TEXT('\\'));  

	while(idx !=TSTRING::npos)  
	{  
		TSTRING strCreateDirPath;  
		strCreateDirPath.assign(SourcePath,0,idx);  
		if(!CheckDirectoryExist(strCreateDirPath))  
		{  
			CreateDirectory(strCreateDirPath.c_str(),NULL);  
		}  

		idx = SourcePath.find(TEXT('\\'),idx + 1);  
	}  

	CreateDirectory(SourcePath.c_str(),NULL);  
}  

void DirectoryOperate::DeleteSpecFile(const TSTRING &strPath, const TSTRING &fileType)  
{  
	TSTRING strDirPath = strPath;  
#ifdef _WIN32_WCE  
	if(strDirPath[0] != TEXT('\\'))  
	{  
		return;  
	}  
#endif  
	if(strDirPath[strDirPath.size() - 1] == TEXT('\\'))  
	{  
		strDirPath += TEXT("*.*");  
	}  
	else  
	{  
		strDirPath += TEXT("\\*.*");  
	}  

	WIN32_FIND_DATA fd;  
	HANDLE hdFind;  
	hdFind = FindFirstFile(strDirPath.c_str(),&fd);  
	if(hdFind != INVALID_HANDLE_VALUE)  
	{  
		do{  
			//At winXP system the "." means current directory, the ".."means parent directory.  
			if(fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY && fd.cFileName[0] != TEXT('.'))  
			{         
				//It is directory     
				continue;
			}  
			else if(fd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)  
			{     
				//It is file   
				TSTRING strPathFile = strPath;  
				if(strPathFile[strPathFile.size() - 1] !=TEXT( '\\'))  
					strPathFile += TEXT("\\"); 

				CString filename(fd.cFileName);
                if( filename.Find(_T("bak")) != -1 )
                {
					strPathFile += fd.cFileName; 
					DeleteFile(strPathFile.c_str());  
                }
			}  
		}while(FindNextFile(hdFind,&fd));  
	}  
	FindClose(hdFind);  

 }  

 void DirectoryOperate::DeleteDirectory(const TSTRING &strPath)  
{  
    TSTRING strDirPath = strPath;  
#ifdef _WIN32_WCE  
    if(strDirPath[0] != TEXT('\\'))  
    {  
        return;  
    }  
#endif  
    if(strDirPath[strDirPath.size() - 1] == TEXT('\\'))  
    {  
        strDirPath += TEXT("*.*");  
    }  
    else  
    {  
        strDirPath += TEXT("\\*.*");  
    }  
  
    WIN32_FIND_DATA fd;  
    HANDLE hdFind;  
    hdFind = FindFirstFile(strDirPath.c_str(),&fd);  
    if(hdFind != INVALID_HANDLE_VALUE)  
    {  
        do{  
            //At winXP system the "." means current directory, the ".."means parent directory.  
            if(fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY && fd.cFileName[0] != TEXT('.'))  
            {         
                //It is directory     
                TSTRING strNextDir = strPath;  
                if(strNextDir[strNextDir.size() -1] != TEXT('\\'))  
                    strNextDir += TEXT("\\");  
  
                strNextDir += fd.cFileName;   
                DeleteDirectory(strNextDir);  
                RemoveDirectory(strNextDir.c_str());  
            }  
            else if(fd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)  
            {     
                //It is file   
                TSTRING strPathFile = strPath;  
                if(strPathFile[strPathFile.size() - 1] !=TEXT( '\\'))  
                    strPathFile += TEXT("\\");  
  
                strPathFile += fd.cFileName;  
                DeleteFile(strPathFile.c_str());  
            }  
        }while(FindNextFile(hdFind,&fd));  
    }  
    FindClose(hdFind);  
  
    RemoveDirectory(strPath.c_str());  
}  

int DirectoryOperate::FindString(const TCHAR *szSource, const TCHAR *szFind, const int iBeginPos)  
{  
	int iLenSource = _tcslen(szSource);  
	int iLenFind = _tcslen(szFind);  

	if(iLenSource - 1 < iBeginPos)  
	{  
		return -1;  
	}  

	int iCount = 0;  
	int iFindCount = 0;  
	BOOL bPair = FALSE;  
	for(iCount = 0; iCount < iLenSource - iBeginPos; iCount++)  
	{  
		if(szSource[iCount + iBeginPos] == szFind[iFindCount])  
		{  
			if(iFindCount == iLenFind - 1)  
			{  
				bPair = TRUE;  
				break;  
			}  
			iFindCount++;             
		}  
		else  
		{  
			iFindCount = 0;  
		}  
	}  

	int iFindPos ;  

	if(bPair == FALSE)  
	{  
		iFindPos = -1;  
	}  
	else  
	{  
		iFindPos = iCount + iBeginPos - iLenFind + 1;  
	}  
	return iFindPos;  
}  

DWORD DirectoryOperate::GetDirectorySize(const TSTRING &strPath)  
{  
	DWORD dSize = 0;  
	//查找路径下的文件夹和所需文件  
	TSTRING strDirPath = strPath;  

#ifdef _WIN32_WCE  
	if(strDirPath[0] != TEXT('\\'))  
	{  
		return FALSE;  
	}  
#endif  

	if(strDirPath[strDirPath.size() - 1] == TEXT('\\'))  
	{  
		strDirPath += TEXT("*.*");  
	}  
	else  
	{  
		strDirPath += TEXT("\\*.*");  
	}  

	WIN32_FIND_DATA fd;  
	HANDLE hdFind;  
	hdFind = FindFirstFile(strDirPath.c_str(),&fd);  
	if(hdFind != INVALID_HANDLE_VALUE)  
	{  
		do{  
			//At winXP system the "." means current directory, the ".."means parent directory.  
			if(fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY && fd.cFileName[0] != TEXT('.'))  
			{             
				//It is directory     
				TSTRING strNextDir = strPath;  
				if(strNextDir[strNextDir.size() -1] != TEXT('\\'))  
					strNextDir += TEXT("\\");  

				strNextDir += fd.cFileName;  

				DWORD dDirectorySize = GetDirectorySize(strNextDir);  
				dSize += dDirectorySize;          
			}  
			else if(fd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)  
			{     
				//It is file   
				TSTRING strPathFile = strPath;  
				if(strPathFile[strPathFile.size() - 1] != TEXT('\\'))  
					strPathFile += TEXT("\\");  

				strPathFile += fd.cFileName;  

				HANDLE hFile = CreateFile(strPathFile.c_str(),GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);  
				dSize += ::GetFileSize(hFile,NULL);  
				CloseHandle(hFile);  
			}  
		}while(FindNextFile(hdFind,&fd));  
	}  
	FindClose(hdFind);  
	return dSize;  
}  

BOOL DirectoryOperate::CheckDirectoryExist(const TSTRING &strPath)  
{  
	BOOL bReturn = FALSE;  
	if(strPath.size() >= MAX_PATH)  
	{  
		return FALSE;  
	}  
	WIN32_FIND_DATA fd;  
	HANDLE hdFind = FindFirstFile(strPath.c_str(),&fd);  
	if(hdFind != INVALID_HANDLE_VALUE)  
	{  
		if(fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)  
		{  
			bReturn = TRUE;  
		}  
	}  
	FindClose(hdFind);  
	return bReturn;  
}  

BOOL DirectoryOperate::CopyDirectory(const TSTRING &strDestinationPath,const TSTRING &strSourcePath)   
{  
	TSTRING strDirPath = strSourcePath;  
	BOOL bResult = TRUE;  
#ifdef _WIN32_WCE  
	if(strDirPath[0] != '\\')  
	{  
		return FALSE;  
	}  
#endif  
	if(strDirPath[strDirPath.size() - 1] == '\\')  
	{  
		strDirPath += TEXT("*.*");  
	}  
	else  
	{  
		strDirPath += TEXT("\\*.*");  
	}  

	WIN32_FIND_DATA fd;  
	HANDLE hdFind;  
	hdFind = FindFirstFile(strDirPath.c_str(),&fd);  
	if(hdFind != INVALID_HANDLE_VALUE)  
	{  
		do{  
			if(!CheckDirectoryExist(strDestinationPath))  
			{  
				CreateDirectory(strDestinationPath.c_str(),NULL);  

			}  

			//At winXP system the "." means current directory, the ".."means parent directory.  
			if(fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY && fd.cFileName[0] != TEXT('.'))  
			{         
				//It is directory     
				TSTRING strSourceNextDir = strSourcePath;  
				TSTRING strDestinationNextDir = strDestinationPath;  

				if(strSourceNextDir[strSourceNextDir.size() -1] != TEXT('\\'))  
				{  
					strSourceNextDir += TEXT("\\");  
				}  

				if(strDestinationNextDir[strDestinationNextDir.size() -1] != TEXT('\\'))  
				{  
					strDestinationNextDir += TEXT("\\");  
				}  

				strSourceNextDir += fd.cFileName;  
				strDestinationNextDir += fd.cFileName;  

				CreateDirectory(strDestinationNextDir.c_str(),NULL);  

				CopyDirectory(strDestinationNextDir,strSourceNextDir);  
			}  
			else if(fd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)  
			{     
				//It is file   
				TSTRING strSourceFilePath = strSourcePath;  
				TSTRING strDestinationFilePath = strDestinationPath;  

				if(strSourceFilePath[strSourceFilePath.size() - 1] != TEXT('\\'))  
				{  
					strSourceFilePath += TEXT("\\");  
				}  

				if(strDestinationFilePath[strDestinationFilePath.size() - 1] != TEXT('\\'))  
				{  
					strDestinationFilePath += TEXT("\\");  
				}  

				strSourceFilePath += fd.cFileName;  
				strDestinationFilePath += fd.cFileName;  

				//The file is read-only ,cancle the attributes of read-only  
				if(GetFileAttributes(strSourceFilePath.c_str()) & FILE_ATTRIBUTE_READONLY)   
					SetFileAttributes(strSourceFilePath.c_str(), FILE_ATTRIBUTE_NORMAL);   

				if(GetFileAttributes(strDestinationFilePath.c_str()) &FILE_ATTRIBUTE_READONLY)   
					SetFileAttributes(strDestinationFilePath.c_str(), FILE_ATTRIBUTE_NORMAL);  

				if(CopyFile(strSourceFilePath.c_str(),strDestinationFilePath.c_str(),FALSE)==FALSE)  
				{     
#ifdef _WIN32_WCE  
					RETAILMSG(TRUE,(TEXT("Copy File  %s  Failed ! Error Code : 0x%x/r/n"),strSourceFilePath.c_str(),GetLastError()));  
					ASSERT(FALSE);  
#else   
					TSTRING strInfo;  
					strInfo = TEXT("Copy File ");  
						strInfo += strSourceFilePath;  
					strInfo += TEXT("  Failed !");  
						OutputDebugString(strInfo.c_str());  
#endif  
					return FALSE;  
				}  
			}  
		}while(FindNextFile(hdFind,&fd));  
	}  
	else  
	{  
		bResult = FALSE;  
	}  
	FindClose(hdFind);  
	return bResult;  
}  

BOOL DirectoryOperate::FindFileFromDirectory(const TSTRING &DirectoryPath,const TSTRING &SuffixName,std::vector<TSTRING> &vFilePathList)  
{  
	if(DirectoryPath.size() >= MAX_PATH)  
	{  
		return FALSE;  
	}  

	TSTRING FindDirPath = DirectoryPath;  

	if(FindDirPath[FindDirPath.size() - 1] == TEXT('\\'))  
	{  
		FindDirPath += TEXT("*.*");  
	}  
	else  
	{  
		FindDirPath += TEXT("\\*.*");  
	}  

	WIN32_FIND_DATA fd;  
	HANDLE hdFind;  
	hdFind = FindFirstFile(FindDirPath.c_str(),&fd);  
	if(hdFind != INVALID_HANDLE_VALUE)  
	{  
		do{  
			//At winXP system the "." means current directory, the ".."means parent directory.  
			if(fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY && fd.cFileName[0] != TEXT('.'))  
			{         
				//it must be directory    
				TSTRING strFindNextDir = FindDirPath;  
				if(strFindNextDir[strFindNextDir.size() -1] != TEXT('\\'))  
					strFindNextDir += TEXT("\\");  

				strFindNextDir += fd.cFileName;  

				FindFileFromDirectory(strFindNextDir,SuffixName,vFilePathList);  
			}  
			else if(fd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)  
			{     
				//it is file   
				TSTRING strFindPathFile = DirectoryPath;  
				if(strFindPathFile[strFindPathFile.size() - 1] != TEXT('\\'))  
					strFindPathFile += TEXT("\\");  

				strFindPathFile += fd.cFileName;  
				if(CheckFileSuffix(fd.cFileName,SuffixName) == TRUE)  
				{  
					vFilePathList.push_back(strFindPathFile);  
				}  
			}  
		}while(FindNextFile(hdFind,&fd));  
	}  
	FindClose(hdFind);  

	return TRUE;  
}  

BOOL DirectoryOperate:: CheckFileSuffix(const TSTRING &FileName,const TSTRING &SuffixName)  
{  
	if(SuffixName.empty())  
	{  
		return FALSE;  
	}  

	if(FileName.empty())  
	{  
		return FALSE;  
	}  

	if(SuffixName[0] != TEXT('*'))  
	{     
		return (FileName.compare(SuffixName) == 0) ? TRUE : FALSE;  
	}  
	else  
	{  
		if(FileName.rfind(&SuffixName[1]) != TSTRING::npos)  
			return TRUE;  
	}  
	return FALSE;  
}  

