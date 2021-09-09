#include "stdafx.h"
 
#include "SqlDatabase.h"
#include "../stringconvert.h"


#if _DEBUG
#pragma comment(lib, "../Debug/easySQLite.lib")
#else
#pragma comment(lib, "../Release/easySQLite.lib")
#endif

#define  MAX_JOURNAL_FILE_SIZE ( 1*1024*1024)   // 1M 

using namespace sql;
using namespace std;

SqlDatabase* SqlDatabase::main_sql_database = NULL;

SqlDatabase&  SqlDatabase::get_main_sql_database () 
{ 
	return * SqlDatabase::main_sql_database; 
}

bool  SqlDatabase::is_main_sql_database_available ()  
{
	return main_sql_database != NULL;
}

SqlDatabase::~SqlDatabase()
{
	m_systemDatabase.close();
	m_logDatabase.close();

	if(main_sql_database == this)
		main_sql_database = NULL;

	for (unsigned int i=0; i < fields_list.size(); i++)
	{
		delete fields_list[i];
	}
	fields_list.clear();
	 
	for (unsigned int i=0; i < table_list.size(); i++)
	{
		delete table_list[i];
	}
	table_list.clear();
}

SqlDatabase::SqlDatabase()
{
	if (main_sql_database == NULL) 
		main_sql_database = this;

	theTimer.update();
 
	check_journal_file_size(SQL_LOG_FILE_NAME);

	m_systemDatabase.open("YsbotSystem.db");
	m_logDatabase.open(SQL_LOG_FILE_NAME);
 
	Table* tbl = NULL;
	FieldSet* fset = NULL;

	for ( int i = 0; i < TBLNUM; ++i )
	{
		fset = new FieldSet( definition( (TableType)(i) ) );
		if (  (TableType)(i) == TBLLogger )
		{
			tbl = new Table(m_logDatabase.getHandle(),table_names[i],fset);
 		}
		else
			tbl = new Table(m_systemDatabase.getHandle(),table_names[i],fset);

		if ( !tbl->exists() )
		{
			tbl->create();
		}
		tbl->open();

		fields_list.push_back(fset);
		table_list.push_back(tbl);
	}
 
}

bool SqlDatabase::check_journal_file_size(const char* fname)
{
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD fileSize = 0;
	CString fileName = Ansi2Unicode(fname).c_str();
	hFind = FindFirstFile( fileName ,&fileInfo);
	if( hFind != INVALID_HANDLE_VALUE )
		fileSize = fileInfo.nFileSizeLow;
	FindClose( hFind );
    
	bool sizeOver = false;

	if ( fileSize > MAX_JOURNAL_FILE_SIZE ) // limit 1M size
	{
		BOOL ret = PathIsDirectory(Ansi2Unicode(SQL_LOG_DIR).c_str());
		if ( !ret )
		{
			CreateDirectory(Ansi2Unicode(SQL_LOG_DIR).c_str(), nullptr );
		}

		char szCurrentDate[32] = {'\0'};     
 		sql::time timenow = sql::time::now();     
		sprintf(szCurrentDate, "%d", timenow.asInteger() );   
        
		CString newfileName = fileName;
		newfileName = CString(SQL_LOG_DIR) + CString(_T("\\")) + CString(Ansi2Unicode(szCurrentDate).c_str())+newfileName;
		ret = CopyFile( fileName, newfileName, FALSE );

		ret = DeleteFile( fileName );

		sizeOver = true;
	}

	return sizeOver;
}

Table* SqlDatabase::getTable( TableType tbTyp )
{
	return table_list[(int) tbTyp];
}

FieldSet* SqlDatabase::getFieldSet(TableType tb)
{
	return fields_list[(int) tb];
}

//////////////////////////////////////////////////////////////////////////
int SqlDatabase::insert_record( TableType tbType, sql::Record* rcord, bool unique )
{
	Record rnew(*rcord);
	if ( unique && ( exists_record( tbType, rcord ) != -1 ) )
	{
		return -1; //
	}

	bool ret = table_list[tbType]->addRecord(&rnew);
	return ret ? 0 : -2;
 	
}

