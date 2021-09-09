#include "stdafx.h"
#include "SystemInformation.h"

//SystemInformation.cpp

#include "SystemInformation.h"
#include <winsock2.h>
#include <Iphlpapi.h>
#pragma  comment(lib,"Iphlpapi.lib")
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")


using std::string;

void byte2Hex(unsigned char bData,unsigned char hex[])
{
	int high=bData/16,low =bData %16;
	hex[0] = (high <10)?('0'+high):('A'+high-10);
	hex[1] = (low <10)?('0'+low):('A'+low-10);
}

bool getLocalMac1(unsigned char *mac) //��ȡ����MACַ 
{
	ULONG ulSize=0;
	PIP_ADAPTER_INFO pInfo=NULL;
	int temp = GetAdaptersInfo(pInfo,&ulSize);//��һ�����ã���ȡ��������С
	pInfo=(PIP_ADAPTER_INFO)malloc(ulSize);
	temp = GetAdaptersInfo(pInfo,&ulSize);
	int iCount=0;
	while(pInfo)//����ÿһ������
	{
		//  pInfo->Address MACַ
		for(int i=0;i<(int)pInfo->AddressLength;i++)
		{
			byte2Hex(pInfo->Address[i],&mac[iCount]);
			iCount+=2;
			if(i<(int)pInfo->AddressLength-1)
			{
				mac[iCount++] = ':';
			}
			else
			{
				mac[iCount++] = '#';
			}
		}
		break;
		pInfo = pInfo->Next;
	}
	free(pInfo);

	if(iCount >0)
	{
		mac[--iCount]='\0';
		return true;
	}
	else
	{
		return false;
	}
}


//ͨ��GetAdaptersInfo������������Windows 2000�����ϰ汾��
bool getLocalMac2(unsigned char *mac)
{
	bool ret = false;
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	PIP_ADAPTER_INFO pAdapterInfo = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
	if(pAdapterInfo == NULL)
		return false;
	// Make an initial call to GetAdaptersInfo to get the necessary size into the ulOutBufLen variable
	if(GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) 
	{
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);
		if (pAdapterInfo == NULL) 
			return false;
	}
	if(GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR)
	{
		for(PIP_ADAPTER_INFO pAdapter = pAdapterInfo; pAdapter != NULL; pAdapter = pAdapter->Next)
		{
			// ȷ������̫��
			if(pAdapter->Type != MIB_IF_TYPE_ETHERNET)
				continue;
			// ȷ��MAC��ַ�ĳ���Ϊ 00-00-00-00-00-00
			if(pAdapter->AddressLength != 6)
				continue;
			char acMAC[20] = {0};
			sprintf(acMAC, "%02X:%02X:%02X:%02X:%02X:%02X",
				int (pAdapter->Address[0]),
				int (pAdapter->Address[1]),
				int (pAdapter->Address[2]),
				int (pAdapter->Address[3]),
				int (pAdapter->Address[4]),
				int (pAdapter->Address[5]));
			memcpy(mac,acMAC,20);
			ret = true;
			break;
		}
	}
	free(pAdapterInfo);
	return ret;
}


//ͨ��GetAdaptersAddresses������������Windows XP�����ϰ汾��
bool getLocalMac3(unsigned char *mac)
{
	bool ret = false;
	ULONG outBufLen = sizeof(IP_ADAPTER_ADDRESSES);
	PIP_ADAPTER_ADDRESSES pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
	if (pAddresses == NULL) 
		return false;
	// Make an initial call to GetAdaptersAddresses to get the necessary size into the ulOutBufLen variable
	if(GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &outBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		free(pAddresses);
		pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
		if (pAddresses == NULL) 
			return false;
	}

	if(GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &outBufLen) == NO_ERROR)
	{
		// If successful, output some information from the data we received
		for(PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses; pCurrAddresses != NULL; pCurrAddresses = pCurrAddresses->Next)
		{
			// ȷ��MAC��ַ�ĳ���Ϊ 00-00-00-00-00-00
			if(pCurrAddresses->PhysicalAddressLength != 6)
				continue;
			char acMAC[20] = {0};
			sprintf(acMAC, "%02X:%02X:%02X:%02X:%02X:%02X",
				int (pCurrAddresses->PhysicalAddress[0]),
				int (pCurrAddresses->PhysicalAddress[1]),
				int (pCurrAddresses->PhysicalAddress[2]),
				int (pCurrAddresses->PhysicalAddress[3]),
				int (pCurrAddresses->PhysicalAddress[4]),
				int (pCurrAddresses->PhysicalAddress[5]));
			memcpy(mac,acMAC,20);
			ret = true;
			break;
		}
	} 
	free(pAddresses);
	return ret;
}
 
