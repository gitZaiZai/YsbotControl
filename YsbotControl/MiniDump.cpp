/*
	Author:	Vladimir Sedach.

	Purpose: demo of Call Stack creation by our own means,
	and with MiniDumpWriteDump() function of DbgHelp.dll.
*/

#include "StdAfx.h"
#include "MiniDump.h"
#include <Shlwapi.h>

#pragma comment(lib,"shlwapi.lib")

HMODULE	hDbgHelp;
MINIDUMP_WRITE_DUMP	MiniDumpWriteDump_;

CREATE_TOOL_HELP32_SNAPSHOT	CreateToolhelp32Snapshot_;
MODULE32_FIRST	Module32First_;
MODULE32_NEST	Module32Next_;

#define	DUMP_SIZE_MAX	8000	//max size of our dump
#define	CALL_TRACE_MAX	((DUMP_SIZE_MAX - 2000) / (MAX_PATH + 40))	//max number of traced calls
#define	NL				_T("\r\n")	//new line

extern CString GetExePath();

//****************************************************************************************
BOOL WINAPI Get_Module_By_Ret_Addr(PBYTE Ret_Addr, PTCHAR Module_Name, PBYTE & Module_Addr)
//****************************************************************************************
// Find module by Ret_Addr (address in the module).
// Return Module_Name (full path) and Module_Addr (start address).
// Return TRUE if found.
{
	MODULEENTRY32	M = {sizeof(M)};
	HANDLE	hSnapshot;

	Module_Name[0] = 0;
	
	if (CreateToolhelp32Snapshot_)
	{
		hSnapshot = CreateToolhelp32Snapshot_(TH32CS_SNAPMODULE, 0);
		
		if ((hSnapshot != INVALID_HANDLE_VALUE) &&
			Module32First_(hSnapshot, &M))
		{
			do
			{
				if (DWORD(Ret_Addr - M.modBaseAddr) < M.modBaseSize)
				{
					lstrcpyn(Module_Name, M.szExePath, MAX_PATH);
					Module_Addr = M.modBaseAddr;
					break;
				}
			} while (Module32Next_(hSnapshot, &M));
		}

		CloseHandle(hSnapshot);
	}

	return !!Module_Name[0];
} //Get_Module_By_Ret_Addr

//******************************************************************
int WINAPI Get_Call_Stack(PEXCEPTION_POINTERS pException, PTCHAR Str)
//******************************************************************
// Fill Str with call stack info.
// pException can be either GetExceptionInformation() or NULL.
// If pException = NULL - get current call stack.
{
	TCHAR	Module_Name[MAX_PATH];
	PBYTE	Module_Addr = 0;
	PBYTE	Module_Addr_1;
	int		Str_Len;
	
	typedef struct STACK
	{
		STACK *	Ebp;
		PBYTE	Ret_Addr;
		DWORD	Param[0];
	} STACK, * PSTACK;

	STACK	Stack = {0, 0};
	PSTACK	Ebp;

	if (pException)		//fake frame for exception address
	{
		Stack.Ebp = (PSTACK)pException->ContextRecord->Ebp;
		Stack.Ret_Addr = (PBYTE)pException->ExceptionRecord->ExceptionAddress;
		Ebp = &Stack;
	}
	else
	{
		Ebp = (PSTACK)&pException - 1;	//frame addr of Get_Call_Stack()

		// Skip frame of Get_Call_Stack().
		if (!IsBadReadPtr(Ebp, sizeof(PSTACK)))
			Ebp = Ebp->Ebp;		//caller ebp
	}

	Str[0] = 0;
	Str_Len = 0;

	// Trace CALL_TRACE_MAX calls maximum - not to exceed DUMP_SIZE_MAX.
	// Break trace on wrong stack frame.
	for (int Ret_Addr_I = 0;
		(Ret_Addr_I < CALL_TRACE_MAX) && !IsBadReadPtr(Ebp, sizeof(PSTACK)) && !IsBadCodePtr(FARPROC(Ebp->Ret_Addr));
		Ret_Addr_I++, Ebp = Ebp->Ebp)
	{
		// If module with Ebp->Ret_Addr found.
		if (Get_Module_By_Ret_Addr(Ebp->Ret_Addr, Module_Name, Module_Addr_1))
		{
			if (Module_Addr_1 != Module_Addr)	//new module
			{
				// Save module's address and full path.
				Module_Addr = Module_Addr_1;
				Str_Len += wsprintf(Str + Str_Len, NL _T("%08X  %s"), Module_Addr, Module_Name);
			}

			// Save call offset.
			Str_Len += wsprintf(Str + Str_Len,
				NL _T("  +%08X"), Ebp->Ret_Addr - Module_Addr);

			// Save 5 params of the call. We don't know the real number of params.
			if (pException && !Ret_Addr_I)	//fake frame for exception address
				Str_Len += wsprintf(Str + Str_Len, _T("  Exception Offset"));
			else if (!IsBadReadPtr(Ebp, sizeof(PSTACK) + 5 * sizeof(DWORD)))
			{
				Str_Len += wsprintf(Str + Str_Len, _T("  (%X, %X, %X, %X, %X)"),
					Ebp->Param[0], Ebp->Param[1], Ebp->Param[2], Ebp->Param[3], Ebp->Param[4]);
			}
		}
		else
			Str_Len += wsprintf(Str + Str_Len, NL _T("%08X"), Ebp->Ret_Addr);
	}

	return Str_Len;
} //Get_Call_Stack

