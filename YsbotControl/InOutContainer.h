// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef _inout_container_h
#define _inout_container_h

#include "InOutObject.h"
#include "IoModuleParameters.h"

class InOutContainer
{
private:
 	std::vector <InOutObjectList*> ioObjects;
	const InOutObjectList& get_motioncard () const throw ();
	const InOutObjectList& get_fieldbus () const throw ();
	const InOutObjectList& get_panel () const throw ();
	void add_mcc_rio ( const IoModuleParameters& mcc ) throw();
	void add_fieldbus_io ( const IoModuleParameters& field ) throw();
	void add_panel_io() throw();

public:
	InOutContainer(void) throw (std::bad_alloc);
	~InOutContainer(void) throw ();

	void set (const InOutObject&, Ysbot::Time) throw (std::bad_alloc);

	void clear () throw ();

	const InOutObjectList& get_io_objects ( const IoSource ) const throw ();

	void add_io ( const IoModuleParameters&, const IoSource ) throw();

	Ysbot::Time get_timestamp (const IoSource =IOSfieldbus) const throw ();
	void set_timestamp (Ysbot::Time, const IoSource =IOSfieldbus) throw ();

 };

#endif