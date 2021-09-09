/******************************************************************
++  File Name : FFMClass.cpp
++  Description: ����ȴ��
---------------------------------------------------------------
++  Author:  Fei ZhaoDong
++  Create time: 2004/3/25 ���� 10:00:00
++  Version:     1.0
++  Modifier:  
++   Activities:    
++  Update List: 2004/3/29 ���� 02:59:45
*******************************************************************/
// FFMClass.cpp: implementation of the CFFMClass.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "FFMClass.h"
//#include <aclapi.h>
//#include <windows.h>
//#include <aclapi.h>
//#include <lmerr.h>
//
//#include <stdio.h>
//////////////////////////////////////////////////////////////////////
// CFFMServer
//////////////////////////////////////////////////////////////////////
using namespace Ysbot;

CFFMServer::CFFMServer()
{
	m_dwLastError = 0;
	m_fnpSetEntriesInAcl = NULL;
	_Init();
}

CFFMServer::~CFFMServer()
{
	Destory();
}

CFFMServer::CFFMServer(char *szFileName, char *szMapName, DWORD dwSize)
{
	// ���Զ��x�O�Ä�������ȴ�K
	_Init();
	Create(szFileName, szMapName, dwSize);
}

// ��ʼ����������
void CFFMServer::_Init()
{
	m_hFile = NULL;
	m_hFileMap = NULL;
	m_lpFileMapBuffer = NULL;
	m_pFileName = NULL;
	m_pMapName = NULL;
	m_dwSize = 0;
	m_bCreateFlag = FALSE;
	m_fnpSetEntriesInAcl = NULL;
}

// �Д��Ƿ�NT4.0���ϲ���ϵ�y
BOOL CFFMServer::_IsWinNTLater()
{
	OSVERSIONINFO Ver;
	BOOL bAbleVersion = FALSE;
	Ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (GetVersionEx(&Ver))
	{
		if (Ver.dwPlatformId == VER_PLATFORM_WIN32_NT
			&& Ver.dwMajorVersion >= 4)
		{
			bAbleVersion = TRUE;
		}
	}
	else
	{
		m_dwLastError = GetLastError();
	}
	return bAbleVersion;
}

// ጷŮ�ǰ����ȴ�,�K���³�ʼ������
void CFFMServer::Destory()
{
	if (m_lpFileMapBuffer != NULL)
	{
		UnmapViewOfFile(m_lpFileMapBuffer);
		m_lpFileMapBuffer = NULL;
	}
	if (m_hFileMap != NULL)
	{
		CloseHandle(m_hFileMap);
		m_hFileMap = NULL;
	}
	if (m_hFile && m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile = NULL;
	}
	if (m_pFileName != NULL)
	{
		free(m_pFileName);
		m_pFileName = NULL;
	}
	if (m_pMapName != NULL)
	{
		free(m_pMapName);
		m_pMapName = NULL;
	}
	_Init();
}

static void FreeSidEx(PSID oSID)
{
	try
	{
		FreeSid(oSID);
	}
	catch(...)
	{
	}
}

