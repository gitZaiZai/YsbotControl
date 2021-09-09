#include "StdAfx.h"
#include "InOutContainer.h"
#include "MotionControl.h"

InOutContainer::InOutContainer(void) throw (std::bad_alloc) 
{
 	Ysbot::Time null;
	null.set_msec(0);
   
	ioObjects.clear();
	for (int i=0; i < 3; i++) 
	{
		ioObjects.push_back(new InOutObjectList());
 	}
	ioObjects[0]->timestamp = null;
	ioObjects[1]->timestamp = null;
	ioObjects[2]->timestamp = null;
/////////////////// teach panel I/O ////////////////////// 
	add_panel_io();
}

InOutContainer::~InOutContainer(void) throw ()
{
//  	clear();
	for (unsigned int i=0; i < ioObjects.size(); i++) 
	{
		delete ioObjects[i];
	}
	ioObjects.clear();
}

void InOutContainer::add_mcc_rio( const IoModuleParameters& mcc ) throw()
{
// 	ioObjects.at(0).objectlist.clear();
	ioObjects[0]->objectlist.clear();
////////////////////// motion card I/O //////////////////// 
	InOutObject otp (    InOutObject::ot_positive ); // one bit for one port
	InOutObject otn (    InOutObject::ot_negative );
	InOutObject h   (    InOutObject::home_sensor        );
	InOutObject emgc (   InOutObject::estop       );
	InOutObject enable ( InOutObject::servo_enable);
	InOutObject prdy (   InOutObject::pos_ready   );
	InOutObject exin (   InOutObject::ext_input   ); // one byte for one port
	InOutObject exout (  InOutObject::ext_output  );

	InOutObject gpin (   InOutObject::gpio_input  ); // one byte for one port
	InOutObject gpout (  InOutObject::gpio_output );
	InOutObject alarm (  InOutObject::alarm_input );

	ioObjects[0]->objectlist.push_back ( otp );
	ioObjects[0]->objectlist.push_back ( otn );
	ioObjects[0]->objectlist.push_back ( h    );
	ioObjects[0]->objectlist.push_back ( emgc  );
	ioObjects[0]->objectlist.push_back ( enable);
	ioObjects[0]->objectlist.push_back ( prdy  );

	ioObjects[0]->objectlist.push_back ( gpin   );
	ioObjects[0]->objectlist.push_back ( gpout  );
	ioObjects[0]->objectlist.push_back ( alarm  );
	
	// MCC RIO
	for ( int i = 0; i < mcc.x_word_size; i++ ) // x_point_num para config mcc.x_word_size;
	{
		ioObjects[0]->objectlist.push_back ( exin  );
 		exin.index++;
 	}
	for ( int i = 0; i < mcc.y_word_size; i++ )   // mcc.y_point_num;
	{
  		ioObjects[0]->objectlist.push_back ( exout );
 		exout.index++;
	}
}

void InOutContainer::add_fieldbus_io( const IoModuleParameters& field ) throw()
{
	ioObjects[1]->objectlist.clear();

////////////////////// field bus device input and output /////////// 
	InOutObject fbx (    InOutObject::fieldbus_x   );
	InOutObject fbmx (   InOutObject::fieldbus_mx  );
	InOutObject fby (    InOutObject::filedbus_y   );
	InOutObject fbmy (   InOutObject::filedbus_my  );
	InOutObject fbdreg ( InOutObject::filedbus_d );

	for ( int i = 0; i < field.x_word_size; i++ ) // para config
	{
		ioObjects[1]->objectlist.push_back (fbx);
		fbx.index++;
	}

	for ( int i = 0; i < field.mx_word_size; i++ )  
	{
		ioObjects[1]->objectlist.push_back (fbmx);
		fbmx.index++;
	}

	for ( int i = 0; i < field.y_word_size; i++ )  
	{
		ioObjects[1]->objectlist.push_back (fby);
		fby.index++;
	}

	for ( int i = 0; i < field.my_word_size; i++ )  
	{
		ioObjects[1]->objectlist.push_back (fbmy);
		fbmy.index++;
	}

	// 	ifstream datafile ("plc_data.txt", ios::in );    
	//  	ioObjects[1]->readFrom( datafile );

	// may use read method to load data from hard disk.
	for ( int i = 0; i < field.d_register_num; i++ ) 
	{
		ioObjects[1]->objectlist.push_back (fbdreg);
		fbdreg.index++;
	}
}

