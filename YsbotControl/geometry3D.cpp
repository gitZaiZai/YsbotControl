#include "StdAfx.h"
#include "geometry3D.h"
#include <cmath>

 
using namespace std;

namespace {

  inline double teilverhaeltnis (const Vec3D& v1, const Vec3D& v2, const Vec3D& p) {
    if (v1.x!=v2.x)
      return (p.x-v1.x)/(v2.x-v1.x);
    else if(v1.y!=v2.y)
      return (p.y-v1.y)/(v2.y-v1.y);
    else 
      return (p.z-v1.z)/(v2.z-v1.z);
  }

}


std::ostream& operator<< (std::ostream& os, const Line3D& v) {
  os << v.p1 << " --> " << v.p2;
  return os;
}

Line3D::Line3D (const Vec3D pp1, const Vec3D pp2) throw (std::invalid_argument)
                        : p1(pp1), p2(pp2) {
  if (pp1==pp2)
    throw std::invalid_argument ("invalid 3D-line definition in Line3D::Line3D");
}

//Line3D::Line3D (const Line& ln) throw () : p1(Vec3D(ln.p1)), p2(Vec3D(ln.p2)) {;}

Line3D::Line3D (const Line3D& ln) throw () : p1(ln.p1), p2(ln.p2) {;}

Line3D::Line3D () throw () : p1(Vec3D(0.,0.,0.)), p2(Vec3D(0.,0.,0.))
{;}

const Line3D& Line3D::operator= (const Line3D& ln) throw () {
  p1=ln.p1;
  p2=ln.p2;
  return (*this);
}

Vec3D
Line3D::intersectZPlane(const double z) const{
   double s = (z-p1.z)/(p2.z-p1.z);
   return p1+s*(p2-p1);
}

Vec3D
Line3D::intersectYPlane(const double y) const{
   double s = (y-p1.y)/(p2.y-p1.y);
   return p1+s*(p2-p1);
}


double Line3D::distance (const Vec3D p) throw () {
  Vec3D d = p2-p1;
  Vec3D l=  p1+((p*d-p1*d)/(d.squared_length()))*d;
  return (p-l).length();
}


Line3D&
Line3D::s_rotate (const Angle& a) throw () 
{
  p1=p1.rotate(a); p2=p2.rotate(a);  
  return *this;
}

Line3D&
Line3D::s_translate (const Vec3D& v) throw () 
{
  p1+=v; p2+=v;                      
  return *this;
}



LineSegment3D&
LineSegment3D::s_rotate (const Angle& a) throw () 
{
  p1=p1.rotate(a); p2=p2.rotate(a); 
  return *this;
}

LineSegment3D&
LineSegment3D::s_translate (const Vec3D& v) throw () 
{
  p1+=v; p2+=v;                     
  return *this;
}

Line3D Line3D::rotate (const Angle a) const throw () {
  Line3D dest (*this);
  dest.s_rotate (a);
  return dest;
}

Line3D Line3D::translate (const Vec3D v) const throw () {
  Line3D dest (*this);
  dest.s_translate (v);
  return dest;
}

Vec3D intersectSkewLines(Line3D& l1, Line3D& l2)
{
  return intersectSkewLines(l1.p1,l1.p2-l1.p1,l2.p1,l2.p2-l2.p1);
}

Vec3D intersectSkewLines(Vec3D L1Start, Vec3D L1Rel, Vec3D L2Start, Vec3D L2Rel)
{
  // see http://fermi.jhuapl.edu/s1r/idl/s1rlib/vectors/v_skew.html

  // U1 = (B-A)/| B-A |
  Vec3D U1 = L1Rel.normalize();
  // U2 = (D-C)/| D-C |
  Vec3D U2 = L2Rel.normalize();
  // U3 = (U1 cross U2)/| U1 cross U2 |
  Vec3D U3 = U1.crossProduct(U2);
  U3 = U3.normalize();

  // E = ((A-C) dot U3) U3
  Vec3D E = ((L1Start-L2Start) * U3) * U3;

  // 'virtual L1' line which is coplanar with L2
  // (we only need starting point, since line practically has no end)
  Vec3D CDash = L2Start + E;
  
  // Unit vector U4 perpendicular to Line 2': 
  //  V = A - C' 
  // V_perp = V - (V dot U2) U2 
  // U4 = V_perp/| V_perp |
  Vec3D V = L1Start - CDash;
  Vec3D U4 = V - ((V * U2) * U2);
  U4 = U4.normalize();
  
  Vec3D IDash = intersect(Line3D(L1Start,L1Start+L1Rel),Line3D(CDash,CDash+L2Rel));
  Vec3D I = IDash - (E / 2.0);

  return(I);
}

Vec3D intersect (const Line3D& ln1, const Line3D& ln2)
    throw (std::invalid_argument) {

  // Adapted from Mathworld, http://mathworld.wolfram.com/Line-LineIntersection.html
  
  Vec3D a = ln1.p2 - ln1.p1;
  Vec3D b = ln2.p2 - ln2.p1;
  Vec3D c = ln2.p1 - ln1.p1;  
  Vec3D x = ln1.p1 + a*((c.crossProduct(b))*(a.crossProduct(b))/a.crossProduct(b).squared_length());
  

//  if(x.x !=nan && x.y != nan&& x.z != nan)
  return x;
//  else
//     throw std::invalid_argument("3Dlines do not intersect in intersect");
    
}


Vec3D Line3D::perpendicular_point (const Vec3D& p) throw () {
  Vec3D d=p2-p1;
  return p1+((p*d-p1*d)/(d.squared_length()))*d;
}



 