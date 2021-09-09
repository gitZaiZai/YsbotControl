#include "StdAfx.h"
#include "Vec3D.h"
#include <cmath>

using std::atan2;
using std::sqrt;

#define CONSTWURZELDREIHALBE 0.86602540378443859659
#define CONSTWURZELZWEIHALBE 0.70710678118654757274

const Vec3D Vec3D::unit_vector_x (1,0,0);
const Vec3D Vec3D::unit_vector_y (0,1,0);
const Vec3D Vec3D::unit_vector_z (0,0,1);
const Vec3D Vec3D::zero_vector (0,0,0);

std::ostream& operator<< (std::ostream& os, const Vec3D& v) {
	os << v.x << ' ' << v.y << ' ' << v.z ;
	return os;
}

bool Vec3D::operator== (const Vec3D v) const throw () {
	return (x==v.x) && (y==v.y) && (z==v.z);
}

bool Vec3D::operator!= (const Vec3D v) const throw () {
	return (x!=v.x) || (y!=v.y) || (z!=v.z);
}

Vec3D Vec3D::operator+ (const Vec3D v) const throw () {
	Vec3D res (*this);
	res+=v;
	return res;
}

const Vec3D& Vec3D::operator+= (const Vec3D v) throw () {
	x+=v.x;
	y+=v.y;
	z+=v.z;
	return *this;
}

Vec3D Vec3D::operator- (const Vec3D v) const throw () {
	Vec3D res (*this);
	res-=v;
	return res;
}

const Vec3D& Vec3D::operator-= (const Vec3D v) throw () {
	x-=v.x;
	y-=v.y;
	z-=v.z;
	return *this;
}

Vec3D Vec3D::operator- () const throw () {
	return Vec3D (-x, -y, -z);
}

const Vec3D& Vec3D::operator*= (double s) throw () {
	x*=s;
	y*=s;
	z*=s;
	return *this;
}

const Vec3D& Vec3D::operator/= (double s) throw () {
	x/=s;
	y/=s;
	z/=s;
	return *this;
}

Vec3D Vec3D::crossProduct(const Vec3D v) const throw (){
	return Vec3D(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x);
}

Vec3D operator* (Vec3D v, double s) throw () {
	Vec3D res (v);
	res*=s;
	return res;
}

Vec3D operator* (double s, Vec3D v) throw () {
	Vec3D res (v);
	res*=s;
	return res;
}

Vec3D operator/ (Vec3D v, double s) throw () {
	Vec3D res(v);
	res/=s;
	return res;
}

double Vec3D::operator* (const Vec3D v) const throw () {
	return x*v.x+y*v.y+z*v.z;
}

Vec3D Vec3D::operator* (const Angle a) const throw () {
	double sina = sin(a.get_rad());
	double cosa = cos(a.get_rad());
	return Vec3D (x*cosa-y*sina, x*sina+y*cosa, z);
}  

const Vec3D& Vec3D::operator*= (const Angle a) throw () {
	(*this) = operator* (a);
	return (*this);
}  

Vec3D Vec3D::operator/ (const Angle a) const throw () {
	double sina = sin(a.get_rad());
	double cosa = cos(a.get_rad());
	return Vec3D (x*cosa+y*sina, -x*sina+y*cosa, z);
}  

const Vec3D& Vec3D::operator/= (const Angle a) throw () {
	(*this) = operator/(a);
	return (*this);
}

Vec3D Vec3D::rotate (const Angle a) const throw () {
	double sina = sin(a.get_rad());
	double cosa = cos(a.get_rad());
	return Vec3D (x*cosa-y*sina, x*sina+y*cosa, z);
}
Vec3D Vec3D::s_rotate (const Angle a) throw () {
	(*this) = rotate (a);
	return (*this);
}
Vec3D Vec3D::rotate_twelvth () const throw () {
	return Vec3D (x*CONSTWURZELDREIHALBE-y*0.5, x*0.5+y*CONSTWURZELDREIHALBE, z);
}
Vec3D Vec3D::rotate_eleven_twelvth () const throw () {
	return Vec3D (x*CONSTWURZELDREIHALBE+y*0.5, -0.5*x+y*CONSTWURZELDREIHALBE, z);
}
Vec3D Vec3D::rotate_eighth () const throw () {
	return Vec3D (CONSTWURZELZWEIHALBE*(x-y), CONSTWURZELZWEIHALBE*(x+y), z);
}
Vec3D Vec3D::rotate_seven_eighth () const throw () {
	return Vec3D (CONSTWURZELZWEIHALBE*(x+y), CONSTWURZELZWEIHALBE*(y-x), z);
}
Vec3D Vec3D::rotate_sixth () const throw () {
	return Vec3D (0.5*x-y*CONSTWURZELDREIHALBE, x*CONSTWURZELDREIHALBE+y*0.5, z);
}
Vec3D Vec3D::rotate_five_sixth () const throw () {
	return Vec3D (0.5*x+y*CONSTWURZELDREIHALBE, -x*CONSTWURZELDREIHALBE+y*0.5, z);
}
Vec3D Vec3D::rotate_quarter () const throw () {
	return Vec3D (-y,x, z);
}
Vec3D Vec3D::rotate_three_quarters () const throw () {
	return Vec3D (y,-x, z);
}
Vec3D Vec3D::rotate_half () const throw () {
	return Vec3D (-x,-y, z);
}

