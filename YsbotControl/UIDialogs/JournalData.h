#include "SqlDatabase.h" 

#ifndef _journal_data_h
#define _journal_data_h
 
#define MAX_DESCSTR_LEN   1024

enum JournalType {
	JTMESSAGE = 0,
	JTWARNING,
	JTERROR,
	JTILLEGAL
}; 

struct JournalData
{ 
	JournalData() throw ();
	JournalData (const JournalData& gi ) throw () { operator= (gi); }
	const JournalData& operator= (const JournalData&) throw ();

	int     nType;
	int     nCode;
	int     nState;
	char    strUserName[100];
	char    strBehavior[100];
	char    strObject[100];
	char    strDesc[MAX_DESCSTR_LEN];
	sql::time timeRecord;
 
	void    set_user(const char*);
	void    set_behavior(const char*);
	void    set_object(const char*);
	void    set_description(const char*);
};

#endif

