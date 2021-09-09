#include "StdAfx.h"
#include "ServoFactory.h"
 
using namespace std;

ServoFactory* ServoFactory::the_only_factory = (NULL);

ServoFactory::ServoFactory () throw () {;}

ServoFactory* ServoFactory::get_servo_factory () throw (std::bad_alloc) {
	if (!the_only_factory)
	{
		the_only_factory = new ServoFactory;
		::atexit(destroy_factory);
	}
	return the_only_factory;
}

ServoFactory::~ServoFactory() throw ()
{

	list_of_plugins.clear();
}

void ServoFactory::sign_up (const std::string descriptor, ServoBuilder* pointer) throw (std::bad_alloc) {
	list_of_plugins [descriptor] = pointer;
}

ServoType* ServoFactory::get_servo (const std::string descriptor, const ConfigReader& reader) throw (YsbotsException, std::bad_alloc,invalid_argument) {
	map<std::string, ServoBuilder*>::iterator mit = list_of_plugins.find (descriptor);
	ServoType* new_wm = NULL;
	if (mit!=list_of_plugins.end())
		new_wm = mit->second->get_servo (descriptor, reader, NULL);
	else
		throw invalid_argument (string("unknown Servo type ")+descriptor);

	return new_wm;
}

void ServoFactory::servo_list (std::vector<std::string>& list) const throw (std::bad_alloc) {
	map<string, ServoBuilder*>::const_iterator it = list_of_plugins.begin();
	unsigned int sz = list_of_plugins.size();
	list.resize (sz);
	for (unsigned int i=0; i<sz; i++)
		list [i] = (it++)->first;
}

void ServoFactory::destroy_factory()
{
	if ( the_only_factory )
	{
		delete the_only_factory;
		the_only_factory = NULL;
	}

}