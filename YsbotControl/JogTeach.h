#ifndef jog_teach_h
#define jog_teach_h
#include "epciobase.h"

namespace Mtc
{

typedef struct _CONE
{
	double  pt_start[3];   // wrist center start point
	double  vec_center[3]; // center vector
	double  vec_start[3];  // start vector
	double  vec_normal[3]; // normal vector
	double  vec_U[3];
	double  theta;
	double  radius;
	Matrix  mat_prev;
	Matrix  mat_post;

	_CONE()
	{
		mat_prev.resize(3, 3);
		mat_post.resize(3, 3);
	}

} CONE;

class JogTeach
{
public:
	JogTeach(const ConfigReader&,  EpcioBase&);
	~JogTeach(void);

	void  set_teach_speed(double extVel);
 	int   grp_jog( const CmdInfo& );
	void  DriverArmAlongXYBase( int* nDir, unsigned int nAxis );
	void  DriverArmAlongTool( int* nDir, unsigned int nAxis );
 
protected:
private:
	EpcioBase& epcio;

};

int	 rbt_RX_V6(	double dfTheta, WORD wGroupIndex = 0);
int	 rbt_RY_V6(	double dfTheta, WORD wGroupIndex = 0);
int	 rbt_RZ_V6(	double dfTheta, WORD wGroupIndex = 0);
int	 rbt_Cone_V6(double dfBaseVectX,
		double dfBaseVectY,
		double dfBaseVectZ,
		double dfTheta,
		WORD wGroupIndex /*=0*/);

}

#endif