int SqlDatabase::delete_single_record( TableType tbType, sql::Record* rcord )
{
	Record rnew(*rcord);

	if ( exists_record( tbType, rcord ) != -1 )
	{
		sql::string strMainValue = rnew.getValue( fields_list[tbType]->getByIndex(1)->getName() )->asString();
		char chCondition[200] = {'\0'};
		sprintf(chCondition,"%s = '%s'",fields_list[tbType]->getByIndex(1)->getName().c_str(), strMainValue.c_str()); // " filed = 'XXX' "
		bool ret = table_list[tbType]->deleteRecords(chCondition);
		return ret ? 0 : -1;
 	}

	return 0;
}

int SqlDatabase::modify_record( TableType tbType, sql::Record* rcord )
{
	Record rnew(*rcord);
	if ( exists_record( tbType, rcord ) != -1 )
	{
 		bool ret = table_list[tbType]->updateRecord(&rnew);
		return ret ? 0 : -1;
	}
	return 0;
}

int SqlDatabase::exists_record( TableType tbType, sql::Record* rcord )
{
	table_list[tbType]->open();

	sql::string strMainValue = rcord->getValue(fields_list[tbType]->getByIndex(1)->getName())->asString();
	sql::string strHaveValue = "";
	int nKeyPrimary = -1;
	for (int index = 0; index < table_list[tbType]->recordCount(); index++)
	{
		if ( Record* record = table_list[tbType]->getRecord(index) )
		{
			strHaveValue = record->getValue(fields_list[tbType]->getByIndex(1)->getName())->asString();
			if ( strcmp( strMainValue.c_str(), strHaveValue.c_str() ) == 0 )
			{
				nKeyPrimary = (int)record->getValue(fields_list[tbType]->getByIndex(0)->getName())->asInteger();
				return nKeyPrimary;
			}
		}
	}
	return -1;
}

//
int SqlDatabase::check_equal( TableType tbType, sql::Record* rcord )
{
	int nKeyPrimary = exists_record( tbType, rcord );

	if ( nKeyPrimary != -1 )
	{
		Record* sqlRecord = table_list[tbType]->getRecordByKeyId(nKeyPrimary);
		sql::string inputValue = rcord->getValue(fields_list[tbType]->getByIndex(2)->getName())->asString();
 		sql::string saveValue  = sqlRecord->getValue(fields_list[tbType]->getByIndex(2)->getName())->asString();
		if ( strcmp( inputValue.c_str(), saveValue.c_str() ) == 0 )
 			return nKeyPrimary;
 		else
			return -2; 

	}
	return -1;
}

int SqlDatabase::find_primary_key(TableType tb, sql::Record* rcord)
{
	int index = exists_record( tb, rcord );
	return index;
}


//////////////////////////////////////////////////////////////////////////
bool seval ( int&d, Record* record )
{
	sql::integer keyval = record->getValue( SQL.getFieldSet(TBLValInt)->getByIndex(2)->getName())->asInteger();
    d = (int) keyval;
	return true;
}

bool seval ( std::string&d, Record* record )
{
	sql::string keyval = record->getValue( SQL.getFieldSet(TBLValText)->getByIndex(2)->getName())->asString();
	d = (std::string) keyval;
	return true;
}

bool seval ( double&d, Record* record )
{
	d = record->getValue( SQL.getFieldSet(TBLValReal)->getByIndex(2)->getName())->asDouble();
 	return true;
}

bool seval ( bool&d, Record* record )
{
	bool keyval = record->getValue( SQL.getFieldSet(TBLValBool)->getByIndex(2)->getName())->asBool();
	d = keyval;
	return true;
}

bool seval ( sql::time&d, Record* record )
{
	sql::time keyval = record->getValue( SQL.getFieldSet(TBLValTime)->getByIndex(2)->getName())->asBool();
	d = keyval;
	return true;
}