void InOutContainer::add_panel_io()
{
	InOutObject pkey ( InOutObject::panel_key );
	InOutObject pout ( InOutObject::panel_out );
	ioObjects[2]->objectlist.push_back (pkey);
	ioObjects[2]->objectlist.push_back (pout);
}

void InOutContainer::add_io( const IoModuleParameters& ip, const IoSource ioc)
{
	if ( ioc == IOSmotioncard )
 		add_mcc_rio(ip);
 	else if ( ioc == IOSfieldbus )
 		add_fieldbus_io(ip);
 }

void InOutContainer::set (const InOutObject& obj, Ysbot::Time t) throw (std::bad_alloc) 
{
 	std::vector<InOutObject>::iterator it;
	std::vector<InOutObject>::iterator itend;
	if (   obj.object_type==InOutObject::ot_positive || obj.object_type==InOutObject::ot_negative  || obj.object_type==InOutObject::home_sensor 
		|| obj.object_type==InOutObject::estop       || obj.object_type==InOutObject::servo_enable || obj.object_type==InOutObject::pos_ready 
		|| obj.object_type==InOutObject::ext_input   || obj.object_type==InOutObject::ext_output  
		|| obj.object_type==InOutObject::gpio_input  || obj.object_type==InOutObject::gpio_output 
		|| obj.object_type==InOutObject::alarm_input )
	{
		it    = ioObjects[0]->objectlist.begin();
		itend = ioObjects[0]->objectlist.end();
		ioObjects[0]->timestamp = t;
	 
 	}
	else if (obj.object_type==InOutObject::fieldbus_x  || obj.object_type==InOutObject::fieldbus_mx || obj.object_type==InOutObject::filedbus_y 
		  || obj.object_type==InOutObject::filedbus_my || obj.object_type==InOutObject::filedbus_d )
	{
		it    = ioObjects[1]->objectlist.begin();
		itend = ioObjects[1]->objectlist.end();
		ioObjects[1]->timestamp = t;

  	}
	else if (obj.object_type==InOutObject::panel_key || obj.object_type==InOutObject::panel_out )
	{
		it    = ioObjects[2]->objectlist.begin();
		itend = ioObjects[2]->objectlist.end();
		ioObjects[2]->timestamp = t;
	}
 
	while( it < itend ) 
	{
		if ( it->object_type == obj.object_type && it+obj.index < itend )
		{
 			*(it+obj.index) = obj;
 			break;
		}
		it++;
	}
}

const InOutObjectList& InOutContainer::get_motioncard () const throw () { return *ioObjects[0]; }

const InOutObjectList& InOutContainer::get_fieldbus () const throw () { return *ioObjects[1]; }

const InOutObjectList& InOutContainer::get_panel () const throw () { return *ioObjects[2]; }

const InOutObjectList& InOutContainer::get_io_objects( const IoSource ioc ) const throw () 
{
	return *ioObjects[ioc];
}
  
void InOutContainer::clear () throw () {

	for (unsigned int i = 0; i < ioObjects.size(); i++ )
	{
 		ioObjects[i]->objectlist.clear();
	}
}
 
Ysbot::Time InOutContainer::get_timestamp (const IoSource ioc) const throw () {
	return ioObjects[ioc]->timestamp;
}

void InOutContainer::set_timestamp (Ysbot::Time t, const IoSource ioc) throw () {
	ioObjects[ioc]->timestamp = t;
}
 