Vec3D Vec3D::mirror (const Vec3D v) const throw () {
	double n=v.x*v.x+v.y*v.y+v.z*v.z;
	n = (n <= 0 ? 1 : n);   // fuer den Fall, in dem v==0 ist --> Punktspiegelung
	double f =2*(v.x*x+v.y*y+v.z*z)/n;  // Martin, bitte kontrollieren, ob z gewuenscht
	return Vec3D (f*v.x-x, f*v.y-y, f*v.z-z);
}
Vec3D Vec3D::mirror_x () const throw () {
	return Vec3D (x,-y,z);
}
Vec3D Vec3D::mirror_y () const throw () {
	return Vec3D (-x,y,z);
}
Vec3D Vec3D::mirror_z () const throw () {
	return Vec3D (-x,-y,z);
}
Vec3D Vec3D::mirror_eighth () const throw () {
	return Vec3D (y,x,z);
}
Vec3D Vec3D::mirror_three_eighth () const throw () {
	return Vec3D (-y,-x, z);
}

double Vec3D::squared_length () const throw () {
	return x*x+y*y+z*z;
}

double Vec3D::length () const throw () {
	return sqrt(x*x+y*y+z*z);
}

Angle Vec3D::angle () const throw () {
	return atan2 (y,x);
}

Angle Vec3D::angle (const Vec3D v) const throw () {
	double z = (x*v.x+y*v.y)/sqrt((x*x+y*y)*(v.x*v.x+v.y*v.y));  // normiertes Skalarprodukt
	double phi = atan2 (sqrt(1-z*z),z);  // Pythagoras
	return Angle ( -y*v.x+x*v.y >= 0 ? phi : -phi);  // Skalarprodukt mit Orthogonalvektor
}

Vec3D Vec3D::unit_vector (Angle a) throw () {
	return Vec3D (cos(a.get_rad()), sin(a.get_rad()), 0.0);
}

bool linearly_independent (const Vec3D v1, const Vec3D v2) {
	// Can one vector be scaled to the other?
	return (v1.x*v2.y-v1.y*v2.x!=0 || v1.x*v2.z-v1.z*v2.x!=0);
}

Vec3D Vec3D::normalize () const throw () {
	double len = length();
	if (len==0)
		return Vec3D (0,0,0);
	return (1.0/len)*(*this);
}

Vec3D Vec3D::normal_maxmin (double minval, double maxval) const throw () {
	Vec3D vmaxmin;
	vmaxmin.x = valid_angle((*this).x,minval, maxval);
	vmaxmin.y = valid_angle((*this).y,minval, maxval);
	vmaxmin.z = valid_angle((*this).z,minval, maxval);
	return vmaxmin;
}

// ReturnMatrix Vec3D::toTranslMat() const throw() 
// {
//  	ColumnVector pos(3); 
// 	Real org[] = {x,y,z};
// 	pos << org;
// 	return trans( pos );
// }
// 
// ReturnMatrix Vec3D::toRotMat() const throw()
// {
// 	ColumnVector ang(3); 
// 	ang(1) = x;
// 	ang(2) = y;
// 	ang(3) = z;
//  
// 	return rpy( ang );
// }

Vec3D Vec3D::toDeg() const throw()
{
	return Vec3D(x*CONST180PI,y*CONST180PI,z*CONST180PI);
}

double vector_dot(const double *v1, const double *v2)
{
	return (v1[0] * v2[0]) + (v1[1] * v2[1]) + (v1[2] * v2[2]);
}

void vector_cross(const double *v1, const double *v2, double *result)
{
	double  r[3];
	r[0] = v1[1] * v2[2] - v1[2] * v2[1];
	r[1] = v1[2] * v2[0] - v1[0] * v2[2];
	r[2] = v1[0] * v2[1] - v1[1] * v2[0];
	memcpy(result, r, sizeof(double) * 3);
}

void vector_unitize(const double *v, double *result)
{
	double  vector_length = sqrt( vector_dot(v, v) );
	ASSERT( vector_length > 0 ); // error

	result[0] = v[0] / vector_length;
	result[1] = v[1] / vector_length;
	result[2] = v[2] / vector_length;
}

double round(double x)
{
	if( x >= 0 )
	{
		return ((x - long(x)) >= 0.5) ? ceil(x): floor(x);
	}
	else
	{
		return ((x - long(x)) <= 0.5) ? floor(x): ceil(x);
	}
}

void vector_sub(const double *v1, const double *v2, double *result)
{
	result[0] = v1[0] - v2[0];
	result[1] = v1[1] - v2[1];
	result[2] = v1[2] - v2[2];
}

inline void vector_add(const double *v1, const double *v2, double *result)
{
	result[0] = v1[0] + v2[0];
	result[1] = v1[1] + v2[1];
	result[2] = v1[2] + v2[2];
}
