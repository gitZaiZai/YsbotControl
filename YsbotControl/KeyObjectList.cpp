#include "stdafx.h"
#include "KeyObjectList.h"
#include <iomanip>

KeyObjectList::KeyObjectList(unsigned int len) : keylist(len) 
{
	for (unsigned int i = 0; i < len; ++i)
 		keylist[i] = 0;
}

KeyObjectList::KeyObjectList (const  KeyObjectList& src) throw (std::bad_alloc) : 
timestamp (src.timestamp), keylist (src.keylist) 
{
	;
}

KeyObjectList::KeyObjectList( unsigned char* charkey,  unsigned int len ) throw (std::bad_alloc)
{
	keylist.resize(len,0);
	if ( charkey )
	{
		for ( unsigned int i = 0; i < len; ++i )
 			keylist[i] = charkey[i];
 	}
}

KeyObjectList::~KeyObjectList() throw ()
{
	keylist.clear();
}

const  KeyObjectList&  KeyObjectList::operator= (const  KeyObjectList& src) throw (std::bad_alloc) 
{
	if ( keylist.size() == src.keylist.size())
	{
		timestamp = src.timestamp;
		for (unsigned int i=0; i < src.keylist.size(); ++i)
		{
			keylist[i] = src.keylist[i];
		}
	}

	//keylist   = src.keylist;
	return *this;
} 

void  KeyObjectList::show(std::ostream &stream) const
{
 	for (unsigned int i=0; i< keylist.size(); i++)
		stream << std::hex <<  "["  << std::setw(2) << std::setfill('0') <<(unsigned int)keylist[i]<< "]";
	stream << '\n';
}