bool seval( CartPos&d, Record* record )
{
	d.pos.x = record->getValue( SQL.getFieldSet(TBLCartPose)->getByIndex(2)->getName())->asDouble();
 	d.pos.y = record->getValue( SQL.getFieldSet(TBLCartPose)->getByIndex(3)->getName())->asDouble();
	d.pos.z = record->getValue( SQL.getFieldSet(TBLCartPose)->getByIndex(4)->getName())->asDouble();
	d.ang.x = record->getValue( SQL.getFieldSet(TBLCartPose)->getByIndex(5)->getName())->asDouble();
	d.ang.y = record->getValue( SQL.getFieldSet(TBLCartPose)->getByIndex(6)->getName())->asDouble();
	d.ang.z = record->getValue( SQL.getFieldSet(TBLCartPose)->getByIndex(7)->getName())->asDouble();
	d.fig = (int)record->getValue( SQL.getFieldSet(TBLCartPose)->getByIndex(8)->getName())->asInteger();
	return true;
}

bool seval( AxisPos&d, Record* record )
{
	d.q1 = record->getValue( SQL.getFieldSet(TBLJointPose)->getByIndex(2)->getName())->asDouble();
	d.q2 = record->getValue( SQL.getFieldSet(TBLJointPose)->getByIndex(3)->getName())->asDouble();
	d.q3 = record->getValue( SQL.getFieldSet(TBLJointPose)->getByIndex(4)->getName())->asDouble();
	d.q4 = record->getValue( SQL.getFieldSet(TBLJointPose)->getByIndex(5)->getName())->asDouble();
	d.q5 = record->getValue( SQL.getFieldSet(TBLJointPose)->getByIndex(6)->getName())->asDouble();
	d.q6 = record->getValue( SQL.getFieldSet(TBLJointPose)->getByIndex(7)->getName())->asDouble();
 
	return true;
}

template<class T>
int get_intern( Record* record, T& dest )
{
    int nRet = -1;
	Record* pcord = record;
	if ( pcord != NULL )
 		nRet = seval( dest, pcord );
 	return nRet;
}
 
int SqlDatabase::get( const char* key, int& val )  
{
	sql::Record* rcord = find_first(TBLValInt,key);
    return get_intern(rcord, val);	 

}

int SqlDatabase::get( const char* key, std::string& val )  
{
	sql::Record* rcord = find_first(TBLValText,key);
	return get_intern(rcord, val);	 
}

int SqlDatabase::get( const char* key, double& val )  
{
	sql::Record* rcord = find_first(TBLValReal,key);
	return get_intern(rcord, val);	 
}

int SqlDatabase::get( const char* key, bool& val )  
{
	sql::Record* rcord = find_first(TBLValBool,key);
	return get_intern(rcord, val);	 
}
  
int SqlDatabase::get( const char* key, sql::time& val )  
{
	sql::Record* rcord = find_first(TBLValTime,key);
	return get_intern(rcord, val);	 
}
 
int SqlDatabase::get( const char* key, CartPos& val )  
{
	sql::Record* rcord = find_first(TBLCartPose,key);
	return get_intern(rcord, val);	 
}

int SqlDatabase::get( const char* key, AxisPos& val )  
{
	sql::Record* rcord = find_first(TBLJointPose,key);
	return get_intern(rcord, val);	 
}

sql::Record* SqlDatabase::find_first( const TableType tbType, const char* key   )
{
	Record* valRecord = NULL;
	sql::Record strValRecord( fields_list[tbType] );
	strValRecord.setString(1, key );
	int nPrimaryKey = find_primary_key(tbType,&strValRecord);
	 
	if ( nPrimaryKey < 0 )
	{
		;
 	}
	else
	{
		valRecord = table_list[tbType]->getRecordByKeyId(nPrimaryKey);
	}

	return valRecord;
}

//////////////////////////////////////////////////////////////////////////

void insertval ( int dest, Record* record )
{
	record->setInteger( 2, dest );
}

void insertval ( const char* dest, Record* record )
{
	record->setString( 2, dest );
}

void insertval ( double dest, Record* record )
{
	record->setDouble( 2, dest );
}

void insertval ( bool dest, Record* record )
{
	record->setBool( 2, dest );
}

void insertval ( sql::time& dest, Record* record )
{
	record->setTime( 2, dest );
}

void insertval( CartPos& dest, Record* record )
{
	record->setDouble( 2, dest.pos.x );
	record->setDouble( 3, dest.pos.y );
	record->setDouble( 4, dest.pos.z );
	record->setDouble( 5, dest.ang.x );
	record->setDouble( 6, dest.ang.y );
	record->setDouble( 7, dest.ang.z );
	record->setInteger( 8, dest.fig );
	record->setDouble( 9, 0 );
	record->setDouble( 10, 0 );
	record->setString(11, "CartValueDesc" );
	record->setInteger(12, 1 );
   
}

