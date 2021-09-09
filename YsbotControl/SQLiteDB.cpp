#include "stdafx.h"
//#include "io.h"
#include "SQLiteDB.h"
#include "stringconvert.h"

#if _DEBUG
	#pragma comment(lib, "../Debug/easySQLite.lib")
#else
	#pragma comment(lib, "../Release/easySQLite.lib")
#endif
using namespace sql;
 
SQLiteDB* SQLiteDB::main_db=NULL;

SQLiteDB::~SQLiteDB()
{
	if(main_db == this)
		main_db = NULL;

	delete _userTable;
	delete _logTable;
	delete _dataTable;

}

SQLiteDB::SQLiteDB(CString filename)
{
	if (main_db == NULL) main_db = this;

	theTimer.update();
	//define table structure
	sql::Field userDB_Field[]=
	{
		Field("_ID",type_int,flag_primary_key),
		Field("userName",type_text,flag_not_null),
		Field("userPassword",type_int,flag_not_null),
		Field("level",type_int,flag_not_null),
		Field("creatTime",type_text,flag_not_null),
		Field(DEFINITION_END),
	};
	sql::Field logDB_Field[]=
	{
		Field("_ID",type_int,flag_primary_key),
		Field("state",type_bool,flag_not_null),
		Field("description",type_text,flag_not_null),
		Field("level",type_int,flag_not_null),
		Field("time",type_text,flag_not_null),
		Field("projectName",type_text,flag_not_null),
		Field(DEFINITION_END),
	};
	sql::Field dataDB_Field[]=
	{
		Field("_ID",type_int,flag_primary_key),
		Field("dataName",type_text,flag_not_null),
		Field("dataValue",type_text,flag_not_null),
		Field("remark",type_text,flag_not_null),
		Field("dataType",type_int,flag_not_null),
		Field(DEFINITION_END),
	};
	sql::FieldSet _userFieldSet(userDB_Field);
	sql::FieldSet _logFieldSet(logDB_Field);
	sql::FieldSet _dataFieldSet(dataDB_Field);

	CFileFind finder;
	BOOL existDB = finder.FindFile(filename);
	if(!existDB)
	{
		AfxMessageBox(_T("找不到系统数据库"));
		return;
	}
	else
	{
		if (finder.FindFile(filename+_T(".backup")))
			DeleteFile(filename+_T(".backup"));
		CopyFile(filename,filename+_T(".backup"),FALSE);
	
	}
	try
	{
		std::string str;
		str = Unicode2Ansi((std::wstring)filename);
		bool open_result = _db.open(str);
		_userTable = new Table(_db.getHandle(),"user",&_userFieldSet);
		_logTable = new Table(_db.getHandle(),"log",&_logFieldSet);
		_dataTable = new Table(_db.getHandle(),"data",&_dataFieldSet);
		if(!_userTable->exists()) 
		{

			AfxMessageBox(_T("找不到用户帐户数据库"));
		}
		if(!_logTable->exists())
		{
			AfxMessageBox(_T("找不到日志数据库"));
		}
		if(!_dataTable->exists())
		{
			AfxMessageBox(_T("找不到自定义数据数据库"));
		}
		RefreshLogRecords();
	}	
	catch (Exception e) {
		printf("ERROR: %s\r\n", e.msg().c_str());
	}

}

