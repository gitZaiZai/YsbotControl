#include "StdAfx.h"
#include "Angle.h"
#include <cmath>

const Angle Angle::zero (0);
const Angle Angle::twelvth (M_PI/6);
const Angle Angle::eighth (M_PI/4);
const Angle Angle::sixth (M_PI/3);
const Angle Angle::quarter (M_PI/2);
const Angle Angle::three_eighth (3*M_PI/4);
const Angle Angle::half (M_PI);
const Angle Angle::five_eighth (5*M_PI/4);
const Angle Angle::three_quarters (1.5*M_PI);
const Angle Angle::five_sixth (5*M_PI/3);
const Angle Angle::seven_eighth (7*M_PI/4);
const Angle Angle::eleven_twelvth (11*M_PI/6);

bool Angle::operator== (const Angle& a) const throw () 
{
  return the_angle==a.the_angle;
}

bool Angle::operator!= (const Angle& a) const throw () 
{
  return the_angle!=a.the_angle;
}

bool Angle::in_between (const Angle a, const Angle b) const throw () 
{
  if (b.the_angle>=a.the_angle)
    return (a.the_angle<=the_angle) && (the_angle<=b.the_angle);
  else
    return (a.the_angle<=the_angle) || (the_angle<=b.the_angle);
}

void Angle::set_rad (double a) throw () 
{
  if (a<0) 
  {
    double k = ceil (-a/(CONSTZWEIPI));
    the_angle = a+k*CONSTZWEIPI;
    return;
  }
  if (a>=CONSTZWEIPI) 
  {
    double k = floor (a/(CONSTZWEIPI));
    the_angle = a-k*CONSTZWEIPI;
    return;
  }
  the_angle=a;
}

Angle Angle::rad_angle (double d) throw () 
{
  Angle res (d);
  return res;
}

Angle Angle::deg_angle (double d) throw () 
{
  Angle res;
  res.set_deg (d);
  return res;
}

void Angle::set_deg (double a) throw () 
{
  set_rad (a*CONSTPI180);
}

double Angle::get_rad () const throw () 
{
  return the_angle;
}

double Angle::get_deg () const throw () 
{
  return the_angle*CONST180PI;
}

double Angle::get_rad_pi () const throw () 
{
  return the_angle - (the_angle<M_PI ? 0.0: CONSTZWEIPI);
}

double Angle::get_deg_180 () const throw () 
{
  return get_deg() - (the_angle<M_PI ? 0.0: 360.0);
}

Angle Angle::operator+ (const Angle a) const throw () 
{
  Angle res (*this);
  res+=a;
  return res;
}

const Angle& Angle::operator+= (const Angle a) throw () 
{
  the_angle+=a.the_angle;
  if (the_angle>=CONSTZWEIPI)
    the_angle-=CONSTZWEIPI;
  return *this;
}

Angle Angle::operator- (const Angle a) const throw () 
{
  Angle res (*this);
  res-=a;
  return res;
}

const Angle& Angle::operator-= (const Angle a) throw () 
{
  the_angle-=a.the_angle;
  if (the_angle<0)
    the_angle+=CONSTZWEIPI;
  return *this;
}

Angle Angle::operator- () const throw () 
{
  return Angle (CONSTZWEIPI-the_angle);
}

const Angle& Angle::operator*= (double s) throw () 
{
  set_rad (s*get_rad_pi());
  return *this;
}

Angle operator* (const Angle a, double s) throw () 
{
  Angle res (a);
  res*=s;
  return res;
}

Angle operator* (double s, const Angle a) throw ()
{
  Angle res (a);
  res*=s;
  return res;
}

std::ostream& operator<<(std::ostream& os, const Angle& li)
{
	os << ' ' << li.get_deg() << ' '; return os;
}

double valid_angle(double x, double min, double max)
{
	double  modulus = max - min;
	x -= (modulus * floor(x/modulus));
	x = fmod(x - min, modulus) + min;
	return ( fabs(x - min) >= FloatingPointPrecision::Epsilon() ) ? x: max;
}