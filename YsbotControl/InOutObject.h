
#ifndef _inout_object_h
#define _inout_object_h

#include "Time.h"
#include "Vec.h"
#include <vector>

#define SETBIT(a, b) ( a |=  (1 << b))
#define CLRBIT(a, b) ( a &= ~(1 << b))
#define GETBIT(a, b) ( (a >> b) & 1)

enum IoSource {
	IOSmotioncard = 0,
 	IOSfieldbus,
	IOSpanel,
};

enum IO_DEVICE
{
	MCS_GPIO      = 0,
	MCS_REMOTEIO  = 1,
	MCS_LOCALIO   = 2,
	PLC_DEVICE    = 3,
};

struct InOutObject {
    enum ObjectType {
      unknown,      
      ot_positive,         
      ot_negative,     
      home_sensor,   
      estop,
      alarm_input,     
      servo_enable,   
      pos_ready, 
	  ext_input,
	  ext_output,
      fieldbus_x,      
      fieldbus_mx,      
      filedbus_y,   
      filedbus_my,      
      filedbus_d, 
	  panel_key,
	  panel_out,
	  gpio_input,
	  gpio_output,
     };
    
    InOutObject () throw ();
	InOutObject (ObjectType) throw ();
    InOutObject (unsigned short, ObjectType =unknown, unsigned short =0, bool =false ) throw ();
    InOutObject (const InOutObject&) throw ();
    const InOutObject& operator= (const InOutObject&) throw ();
    
    unsigned short state;             
    ObjectType object_type;     
    unsigned short index;
	bool simulated_signal; // ??????????????????
  };


   struct InOutObjectList {
    Ysbot::Time timestamp;                          
    std::vector<InOutObject> objectlist;   

    InOutObjectList (unsigned int len =0) throw () : objectlist(len) {;}
	 
   ~InOutObjectList () throw () {objectlist.clear();}
    InOutObjectList (const InOutObjectList&) throw (std::bad_alloc);
    const InOutObjectList& operator= (const InOutObjectList&) throw (std::bad_alloc);

   
    void writeAt(std::ostream &stream) const;
 
    int  readFrom(std::istream &stream);
	const unsigned short get_state(InOutObject::ObjectType, unsigned short ) const throw();

  };
  
 

#endif

