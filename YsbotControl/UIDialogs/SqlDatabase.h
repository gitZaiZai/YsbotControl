#ifndef _sql_database_h
#define _sql_database_h

#include "SqlTableField.h"
#include "../Time.h"
#include "../CartPos.h"
#include "../AxisPos.h"
#include "JournalData.h"

#define  SQL_LOG_FILE_NAME    ("YsbotLog.db")
#define  SQL_LOG_DIR          ("YSLogFile")	//ÈÕÖ¾Ä¿Â¼

class SqlDatabase 
{
public:
 	SqlDatabase();
	~SqlDatabase();
	static SqlDatabase& get_main_sql_database();
	static bool is_main_sql_database_available();
	//database
	sql::Table* getTable(TableType tbTyp);
    sql::FieldSet* getFieldSet(TableType tbTyp);
 
	int insert_record( TableType, sql::Record*, bool unique = true );
	int delete_single_record( TableType, sql::Record* );
	int modify_record( TableType, sql::Record* );
	int exists_record( TableType, sql::Record* );
	int find_primary_key(TableType, sql::Record* );

	// 0 : OK, -1: username error, -2 : password incorrect.
    int check_equal( TableType, sql::Record* );

	int get (const char*, int&);                   
 	int get (const char*, std::string&);           
	int get (const char*, double&);                 
 	int get (const char*, bool&);                  
 	int get (const char*, sql::time&);                  
    int get (const char*, CartPos&);
    int get (const char*, AxisPos&);

	void set (const char*, int);                   
 	void set (const char*, const char*);    
	void set (const char*, double);                 
 	void set (const char*, bool);                  
	void set (const char*, sql::time& );           
 	void set (const char*, CartPos&);                  
	void set (const char*, AxisPos& );           
	void insert_value(const TableType tbType, const char* key);

	sql::Record* find_first( const TableType, const char* );

	int insert_journal( JournalData& );
	bool check_journal_file_size ( const char* );

private:
	static SqlDatabase* main_sql_database;

	Ysbot::Time theTimer;  
 	sql::Database m_systemDatabase;
	sql::Database m_logDatabase;

 	std::vector<sql::Table*> table_list;
    std::vector<sql::FieldSet*> fields_list;
  
};

#define SQL SqlDatabase::get_main_sql_database()

#endif