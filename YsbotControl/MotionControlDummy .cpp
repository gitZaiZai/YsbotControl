#include "StdAfx.h"
#include "MotionControlDummy.h"
#include "MotionControlFactory.h"
#include "YsibotsException.h"
#include "Journal.h"

namespace
{
	class Builder : public MotionControlBuilder 
	{
		static Builder the_builder;
	public:
		Builder () {
			MotionControlFactory::get_motion_control_factory ()->sign_up (std::string("Dummy"), this);
		}
		MotionControlType* get_motion_control (const std::string&, const ConfigReader& reader, MotionControlType*) throw (std::bad_alloc) {
			return new MotionControlDummy (reader);
		}
	};
	Builder the_builder;
}

MotionControlDummy::MotionControlDummy(const ConfigReader& reader) throw (InvalidConfigurationException)  
{
	TRACE(_T("Constuct MotionControlDummy!\n")); 
}


MotionControlDummy::~MotionControlDummy(void)
{
	;
}

 

 