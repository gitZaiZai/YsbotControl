#ifndef CartPos_h
#define CartPos_h

#include "Angle.h"
#include "Vec.h"
#include "Vec3D.h"
#include "LuaScript/BasicScriptVariable.h"

#define  VAR_STR_LEN    (256)

class CartPos : public BasicScriptVariable 
{
public:
	Vec3D pos;                   
	Vec3D ang;   // heading          
	int   fig;

	CartPos () : pos(0,0,0), ang(0,0,0),fig(1) {;}
	CartPos (double dX, double dY, double dZ, double dA, double dB, double dC, int _fig = -1 ) : pos(dX,dY,dZ), ang(dA,dB,dC),fig(_fig) {;}

	CartPos (const CartPos& rl) { operator= (rl); }
	CartPos (const Vec3D& p, const Vec3D& h,  const int f =5 ) : pos(p), ang(h), fig(f) {; }
// 	CartPos (const Matrix& mat, const int& _fg = 5 );
	const CartPos& operator= (const CartPos& rl) 
	{
		pos=rl.pos;
		ang=rl.ang;
		fig=rl.fig;
		return *this;
	}

// 	ReturnMatrix  toHomogeneousMat();
// 	ReturnMatrix  getTranslMat();
// 	ReturnMatrix  getRotMat();
	void set_pos (double dX, double dY, double dZ, double dA, double dB, double dC, int _fig = -1 ) throw ();
// 	void set_mat(const Matrix& mat);
	char* get_value( const char* name, char* strValue);
	void  set_value( const char* name, char* strValue);
	bool  check_value( const char* name, const bool, char* );

	bool operator== (const CartPos) const throw ();
	bool operator!= (const CartPos) const throw ();


	CartPos operator+ (const CartPos) const throw ();
	const CartPos& operator+= (const CartPos) throw ();
	CartPos operator- (const CartPos) const throw ();
	const CartPos& operator-= (const CartPos) throw ();
	CartPos operator- () const throw ();
	const CartPos& operator*= (double) throw ();
	const CartPos& operator/= (double) throw ();        
	double operator* (const CartPos) const throw ();
	
    friend std::ostream& operator<< (std::ostream& os, const CartPos& v);
	friend std::istream& operator>> (std::istream &in, CartPos& v);

};

class CartRef : public BasicScriptVariable
{
public:
	CartRef(); 
    ~CartRef() {;}
   CartRef (const CartRef& cr) { operator= (cr); }
   const CartRef& operator= (const CartRef& cr); 
 
   CartPos cpos;
   char cartname[VAR_STR_LEN];
   char selfname[VAR_STR_LEN];

   void setName(const char* cname) { sprintf(cartname,"%s",  cname); }
   const char* getName() const { return cartname; }

   void setSelfName(const char* cname) { sprintf(selfname,"%s",  cname); }
   const char* getSelfName() const { return selfname; }

   char* get_value( const char* name, char* strValue);
   void  set_value( const char* name, char* strValue);

   friend std::ostream& operator<< (std::ostream& os, const CartRef& v);

};

class Tool : public CartRef
{
public:
	Tool():CartRef() {;}
	~Tool() {;}
};

struct VisionPos
{
	VisionPos():pos_known(false),simulator_vision(false) {;}
	VisionPos (const VisionPos& cr) { operator= (cr); }
	const VisionPos& operator= (const VisionPos& cr) 
	{
 		cpos      = cr.cpos;
		absPosRefWorkCoord = cr.absPosRefWorkCoord;
		pos_known = cr.pos_known;
		simulator_vision = cr.simulator_vision;
		return *this;
	}
	CartPos cpos;
	CartPos absPosRefWorkCoord;
	bool pos_known;
	bool simulator_vision;
};

CartPos operator* (CartPos, double) throw ();
CartPos operator* (double, CartPos) throw ();
CartPos operator/ (CartPos, double) throw ();    


#endif