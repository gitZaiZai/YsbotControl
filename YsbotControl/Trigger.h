#ifndef _trigger_h
#define _trigger_h

#include "Time.h"
#include "DigitalInput.h"
#include "DigitalOutput.h"
#include "PosInfo.h"

enum TriggerType
{
	onDISTANCE,
	onPOSITION,
	onPARAMETER,
	onREADWHILEMOTION
};

enum DistanceType
{
	FROMBEGIN,
	FROMEND,
};

struct Trigger
{
	Trigger() throw();
	Trigger (const Trigger& rl) { operator= (rl); }
 	const Trigger& operator= ( const Trigger& rl); 
	
	DigitalOutput dout;
	DigitalInput* din;
	TriggerType   trigger_type;
	DistanceType  distance_type;

	long          time_ms;
	double        distance;
	double        path_percent;
	Ysbot::Time          time_clock;

	PosInfo       path_start;
	PosInfo       path_end;

	unsigned long long           command_index;
};


#endif