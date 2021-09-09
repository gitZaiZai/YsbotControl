#ifndef _groupstate_machine_h
#define _groupstate_machine_h

#include "CmdInfo.h"

class GroupStateMachine
{
public:
	GroupStateMachine(void);
	~GroupStateMachine(void);
	const GroupStateMachine& operator= (const GroupStateMachine&) throw ();

	GroupState get_state () const throw ();

	void set_state (GroupState) throw ();
 
	void update (const CmdInfo& ) throw ();
	void update( MotionCoreInfo::MotionStatus  ) throw();

private:
	GroupState current;
	CmdInfo    last_cmd;

	bool       isWait;
	bool       isHold;
	bool       isDelaying;
};

#endif