#ifndef _random_h_
#define _random_h_

#include "Vec.h"
#include <cstdlib>
#include <cmath>

inline void random_seed (const unsigned int& s) throw () 
{
	srand(s); }

inline double urandom () throw () 
{
    return static_cast<double>( rand())/RAND_MAX; 
}

inline double urandom (const double& f, const double& t) throw () 
{
    return (t-f)*urandom ()+f; 
}

inline bool brandom (const double& p) throw ()
{
    return (urandom()<p); 
}

inline double nrandom () throw () 
{
    double u1 = urandom();
    double u2 = urandom();
	if (u1 <= 0) 
		u1 = 1e-50;
    return ( sqrt(-2.0* log(u1))*cos(6.2831853071*u2));
}

inline double nrandom (const double& mu, const double& sigma) throw () 
{
    return nrandom()*sigma+mu; 
}

inline Vec n2random () throw () 
{
    double u1 = urandom();
    double u2 = urandom();
	if (u1 <= 0) 
		u1 = 1e-50;
    double s =  sqrt(-2.0* log(u1));
    return Vec (s*cos(6.2831853071*u2), s*sin(6.2831853071*u2));
}

#endif
