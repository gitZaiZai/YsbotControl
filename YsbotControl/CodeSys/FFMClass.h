/******************************************************************
++  File Name :   FFMClass.h
++  Description: ����ȴ��
---------------------------------------------------------------
++  Author:  Fei ZhaoDong
++  Create time: 2004/3/25 ���� 10:00:00
++  Version:     1.1
++  Modifier:  
++  Activities:    
++  Update List: 2004/3/30 ���� 02:59:45
*******************************************************************/
// FFMClass.h: interface for the CFFMClass.
//
//////////////////////////////////////////////////////////////////////
#ifndef FZD_FFMCLASS_H
#define FZD_FFMCLASS_H
//#include <Winbase.h>
#include <aclapi.h>
#include "ServerData.h"

//////////////////////////////////////////////////////////////////////
// Macro definition
// ���鹲��ȴ涨�x
#define DEFAULT_FILENAME NULL    // Ĭ�J���ļ���
#define DEFAULT_MAPNAME  "Local//_FZD_MAP_"   // Ĭ�J�Ĺ���ȴ���
#define SERVER_MAPNAME  "Local//MAP_SERVER"   // Ĭ�J�Ĺ���ȴ���

#define DEFAULT_MAPSIZE  (0xFFFF + 1)  // Ĭ�J�Ĺ���ȴ��С
  

typedef DWORD (WINAPI *PSetEntriesInAcl)(ULONG, PEXPLICIT_ACCESS, PACL, PACL*);

// ��춴惦���𔵓��Ĺ���ȴ��ļ���ʽ (������)
typedef struct _tagANS_HEADER
{
}ANS_HEADER, *LPANS_HEADER;
 
//////////////////////////////////////////////////////////////////////
// ��x������ȴ���ն�
class CFFMServer  
{
public:
	CFFMServer();
	virtual ~CFFMServer();
	CFFMServer(char *szFileName, char *szMapName, DWORD dwSize);
protected:
	PSetEntriesInAcl m_fnpSetEntriesInAcl;
	HANDLE m_hFile;   // ӳ���ļ����
	HANDLE m_hFileMap;   // �ȴ��ļ����
	LPVOID m_lpFileMapBuffer; // ���n�^ָ�
	char *m_pFileName;  // ӳ���ļ���
	char *m_pMapName;  // �ȴ��ļ���
	DWORD m_dwSize;   // ���n�^��С
	BOOL m_bCreateFlag;  // �Ƿ��ф�������ȴ�
	DWORD   m_dwLastError;  // �e�`���a
private:
	void _Init();    // ��ʼ������
	BOOL _IsWinNTLater();  // �Дஔǰ����ϵ�y
public:
	BOOL Create(char *szFileName = DEFAULT_FILENAME,
		char *szMapName = DEFAULT_MAPNAME,
		DWORD dwSize = DEFAULT_MAPSIZE); // �½�����ȴ�
	LPVOID GetBuffer();      // �@ȡ�ȴ��ļ�ָ�
	DWORD GetSize();      // �@ȡ�ȴ��ļ���С
	void Destory();       // �N�����еĹ���ȴ�
	BOOL WriteCmdData(      // ���������
		DWORD nCommandCode,
		DWORD dwDataSize,
		const LPVOID pBuf);
	BOOL ReadCmdData(       // �xȡ�����
		DWORD dwCommandCode,
		DWORD dwBufSize,
		LPVOID pOutBuf);

	BOOL WriteBuffer( const char* pBuf, unsigned int datalen);
	BOOL ReadBuffer( char* pBuf, unsigned int& datalen);

};

//////////////////////////////////////////////////////////////////////
// ��x������ȴ�͑���
class CFFMClient  
{
public:
	CFFMClient();
	virtual ~CFFMClient();
	CFFMClient(DWORD dwAccess, char *szMapName, DWORD dwSize);
protected:
	HANDLE m_hFileMap;   // �ȴ��ļ����
	LPVOID m_lpFileMapBuffer; // �ȴ��ļ�ָ�
	char *m_pMapName;  // �ȴ��ļ���
	DWORD m_dwSize;   // ���n�^��С
	BOOL m_bOpenFlag;  // �Ƿ��ѽ����_��һ���ȴ��ļ�
	DWORD   m_dwLastError;  // �e�`���a
private:
	void _Init();    // ��ʼ������
public:
	BOOL Open(DWORD dwAccess = FILE_MAP_READ | FILE_MAP_WRITE,
		char *szMapName = DEFAULT_MAPNAME,
		DWORD dwSize = 0);      // ���_һ���ȴ��ļ�
	LPVOID GetBuffer();       // �@ȡ��ǰ�ȴ��ļ���ָ�
	void Destory();        // �P�]��ǰ���ȴ��ļ����L��
	DWORD GetSize();      // �@ȡ�ȴ��ļ���С
	BOOL GetCmdDataSize(DWORD *pDataSize);  // �xȡ�������С
	BOOL ReadCmdData(       // �xȡ�����
		DWORD dwCommandCode,
		DWORD dwBufSize,
		LPVOID pOutBuf);
	BOOL WriteCmdData(      // ���������
		DWORD memSize, 
		DWORD nCommandCode,
		DWORD dwDataSize,
		const LPVOID pBuf);
	BOOL ReadBuffer(char* pBuf, unsigned int& datalen);

};
#endif // FZD_FFMCLASS_H
