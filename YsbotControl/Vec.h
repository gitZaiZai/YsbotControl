#ifndef Vec_h
#define Vec_h
#include "Angle.h"
#include <iostream>
 
enum CoordSystemT 
{
  CARTESIAN,
  POLAR
};

class Vec
{ 
public:
	double x;    
	double y;     

	Vec () throw () { x=y=0.;}
	Vec (double x1, double y1,CoordSystemT cs=CARTESIAN) throw ();
	Vec (const Vec& v) throw () : x(v.x), y(v.y) {;}
	const Vec& operator= (const Vec& v) { x=v.x; y=v.y; return *this; }

	bool operator== (const Vec) const throw ();
	bool operator!= (const Vec) const throw ();

	Vec operator+ (const Vec) const throw ();
	const Vec& operator+= (const Vec) throw ();
	Vec operator- (const Vec) const throw ();
	const Vec& operator-= (const Vec) throw ();
	Vec operator- () const throw ();
	const Vec& operator*= (double) throw ();
	const Vec& operator/= (double) throw ();    
	double operator* (const Vec) const throw ();

	Vec operator* (const Angle) const throw ();
	const Vec& operator*= (const Angle) throw ();
	Vec operator/ (const Angle) const throw ();
	const Vec& operator/= (const Angle) throw ();

	Vec rotate (const Angle) const throw ();     // rotation around arbitrary angle      
	Vec rotate_twelvth () const throw ();        // rotation around 30 degrees      
	Vec rotate_eleven_twelvth () const throw (); // rotation around -30 degrees   
	Vec rotate_eighth () const throw ();         // rotation around 45 degrees       
	Vec rotate_seven_eighth () const throw ();   // rotation around -45 degrees      
	Vec rotate_sixth () const throw ();          // rotation around 60 degrees    
	Vec rotate_five_sixth () const throw ();     // rotation around -60 degrees   
	Vec rotate_quarter () const throw ();        // rotation around 90 degrees   
	Vec rotate_three_quarters () const throw (); // rotation around -90 degrees 
	Vec rotate_half () const throw ();           // rotation around 180 degrees   

	Vec s_rotate (const Angle) throw ();           

	Vec mirror (const Vec) const throw ();          
	Vec mirror_x () const throw ();                 
	Vec mirror_y () const throw ();                
	Vec mirror_eighth () const throw ();           
	Vec mirror_three_eighth () const throw ();      
         
	double squared_length () const throw ();
	double length () const throw ();
	/* angles of the vector(0 more fuer the zero-vector */
	Angle angle () const throw ();
	Vec normalize () const throw ();
    
	/* gives the angle between two Vector (nan with zero-vetor) */
	Angle angle (const Vec) const throw ();

	static Vec unit_vector (Angle) throw ();

    static Vec getVecPositionFromPolar(double dMag, Angle dAng );
    Vec		   globalToRelative(Vec origin, Angle ang);
	Vec        relativeToGlobal(Vec origin, Angle ang);
    double	   getDistanceTo(const Vec p) const;
	double     sqr_distance(const Vec& p)  const;
	Vec        normalize(double l);
	Angle      normalize_angle(const Angle& a) const;
    Vec        rotate_ang(double a)  throw ();

	static const Vec unit_vector_x;            
	static const Vec unit_vector_y;            
	static const Vec zero_vector;             

};

Vec operator* (Vec, double) throw ();
Vec operator* (double, Vec) throw ();
Vec operator/ (Vec, double) throw ();     

bool linearly_independent (const Vec, const Vec);
   
   

std::ostream& operator<< (std::ostream& os, const Vec& v);

#endif
