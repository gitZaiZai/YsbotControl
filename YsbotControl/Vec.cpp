#include "StdAfx.h"
#include "Vec.h"
#include <cmath>
  
//using std::atan2;
//using std::sqrt;

#define CONSTWURZELDREIHALBE 0.86602540378443859659
#define CONSTWURZELZWEIHALBE 0.70710678118654757274

const Vec Vec::unit_vector_x (1,0);
const Vec Vec::unit_vector_y (0,1);
const Vec Vec::zero_vector (0,0);

Vec::Vec(double x1, double y1,CoordSystemT cs/* =CARTESIAN */)
{
	if( cs == CARTESIAN )
	{
		x = x1;
		y = y1;
	}
	else
		*this = getVecPositionFromPolar(x1,Angle::rad_angle(y1));
}

std::ostream& operator<< (std::ostream& os, const Vec& v)
{
  os <<' '<< v.x << ' ' << v.y<<' ';
  return os;
}

bool Vec::operator== (const Vec v) const throw () 
{
  return (x==v.x) && (y==v.y);
}

bool Vec::operator!= (const Vec v) const throw () 
{
  return (x!=v.x) || (y!=v.y);
}

Vec Vec::operator+ (const Vec v) const throw () 
{
  Vec res (*this);
  res+=v;
  return res;
}

const Vec& Vec::operator+= (const Vec v) throw () 
{
  x+=v.x;
  y+=v.y;
  return *this;
}

Vec Vec::operator- (const Vec v) const throw () 
{
  Vec res (*this);
  res-=v;
  return res;
}

const Vec& Vec::operator-= (const Vec v) throw () 
{
  x-=v.x;
  y-=v.y;
  return *this;
}

Vec Vec::operator- () const throw () 
{
  return Vec (-x, -y);
}

const Vec& Vec::operator*= (double s) throw () 
{
  x*=s;
  y*=s;
  return *this;
}

const Vec& Vec::operator/= (double s) throw () 
{
  x/=s;
  y/=s;
  return *this;
}

Vec operator* (Vec v, double s) throw () 
{
  Vec res (v);
  res*=s;
  return res;
}

Vec operator* (double s, Vec v) throw () 
{
  Vec res (v);
  res*=s;
  return res;
}

Vec operator/ (Vec v, double s) throw () 
{
  Vec res(v);
  res/=s;
  return res;
}

double Vec::operator* (const Vec v) const throw () 
{
  return x*v.x+y*v.y;
}

Vec Vec::operator* (const Angle a) const throw () 
{
  double sina = sin(a.get_rad());
  double cosa = cos(a.get_rad());
  return Vec (x*cosa-y*sina, x*sina+y*cosa);
}  

const Vec& Vec::operator*= (const Angle a) throw () 
{
  (*this) = operator* (a);
  return (*this);
}  

Vec Vec::operator/ (const Angle a) const throw () 
{
  double sina = sin(a.get_rad());
  double cosa = cos(a.get_rad());
  return Vec (x*cosa+y*sina, -x*sina+y*cosa);
}  

const Vec& Vec::operator/= (const Angle a) throw () 
{
  (*this) = operator/(a);
  return (*this);
}

Vec Vec::rotate (const Angle a) const throw ()
 {
  double sina = sin(a.get_rad());
  double cosa = cos(a.get_rad());
  return Vec (x*cosa-y*sina, x*sina+y*cosa);
}
Vec Vec::s_rotate (const Angle a) throw () 
{
  (*this) = rotate (a);
  return (*this);
}
Vec Vec::rotate_twelvth () const throw () 
{
  return Vec (x*CONSTWURZELDREIHALBE-y*0.5, x*0.5+y*CONSTWURZELDREIHALBE);
}
Vec Vec::rotate_eleven_twelvth () const throw () 
{
  return Vec (x*CONSTWURZELDREIHALBE+y*0.5, -0.5*x+y*CONSTWURZELDREIHALBE);
}
Vec Vec::rotate_eighth () const throw () 
{
  return Vec (CONSTWURZELZWEIHALBE*(x-y), CONSTWURZELZWEIHALBE*(x+y));
}
Vec Vec::rotate_seven_eighth () const throw () 
{
  return Vec (CONSTWURZELZWEIHALBE*(x+y), CONSTWURZELZWEIHALBE*(y-x));
}
Vec Vec::rotate_sixth () const throw () 
{
  return Vec (0.5*x-y*CONSTWURZELDREIHALBE, x*CONSTWURZELDREIHALBE+y*0.5);
}
Vec Vec::rotate_five_sixth () const throw () 
{
  return Vec (0.5*x+y*CONSTWURZELDREIHALBE, -x*CONSTWURZELDREIHALBE+y*0.5);
}
Vec Vec::rotate_quarter () const throw () 
{
  return Vec (-y,x);
}
Vec Vec::rotate_three_quarters () const throw () 
{
  return Vec (y,-x);
}
Vec Vec::rotate_half () const throw () 
{
  return Vec (-x,-y);
}

