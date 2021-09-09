#ifndef _digital_output_h
#define _digital_output_h
#include "LuaScript/BasicScriptVariable.h"

class DigitalOutput : public BasicScriptVariable
{
public:
	int  device_id;
	int  port;
	int  output_value;
	int  pulse_time;
    bool force_output;

	DigitalOutput () : device_id(0),port(0), output_value(0), pulse_time(0),force_output(false) {;}
	DigitalOutput (int id, int pnb, int val, int wt, bool vi ) : device_id(id), port(pnb), output_value(val), pulse_time(wt),force_output(vi) {;}
	DigitalOutput (const DigitalOutput& di) { operator= (di); }
	const DigitalOutput& operator= (const DigitalOutput& di) 
	{
		this->device_id    = di.device_id;
		this->port         = di.port;
		this->output_value = di.output_value;
		this->pulse_time   = di.pulse_time;
		this->force_output = di.force_output;
		return *this;
	}

	void set ( bool ) throw ();
	void pulse ( bool, int ) throw();
	void setbit (int) throw();
	void set_on_trigger(bool) throw();

	char* get_value( const char* name, char* strValue);
	void  set_value( const char* name, char* strValue);
	void  check_value( const char* name, const bool, char* );

};

std::ostream& operator<< (std::ostream& os, const DigitalOutput& v);
std::istream& operator>> (std::istream &in, DigitalOutput& v);


#endif