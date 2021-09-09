#ifndef fieldbus_device_dummy_h
#define fieldbus_device_dummy_h

#include "FieldBusDevicetype.h"
#include "ConfigReader.h"

class FieldBusDeviceDummy :
	public FieldBusDeviceType
{
public:
	FieldBusDeviceDummy(const ConfigReader&) throw (InvalidConfigurationException);
	~FieldBusDeviceDummy(void);
   
	virtual int  connect ( ) { return 0; }
	virtual int  disconnect ( ) { return 0; }

	virtual int  din( const int, bool* );
	virtual int  dout( const int, const bool );

	virtual int  read_data_reg (  const int, unsigned short* ) { return 0; }
	virtual int  write_data_reg ( const int, const unsigned short  ) { return 0; }

	virtual int  read_multiple_word (  const int, const unsigned int, unsigned short* ) { return 0; }
	virtual int  write_multiple_word ( const int, const unsigned int, const unsigned short*  ) { return 0; }

	virtual const  IoModuleParameters& get_fieldbus_io_parameters() const throw () { return fp; }

private:
	 IoModuleParameters fp; 
};

#endif