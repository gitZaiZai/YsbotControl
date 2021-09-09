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
	// 获取MAC地址
	std::string& GetMAC();
	// 获取IP地址
	std::string& GetIP();
	// 获取操作系统
	std::string& GetOS();
	std::string& GetOSVersion();
	// 是否为网吧
	bool IsInternetBar();
	void GetVersionMark(CString& vmark);

public:
	std::string m_strIP;
	std::string m_strMAC;
	std::string m_strOS;
	std::string m_strVersion;
	bool m_bIsInternetBar;
};