int SQLiteDB::RefreshLogRecords(unsigned int logType ,int rdCount)
{
	char chr[128];
	if(logType == 0)
		sprintf(chr,"select _ID,state,description,level,time,projectName from log ORDER BY _ID DESC LIMIT %d",rdCount);
	if (logType > 0)
		sprintf(chr,"select _ID,state,description,level,time,projectName from log WHERE level='%d' ORDER BY _ID DESC LIMIT %d",logType,rdCount);
	std::string str(chr);
	try
	{
		_logTable->query(chr);			
	}
	catch (Exception e) {
		printf("ERROR: %s\r\n", e.msg().c_str());
		return -1;
	}
	_LogVec.clear();
	int num = _logTable->recordCount();
	if (num > rdCount) num = rdCount;
	for(int i=0; i<num; i++)
	{
		LogInfo loginfo;
		CString str;
		str.Format(_T("%d"),i);
		loginfo.LogID = str;
		str = Ansi2Unicode(_logTable->getRecord(i)->getValue("state")->asString()).c_str();
		loginfo.LogState = str;
		str = Ansi2Unicode(_logTable->getRecord(i)->getValue("description")->asString()).c_str();
		loginfo.LogDescription = str;
		str = Ansi2Unicode(_logTable->getRecord(i)->getValue("level")->asString()).c_str();
		loginfo.LogLevel = str;
		str = Ansi2Unicode(_logTable->getRecord(i)->getValue("time")->asString()).c_str();
		loginfo.LogTime = str;
		str = Ansi2Unicode(_logTable->getRecord(i)->getValue("projectName")->asString()).c_str();
		loginfo.LogPrgName = str;
		_LogVec.push_back(loginfo);
	}
	return 0;
}

int SQLiteDB::InsertLogRecords(LogInfo loginfo,std::string *err)
{
	//sql::Record rd(_logTable->fields());

	CheckLogSize(500);

	std::string sval;
	sval =  "'";
	//sval =  sval + Unicode2Ansi((std::wstring)loginfo.LogID) + "','";
	sval = sval + Unicode2Ansi((std::wstring)loginfo.LogState) + "','";
	sval = sval + Unicode2Ansi((std::wstring)loginfo.LogDescription) + "','";
	sval = sval + Unicode2Ansi((std::wstring)loginfo.LogLevel) + "','";
	//sval = sval + Unicode2Ansi((std::wstring)loginfo.LogTime) + "','";
	sval = sval + Unicode2Ansi((std::wstring)loginfo.LogPrgName) + "'";
    sval = "insert into log (state,description,level,projectName) values(" + sval +")";
	try
	{
		_logTable->query(sval);
	}
	catch (Exception e)
	{
		if(err)	*err = e.msg().c_str();
		printf("ERROR: %s\r\n", e.msg().c_str());
		return -1;
	}

	return 0;
}

int SQLiteDB::InsertLogRecords(CString Ldescription,CString Lstate ,unsigned int Llevel,
	CString LprgName, std::string *err )
{
	LogInfo li;
	li.LogDescription = Ldescription;
	li.LogState = Lstate;
	CString lstr;
	lstr.Format(_T("%d"),Llevel);
	li.LogLevel = lstr;
	li.LogPrgName =LprgName;
	return InsertLogRecords(li,err);	
}

int SQLiteDB::CheckLogSize(int MaxSize )
{
	if(theTimer.elapsed_sec() >= 3600 && MaxSize > 1)
	{
		char chr[128];
		sprintf(chr,"select * from log ORDER BY _ID DESC LIMIT %d",MaxSize);
		std::string str(chr);
		try
		{
			_logTable->query(chr);			
		}
		catch (Exception e) {
			printf("ERROR: %s\r\n", e.msg().c_str());
			return -1;
		}

		int num = _logTable->recordCount();
		if (num < MaxSize) return 0;
		int deleteId = (int)_logTable->getRecord(num-1)->getValue("_ID")->asInteger();

		sprintf(chr,"delete from log where _ID < %d",deleteId);
		try
		{
			_logTable->query(chr);			
		}
		catch (Exception e) {
			printf("ERROR: %s\r\n", e.msg().c_str());
			return -1;
		}

		theTimer.update();
		return deleteId;
	}
	return 0;
}

