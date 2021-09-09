#include "StdAfx.h"
#include "YsibotsException.h"
 

YsbotsException::YsbotsException(const char* err_type)
{
	what_string = std::string ("YsbotsException: ")+std::string(err_type);
 }

const char* YsbotsException::what () throw () 
{
	return what_string.c_str();
}
 

HardwareException::HardwareException (const char* err_type) : YsbotsException (err_type) {;}

BadHardwareException::BadHardwareException (const char* err_type) : YsbotsException (err_type) {;}

InvalidConfigurationException::InvalidConfigurationException (const char* err_line) : YsbotsException (
	(std::string ("invalid configuration line \"")+std::string(err_line)+std::string("\"")).c_str() ) {;}
