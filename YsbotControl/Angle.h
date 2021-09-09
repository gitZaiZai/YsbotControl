#ifndef Angle_h
#define Angle_h

#include "..\roboop\quaternion.h"
#include "..\roboop\robot.h"
#include "..\roboop\utils.h"
#include "..\newmat\precisio.h"

#ifndef M_PI
#define M_PI  3.1415926535897932384626433832795029
#endif

#define CONSTZWEIPI 6.283185307179586232
#define CONSTPI180 0.017453292519943295474
#define CONST180PI 57.295779513082322865

class Angle 
{
private:
	double the_angle;    
	public:

	Angle () throw () { the_angle = 0.;}

	Angle (double a) throw () {set_rad (a);}

	Angle (const Angle& a) throw () : the_angle(a.the_angle) {;}

	const Angle& operator= (const Angle& a) throw () { the_angle=a.the_angle; return *this; }


	bool operator== (const Angle&) const throw ();
	bool operator!= (const Angle&) const throw ();
	bool in_between (const Angle, const Angle) const throw ();

	void set_rad (double) throw ();
	double get_rad () const throw ();

	static Angle rad_angle (double) throw ();
	static Angle deg_angle (double) throw ();

	void set_deg (double) throw ();
	double get_deg () const throw ();

	double get_rad_pi () const throw ();
	double get_deg_180 () const throw ();

	Angle operator+ (const Angle) const throw ();
	const Angle& operator+= (const Angle) throw ();
	Angle operator- (const Angle) const throw ();
	const Angle& operator-= (const Angle) throw ();
	Angle operator- () const throw ();
	const Angle& operator*= (double) throw();

	static const Angle zero;           ///< 0 Grad
	static const Angle twelvth;        ///< 30 Grad
	static const Angle eighth;         ///< 45 Grad
	static const Angle sixth;          ///< 60 Grad
	static const Angle quarter;        ///< 90 Grad
	static const Angle three_eighth;   ///< 135 Grad
	static const Angle half;           ///< 180 Grad
	static const Angle five_eighth;    ///< 225 Grad
	static const Angle three_quarters; ///< 270 Grad
	static const Angle five_sixth;     ///< 300 Grad
	static const Angle seven_eighth;   ///< 315 Grad
	static const Angle eleven_twelvth; ///< 330 Grad
};
double valid_angle(double x, double min, double max);

Angle operator* (const Angle, double) throw ();
Angle operator* (double, const Angle) throw ();

std::ostream& operator<<(std::ostream& os, const Angle& li);

#endif