int SQLiteDB::AddUser(CString userName,CString password,int level,std::string *err)
{
	//sql::Record rd(_userTable->fields());
	std::string sval;
	sval =  "'";
	sval = sval + Unicode2Ansi((std::wstring)userName) + "','";
	sval = sval + Unicode2Ansi((std::wstring)password) + "','";
	char chr[10];
	itoa(level,chr,10);
	std::string str = chr;
	sval = sval + str + "'";
	sval = "insert into user (userName,userPassword,level) values(" + sval +")";
	try 
	{
		_userTable->query(sval);
	}
	catch (Exception e)
	{
		if(err)	*err = e.msg().c_str();
		printf("ERROR: %s\r\n", e.msg().c_str());
		return -1;
	}
	return 0;
}

int SQLiteDB::CheckPassword(CString userName,CString password)
{
	//sql::Record rd(_userTable->fields());
	std::string sval;
	sval = "'" + Unicode2Ansi((std::wstring)userName) + "'";
	sval = "SELECT * FROM user WHERE userName = " + sval;
	try 
	{
		_userTable->query(sval);
	}
	catch (Exception e)
	{
		printf("ERROR: %s\r\n", e.msg().c_str());
		return -1;
	}
	if(_userTable->recordCount()<=0)
		return -1;
	sval = _userTable->getRecord(0)->getValue("userPassword")->asString();
	int ival =(int)_userTable->getRecord(0)->getValue("level")->asInteger();
	if(password == Ansi2Unicode(sval).c_str())
		return ival;
	else
		return -1;
}

int SQLiteDB::ModifyUser(CString userName,CString password ,int ulevel,std::string *err)
{
	std::string sval,spwd;
	if (password != _T(""))
	{
		spwd = Unicode2Ansi(password.GetBuffer()).c_str();
		sval = "'" + Unicode2Ansi((std::wstring)userName) + "'";
		sval = "Update user set userPassword = '" + spwd + "' WHERE userName = " + sval;
		try 
		{
			_userTable->query(sval);
		}
		catch (Exception e)
		{
			if(err)	*err = e.msg().c_str();
			printf("ERROR: %s\r\n", e.msg().c_str());
			return -1;
		}
	}
	if (ulevel > 0)
	{
		char chr[10];
		sprintf(chr,"%d",ulevel);
		std::string sul(chr);
		sval = "'" + Unicode2Ansi((std::wstring)userName) + "'";
		sval = "Update user set level = " + sul + " WHERE userName = " + sval;
		try 
		{
			_userTable->query(sval);
		}
		catch (Exception e)
		{
			if(err)	*err = e.msg().c_str();
			printf("ERROR: %s\r\n", e.msg().c_str());
			return -1;
		}
	}



	return 0;
}

int SQLiteDB::DeleteUser(CString userName,std::string *err)
{
	std::string sval;
	sval = "'" + Unicode2Ansi((std::wstring)userName) + "'";
	sval = "DELETE FROM user WHERE userName = " + sval ;
	try 
	{
		_userTable->query(sval);
	}
	catch (Exception e)
	{
		if(err)	*err = e.msg().c_str();
		printf("ERROR: %s\r\n", e.msg().c_str());
		return -1;
	}
	return 0;
}

std::vector<CString> SQLiteDB::GetUserList()
{
	std::vector<CString> userlist;
	std::string sval;
	sval = "SELECT * FROM user ";
	try 
	{
		_userTable->query(sval);
	}
	catch (Exception e)
	{
		printf("ERROR: %s\r\n", e.msg().c_str());
	}
	int num = _userTable->recordCount();
	for (int i=0;i<num;i++)
	{
		sval=_userTable->getRecord(i)->getValue("userName")->asString();
		userlist.push_back(Ansi2Unicode(sval).c_str());
	}
	return userlist;
}

