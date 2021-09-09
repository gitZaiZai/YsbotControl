#include "StdAfx.h"
#include "UserData.h"

using namespace std;

UserData::UserData() throw ()
{
	sprintf( strUserName, "Administrator" );
	sprintf( strPassWord, "" );
	sprintf( strLanguage, "China" );

	nUserID = 0;
	nGrade  = 1;
	nState  = 0;
	administrator = false;

}

const UserData& UserData::operator= (const UserData& src) throw ()
{
	sprintf( strUserName, "%s", src.strUserName );
	sprintf( strPassWord, "%s", src.strPassWord );
	sprintf( strLanguage, "%s", src.strLanguage );

	nUserID = src.nUserID;
	nGrade  = src.nGrade;
	nState  = src.nState;
	administrator = src.administrator;

	return *this;
}
