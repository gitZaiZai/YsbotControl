#ifndef  _FieldBusDevice_type_h
#define  _FieldBusDevice_type_h

#include "CmdInfo.h"
#include "IoModuleParameters.h"



class FieldBusDeviceType
{
public:

	virtual ~FieldBusDeviceType () throw () {;}
	
	virtual int  connect ( ) = 0;
	virtual int  disconnect ( ) = 0;

	virtual int  din (  const int, bool* ) = 0;
	virtual int  dout ( const int, const bool ) = 0;

	virtual int  read_data_reg (  const int, unsigned short* ) = 0;
	virtual int  write_data_reg ( const int, const unsigned short ) = 0;

	virtual int  read_multiple_word (  const int, const unsigned int, unsigned short* ) = 0;
	virtual int  write_multiple_word ( const int, const unsigned int, const unsigned short*  ) = 0;

	virtual const  IoModuleParameters& get_fieldbus_io_parameters() const throw () = 0;
 };

#endif