std::vector<UserInfo> SQLiteDB::GetUserInfo()
{
	std::vector<UserInfo> userList;
	std::string sval;
	UserInfo uinfo;
	sval = "SELECT * FROM user ";
	try 
	{
		_userTable->query(sval);
	}
	catch (Exception e)
	{
		printf("ERROR: %s\r\n", e.msg().c_str());
	}
	int num = _userTable->recordCount();
	for (int i=0;i<num;i++)
	{
		uinfo.UserName=CString(Ansi2Unicode(_userTable->getRecord(i)->getValue("userName")->asString()).c_str());
		uinfo.UserLevel=(int)_userTable->getRecord(i)->getValue("level")->asInteger();
		uinfo.CreateTime=CString(Ansi2Unicode(_userTable->getRecord(i)->getValue("creatTime")->asString()).c_str());
		userList.push_back(uinfo);
	}
	return userList;
}

int SQLiteDB::InsertDataToDB(DataInfo dInfo,std::string *err)
{
	std::string sval;
	sval =  "'";
	sval = sval + Unicode2Ansi((std::wstring)dInfo.DataName) + "','";
	sval = sval + Unicode2Ansi((std::wstring)dInfo.DataValue) + "','";
	sval = sval + Unicode2Ansi((std::wstring)dInfo.DataType) + "','";
	sval = sval + Unicode2Ansi((std::wstring)dInfo.DataRemark) + "'";
	sval = "insert into data (DataName,DataValue,DataType,remark) values(" + sval +")";
	try 
	{
		_dataTable->query(sval);
	}
	catch (Exception e)
	{
		if(err)	*err = e.msg().c_str();
		printf("ERROR: %s\r\n", e.msg().c_str());
		return -1;
	}

	return 0;
}

int SQLiteDB::SetDataToDB(DataInfo dInfo,std::string *err)
{
	std::string sval,svaln,svalt,svalv,svalr;
	svalv =  "'" + Unicode2Ansi((std::wstring)dInfo.DataValue) + "'";
	svaln =  "'" + Unicode2Ansi((std::wstring)dInfo.DataName) + "'";
	svalt =  "'" + Unicode2Ansi((std::wstring)dInfo.DataType) + "'";
	svalr =  "'" + Unicode2Ansi((std::wstring)dInfo.DataRemark) + "'";
	sval = "UPDATE data SET DataValue=" + svalv + ",remark=" + svalr;
	sval =sval +  " Where DataName=" + svaln +" AND DataType=" + svalt;
	try 
	{
		_dataTable->query(sval);
	}
	catch (Exception e)
	{
		if(err)	*err = e.msg().c_str();
		printf("ERROR: %s\r\n", e.msg().c_str());
		return -1;
	}

	return 0;
}

int SQLiteDB::SetDataToDB(CString dataName,CString dataType,CString dataValue,CString dataRemark,std::string *err)
{
	std::string sval,svaln,svalt,svalv,svalr;
	svalv =  "'" + Unicode2Ansi((std::wstring)dataValue) + "'";
	svaln =  "'" + Unicode2Ansi((std::wstring)dataName) + "'";
	svalt =  "'" + Unicode2Ansi((std::wstring)dataType) + "'";
	svalr =  "'" + Unicode2Ansi((std::wstring)dataRemark) + "'";
	sval = "UPDATE data SET DataValue=" + svalv + ",remark=" + svalr;
	sval =sval +  " Where DataName=" + svaln +" AND DataType=" + svalt;
	try 
	{
		_dataTable->query(sval);
	}
	catch (Exception e)
	{
		if(err)	*err = e.msg().c_str();
		printf("ERROR: %s\r\n", e.msg().c_str());
		return -1;
	}

	return 0;
}

