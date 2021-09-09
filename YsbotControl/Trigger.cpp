#include "stdafx.h"
#include "Trigger.h"

Trigger::Trigger() throw()
{
 	trigger_type  = onDISTANCE;
	distance_type = FROMBEGIN;

	time_ms  = 0;
	distance = 0;
	path_percent = 0;

	time_clock.update();
 
	command_index = -1;
	din = NULL;
}

const Trigger& Trigger::operator=(const Trigger& tr)
{
	this->trigger_type  = tr.trigger_type;
	this->distance_type = tr.distance_type;
	this->dout          = tr.dout;
	this->din           = tr.din;
	this->path_start    = tr.path_start;
	this->path_end      = tr.path_end;
	this->distance      = tr.distance;
	this->path_percent  = tr.path_percent;
	this->command_index = tr.command_index;
	this->time_clock    = tr.time_clock;
	this->time_ms       = tr.time_ms;

	return *this;
}