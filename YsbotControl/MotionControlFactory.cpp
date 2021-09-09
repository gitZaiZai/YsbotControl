#include "StdAfx.h"
#include "MotionControlFactory.h"
 
using namespace std;

MotionControlFactory* MotionControlFactory::the_only_factory = (NULL);

MotionControlFactory::MotionControlFactory () throw () {;}

MotionControlFactory* MotionControlFactory::get_motion_control_factory () throw (std::bad_alloc) {
	if (!the_only_factory)
	{
		the_only_factory = new MotionControlFactory;
		::atexit(destroy_factory);
	}
	return the_only_factory;
}

MotionControlFactory::~MotionControlFactory() throw ()
{

	list_of_plugins.clear();
}

void MotionControlFactory::sign_up (const std::string descriptor, MotionControlBuilder* pointer) throw (std::bad_alloc) {
	list_of_plugins [descriptor] = pointer;
}

MotionControlType* MotionControlFactory::get_motion_control (const std::string descriptor, const ConfigReader& reader) throw (YsbotsException, std::bad_alloc,invalid_argument) {
	map<std::string, MotionControlBuilder*>::iterator mit = list_of_plugins.find (descriptor);
	MotionControlType* new_wm = NULL;
	if (mit!=list_of_plugins.end())
		new_wm = mit->second->get_motion_control (descriptor, reader, NULL);
	else
		throw invalid_argument (string("unknown MotionControl type ")+descriptor);

	return new_wm;
}

void MotionControlFactory::motion_control_list (std::vector<std::string>& list) const throw (std::bad_alloc) {
	map<string, MotionControlBuilder*>::const_iterator it = list_of_plugins.begin();
	unsigned int sz = list_of_plugins.size();
	list.resize (sz);
	for (unsigned int i=0; i<sz; i++)
		list [i] = (it++)->first;
}

void MotionControlFactory::destroy_factory()
{
	if ( the_only_factory )
	{
		delete the_only_factory;
		the_only_factory = NULL;
	}

}