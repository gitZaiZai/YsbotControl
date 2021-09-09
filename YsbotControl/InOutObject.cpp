#include "stdafx.h"
#include "InOutObject.h"
#include <limits>
 

InOutObject::InOutObject () throw () {
	state=0;
	index=0;
	simulated_signal = false;
 	object_type=InOutObject::unknown;
}

InOutObject::InOutObject (unsigned short s1, ObjectType ot1, unsigned short idx, bool b) throw () : state(s1), object_type(ot1), index(idx),simulated_signal(b) {;}

InOutObject::InOutObject (ObjectType ot1) throw () : state(0), object_type(ot1), index(0), simulated_signal(false) {;}

InOutObject::InOutObject (const InOutObject& src) throw () { operator= (src); }

const InOutObject& InOutObject::operator= (const InOutObject& src) throw () {
	state=src.state;
	object_type=src.object_type;
	index = src.index;
	simulated_signal = src.simulated_signal;
	return (*this);
}

InOutObjectList::InOutObjectList (const InOutObjectList& src) throw (std::bad_alloc) 
{
	operator= (src);
}

const InOutObjectList& InOutObjectList::operator= (const InOutObjectList& src) throw (std::bad_alloc) {
	timestamp  = src.timestamp;
	objectlist.clear();
	for (unsigned int i=0; i<src.objectlist.size(); i++) 
	{
		objectlist.push_back (src.objectlist[i]);
 	}
	 
	return *this;
}                                              

void InOutObjectList::writeAt(std::ostream &stream) const
{
	for (unsigned int i=0; i< objectlist.size(); i++)
		stream << "("<< objectlist[i].state << "," << objectlist[i].object_type <<  ")";
}

int InOutObjectList::readFrom(std::istream &stream)
{
	int n=0;
	objectlist.clear();

	do {
		InOutObject o;

		stream.ignore( (std::numeric_limits<std::streamsize>::max)(),'(');
		stream >> o.state;
		if (stream.fail() || stream.eof()) break;
		 
		int ot;
		stream >> ot;
		o.object_type = InOutObject::ObjectType (ot);
		if (stream.fail() || stream.eof()) break;
		stream.ignore( (std::numeric_limits<std::streamsize>::max)(),')');
		objectlist.push_back(o);
 		n++;
	} while (!(stream.fail() || stream.eof()));

	return n;
}

const unsigned short InOutObjectList::get_state(InOutObject::ObjectType ot, unsigned short c) const throw()
{
	std::vector<InOutObject>::const_iterator it    = objectlist.begin();
	std::vector<InOutObject>::const_iterator itend = objectlist.end();

	while( it < itend ) 
	{
		if ( it->object_type == ot && (it+c) < itend )
		{
			return (it+c)->state;
 		}
		it++;
	}
	return 0;
}
