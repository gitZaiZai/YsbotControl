#ifndef _ysbots_exception_h
#define _ysbots_exception_h

#include <exception>
#include <string>
#include <vector>

class YsbotsException : public std::exception
{
protected:
	std::string what_string;    
	 
public:
 
	YsbotsException (const char*);
	~YsbotsException () throw () {;}

 	virtual const char* what () throw();
  	 
};

class HardwareException : public YsbotsException
{
protected:
	std::string config_argument;   
public:    
	HardwareException (const char*);
	~HardwareException () throw () {;}
};

class BadHardwareException : public YsbotsException 
{
public:
	BadHardwareException (const char*);
	~BadHardwareException () throw () {;}
};

class InvalidConfigurationException : public YsbotsException 
{
protected:
	std::string config_argument;   
public:    
	InvalidConfigurationException (const char*);
	~InvalidConfigurationException () throw () {;}
};

#endif