// ��������ȴ�K
BOOL CFFMServer::Create(char *szFileName, char *szMapName, DWORD dwSize)
{
	// ጷ����еĹ���ȴ�K
	if (m_bCreateFlag)
	{
		Destory();
	}
	// ��ؐ��������
	if (szFileName)
	{
		m_pFileName = _strdup(szFileName);
	}
	if (szMapName)
	{
		m_pMapName = _strdup(szMapName);
	}
	else
	{
		m_pMapName = _strdup(DEFAULT_MAPNAME);
	}
	if (dwSize > 0)
	{
		m_dwSize = dwSize;
	}
	else
	{
		m_dwSize = DEFAULT_MAPSIZE;
	}
	// ����������ȴ�
	if (m_pFileName)
	{
		m_hFile = CreateFile(
			CStringW(m_pFileName),
			GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			);
	}
	else
	{
		// Ĭ�J��r��,������ļ��Є�������ȴ�
		m_hFile = INVALID_HANDLE_VALUE;
	}
	if (_IsWinNTLater())
	{
		// Set DACL
		const int NUM_ACES = 2;   // number if ACEs int DACL
		// evryone -- read
		// creator -- full access
		// ��ʼ������
		PSID pEveryoneSID        = NULL; // everyoneȺ�MSID
		PSID pCreatorSID   = NULL; // creatorȺ�MSID
		PACL pFileMapACL         = NULL; // �ʂ��ȴ��ļ���DACL
		PSECURITY_DESCRIPTOR  pSD = NULL; // �ȴ��ļ���SD
		SECURITY_ATTRIBUTES   saFileMap; // �ȴ��ļ���SA
		EXPLICIT_ACCESS    ea[NUM_ACES]; // �ⲿ�L���Y�� 
		BOOL bHasErr   = FALSE; // ����ֵ
		// ������SID
		SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
		SID_IDENTIFIER_AUTHORITY SIDAuthCreator = SECURITY_CREATOR_SID_AUTHORITY;
		// Evryone
		if (!AllocateAndInitializeSid(&SIDAuthWorld, 1, SECURITY_WORLD_RID,
			0, 0, 0, 0, 0, 0, 0, &pEveryoneSID))
		{
			bHasErr = TRUE;
			goto Finish;
		}
		// Creator
		if (!AllocateAndInitializeSid(&SIDAuthCreator, 1, SECURITY_CREATOR_OWNER_RID,
			0, 0, 0, 0, 0, 0, 0, &pCreatorSID))
		{
			bHasErr = TRUE;
			goto Finish;
		}
		// ���ACE
		ZeroMemory(&ea, NUM_ACES * sizeof(EXPLICIT_ACCESS));
		// S-1-1-0 evryone, Ψ�x����
		ea[0].grfAccessPermissions = GENERIC_READ | GENERIC_WRITE;
		ea[0].grfAccessMode = SET_ACCESS;
		ea[0].grfInheritance= NO_INHERITANCE;
		ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
		ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
		ea[0].Trustee.ptstrName  = (LPTSTR) pEveryoneSID;
		// S-1-3-0 creator owner, ��ȫ����
		ea[1].grfAccessPermissions = STANDARD_RIGHTS_ALL;
		ea[1].grfAccessMode = SET_ACCESS;
		ea[1].grfInheritance= NO_INHERITANCE;
		ea[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
		ea[1].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
		ea[1].Trustee.ptstrName  = (LPTSTR) pCreatorSID;
		// �����K���ACL
		if(NULL == m_fnpSetEntriesInAcl)
		{
			HINSTANCE hLib = ::LoadLibrary(_T("Advapi32.dll"));
			if(NULL != hLib)
			{
				m_fnpSetEntriesInAcl = (PSetEntriesInAcl)GetProcAddress(hLib,"SetEntriesInAclA");
				::FreeLibrary(hLib);
				hLib = NULL;
			}
		}
		if (ERROR_SUCCESS != m_fnpSetEntriesInAcl(NUM_ACES, ea, NULL, &pFileMapACL))
		{
			bHasErr = TRUE;
			goto Finish;
		}
		// �����K��ʼ��SD
		pSD = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH); 
		if (NULL == pSD)
		{
			bHasErr = TRUE;
			goto Finish;
		} 
		if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION)) 
		{
			bHasErr = TRUE;
			goto Finish;
		}
		// ���ACL��SD��ȥ
		if (!SetSecurityDescriptorDacl(pSD,
			TRUE,     // fDaclPresent flag   
			pFileMapACL, 
			FALSE))   // not a default DACL 
		{
			bHasErr = TRUE;
			goto Finish;
		}
		// �O��SA
		saFileMap.nLength = sizeof(SECURITY_ATTRIBUTES);
		saFileMap.bInheritHandle = FALSE;
		saFileMap.lpSecurityDescriptor = pSD;
		// ��������ȴ��ļ� step1
		if (m_hFile != NULL)
		{
			m_hFileMap = CreateFileMapping(
				m_hFile,
				&saFileMap,
				PAGE_READWRITE,
				0,
				m_dwSize,
				CStringW(m_pMapName));
			if (NULL == m_hFileMap)
			{
				m_hFileMap = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE,
					TRUE, CStringW(m_pMapName));
				if (NULL == m_hFileMap)
				{
					m_dwLastError = GetLastError(); 
					Destory();
					goto Finish;
				}
			}
		}
