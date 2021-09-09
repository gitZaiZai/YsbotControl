/******************************************************************
++  File Name :   FFMClass.h
++  Description: 共享內存類
---------------------------------------------------------------
++  Author:  Fei ZhaoDong
++  Create time: 2004/3/25 上午 10:00:00
++  Version:     1.1
++  Modifier:  
++  Activities:    
++  Update List: 2004/3/30 下午 02:59:45
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
// 以下為共享內存定義
#define DEFAULT_FILENAME NULL    // 默認的文件名
#define DEFAULT_MAPNAME  "Local//_FZD_MAP_"   // 默認的共享內存名
#define SERVER_MAPNAME  "Local//MAP_SERVER"   // 默認的共享內存名

#define DEFAULT_MAPSIZE  (0xFFFF + 1)  // 默認的共享內存大小
  

typedef DWORD (WINAPI *PSetEntriesInAcl)(ULONG, PEXPLICIT_ACCESS, PACL, PACL*);

// 用於存儲應答數據的共享內存文件格式 (暫不用)
typedef struct _tagANS_HEADER
{
}ANS_HEADER, *LPANS_HEADER;
 
//////////////////////////////////////////////////////////////////////
// 類定義，共享內存服務端
class CFFMServer  
{
public:
	CFFMServer();
	virtual ~CFFMServer();
	CFFMServer(char *szFileName, char *szMapName, DWORD dwSize);
protected:
	PSetEntriesInAcl m_fnpSetEntriesInAcl;
	HANDLE m_hFile;   // 映射文件句柄
	HANDLE m_hFileMap;   // 內存文件句柄
	LPVOID m_lpFileMapBuffer; // 緩衝區指針
	char *m_pFileName;  // 映射文件名
	char *m_pMapName;  // 內存文件名
	DWORD m_dwSize;   // 緩衝區大小
	BOOL m_bCreateFlag;  // 是否已創建共享內存
	DWORD   m_dwLastError;  // 錯誤代碼
private:
	void _Init();    // 初始化參數
	BOOL _IsWinNTLater();  // 判斷當前操作系統
public:
	BOOL Create(char *szFileName = DEFAULT_FILENAME,
		char *szMapName = DEFAULT_MAPNAME,
		DWORD dwSize = DEFAULT_MAPSIZE); // 新建共享內存
	LPVOID GetBuffer();      // 獲取內存文件指針
	DWORD GetSize();      // 獲取內存文件大小
	void Destory();       // 銷毀已有的共享內存
	BOOL WriteCmdData(      // 寫入命令數據
		DWORD nCommandCode,
		DWORD dwDataSize,
		const LPVOID pBuf);
	BOOL ReadCmdData(       // 讀取命令數據
		DWORD dwCommandCode,
		DWORD dwBufSize,
		LPVOID pOutBuf);

	BOOL WriteBuffer( const char* pBuf, unsigned int datalen);
	BOOL ReadBuffer( char* pBuf, unsigned int& datalen);

};

//////////////////////////////////////////////////////////////////////
// 類定義，共享內存客戶端
class CFFMClient  
{
public:
	CFFMClient();
	virtual ~CFFMClient();
	CFFMClient(DWORD dwAccess, char *szMapName, DWORD dwSize);
protected:
	HANDLE m_hFileMap;   // 內存文件句柄
	LPVOID m_lpFileMapBuffer; // 內存文件指針
	char *m_pMapName;  // 內存文件名
	DWORD m_dwSize;   // 緩衝區大小
	BOOL m_bOpenFlag;  // 是否已經打開了一個內存文件
	DWORD   m_dwLastError;  // 錯誤代碼
private:
	void _Init();    // 初始化參數
public:
	BOOL Open(DWORD dwAccess = FILE_MAP_READ | FILE_MAP_WRITE,
		char *szMapName = DEFAULT_MAPNAME,
		DWORD dwSize = 0);      // 打開一個內存文件
	LPVOID GetBuffer();       // 獲取當前內存文件的指針
	void Destory();        // 關閉當前對內存文件的訪問
	DWORD GetSize();      // 獲取內存文件大小
	BOOL GetCmdDataSize(DWORD *pDataSize);  // 讀取命令數據大小
	BOOL ReadCmdData(       // 讀取命令數據
		DWORD dwCommandCode,
		DWORD dwBufSize,
		LPVOID pOutBuf);
	BOOL WriteCmdData(      // 寫入命令數據
		DWORD memSize, 
		DWORD nCommandCode,
		DWORD dwDataSize,
		const LPVOID pBuf);
	BOOL ReadBuffer(char* pBuf, unsigned int& datalen);

};
#endif // FZD_FFMCLASS_H
