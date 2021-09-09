#include "StdAfx.h"
#include "JournalData.h"

using namespace std;

JournalData::JournalData() throw ()
{
	sprintf( strUserName, "Administrator" );
	sprintf( strBehavior, "" );
	sprintf( strObject, "" );
	sprintf( strDesc, "" );

	nType = 0;
	nCode = 0;
	nState = 0;
	 
	timeRecord = sql::time::now();
}

const JournalData& JournalData::operator= (const JournalData& src) throw ()
{
	sprintf( strUserName, "%s", src.strUserName );
	sprintf( strBehavior, "%s", src.strBehavior );
	sprintf( strObject, "%s", src.strObject );
	sprintf( strDesc, "%s", src.strDesc );

	nType = src.nType;
	nCode  = src.nCode;
	nState  = src.nState;
	nState = src.nState;
	timeRecord = src.timeRecord;

	return *this;
}

void JournalData::set_user(const char* sv)
{
	if ( sv[0] != '\0' )
	{
		sprintf( strUserName, "%s", sv );
 	}
}

void JournalData::set_behavior(const char* sv)
{
	if ( sv[0] != '\0' )
	{
		sprintf( strBehavior, "%s", sv );
	}
}

void JournalData::set_object(const char* sv)
{
	if ( sv[0] != '\0' )
	{
		sprintf( strObject, "%s", sv );
	}
}

void JournalData::set_description( const char* sv )
{
	if ( sv[0] != '\0' )
	{
		sprintf( strDesc, "%s", sv );
	}
}