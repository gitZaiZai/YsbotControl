// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef _user_data_h
#define _user_data_h

#define MAX_IDSTR_LEN     200
#define MAX_DESCSTR_LEN   1024

struct UserData
{ 
 	UserData() throw ();
	UserData (const UserData& gi ) throw () { operator= (gi); }
	const UserData& operator= (const UserData&) throw ();

	int     nUserID;  
 	char    strUserName[MAX_IDSTR_LEN];
	char    strPassWord[MAX_IDSTR_LEN];
	char    strLanguage[MAX_IDSTR_LEN];
	int     nGrade;
	int     nState;
 	bool    administrator;
	 
};
  

#endif

