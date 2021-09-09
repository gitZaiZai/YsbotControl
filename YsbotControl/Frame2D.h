#ifndef _FRAME2D_
#define _FRAME2D_

#include "Vec.h"
#include "iostream"
#include "Angle.h"

class Frame2d
{
  friend Frame2d operator*(const Frame2d &, const Frame2d &);
  friend Vec operator*(const Frame2d &, const Vec &);
public:
  Frame2d(); //init as identity frame
  Frame2d(Vec, Angle);
  static Frame2d Translation(double, double);
  static Frame2d Rotation(const Angle&);
  Angle get_angle() const;
  Vec get_heading() const;

  Vec get_x_axis() const { return Vec(n_x,n_y); }
  Vec get_y_axis() const { return Vec(-n_y,n_x); }
  Vec get_pos() const { return Vec(p_x,p_y); }
  void set_angle(const Angle&);
  void set_angle(Vec);
  void set_position(double,double);
  void set_position(Vec);
  void set_scale(double,double);
  void set_scale(double);

  double get_scale() const { return scale; }

  void invert();
  double n_x, n_y, p_x, p_y;
 
private:
	double scale;
};

#endif