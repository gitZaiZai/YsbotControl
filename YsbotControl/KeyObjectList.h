#ifndef _key_object_h
#define _key_object_h

#include "Time.h"
#include <vector>

struct KeyObjectList 
{
	Ysbot::Time timestamp;                         
	std::vector<unsigned char> keylist;  

	KeyObjectList (unsigned int len =5) throw ();
	~KeyObjectList () throw ();
	KeyObjectList (const KeyObjectList&) throw (std::bad_alloc);
	KeyObjectList ( unsigned char* charkey, unsigned int len =5 ) throw (std::bad_alloc);
	const KeyObjectList& operator= (const KeyObjectList&) throw (std::bad_alloc);

	void show(std::ostream &stream) const;

};


#endif