Vec Vec::mirror (const Vec v) const throw () 
{
  double n=v.x*v.x+v.y*v.y;
  n = (n <= 0 ? 1 : n);   
  double f =2*(v.x*x+v.y*y)/n;
  return Vec (f*v.x-x, f*v.y-y);
}
Vec Vec::mirror_x () const throw () 
{
  return Vec (x,-y);
}
Vec Vec::mirror_y () const throw () 
{
  return Vec (-x,y);
}
Vec Vec::mirror_eighth () const throw () 
{
  return Vec (y,x);
}
Vec Vec::mirror_three_eighth () const throw () 
{
  return Vec (-y,-x);
}
 
double Vec::squared_length () const throw () 
{
  return x*x+y*y;
}

double Vec::length () const throw () 
{
  return sqrt(x*x+y*y);
}

Angle Vec::angle() const throw ()  // lrd edit
{
  /* if (x == 0.0 && y == 0.0 )
   {
	   return 0.0;
   }
   double tmp = atan2 (y,x);
   if (tmp < 0.0)
   {
	   return tmp+2*M_PI;
   }
   else
      return tmp;*/
   return atan2 (y,x);
}

Angle Vec::angle (const Vec v) const throw () // lrd edit
{
   double z = (x*v.x+y*v.y)/sqrt((x*x+y*y)*(v.x*v.x+v.y*v.y));   
   double z1=1-(z*z);
   if (z1<0) z1=0;   
   double phi = atan2 (sqrt(z1),z);   
   return Angle ( -y*v.x+x*v.y >= 0 ? phi : -phi);   
}

Vec Vec::unit_vector (Angle a) throw () 
{
  return Vec (cos(a.get_rad()), sin(a.get_rad()));
}

Vec Vec::getVecPositionFromPolar(double dMag, Angle dAng )
{
	double cosang = cos(dAng.get_rad());
	double sinang = sin(dAng.get_rad());
	return ( Vec( cosang * dMag, sinang * dMag ) );
}

bool linearly_independent (const Vec v1, const Vec v2) 
{
  return (v1.x*v2.y-v1.y*v2.x!=0);
}

Vec Vec::normalize() const throw () 
{
  double len = length();
  if (len==0)
    return Vec (0,0);
  return (1.0/len)*(*this);
}

Vec Vec::normalize(double l)
{
  double len = length();
  if (len==0)
    return Vec (0,0);
  return (l/len)*(*this);
}

Vec Vec::globalToRelative(Vec origin, Angle ang)
{
	*this = *this - origin;
	return (rotate_ang(-ang.get_rad()));
}

Vec Vec::relativeToGlobal(Vec origin, Angle ang)
{
	rotate_ang(ang.get_rad());
	*this = *this + origin;
    return (*this);
}

double Vec::getDistanceTo(const Vec p) const
{
	return ( ( *this - p ).length() );
}

double Vec::sqr_distance(const Vec& p) const
{
	return ( ( *this - p ).squared_length() );
}

Angle Vec::normalize_angle(const Angle& a) const
{
	double tmp = fmod(a.get_rad(),2*M_PI);
	if (tmp < 0.0)
	{
		return tmp + 2*M_PI;
	}
	else
	{
		return tmp;
	}
}

Vec Vec::rotate_ang(double a)  throw ()
{
	double dMag    = this->length();
	double dNewDir = this->angle().get_rad() + a;
	double cosang = cos(dNewDir);
	double sinang = sin(dNewDir);
 	*this = Vec( cosang * dMag, sinang * dMag );
 	return (*this);
}