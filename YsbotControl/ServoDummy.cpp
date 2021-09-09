#include "StdAfx.h"
#include "ServoDummy.h"
#include "ServoFactory.h"
#include "YsibotsException.h"
#include "Journal.h"
#include "WorldModel.h"
#include "stringconvert.h"

using namespace std;

namespace
{
	class Builder : public ServoBuilder 
	{
		static Builder the_builder;
	public:
		Builder () {
			ServoFactory::get_servo_factory ()->sign_up (std::string("Dummy"), this);
		}
		ServoType* get_servo (const std::string&, const ConfigReader& reader, ServoType*) throw (std::bad_alloc) {
			return new ServoDummy (reader);
		}
	};
	Builder the_builder;
}

ServoDummy::ServoDummy(const ConfigReader& reader) throw (InvalidConfigurationException) : configuration_list(const_cast<ConfigReader&>(reader)),open_port(false)
{
	TRACE(_T("Constuct ServoDummy!\n")); 
}


ServoDummy::~ServoDummy(void)
{
	;
}

int   ServoDummy::read_pos( unsigned int iDeviceID, long* pData)
{
	if ( !open_port )
	{
		TRACE(_T("servo port no open!\n"));
		return -1;
	}
//   	TRACE(_T("read pos NO.%d ServoDummy!\n"),iDeviceID); 

	short nEncoderLaps   = 10;
	DWORD wEncoderPulses = 3000;
	static double delta = 0.01;
	delta += 0.01;
	*pData = ( iDeviceID * nEncoderLaps ) * 100 + static_cast<int>(delta * 100);
	 
	return 0;
}

int   ServoDummy::clear_pos( unsigned int iDeviceID )
{
	if ( !open_port )
	{
		TRACE(_T("servo port no open!\n"));
		return -1;
	}
//  	TRACE(_T("clear_pos NO.%d ServoDummy!\n"),iDeviceID+1); 
	// serial port operation...

    long residual_pls = 0;

	read_pos( iDeviceID, &residual_pls);
  	
	unsigned long long id = iDeviceID;
 	string prefix = "AXIS_" + to_string(id);
 
  	configuration_list.set( ( prefix+string("::encoder_origin") ).c_str(), residual_pls );   
  
    if ( residual_pls != 0 )
    {
		//configuration_list.write_config_file( strconfile.c_str() );
		configuration_list.write_all();

    }
	return 0;
}

int   ServoDummy::clear_alarm( unsigned int iDeviceID )
{
	if ( !open_port )
	{
		TRACE(_T("servo port no open!\n"));
		return -1;
	}
	TRACE(_T("clear_alarm NO.%d ServoDummy!\n"),iDeviceID); 

	return 0;
}

bool ServoDummy::init_port()
{
	TRACE(_T("init_port ServoDummy!\n")); 
	open_port = true;
  	return true;
}

bool ServoDummy::close()
{
	TRACE(_T("close_port ServoDummy!\n")); 
	open_port = false;

 	return true;
}