#include "StdAfx.h"
#include "Time.h"

/************************************************************************/
/*   testing time                                                       */
/************************************************************************/

const Ysbot::Time Ysbot::Time::starting_time;

Ysbot::Time::Time() throw()
{
  QueryPerformanceFrequency(&litmp);
  dfFreq = (double)litmp.QuadPart; // 获得计数器的时钟频率
  update();
}

Ysbot::Time::Time (const  Time& src) throw () : QPart1( src.QPart1 ),dfFreq(src.dfFreq) {;}
const  Ysbot::Time&  Ysbot::Time::operator= (const  Ysbot::Time& src) throw () 
{
  QPart1 = src.QPart1;
  dfFreq = src.dfFreq;
  return *this;
}

void  Ysbot::Time::set_usec (const long int& d) throw () 
{
  QPart1=starting_time.QPart1;
  dfFreq=starting_time.dfFreq;
  add_usec (d);
}

void  Ysbot::Time::set_msec (const long int& d) throw () 
{
  QPart1=starting_time.QPart1;
  dfFreq=starting_time.dfFreq;
  add_msec( d );
}

void Ysbot::Time::set_sec (const long int& d) throw () 
{
  QPart1=starting_time.QPart1;
  dfFreq=starting_time.dfFreq;
  add_sec (d);
}

void  Ysbot::Time::add_usec( long int n ) throw () 
{
  double dQPart = n * dfFreq * 1e-6;
  QPart1 += static_cast<__int64>(dQPart);
 /* if( QPart1 < 0 ) 
  {
    long int k=-QPart1/1000000+1;
    sec-=k;
    usec+=1000000*k;
  } 
  else if (QPart1>=1000000) 
  {
    long int k=QPart1/1000000;
    sec+=k;
    usec-=1000000*k;
  }*/
}

void  Ysbot::Time::add_msec (long int n) throw () 
{
  add_usec (1000*n);
}

void  Ysbot::Time::add_sec (long int n) throw () 
{
  add_usec (1000000*n); 
}

void Ysbot::Time::update() throw() 
{
 	QueryPerformanceCounter(&litmp);
	QPart1 = litmp.QuadPart;          // 获得初始值
}

long int  Ysbot::Time::elapsed_sec () const throw () 
{
  Ysbot::Time now;
  return now.diff_sec (*this);
}

long int  Ysbot::Time::elapsed_usec () const throw () 
{
  Ysbot::Time now;
  return now.diff_usec (*this);
}

long int  Ysbot::Time::elapsed_msec () const throw () 
{
  Ysbot::Time now;
  return now.diff_msec (*this);
}

long int  Ysbot::Time::diff_usec (const  Ysbot::Time& src) const throw () 
{
 	return  (long int)((QPart1-src.QPart1)/ dfFreq*1e6);
}

long int  Ysbot::Time::diff_msec (const  Ysbot::Time& src) const throw () 
{
   return  (long int)((QPart1-src.QPart1)/ dfFreq*1e3);
}

long int  Ysbot::Time::diff_sec (const  Ysbot::Time& src) const throw () 
{
   return  (long int)((QPart1-src.QPart1)/ dfFreq);
}

bool  Ysbot::Time::operator== (const   Ysbot::Time& src) const throw () 
{
  return (QPart1==src.QPart1);
}

bool  Ysbot::Time::operator!= (const  Ysbot::Time& src) const throw ()
 {
  return !operator==(src);
}

bool  Ysbot::Time::operator<= (const  Ysbot::Time& src) const throw () 
{
  return (diff_usec (src)<=0);
}

bool  Ysbot::Time::operator< (const  Ysbot::Time& src) const throw () 
{
  return (diff_usec (src)<0);
}

bool  Ysbot::Time::operator>= (const Ysbot::Time& src) const throw () 
{
  return (diff_usec (src)>=0);
}

bool  Ysbot::Time::operator> (const  Ysbot::Time& src) const throw () 
{
  return (diff_usec (src)>0);
}

long int  Ysbot::Time::get_usec () const throw () 
{
  return  (long int)((QPart1-Ysbot::Time::starting_time.QPart1)/ dfFreq*1e6);
}

long int Ysbot::Time::get_msec () const throw () 
{
  return (long int)((QPart1-Ysbot::Time::starting_time.QPart1)/ dfFreq*1e3);
}

long int  Ysbot::Time::get_sec () const throw () 
{
  return (long int)((QPart1-Ysbot::Time::starting_time.QPart1)/ dfFreq);
}

std::ostream& operator<< (std::ostream& os, const Ysbot::Time& tt) throw()
{
  os << tt.get_msec();
  return os;
} 