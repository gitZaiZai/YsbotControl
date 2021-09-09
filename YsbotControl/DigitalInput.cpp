#include "stdafx.h"
#include "DigitalInput.h"
#include "LuaScript/ScriptFunc.h"
#include "LuaScript/ScriptVariables.h"

int DigitalInput::read() throw()
{
 	printf(" din read device %d, port %d \n", device_id, port);
	
    input_value = (int)( ScriptFunc::do_in( port, device_id, true, virtual_input ) );
	return input_value;
}

int DigitalInput::readNoWait() throw()
{
	printf(" din read device %d, port %d \n", device_id, port);

	input_value = (int)( ScriptFunc::do_in( port, device_id, false, virtual_input ) );
	
	return input_value;
}

int DigitalInput::wait( bool bval, int elsptime ) throw()
{
	std::cout << " din wait port = " << port << " val = " << input_value << " time = " << elsptime << '\n';
	wait_time   = elsptime;
	input_value = bval;
	 
	int ret = ScriptFunc::WAITIO( device_id, port, input_value, elsptime, virtual_input );

 	return ret;
}

int DigitalInput::readbit() throw()
{
	std::cout << " din readbit port = " << port << '\n';
	return 66;
}

int DigitalInput::waitbit(int ival, int elsptime) throw()
{
	wait_time   = elsptime;
	input_value = ival;
	std::cout << " din waitbit port = " << port << " val = " << input_value << " time = " << elsptime << '\n';
	return 88;
}

int DigitalInput::read_while_motion(double percent) throw()
{
	std::cout << " din read port = " << port << '\n';
	input_value = ScriptFunc::on_position_din( percent, port );
	return input_value;
}

char* DigitalInput::get_value( const char* name, char* strValue )
{
	check_value( name, false, strValue ); 
	return strValue;
}

void DigitalInput::set_value( const char* name, char* strValue)
{
	check_value(name,true,strValue);
}

void DigitalInput::check_value( const char* name, const bool bset,  char* val )
{
	int* intdig[] = { &device_id, &port, &input_value, &wait_time, NULL };
	char prop[VAR_STR_LEN];
	for ( int i = 0; i < VARINFO->vardesc[svDIGITALINPUT]->getValuesTotal(); i++ )
	{
		VARINFO->vardesc[svDIGITALINPUT]->getValue(i+1,prop);
		if ( prop[0] != '\0' &&  strstr( prop, name ) ) 
		{
			if ( bset )
			{
				if ( intdig[i] )
				{
 				 	*intdig[i] = atoi( val ? val : 0 );
				}
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
					virtual_input = ( b == true ) ? true : false;
				}
			}
			else
			{
				sprintf( val, "%d", intdig[i] == NULL ? virtual_input : *intdig[i] );
			}
			return;
		}
	}

	if ( !bset )
		*val = '\0';

}

std::ostream& operator<< (std::ostream& os, const DigitalInput& v)
{
	os << v.device_id <<' '<< v.port << ' ' << v.input_value<<' '<< v.wait_time << ' ' << (int)v.virtual_input;
	return os;
}

std::istream& operator>> (std::istream &in, DigitalInput &v)
{
	in >> v.device_id >> v.port >> v.input_value >> v.wait_time >> v.virtual_input;
	return in;
}