bool GetLocalIP(char* ip)
{
	//1.��ʼ��wsa
	WSADATA wsaData;
	int ret=WSAStartup(MAKEWORD(2,2),&wsaData);
	if (ret!=0)
	{
		return false;
	}
	//2.��ȡ������
	char hostname[256];
	ret=gethostname(hostname,sizeof(hostname));
	if (ret==SOCKET_ERROR)
	{
		return false;
	}
	//3.��ȡ����ip
	HOSTENT* host=gethostbyname(hostname);
	if (host==NULL)
	{
		return false;
	}
	//4.ת��Ϊchar*����������
	strcpy(ip,inet_ntoa(*(in_addr*)*host->h_addr_list));
	return true;
}


bool IsHaveKey(HKEY hKey, char* lpSubKey)
{
	HKEY phkResult = 0;	
	if (ERROR_SUCCESS == RegOpenKeyExA(hKey, lpSubKey, 0, KEY_QUERY_VALUE, &phkResult))
	{
		RegCloseKey(phkResult);
		return true;
	}
	return false;
}

//��ȡ����ϵͳ������
string GetSystemName()
{
	SYSTEM_INFO info;        //��SYSTEM_INFO�ṹ�ж�64λAMD������ 
	GetSystemInfo(&info);    //����GetSystemInfo�������ṹ 
	OSVERSIONINFOEX os; 
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);   

	string osname = "unknown OperatingSystem.";

	if(GetVersionEx((OSVERSIONINFO *)&os))
	{ 
		//������ݰ汾��Ϣ�жϲ���ϵͳ���� 
		switch(os.dwMajorVersion)//�ж����汾��
		{
		case 4:
			switch(os.dwMinorVersion)//�жϴΰ汾�� 
			{ 
			case 0:
				if(os.dwPlatformId==VER_PLATFORM_WIN32_NT)
					osname = "Microsoft Windows NT 4.0"; //1996��7�·��� 
				else if(os.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS)
					osname = "Microsoft Windows 95";
				break;
			case 10:
				osname = "Microsoft Windows 98";
				break;
			case 90:
				osname = "Microsoft Windows Me";
				break;
			}
			break;

		case 5:
			switch(os.dwMinorVersion)	//�ٱȽ�dwMinorVersion��ֵ
			{ 
			case 0:
				osname = "Microsoft Windows 2000";//1999��12�·���
				break;

			case 1:
				osname = "Microsoft Windows XP";//2001��8�·���
				break;

			case 2:
				if(os.wProductType==VER_NT_WORKSTATION 
					&& info.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
				{
					osname = "Microsoft Windows XP Professional x64 Edition";
				}
				else if(GetSystemMetrics(SM_SERVERR2)==0)
					osname = "Microsoft Windows Server 2003";//2003��3�·��� 
				else if(GetSystemMetrics(SM_SERVERR2)!=0)
					osname = "Microsoft Windows Server 2003 R2";
				break;
			}
			break;

		case 6:
			switch(os.dwMinorVersion)
			{
			case 0:
				if(os.wProductType == VER_NT_WORKSTATION)
					osname = "Microsoft Windows Vista";
				else
					osname = "Microsoft Windows Server 2008";//�������汾 
				break;
			case 1:
				if(os.wProductType == VER_NT_WORKSTATION)
					osname = "Microsoft Windows 7";
				else
					osname = "Microsoft Windows Server 2008 R2";
				break;
			case 2:
				if(os.wProductType == VER_NT_WORKSTATION)
					osname = "Microsoft Windows 8";
				else
					osname = "Microsoft Windows Server 2012";
				break;
			case 3:
				if(os.wProductType == VER_NT_WORKSTATION)
					osname = "Microsoft Windows 8.1";
				else
					osname = "Microsoft Windows Server 2012 R2";
				break;
			}
			break;

		case 10:
			switch(os.dwMinorVersion)
			{
			case 0:
				if(os.wProductType == VER_NT_WORKSTATION)
					osname = "Microsoft Windows 10";
				else
					osname = "Microsoft Windows Server 2016 Technical Preview";//�������汾 
				break;
			}
			break;
		}
	}//if(GetVersionEx((OSVERSIONINFO *)&os))
	return osname;
} 
 
