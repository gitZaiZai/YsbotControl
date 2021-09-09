#pragma once

#include "../easySQLite/easySQLite/SqlCommon.h"
#include "../easySQLite/easySQLite/SqlField.h"
#include "../easySQLite/easySQLite/SqlDatabase.h"
#include "../easySQLite/easySQLite/SqlTable.h"

#include "Time.h"
//#include <vector>

#define		LOG_ALARM	_T("1")
#define		LOG_ERR		_T("2")
#define		LOG_EXECUTE _T("3")
#define		LOG_OPERATE _T("4")
#define		LOG_SYSTEM  _T("5")

#define		LOG_ON		_T("1")
#define		LOG_OFF		_T("0")



//enum Log_Type
//{
//	LOG_ALL = 0,
//	LOG_ALARM = 1,
//	LOG_ERR = 2,
//	LOG_EXECUTE = 3,
//	LOG_OPERATE = 4,
//	LOG_SYSTEM = 5,
//};

struct LogInfo
{
	CString LogID;
	CString LogState;
	CString LogDescription;
	CString LogLevel;
	CString LogPrgName;
	CString LogTime;
};

struct DataInfo
{
	int DataID;
	CString DataName;
	CString DataValue;
	CString DataType;
	CString DataRemark;
};

struct UserInfo
{
	int UserID;
	CString UserName;
	CString UserPassword;
	int UserLevel;
	CString CreateTime;
};

class SQLiteDB 
{
public:
	SQLiteDB(CString filename);
	SQLiteDB();
	~SQLiteDB();
	static SQLiteDB& getSQLiteDB(){return *main_db;};
	bool is_main_database_available(){return main_db != NULL;};
//database
	sql::Table* getUserTable(){return _userTable;};
	sql::Table* getLogTable(){return _logTable;};
	sql::Table* getDataTable(){return _dataTable;};

//log data operation
	//refresh log records change
	int RefreshLogRecords(unsigned int logType = 0,int rdCount = 500);
	//insert a log record, return 0 if success
	int InsertLogRecords(LogInfo loginfo,std::string *err = NULL);
	int InsertLogRecords(CString Ldescription,CString Lstate ,unsigned int Llevel,
		CString LprgName = _T(""), std::string *err = NULL);
	//return the last records after use 'LoadRecordsLog()'
	std::vector<LogInfo> GetLogRecords(){return _LogVec;};
	int CheckLogSize(int MaxSize = 500 );

//user data operation
	//add a user, return 0 if success 
	int AddUser(CString userName,CString password,int level,std::string *err = NULL);
	//delete a user by user name, return 0 if success
	int DeleteUser(CString userName,std::string *err = NULL);
	//Change Password or user Level
	int ModifyUser(CString userName,CString password = _T(""),
				int ulevel = -1,std::string *err = NULL);
	//if the password match with username return the level of the user
	int CheckPassword(CString userName,CString password);
	std::vector<CString>GetUserList();
	std::vector<UserInfo>GetUserInfo();

//other data operation
	int InsertDataToDB(DataInfo dInfo,std::string *err = NULL);
	int SetDataToDB(CString dataName,CString dataType,CString dataValue,CString dataRemark=_T(""),std::string *err = NULL);
	int SetDataToDB(DataInfo dInfo,std::string *err = NULL);
	int GetDataFromDB(DataInfo *dInfo,std::string *err = NULL);
	CString GetDataFromDB(CString dataName,CString dataType,std::string *err = NULL);
	int DeleteDataFromDB(CString dataName,CString dataType,std::string *err = NULL);
	bool ifExistData(CString dataName,CString dataType);
	std::vector<DataInfo> GetDataFromDB();

private:
	static SQLiteDB* main_db;
	Ysbot::Time theTimer; //定时整理日志大小
	sql::Database _db;
	sql::Table* _userTable;
	sql::Table* _logTable;
	sql::Table* _dataTable;
	std::vector<LogInfo> _LogVec;
	std::vector<DataInfo> _dataVec;
	
};

#define MDB SQLiteDB::getSQLiteDB()