Finish:
		//if (pSD != NULL)
		//{
		//	LocalFree(pSD);
		//}		
		pSD = NULL;
		if (pFileMapACL != NULL)
		{
			LocalFree(pFileMapACL);
			pFileMapACL = NULL;
		}
		if (pEveryoneSID != NULL)
		{
			FreeSidEx(pEveryoneSID);
			pEveryoneSID = NULL;
		}
		if (pCreatorSID != NULL)
		{
			FreeSidEx(pCreatorSID);
			pCreatorSID = NULL;
		}
		if (bHasErr) 
		{
			m_dwLastError = GetLastError();
			return FALSE;
		}
	}
	else
	{
		// ��������ȴ��ļ�
		if (m_hFile)
		{
			m_hFileMap = CreateFileMapping(
				m_hFile,
				NULL,
				PAGE_READWRITE,
				0,
				m_dwSize,
				CStringW(m_pMapName));
			if (NULL == m_hFileMap)
			{
				m_dwLastError = GetLastError();
				Destory();
				return FALSE;
			}
		}
	}
	// ӳ���ļ�ָᘵ��Ñ�
	if (m_hFileMap) // step2
	{
		m_lpFileMapBuffer = MapViewOfFile(
			m_hFileMap,
			FILE_MAP_ALL_ACCESS,
			0,
			0,
			m_dwSize);
		if (NULL == m_lpFileMapBuffer)
		{
			m_dwLastError = GetLastError();
			Destory();
			return FALSE;
		}
	}
	m_bCreateFlag = TRUE;
	return TRUE;
}

// �@ȡ�ȴ��ļ�ָ�
LPVOID CFFMServer::GetBuffer()
{
	return (m_lpFileMapBuffer)?(m_lpFileMapBuffer):(NULL);
}

// �@ȡ�ȴ��ļ���С
DWORD CFFMServer::GetSize()
{
	return m_dwSize;
}

BOOL CFFMServer::WriteCmdData(DWORD nCommandCode, DWORD dwDataSize, const LPVOID pBuf)
{
	// �z򞔵���ĺ�����
	if (NULL == GetBuffer())
	{
		m_dwLastError = ERROR_NO_MAPFILE;
		SetLastError(ERROR_NO_MAPFILE);
		return FALSE;
	}
	if (NETRGUSER_CMD_NONE == nCommandCode)
	{
		m_dwLastError = ERROR_INVALID_CMDCODE;
		SetLastError(ERROR_INVALID_CMDCODE);
		return FALSE;
	}
	if (dwDataSize > 0 && pBuf == NULL)
	{
		m_dwLastError = ERROR_INVALID_USER_BUFFER;
		SetLastError(ERROR_INVALID_USER_BUFFER);
		return FALSE;
	}
	if (dwDataSize + sizeof(DATA_HEADER) > GetSize())
	{
		m_dwLastError = ERROR_BUFFER_OVERFLOW;
		SetLastError(ERROR_BUFFER_OVERFLOW);
		return FALSE;
	}
	// ������Y��
	// �ļ��^
	DATA_HEADER dataHeader;
// 	dataHeader.dwConfirmCode = 0;
 	dataHeader.nCommandCode = nCommandCode;
	dataHeader.dwDataSize = dwDataSize;
	ZeroMemory(GetBuffer(), GetSize());
	memcpy(GetBuffer(), &dataHeader, sizeof(DATA_HEADER));
	// �����K
	LPDATA_HEADER pData = (LPDATA_HEADER)GetBuffer();
	memcpy(pData->bInfo, pBuf, dwDataSize);
 
	return TRUE;
}

// �xȡ�����
BOOL CFFMServer::ReadCmdData(DWORD dwCommandCode, DWORD dwBufSize, LPVOID pOutBuf)
{
	_ASSERT (pOutBuf != NULL);
	LPDATA_HEADER pHeader = (LPDATA_HEADER)GetBuffer();
	if (NULL == pHeader)
	{
		m_dwLastError = ERROR_NO_MAPFILE;
		SetLastError(ERROR_NO_MAPFILE);
		return FALSE;
	}
	if (NETRGUSER_CFM_CODE != pHeader->dwConfirmCode)
	{
		m_dwLastError = ERROR_INVALID_CFMCODE;
		SetLastError(ERROR_INVALID_CFMCODE);
		return FALSE;
	}
	if (NETRGUSER_CMD_NONE == pHeader->nCommandCode)
	{
		m_dwLastError = ERROR_INVALID_CMDCODE;
		SetLastError(ERROR_INVALID_CMDCODE);
		return FALSE;
	}
	if (pHeader->dwDataSize > dwBufSize)
	{
		m_dwLastError = ERROR_BUFFER_OVERFLOW;
		SetLastError(ERROR_BUFFER_OVERFLOW);
		return FALSE;
	}
	if (pHeader->nCommandCode != dwCommandCode)
	{
		m_dwLastError = ERROR_INVALID_CMDCODE;
		SetLastError(ERROR_INVALID_CMDCODE);
		return FALSE;
	}

	ZeroMemory(pOutBuf, dwBufSize);
	// ��ؐ���������n�^
	memcpy(pOutBuf, pHeader->bInfo, pHeader->dwDataSize);
	pHeader->dwConfirmCode = 0;
	return TRUE;
}