//***********************************
int WINAPI Get_Version_Str(PTCHAR Str)
//***********************************
// Fill Str with Windows version.
{
	OSVERSIONINFOEX	V = {sizeof(OSVERSIONINFOEX)};	//EX for NT 5.0 and later

	if (!GetVersionEx((POSVERSIONINFO)&V))
	{
		ZeroMemory(&V, sizeof(V));
		V.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx((POSVERSIONINFO)&V);
	}

	if (V.dwPlatformId != VER_PLATFORM_WIN32_NT)
		V.dwBuildNumber = LOWORD(V.dwBuildNumber);	//for 9x HIWORD(dwBuildNumber) = 0x04xx

	return wsprintf(Str,
		NL _T("Windows:  %d.%d.%d, SP %d.%d, Product Type %d"),	//SP - service pack, Product Type - VER_NT_WORKSTATION,...
		V.dwMajorVersion, V.dwMinorVersion, V.dwBuildNumber, V.wServicePackMajor, V.wServicePackMinor/*, V.wProductType*/);
} //Get_Version_Str

//*************************************************************
PTCHAR WINAPI Get_Exception_Info(PEXCEPTION_POINTERS pException)
//*************************************************************
// Allocate Str[DUMP_SIZE_MAX] and return Str with dump, if !pException - just return call stack in Str.
{
	PTCHAR		Str;
	int			Str_Len;
	int			i;
	TCHAR		Module_Name[MAX_PATH];
	PBYTE		Module_Addr;
	HANDLE		hFile;
	FILETIME	Last_Write_Time;
	FILETIME	Local_File_Time;
	SYSTEMTIME	T;
	
	Str = new TCHAR[DUMP_SIZE_MAX];

	if (!Str)
		return NULL;

	Str_Len = 0;
	Str_Len += Get_Version_Str(Str + Str_Len);

	Str_Len += wsprintf(Str + Str_Len, NL _T("Process:  "));
	GetModuleFileName(NULL, Str + Str_Len, MAX_PATH);
	Str_Len = lstrlen(Str);

	// If exception occurred.
	if (pException)
	{
		EXCEPTION_RECORD &	E = *pException->ExceptionRecord;
		CONTEXT &			C = *pException->ContextRecord;

		// If module with E.ExceptionAddress found - save its path and date.
		if (Get_Module_By_Ret_Addr((PBYTE)E.ExceptionAddress, Module_Name, Module_Addr))
		{
			Str_Len += wsprintf(Str + Str_Len,
				NL _T("Module:  %s"), Module_Name);

			if ((hFile = CreateFile(Module_Name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL, NULL)) != INVALID_HANDLE_VALUE)
			{
				if (GetFileTime(hFile, NULL, NULL, &Last_Write_Time))
				{
					FileTimeToLocalFileTime(&Last_Write_Time, &Local_File_Time);
					FileTimeToSystemTime(&Local_File_Time, &T);

					Str_Len += wsprintf(Str + Str_Len,
						NL _T("Date Modified:  %02d/%02d/%d"),
						T.wMonth, T.wDay, T.wYear);
				}
				CloseHandle(hFile);
			}
		}
		else
		{
			Str_Len += wsprintf(Str + Str_Len,
				NL _T("Exception Addr:  %08X"), E.ExceptionAddress);
		}
		
		Str_Len += wsprintf(Str + Str_Len,
			NL _T("Exception Code:  %08X"), E.ExceptionCode);
		
		if (E.ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
		{
			// Access violation type - Write/Read.
			Str_Len += wsprintf(Str + Str_Len,
				NL _T("%s Address:  %08X"),
				(E.ExceptionInformation[0]) ? _T("Write") : _T("Read"), E.ExceptionInformation[1]);
		}

		// Save instruction that caused exception.
		Str_Len += wsprintf(Str + Str_Len, NL _T("Instruction: "));
		for (i = 0; i < 16; i++)
			Str_Len += wsprintf(Str + Str_Len, _T(" %02X"), PBYTE(E.ExceptionAddress)[i]);

		// Save registers at exception.
		Str_Len += wsprintf(Str + Str_Len, NL _T("Registers:"));
		Str_Len += wsprintf(Str + Str_Len, NL _T("EAX: %08X  EBX: %08X  ECX: %08X  EDX: %08X"), C.Eax, C.Ebx, C.Ecx, C.Edx);
		Str_Len += wsprintf(Str + Str_Len, NL _T("ESI: %08X  EDI: %08X  ESP: %08X  EBP: %08X"), C.Esi, C.Edi, C.Esp, C.Ebp);
		Str_Len += wsprintf(Str + Str_Len, NL _T("EIP: %08X  EFlags: %08X"), C.Eip, C.EFlags);
	} //if (pException)
	
	// Save call stack info.
	Str_Len += wsprintf(Str + Str_Len, NL _T("Call Stack:") );
	Get_Call_Stack(pException, Str + Str_Len);

	if (Str[0] == NL[0])
		lstrcpy(Str, Str + sizeof(NL) - 1);

	return Str;
} //Get_Exception_Info

//*************************************************************************************
void WINAPI Create_Dump(PEXCEPTION_POINTERS pException, BOOL File_Flag, BOOL Show_Flag)
//*************************************************************************************
// Create dump. 
// pException can be either GetExceptionInformation() or NULL.
// If File_Flag = TRUE - write dump files (.dmz and .dmp) with the name of the current process.
// If Show_Flag = TRUE - show message with Get_Exception_Info() dump.
{
	HANDLE	hDump_File;
	PTCHAR	Str;
	DWORD	Bytes;
	DWORD	nLen = 0;

	CString strDir,strTXTFile,strDMPFile;
	CString strDate,strTotal;
	CTime	tm = CTime::GetCurrentTime();
	
	strDir.Format(_T("%s\\Log"),GetExePath());
	strTXTFile.Format(_T("%s\\Log\\%04d-%02d-%02d %02d%02d%02d.txt"),GetExePath(),tm.GetYear(),tm.GetMonth(),
		tm.GetDay(),tm.GetHour(),tm.GetMinute(),tm.GetSecond());
	strDMPFile.Format(_T("%s\\Log\\%04d-%02d-%02d %02d%02d%02d.dmp"),GetExePath(),tm.GetYear(),tm.GetMonth(),
		tm.GetDay(),tm.GetHour(),tm.GetMinute(),tm.GetSecond());

	if(!PathFileExists(strDir))
		CreateDirectory(strDir,NULL);

	Str = Get_Exception_Info(pException);

	//if (Show_Flag && Str)
	//	MessageBox(NULL, Str, "MiniDump", MB_ICONHAND | MB_OK);

	if (File_Flag)
	{
		if (Str)
		{
			hDump_File = CreateFile(strTXTFile,
				GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			
			nLen = lstrlen(Str);
			Str[nLen] = '\0';

			WriteFile(hDump_File, Str, lstrlen(Str) + 1, &Bytes, NULL);

			CloseHandle(hDump_File);
		}

		// If MiniDumpWriteDump() of DbgHelp.dll available.
		if (MiniDumpWriteDump_)
		{
			MINIDUMP_EXCEPTION_INFORMATION	M;

			M.ThreadId = GetCurrentThreadId();
			M.ExceptionPointers = pException;
			M.ClientPointers = 0;

			hDump_File = CreateFile(strDMPFile,
				GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

			MiniDumpWriteDump_(GetCurrentProcess(), GetCurrentProcessId(), hDump_File,
				MiniDumpNormal, (pException) ? &M : NULL, NULL, NULL);

			CloseHandle(hDump_File);
		}
	} //if (File_Flag)

	delete Str;
} //Create_Dump

CString GetExePath()
{
	TCHAR szFilePath[MAX_PATH + 1];
 	GetCurrentDirectory( MAX_PATH, szFilePath ); 
	CString strPath = CString(szFilePath);
	return strPath;
}