#ifndef _Time_
#define _Time_
#include <iostream>

namespace Ysbot {
class Time
{
public:
     Time () throw ();
     Time (const Time&) throw ();
	 void   update () throw ();
	 long int elapsed_usec () const throw ();
     long int elapsed_msec () const throw ();
     long int elapsed_sec () const throw ();
 
     long int diff_usec (const Time&) const throw ();
     long int diff_msec (const Time&) const throw ();
     long int diff_sec (const Time&) const throw ();

	 void set_usec (const long int&) throw ();
     void set_msec (const long int&) throw ();
     void set_sec (const long int&) throw ();
	 void add_usec (long int) throw ();
     void add_msec (long int) throw ();
     void add_sec (long int) throw ();
     const Time& operator= (const Time&) throw ();
  
     long int get_usec () const throw ();
     long int get_msec () const throw ();
     long int get_sec () const throw ();
   
     bool operator== (const Time&) const throw ();
     bool operator!= (const Time&) const throw ();
     bool operator<= (const Time&) const throw ();
     bool operator< (const  Time&) const throw ();
     bool operator>= (const Time&) const throw ();
     bool operator> (const  Time&) const throw ();
 
protected:
private:
	 long int	   sec;
	 long int	   usec;
	 LARGE_INTEGER litmp;
     LONGLONG      QPart1;
	 double        dfFreq;
     static const Time starting_time;    
};

}
std::ostream& operator<< (std::ostream&, const Ysbot::Time&) throw();   ///< Zeit seit Programmstart in msec

#endif