BOOL CFFMServer::WriteBuffer(const char* pBuf, unsigned int datalen)
{
	if ( GetBuffer() )
	{
		memcpy((char *)GetBuffer(), pBuf, datalen);
 	}
 	return TRUE;
}

BOOL CFFMServer::ReadBuffer(char* pBuf, unsigned int& datalen)
{
	if ( GetBuffer() )
	{
		memcpy(pBuf, (char *)GetBuffer(), datalen);
	}	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CFFMClient
//////////////////////////////////////////////////////////////////////
CFFMClient::CFFMClient()
{
	m_dwLastError = 0;
	_Init();
}

CFFMClient::~CFFMClient()
{
	Destory();
}

CFFMClient::CFFMClient(DWORD dwAccess, char *szMapName, DWORD dwSize)
{
	_Init();
	m_dwSize = dwSize;
	if (!Open(dwAccess, szMapName, dwSize))
	{
		Destory();
	}
}

// ��ʼ������
void CFFMClient::_Init()
{
	m_hFileMap = NULL;
	m_lpFileMapBuffer = NULL;
	m_pMapName = NULL;
	m_bOpenFlag = FALSE;
	m_dwSize = DEFAULT_MAPSIZE;
}

// �P�]��ǰ���ȴ��ļ����L��
void CFFMClient::Destory()
{
	if (m_lpFileMapBuffer)
	{
		UnmapViewOfFile(m_lpFileMapBuffer);
		m_lpFileMapBuffer = NULL;
	}
	if (m_hFileMap)
	{
		CloseHandle(m_hFileMap);
		m_hFileMap = NULL;
	}
	if (m_pMapName)
	{
		free(m_pMapName);
		m_pMapName = NULL;
	}
	_Init();
}

// ���_һ���ȴ��ļ� FILE_MAP_WRITE
BOOL CFFMClient::Open(DWORD dwAccess, char *szMapName, DWORD dwSize)
{
	if (m_bOpenFlag)
	{
		Destory();
	}
	if (szMapName != NULL)
	{
		m_pMapName = _strdup(szMapName);
	}
	else
	{
		m_pMapName = _strdup(DEFAULT_MAPNAME);
	}
	m_hFileMap = OpenFileMapping(dwAccess, TRUE, CStringW(m_pMapName));
	if (NULL == m_hFileMap)
	{
		m_dwLastError = GetLastError();
		Destory();
		return FALSE;
	}
	m_lpFileMapBuffer = MapViewOfFile(m_hFileMap, dwAccess, 0, 0, dwSize);
	if (NULL == m_lpFileMapBuffer)
	{
		m_dwLastError = GetLastError();
		Destory();
		return FALSE;
	}
	m_bOpenFlag = TRUE;
	return TRUE;
}

BOOL CFFMClient::ReadBuffer(char* pBuf, unsigned int& datalen)
{
	if ( GetBuffer() )
	{
		memcpy(pBuf, (char *)GetBuffer(), datalen);
	}	
	return TRUE;
}

// �@ȡ��ǰ�ȴ��ļ���ָ�
LPVOID CFFMClient::GetBuffer()
{
	return (m_lpFileMapBuffer)?(m_lpFileMapBuffer):(NULL);
}

// �xȡ�������С
BOOL CFFMClient::GetCmdDataSize(DWORD *pDataSize)
{
	_ASSERT(pDataSize != NULL);
	*pDataSize = 0;
	LPDATA_HEADER pHeader = (LPDATA_HEADER)GetBuffer();
	if (NULL == pHeader)
	{
		m_dwLastError = ERROR_NO_MAPFILE;
		SetLastError(ERROR_NO_MAPFILE);
		return FALSE;
	}
	if (NETRGUSER_CFM_CODE != pHeader->dwConfirmCode)
	{
		m_dwLastError = ERROR_INVALID_CFMCODE;
		SetLastError(ERROR_INVALID_CFMCODE);
		return FALSE;
	}
	if (NETRGUSER_CMD_NONE == pHeader->nCommandCode)
	{
		m_dwLastError = ERROR_INVALID_CMDCODE;
		SetLastError(ERROR_INVALID_CMDCODE);
		return FALSE;
	}
	*pDataSize = pHeader->dwDataSize;
	return TRUE;
}

// �xȡ�����
BOOL CFFMClient::ReadCmdData(DWORD dwCommandCode, DWORD dwBufSize, LPVOID pOutBuf)
{
	_ASSERT (pOutBuf != NULL);
	LPDATA_HEADER pHeader = (LPDATA_HEADER)GetBuffer();
	if (NULL == pHeader)
	{
		m_dwLastError = ERROR_NO_MAPFILE;
		SetLastError(ERROR_NO_MAPFILE);
		return FALSE;
	}
	if (NETRGUSER_CFM_CODE != pHeader->dwConfirmCode)
	{
		m_dwLastError = ERROR_INVALID_CFMCODE;
		SetLastError(ERROR_INVALID_CFMCODE);
		return FALSE;
	}
	if (NETRGUSER_CMD_NONE == pHeader->nCommandCode)
	{
		m_dwLastError = ERROR_INVALID_CMDCODE;
		SetLastError(ERROR_INVALID_CMDCODE);
		return FALSE;
	}
	if (pHeader->dwDataSize > dwBufSize)
	{
		m_dwLastError = ERROR_BUFFER_OVERFLOW;
		SetLastError(ERROR_BUFFER_OVERFLOW);
		return FALSE;
	}
	if (pHeader->nCommandCode != dwCommandCode)
	{
		m_dwLastError = ERROR_INVALID_CMDCODE;
		SetLastError(ERROR_INVALID_CMDCODE);
		return FALSE;
	}
	 
	ZeroMemory(pOutBuf, dwBufSize);
	// ��ؐ���������n�^
	memcpy(pOutBuf, pHeader->bInfo, pHeader->dwDataSize);
	pHeader->dwConfirmCode = 0;
	return TRUE;
}

BOOL CFFMClient::WriteCmdData(DWORD memSize, DWORD nCommandCode, DWORD dwDataSize, const LPVOID pBuf)
{
	if(!memSize)
		memSize = DEFAULT_MAPSIZE;	
	m_dwSize = memSize;
	// �z򞔵���ĺ�����
	if (NULL == GetBuffer())
	{
		m_dwLastError = ERROR_NO_MAPFILE;
		SetLastError(ERROR_NO_MAPFILE);
		return FALSE;
	}
	if (NETRGUSER_CMD_NONE == nCommandCode)
	{
		m_dwLastError = ERROR_INVALID_CMDCODE;
		SetLastError(ERROR_INVALID_CMDCODE);
		return FALSE;
	}
	if (dwDataSize > 0 && pBuf == NULL)
	{
		m_dwLastError = ERROR_INVALID_USER_BUFFER;
		SetLastError(ERROR_INVALID_USER_BUFFER);
		return FALSE;
	}
	if (dwDataSize + sizeof(DATA_HEADER) > GetSize())
	{
		m_dwLastError = ERROR_BUFFER_OVERFLOW;
		SetLastError(ERROR_BUFFER_OVERFLOW);
		return FALSE;
	}
	// ������Y��
	// �ļ��^
	DATA_HEADER dataHeader;
	dataHeader.nCommandCode = nCommandCode;
	dataHeader.dwDataSize = dwDataSize;
	ZeroMemory(GetBuffer(), GetSize());
	memcpy(GetBuffer(), &dataHeader, sizeof(DATA_HEADER));
	// �����K
	LPDATA_HEADER pData = (LPDATA_HEADER)GetBuffer();
	memcpy(pData->bInfo, pBuf, dwDataSize);
	return TRUE;
}
// �@ȡ�ȴ��ļ���С
DWORD CFFMClient::GetSize()
{
	return m_dwSize;
}
