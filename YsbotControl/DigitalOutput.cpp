#include "stdafx.h"
#include "DigitalOutput.h"
#include "LuaScript/ScriptFunc.h"
#include "FieldBusDevice.h"
#include "LuaScript/ScriptVariables.h"

void DigitalOutput::set(bool bval) throw()
{
//  	printf(" dout set device %d, port %d, value = %d\n", device_id, port, static_cast<int>(bval) );
	if ( force_output )
		return;

	output_value = static_cast<int>(bval);
 
 	ScriptFunc::do_out( port, (output_value == 1 ? true : false), device_id, true, static_cast<double>(pulse_time) );

}

void DigitalOutput::pulse( bool bval, int elsptime ) throw()
{
	std::cout << " dout pulse port = " << port << "value = " << output_value << "time = " << elsptime << '\n';

	if ( device_id == PLC_DEVICE )
	{
		output_value = bval;
		unsigned short data[] = {output_value, static_cast<unsigned short>(elsptime)};
		FieldBusDevice::get_main_fieldbus_device().write_multiple_word( 7400 + port * 2, 2, data );
 	}
}

void DigitalOutput::setbit(int ival) throw()
{
	if ( force_output )
		return;

	output_value = ival;
 	std::cout << " dout setbit port = " << port << "value = " << output_value << '\n';
}

void DigitalOutput::set_on_trigger( bool bval ) throw()
{
	if ( force_output )
		return;

	std::cout << " dout set port = " << port << "value = " << output_value << '\n';
	output_value = static_cast<int>(bval);
	ScriptFunc::do_out( port, (output_value == 1 ? true : false), device_id, false, 0 );

}

char* DigitalOutput::get_value( const char* name, char* strValue )
{
	check_value( name, false, strValue ); 
	return strValue;
}

void DigitalOutput::set_value( const char* name, char* strValue)
{
	check_value(name,true,strValue);
}

void DigitalOutput::check_value( const char* name, const bool bset,  char* val )
{
 	int* intdig[] = { &device_id, &port, &output_value, &pulse_time, NULL };
	char prop[VAR_STR_LEN];
	for ( int i = 0; i < VARINFO->vardesc[svDIGITALOUTPUT]->getValuesTotal(); i++ )
	{
		VARINFO->vardesc[svDIGITALOUTPUT]->getValue(i+1,prop);
		if ( prop[0] != '\0' &&  strstr( prop, name ) ) 
		{
			if ( bset )
			{
				if (intdig[i])
					*intdig[i] = atoi( val ? val : 0 );
				else
				{
 					bool b = false;
					if( !val )
						;
					else if( isdigit( val[ 0 ] ) )
						b = atoi( val ? val : 0 ) > 0 ? true : false;
					else if( strcmp( val, "on"  ) == 0 ||
						strcmp( val, "true") == 0 ||
						strcmp( val, "yes" ) == 0 )
						b = true;
					force_output = ( b == true ) ? true : false;
				}
			}
			else
			{
  				sprintf( val, "%d", intdig[i] == NULL ? force_output : *intdig[i] );
 			}
			return;
		}
	}
	
	if ( !bset )
		*val = '\0';

}

std::ostream& operator<< (std::ostream& os, const DigitalOutput& v)
{
	os << v.device_id << ' '<< v.port << ' ' << v.output_value<<' '<< v.pulse_time << ' ' << (int)v.force_output;
	return os;
}

std::istream& operator>> (std::istream &in, DigitalOutput &v)
{
	in >> v.device_id >> v.port >> v.output_value >> v.pulse_time >> v.force_output;
	return in;
}