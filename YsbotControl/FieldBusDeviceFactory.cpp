#include "StdAfx.h"
#include "FieldBusDeviceFactory.h"
 
using namespace std;

FieldBusDeviceFactory* FieldBusDeviceFactory::the_only_factory = (NULL);

FieldBusDeviceFactory::FieldBusDeviceFactory () throw () {;}

FieldBusDeviceFactory* FieldBusDeviceFactory::get_fieldbus_device_factory () throw (std::bad_alloc) {
	if (!the_only_factory)
	{
		the_only_factory = new FieldBusDeviceFactory;
		::atexit(destroy_factory);
	}
	return the_only_factory;
}

FieldBusDeviceFactory::~FieldBusDeviceFactory() throw ()
{

	list_of_plugins.clear();
}

void FieldBusDeviceFactory::sign_up (const std::string descriptor, FieldBusDeviceBuilder* pointer) throw (std::bad_alloc) {
	list_of_plugins [descriptor] = pointer;
}

FieldBusDeviceType* FieldBusDeviceFactory::get_fieldbus_device (const std::string descriptor, const ConfigReader& reader) throw (YsbotsException, std::bad_alloc,invalid_argument) {
	map<std::string, FieldBusDeviceBuilder*>::iterator mit = list_of_plugins.find (descriptor);
	FieldBusDeviceType* new_wm = NULL;
	if (mit!=list_of_plugins.end())
		new_wm = mit->second->get_fieldbus_device (descriptor, reader, NULL);
	else
		throw invalid_argument (string("unknown FieldBusDevice type ")+descriptor);

	return new_wm;
}

void FieldBusDeviceFactory::fieldbus_device_list (std::vector<std::string>& list) const throw (std::bad_alloc) {
	map<string, FieldBusDeviceBuilder*>::const_iterator it = list_of_plugins.begin();
	unsigned int sz = list_of_plugins.size();
	list.resize (sz);
	for (unsigned int i=0; i<sz; i++)
		list [i] = (it++)->first;
}

void FieldBusDeviceFactory::destroy_factory()
{
	if ( the_only_factory )
	{
		delete the_only_factory;
		the_only_factory = NULL;
	}

}