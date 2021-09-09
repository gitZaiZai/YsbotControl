//CSystemInformation.h

#pragma once
#include <string>

class CSystemInformation
{
public:
	CSystemInformation(void);
	~CSystemInformation(void);
	long GetActiveWindowsTheme(TCHAR* szThemeName);

private:
	// ��ȡMAC��ַ
	std::string& GetMAC();
	// ��ȡIP��ַ
	std::string& GetIP();
	// ��ȡ����ϵͳ
	std::string& GetOS();
	std::string& GetOSVersion();
	// �Ƿ�Ϊ����
	bool IsInternetBar();
	void GetVersionMark(CString& vmark);

public:
	std::string m_strIP;
	std::string m_strMAC;
	std::string m_strOS;
	std::string m_strVersion;
	bool m_bIsInternetBar;
};