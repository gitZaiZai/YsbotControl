
#ifndef Vec3D_h
#define Vec3D_h
#include "Angle.h"
#include "Vec.h"
#include <iostream>
 
/** 3-dimensional vector
*
*  This implementation of an 3-Dimensional vector only provides the most basic
*  functionality - not all methods of the class Vec are available.
*
*  Most methods which involve mirroring or rotation only work on the x and y
*  dimensions (adapted from Vec.h) and leave the z component untouched.
*/

class Vec3D {

public:
	double x,y,z;     
	
	Vec3D () throw () : x(0.), y(0.), z(0.) {;}
	Vec3D (double x1, double y1, double z1) throw () : x(x1), y(y1), z(z1) {;}
	Vec3D (const Vec& v) throw () : x(v.x), y(v.y), z(0.0) {;}
	Vec3D (const Vec3D& v) throw () : x(v.x), y(v.y), z(v.z) {;}
	const Vec3D& operator= (const Vec& v) { x=v.x; y=v.y; z=0.0; return *this; }
	const Vec3D& operator= (const Vec3D& v) { x=v.x; y=v.y; z=v.z; return *this; }

	Vec toVec() const throw() { return Vec(x,y); } 
// 	ReturnMatrix toTranslMat() const throw();
// 	ReturnMatrix toRotMat() const throw();
	Vec3D toDeg () const throw ();        

	bool operator== (const Vec3D) const throw ();
	bool operator!= (const Vec3D) const throw ();


	Vec3D operator+ (const Vec3D) const throw ();
	const Vec3D& operator+= (const Vec3D) throw ();
	Vec3D operator- (const Vec3D) const throw ();
	const Vec3D& operator-= (const Vec3D) throw ();
	Vec3D operator- () const throw ();
	const Vec3D& operator*= (double) throw ();
	const Vec3D& operator/= (double) throw ();       // Division durch Null wird nicht abgefangen, ergibt nan
	double operator* (const Vec3D) const throw ();

	Vec3D crossProduct(const Vec3D) const throw ();
 
	Vec3D operator* (const Angle) const throw ();
	const Vec3D& operator*= (const Angle) throw ();
	Vec3D operator/ (const Angle) const throw ();
	const Vec3D& operator/= (const Angle) throw ();


	// Rotationen:
	Vec3D rotate (const Angle) const throw ();       ///< Rotation um beliebigen Winkel
	Vec3D rotate_twelvth () const throw ();          ///< Rotation um 30 Grad
	Vec3D rotate_eleven_twelvth () const throw ();   ///< Rotation um -30=330 Grad
	Vec3D rotate_eighth () const throw ();           ///< Rotation um 45 Grad
	Vec3D rotate_seven_eighth () const throw ();     ///< Rotation um -45=315 Grad
	Vec3D rotate_sixth () const throw ();            ///< Rotation um 60 Grad
	Vec3D rotate_five_sixth () const throw ();       ///< Rotation um -60=300 Grad
	Vec3D rotate_quarter () const throw ();          ///< Rotation um 90 Grad
	Vec3D rotate_three_quarters () const throw ();   ///< Rotation um -90 Grad
	Vec3D rotate_half () const throw ();             ///< Rotation um 180 Grad=Punktspiegelung am Ursprung

	Vec3D s_rotate (const Angle) throw ();           ///< Rotation von this


	// Spiegelungen:
	Vec3D mirror (const Vec3D) const throw ();       ///< Spiegelung an einer Achse mit gegebener Richtung wenn ||Arg1|| > 0, ansonsten Punktspiegelung 
	Vec3D mirror_x () const throw ();                ///< Spiegelung an x-Achse
	Vec3D mirror_y () const throw ();                ///< Spiegelung an y-Achse
	Vec3D mirror_z () const throw ();                ///< Spiegelung an z-Achse
	Vec3D mirror_eighth () const throw ();           ///< Spiegelung an 1. Winkelhalbierender
	Vec3D mirror_three_eighth () const throw ();     ///< Spiegelung an 2. Winkelhalbierender

	/** Quadrierte Laenge des Vektors */
	double squared_length () const throw ();
	/** Laenge des Vektors */
	double length () const throw ();
	/** Winkel des Vektors (0 fuer den Nullvektor) */
	Angle angle () const throw ();
	/** Normalisieren ((0,0) beim Nullvektor) */
	Vec3D normalize () const throw ();
	Vec3D normal_maxmin (double minval = -M_PI, double maxval = M_PI ) const throw ();

	Angle angle (const Vec3D) const throw ();

	static Vec3D unit_vector (Angle) throw ();

	static const Vec3D unit_vector_x;           ///< Einheitsvektor in x-Richtung
	static const Vec3D unit_vector_y;           ///< Einheitsvektor in y-Richtung
	static const Vec3D unit_vector_z;           ///< Einheitsvektor in z-Richtung
	static const Vec3D zero_vector;            

};

Vec3D operator* (Vec3D, double) throw ();
Vec3D operator* (double, Vec3D) throw ();
Vec3D operator/ (Vec3D, double) throw ();    

bool linearly_independent (const Vec3D, const Vec3D);
  

double vector_dot(const double *v1, const double *v2);
 

void vector_cross(const double *v1, const double *v2, double *result);
 

void vector_unitize(const double *v, double *result);
 

double round(double x);
 

void vector_sub(const double *v1, const double *v2, double *result);
 
inline void vector_add(const double *v1, const double *v2, double *result);
 
 

std::ostream& operator<< (std::ostream& os, const Vec3D& v);

#endif


