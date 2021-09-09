#include "StdAfx.h"
#include "FieldBusDeviceDummy.h"
#include "FieldBusDeviceFactory.h"
#include "YsibotsException.h"
#include "Journal.h"

namespace
{
	class Builder : public FieldBusDeviceBuilder 
	{
		static Builder the_builder;
	public:
		Builder () {
			FieldBusDeviceFactory::get_fieldbus_device_factory ()->sign_up (std::string("Dummy"), this);
		}
		FieldBusDeviceType* get_fieldbus_device (const std::string&, const ConfigReader& reader, FieldBusDeviceType*) throw (std::bad_alloc) {
			return new FieldBusDeviceDummy (reader);
		}
	};
	Builder the_builder;
}

FieldBusDeviceDummy::FieldBusDeviceDummy(const ConfigReader& reader) throw (InvalidConfigurationException)
{
	fp.is_connected = true;
	TRACE(_T("Constuct FieldBusDeviceDummy!\n")); 
}


FieldBusDeviceDummy::~FieldBusDeviceDummy(void)
{
	;
}

int FieldBusDeviceDummy::din( const int port, bool* val )
{
	TRACE(_T("Dummy input!\n"));
	*val = ( port%2 == 0 ) ? true : false;
	return 0;
}

int FieldBusDeviceDummy::dout( const int port, const bool val )
{
	cout << "dummy dout \n";
	return 0;
}

 

 