int SQLiteDB::GetDataFromDB(DataInfo *dInfo,std::string *err)
{
	std::string sval,svaln,svalt;
	svaln = "'" + Unicode2Ansi((std::wstring)dInfo->DataName) + "'";
	svalt =  "'" + Unicode2Ansi((std::wstring)dInfo->DataType) + "'";
	sval = "SELECT * FROM data WHERE dataName = " + svaln + " and dataType=" + svalt;
	try 
	{
		_dataTable->query(sval);
	}
	catch (Exception e)
	{
		if(err)	*err = e.msg().c_str();
		printf("ERROR: %s\r\n", e.msg().c_str());
		return -1;
	}
	if(_dataTable->recordCount()<=0)
		return -1;
	dInfo->DataValue = Ansi2Unicode(_dataTable->getRecord(0)->getValue("dataValue")->asString()).c_str();
	dInfo->DataRemark = Ansi2Unicode(_dataTable->getRecord(0)->getValue("remark")->asString()).c_str();
	return 0;
}

CString SQLiteDB::GetDataFromDB(CString dataName,CString dataType,std::string *err)
{
	std::string sval,sval2;
	sval = "'" + Unicode2Ansi((std::wstring)dataName) + "'";
	sval2 =  "'" + Unicode2Ansi((std::wstring)dataType) + "'";
	sval = "SELECT * FROM data WHERE dataName = " + sval + " and dataType=" + sval2;
	try 
	{
		_dataTable->query(sval);
	}
	catch (Exception e)
	{
		printf("ERROR: %s\r\n", e.msg().c_str());
	}
	if(_dataTable->recordCount()<=0)
		return _T("");
	sval = _dataTable->getRecord(0)->getValue("dataValue")->asString();

	return Ansi2Unicode(sval).c_str();
}

int SQLiteDB::DeleteDataFromDB(CString dataName,CString dataType,std::string *err)
{
	std::string sval,sval2;
	sval = "'" + Unicode2Ansi((std::wstring)dataName) + "'";
	sval2 =  "'" + Unicode2Ansi((std::wstring)dataType) + "'";
	sval = "DELETE FROM data WHERE dataName = " + sval + " and dataType=" + sval2;
	try 
	{
		_dataTable->query(sval);
	}
	catch (Exception e)
	{
		if(err)	*err = e.msg().c_str();
		printf("ERROR: %s\r\n", e.msg().c_str());
		return -1;
	}
	return 0;
}

std::vector<DataInfo> SQLiteDB::GetDataFromDB()
{
	char chr[128];
	unsigned int num = _logTable->totalRecordCount();
	std::string str = "select _ID,dataName,dataValue,dataType,remark from data";
	try
	{
		_dataTable->query(chr);			
	}
	catch (Exception e) {
		printf("ERROR: %s\r\n", e.msg().c_str());
	}
	_dataVec.clear();
	for(unsigned int i=0; i<num; i++)
	{
		DataInfo dInfo;
		CString str;
		dInfo.DataID = (int)_dataTable->getRecord(i)->getValue("_ID")->asInteger();
		dInfo.DataName = Ansi2Unicode(_dataTable->getRecord(i)->getValue("dataName")->asString()).c_str();
		dInfo.DataValue = Ansi2Unicode(_dataTable->getRecord(i)->getValue("dataValue")->asString()).c_str();
		dInfo.DataType = Ansi2Unicode(_dataTable->getRecord(i)->getValue("dataType")->asString()).c_str();
		dInfo.DataRemark = Ansi2Unicode(_dataTable->getRecord(i)->getValue("remark")->asString()).c_str();
		_dataVec.push_back(dInfo);
	}

	return _dataVec;
}

bool SQLiteDB::ifExistData(CString dataName,CString dataType)
{
	std::string sval,sval2;
	sval = "'" + Unicode2Ansi((std::wstring)dataName) + "'";
	sval2 =  "'" + Unicode2Ansi((std::wstring)dataType) + "'";
	sval = "SELECT count(*) FROM data WHERE dataName = " + sval + " and dataType=" + sval2;
	try 
	{
		if(_dataTable->query(sval))
			return true;
	}
	catch (Exception e)
	{
		printf("ERROR: %s\r\n", e.msg().c_str());
	}
	return false;
}