CSystemInformation::CSystemInformation(void)
	:m_bIsInternetBar(false)
{
	m_strIP = GetIP();
	m_strMAC = GetMAC();
	m_strOS = GetOS();
	CString vermark;
	GetVersionMark(vermark);
	m_strVersion = CStringA(vermark);
	//m_bIsInternetBar = IsInternetBar();
}

CSystemInformation::~CSystemInformation(void)
{
}


// ��ȡMAC��ַ
string& CSystemInformation::GetMAC()
{
	unsigned char szMac[20] = {0};
	//Ϊ��ȷ����ȡ��MAC��ַ��ͨ�����ַ�ʽ����ȡ
	if(!getLocalMac3(szMac))
	{
		if(!getLocalMac2(szMac))
		{
			getLocalMac1(szMac);
		}
	}
	m_strMAC.empty();
	m_strMAC.append((const char*)szMac,sizeof(szMac));

	return m_strMAC;
}

// ��ȡIP��ַ
string& CSystemInformation::GetIP()
{
	char ip[30] = {'\0'};
	if (GetLocalIP(ip))
	{
		m_strIP.empty();
		m_strIP.append(ip,sizeof(ip));
	}

	return m_strIP;
}

// ��ȡ����ϵͳ
string& CSystemInformation::GetOS()
{
	m_strOS = GetSystemName();

	return m_strOS;
}

// �Ƿ�Ϊ����
bool CSystemInformation::IsInternetBar()
{
	char* NetRoomKey[] =
	{
		"SOFTWARE\\Hintsoft\\PubwinClient",
		"SOFTWARE\\Hintsoft\\pubwin",
		"SOFTWARE\\Sicent\\wx2004Clt",
		"SOFTWARE\\MpSoft\\smenu",
		"SOFTWARE\\Microsoft\\jingzu",
		"SOFTWARE\\iCafe8",
		"SOFTWARE\\Goyoo\\i8desk",
		"SOFTWARE\\Hintsoft1\\XunShanPro",
		"SOFTWARE\\SyncExpertNetBar",
		"SOFTWARE\\EYOOCLIENTSTATUS",
		"SOFTWARE\\Richtech",
		"SOFTWARE\\\u676d\u5dde\u987a\u7f51\u4fe1\u606f\u6280\u672f\u6709\u9650\u516c\u53f8\\\u7f51\u7ef4\u5927\u5e08\u874c\u86aa\u5a31\u4e50\u5e73\u53f0",
		"SOFTWARE\\iCafe8",
		"SOFTWARE\\Sicent\\wx2004Clt",
		"SOFTWARE\\Sicent\\WxAdv",
		"SOFTWARE\\Grabsun\\Netsense",
	};

	for (int i = 0; i < sizeof(NetRoomKey) / sizeof(char*); i++)
	{
		if (IsHaveKey(HKEY_LOCAL_MACHINE, NetRoomKey[i]))
		{
			m_bIsInternetBar = true;
			break;
		}
	}

	return m_bIsInternetBar;
}
  

