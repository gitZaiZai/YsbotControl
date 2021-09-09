#include "stdafx.h"
#include "CartPos.h"
#include "LuaScript/ScriptVariables.h"

/*****************************************************************************/
/********************** CLASS CartPos    *************************************/
/*****************************************************************************/

// CartPos::CartPos(const Matrix& mat,const int& _fg)
// {
// 	set_mat(mat);
// 	fig = _fg;
// }

// ReturnMatrix CartPos::toHomogeneousMat()
// {
// 	Matrix tor = pos.toTranslMat() * ang.toRotMat();
// 	return tor; 
// }
// 
// ReturnMatrix CartPos::getTranslMat()
// {
// 	return pos.toTranslMat();
// }
// 
// ReturnMatrix CartPos::getRotMat()
// {
// 	return ang.toRotMat();
// }

void CartPos::set_pos(double dX, double dY, double dZ, double dA, double dB, double dC, int _fig) throw()
{
	pos = Vec3D(dX,dY,dZ);
	ang = Vec3D(dA,dB,dC);
	fig = _fig;
}

char* CartPos::get_value( const char* name, char* strValue )
{
	check_value( name, false, strValue ); 
	return strValue;
}

void CartPos::set_value( const char* name, char* strValue)
{
	check_value(name,true,strValue);
}

bool CartPos::check_value( const char* name, const bool bset, char* val )
{
	bool res = false;

	double* dfpos[] = { &pos.x, &pos.y, &pos.z, 
		                &ang.x, &ang.y, &ang.z, 
		                NULL   };

	int datasize = ARRAY_SIZE(dfpos);

	char prop[VAR_STR_LEN];
	for ( int i = 0; i < VARINFO->vardesc[svCARTPOS]->getValuesTotal(); i++ )
	{
		if ( i >= datasize )
 			break;

		VARINFO->vardesc[svCARTPOS]->getValue(i+1,prop);
  		if ( prop[0] != '\0' && strstr( prop, name ) )
		{
			if ( bset )
			{
				if ( dfpos[i] )
				{
 					if ( strstr(prop,"ANGLE") )
						*dfpos[i] = deg2rad(atof( val ? val : 0 )) ;
					else
						*dfpos[i] = atof( val ? val : 0 );
				}
				else
				{
					if( !val )
						;
					else 
					  fig = atoi( val ? val : 0 );
				}
			}
			else
			{
				if ( dfpos[i] == NULL )
				{
					sprintf( val, "%d", fig );
				}
				else
					sprintf( val, "%lf", *dfpos[i] );
			}
			res = true;
			break;

		}
	}

	if ( !bset && !res ) // read failed.
		*val = '\0';

	return res;

}

// void CartPos::set_mat( const Matrix& mat )
// {
// 	if( mat.Ncols() == 4 && mat.Nrows() == 4 )
// 	{
// 		//Matrix rpym = mat.SubMatrix(1,3,1,3);
// 		ColumnVector rpyc = irpy(mat);
// 
// 		pos = Vec3D( mat(1,4), mat(2,4), mat(3,4) );
// 		ang = Vec3D( rpyc(1), rpyc(2), rpyc(3) ); // rpy(zyx)
// 
// 		fig = -1;
// 	}
// }

std::ostream& operator<< (std::ostream& os, const CartPos& v) {
	os << v.pos << ' ' << v.ang.toDeg() << ' ' << v.fig << ' ' ;
	return os;
}

std::ostream& operator<< (std::ostream& os, const CartRef& v) {
	os << v.cpos << ' ' << v.cartname << ' ' ;
	return os;
}

std::istream& operator>> (std::istream &in, CartPos &v)
{
	in >> v.pos.x >> v.pos.y >> v.pos.z >> 
		  v.ang.x >> v.ang.y >> v.ang.z >> 
		  v.fig;
	return in;
}

bool CartPos::operator== (const CartPos v) const throw () {
	return (pos==v.pos) && (ang==v.ang) && (fig==v.fig);
}

bool CartPos::operator!= (const CartPos v) const throw () {
	return (pos!=v.pos) || (ang!=v.ang) || (fig!=v.fig);
}

CartPos CartPos::operator+ (const CartPos v) const throw () {
	CartPos res (*this);
	res+=v;
	return res;
}

const CartPos& CartPos::operator+= (const CartPos v) throw () {
	pos+=v.pos;
	ang+=v.ang;
 	return *this;
}

CartPos CartPos::operator- (const CartPos v) const throw () {
	CartPos res (*this);
	res-=v;
	return res;
}

const CartPos& CartPos::operator-= (const CartPos v) throw () {
	pos-=v.pos;
	ang-=v.ang;
 	return *this;
}

CartPos CartPos::operator- () const throw () {
	return CartPos (-pos, -ang, fig);
}

const CartPos& CartPos::operator*= (double s) throw () {
	pos*=s;
	ang*=s;
 	return *this;
}

const CartPos& CartPos::operator/= (double s) throw () {
	pos/=s;
	ang/=s;
 	return *this;
}

double CartPos::operator* (const CartPos v) const throw () {
	return pos*v.pos+ang*v.ang;
}

CartPos operator* (CartPos v, double s) throw () {
	CartPos res (v);
	res*=s;
	return res;
}

CartPos operator* (double s, CartPos v) throw () {
	CartPos res (v);
	res*=s;
	return res;
}

CartPos operator/ (CartPos v, double s) throw () {
	CartPos res(v);
	res/=s;
	return res;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// class CartRef

CartRef::CartRef()
{
 	sprintf(cartname,"%s","base");
	sprintf(selfname,"%s","Base");
}

const CartRef& CartRef::operator= (const CartRef& cr) 
{
	sprintf(cartname,"%s",  cr.cartname);
	sprintf(selfname,"%s",  cr.selfname);
	cpos = cr.cpos;
	return *this;
}

char* CartRef::get_value( const char* name, char* strValue )
{
 	char prop[VAR_STR_LEN];
	cpos.get_value( name, strValue );
	if ( strValue[ 0 ] == '\0' ) // not cartpos data
	{
		int num = VARINFO->vardesc[svCARTREF]->getValuesTotal();
		VARINFO->vardesc[svCARTREF]->getValue( num, prop );
		if ( prop[0] != '\0' &&  strstr( prop, name ) ) 
 			sprintf( strValue, "%s", cartname );
 		else
			*strValue = '\0';
	}
	
	return strValue;
}

void CartRef::set_value( const char* name, char* strValue)
{
	bool ret = cpos.check_value( name, true, strValue );
 	if ( !ret )
 	{
		strcpy( cartname, strValue );
 	}
 	
}

 

