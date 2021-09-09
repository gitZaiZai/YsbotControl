#include "StdAfx.h"
#include "Frame2D.h"
#include <cmath>

Frame2d operator*(const Frame2d &f1, const Frame2d &f2) 
{
    Frame2d res;
    res.scale= f1.scale* f2.scale;
     
    res.n_x= f1.n_x*f2.n_x - f1.n_y*f2.n_y;
    res.n_y= f1.n_y*f2.n_x + f1.n_x*f2.n_y;
    res.p_x= f1.n_x*f2.p_x - f1.n_y*f2.p_y + f1.p_x;
    res.p_y= f1.n_y*f2.p_x + f1.n_x*f2.p_y + f1.p_y;
    return res;
}
  
  Vec operator*(const Frame2d &f1, const Vec &v)
  {
    Vec res;
    res.x= f1.n_x*v.x - f1.n_y*v.y + f1.p_x;
    res.y= f1.n_y*v.x + f1.n_x*v.y + f1.p_y;
    return res;
  }
  
  
  Frame2d::Frame2d()
  {
    scale= 1.0;
     
    n_x= 1.0;
    n_y= 0.0;
    p_x= 0.0;
    p_y= 0.0;
  }

  Frame2d::Frame2d (Vec p, Angle a) 
  {
    scale=1.0;
    set_angle (a);
    set_position (p);
  }

  Angle Frame2d::get_angle() const 
  {
    Vec vec(n_x,n_y);
    return vec.angle();
  }
  
  Vec Frame2d::get_heading() const 
  {
    return (Vec(n_x,n_y)).normalize();
  }
  
  void Frame2d::set_angle(const Angle& a) 
  {
    n_x= cos(a.get_rad()) * scale;
    n_y= sin(a.get_rad()) * scale;
    
    //f1.n_y= sqrt(1-f1.n_x*f1.n_x);
  }
  void Frame2d::set_angle(Vec v) 
  {
    double a=v.angle().get_rad();
    n_x= cos(a) * scale;
    n_y= sin(a) * scale;
    
    //f1.n_y= sqrt(1-f1.n_x*f1.n_x);
  }
  

void Frame2d::set_position(double x,double y) 
{
  p_x= x;
  p_y= y;
}

void Frame2d::set_position(Vec v)
 {
  p_x= v.x;
  p_y= v.y;
}

void Frame2d::set_scale(double s) 
{
  n_x /= scale;
  n_y /= scale;
  scale= s;


  n_x *= scale;
  n_y *= scale;
}

void Frame2d::invert() 
{
  double old_p_x= p_x;

  p_x= -n_x*p_x     - n_y*p_y;
  p_y=  n_y*old_p_x - n_x*p_y;
   
  n_y= -n_y;
  
  scale = 1/scale;
}

Frame2d Frame2d::Translation(double x, double y) 
{
  Frame2d f1;
  f1.set_position(x,y);
  return f1;
}

Frame2d Frame2d::Rotation(const Angle& a) 
{
  Frame2d f1;
  f1.set_angle(a);
  return f1;
}