//��ȡ����ϵͳ�İ汾����
void CSystemInformation::GetVersionMark(CString& vmark)
{
	OSVERSIONINFOEX os; 
	os.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX); 
	vmark = _T("");

	if(GetVersionEx((OSVERSIONINFO*)&os))
	{ 
		switch(os.dwMajorVersion)
		{                //���жϲ���ϵͳ�汾 
		case 5: 
			switch(os.dwMinorVersion)
			{ 
			case 0:                  //Windows 2000 
				if(os.wSuiteMask==VER_SUITE_ENTERPRISE) 
					vmark=_T("Advanced Server"); 
				break; 
			case 1:                  //Windows XP 
				if(os.wSuiteMask==VER_SUITE_EMBEDDEDNT) 
					vmark=_T("Embedded"); 
				else if(os.wSuiteMask==VER_SUITE_PERSONAL) 
					vmark=_T("Home Edition"); 
				else 
					vmark=_T("Professional"); 
				break; 
			case 2: 
				if(GetSystemMetrics(SM_SERVERR2)==0 
					&&os.wSuiteMask==VER_SUITE_BLADE)  //Windows Server 2003 
					vmark=_T("Web Edition");
				else if(GetSystemMetrics(SM_SERVERR2)==0 
					&& os.wSuiteMask==VER_SUITE_COMPUTE_SERVER)
					vmark=_T("Compute Cluster Edition");
				else if(GetSystemMetrics(SM_SERVERR2)==0
					&& os.wSuiteMask==VER_SUITE_STORAGE_SERVER)
					vmark=_T("Storage Server");
				else if(GetSystemMetrics(SM_SERVERR2)==0 
					&& os.wSuiteMask==VER_SUITE_DATACENTER)
					vmark=_T("Datacenter Edition");
				else if(GetSystemMetrics(SM_SERVERR2)==0 
					&& os.wSuiteMask==VER_SUITE_ENTERPRISE)
					vmark=_T("Enterprise Edition");
				else if(GetSystemMetrics(SM_SERVERR2)!=0
					&& os.wSuiteMask==VER_SUITE_STORAGE_SERVER)
					vmark=_T("Storage Server");
				break;
			}
			break;

		case 6:
			switch(os.dwMinorVersion)
			{
			case 0:
				if(os.wProductType!=VER_NT_WORKSTATION 
					&& os.wSuiteMask==VER_SUITE_DATACENTER)
					vmark=_T("Datacenter Server");
				else if(os.wProductType!=VER_NT_WORKSTATION 
					&& os.wSuiteMask==VER_SUITE_ENTERPRISE)
					vmark=_T("Enterprise");
				else if(os.wProductType==VER_NT_WORKSTATION 
					&& os.wSuiteMask==VER_SUITE_PERSONAL)  //Windows Vista
					vmark =_T("Home");
				break;
			}
			break;
		}
	}
}
 
long CSystemInformation::GetActiveWindowsTheme(TCHAR* szThemeName)
{
 	HKEY hKey;
	DWORD size,type; 
	long lRetCode; 
 	
	// XP
	CString strSubKey  = _T("Software\\Microsoft\\Plus!\\Themes\\Current");
	CString strValName = _T("");
	if ( m_strOS.compare("Microsoft Windows 7") == 0 )
	{
		strSubKey = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Themes");
		strValName = _T("CurrentTheme");
	}
 
	lRetCode = RegOpenKeyExW ( HKEY_CURRENT_USER, strSubKey, 0,KEY_READ,&hKey);  
	if (lRetCode == ERROR_SUCCESS)
	{ 
		size = _MAX_PATH;
		type = REG_SZ;
		lRetCode = ::RegQueryValueExW(hKey,strValName,NULL,&type,	(unsigned char *)szThemeName,&size);
		
		RegCloseKey(hKey);  
	}

	return (lRetCode);
}