void insertval( AxisPos& dest, Record* record )
{
	record->setDouble( 2, dest.q1 );
	record->setDouble( 3, dest.q2 );
	record->setDouble( 4, dest.q3 );
	record->setDouble( 5, dest.q4 );
	record->setDouble( 6, dest.q5 );
	record->setDouble( 7, dest.q6 );
	record->setDouble( 8, 0 );
	record->setDouble( 9, 0 );

	record->setString(10, "AxisValueDesc" );
	record->setInteger(11, 1 );
 
}

template<class T>
int insert_intern( const TableType tbType, const char* key, T dest )
{
	sql::Record strValRecord( SQL.getFieldSet(tbType) );
	strValRecord.setString(1, key );
	insertval(dest,&strValRecord);
	if ( tbType >= TBLCartPose && tbType <= TBLJointPose )
	{
		;
 	}
	else
	{
		strValRecord.setString(3, "valueDesc" );
		strValRecord.setInteger(4, 1 );
 	}
	return SQL.insert_record( tbType, &strValRecord );
}
 
void SqlDatabase::set(const char* key, int val)
{
	sql::Record* rcord = find_first( TBLValInt, key );
	if ( rcord )
	{
 		insertval( val, rcord );
		modify_record(TBLValInt,rcord);
	}
	else
	{
	    insert_intern( TBLValInt, key, val );
	}
 	 
}

void SqlDatabase::set(const char* key,  const char* val)
{
	sql::Record* rcord = find_first( TBLValText, key );
	if ( rcord )
	{
		insertval( val, rcord );
		modify_record(TBLValText,rcord);
	}
	else
	{
		insert_intern( TBLValText, key, val );
	}

}

void SqlDatabase::set(const char* key, double val)
{
	sql::Record* rcord = find_first( TBLValReal, key );
	if ( rcord )
	{
		insertval( val, rcord );
		modify_record(TBLValReal,rcord);
	}
	else
	{
		insert_intern( TBLValReal, key, val );
	}

}

void SqlDatabase::set(const char* key, bool val)
{
	sql::Record* rcord = find_first( TBLValBool, key );
	if ( rcord )
	{
		insertval( val, rcord );
		modify_record(TBLValBool,rcord);
	}
	else
	{
		insert_intern( TBLValBool, key, val );
	}

}

void SqlDatabase::set(const char* key, sql::time& val)
{
	sql::Record* rcord = find_first( TBLValTime, key );
	if ( rcord )
	{
		insertval( val, rcord );
		modify_record(TBLValTime,rcord);
	}
	else
	{
		insert_intern( TBLValTime, key, val );
	}

}
 
void SqlDatabase::set(const char* key, CartPos& val)
{
	sql::Record* rcord = find_first( TBLCartPose, key );
	if ( rcord )
	{
		insertval( val, rcord );
		modify_record(TBLCartPose,rcord);
	}
	else
	{
		insert_intern( TBLCartPose, key, val );
	}

}

void SqlDatabase::set(const char* key, AxisPos& val)
{
	sql::Record* rcord = find_first( TBLJointPose, key );
	if ( rcord )
	{
		insertval( val, rcord );
		modify_record(TBLJointPose,rcord);
	}
	else
	{
		insert_intern( TBLJointPose, key, val );
	}

}

int SqlDatabase::insert_journal( JournalData& jour )
{
 	sql::Record jourRecord( getFieldSet(TBLLogger) );
	jourRecord.setInteger( 1, jour.nType );
	jourRecord.setInteger( 2, jour.nCode );
	jourRecord.setInteger( 3, jour.nState );

	jourRecord.setString( 4, jour.strUserName );
	jourRecord.setString( 5, jour.strBehavior );
	jourRecord.setString( 6, jour.strObject );
	jourRecord.setString( 7, jour.strDesc );
	jourRecord.setTime(   8, jour.timeRecord );
	
	int ret = insert_record(TBLLogger,&jourRecord,false);
 	return ret;
	 
}