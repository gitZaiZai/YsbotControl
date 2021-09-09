
#ifndef _Tribots_geometry_h_
#define _Tribots_geometry_h_

#include "Vec.h"
#include <stdexcept>
#include <vector>
#include <iostream>
#include "Frame2D.h"

class Area;
class Curve;
class CurveAndArea;
class Line;
class LineSegment;
class Circle;
class MyArc;
class Triangle;
class XYRectangle;
class Quadrangle;
class Halfplane;
class UnionArea;

Line operator* (const Frame2d&, const Line&) throw ();
LineSegment operator* (const Frame2d&, const LineSegment&) throw ();
MyArc operator* (const Frame2d&, const MyArc&) throw ();
Circle operator* (const Frame2d&, const Circle&) throw ();
Triangle operator* (const Frame2d&, const Triangle&) throw ();
Quadrangle operator* (const Frame2d&, const Quadrangle&) throw ();
Halfplane operator* (const Frame2d&, const Halfplane&) throw ();

Vec intersect (const Line&, const Line&) throw (std::invalid_argument);
std::vector<Vec> intersect (const LineSegment&, const Line&) throw (std::bad_alloc);
std::vector<Vec> intersect (const Line&, const LineSegment&) throw (std::bad_alloc);
std::vector<Vec> intersect (const LineSegment&, const LineSegment&) throw (std::bad_alloc);
std::vector<Vec> intersect (const Line&, const Circle&) throw (std::bad_alloc);
inline std::vector<Vec> intersect (const Circle& c, const Line& l) throw (std::bad_alloc) { return intersect (l,c); }
std::vector<Vec> intersect (const LineSegment&, const Circle&) throw (std::bad_alloc);
inline std::vector<Vec> intersect (const Circle& c, const LineSegment& l) throw (std::bad_alloc) { return intersect (l,c); }
std::vector<Vec> intersect (const Circle&, const Circle&) throw (std::bad_alloc);
std::vector<Vec> intersect (const Line&, const MyArc&) throw (std::bad_alloc);
std::vector<Vec> intersect (const MyArc&, const Line&) throw (std::bad_alloc);
std::vector<Vec> intersect (const LineSegment&, const MyArc&) throw (std::bad_alloc);
std::vector<Vec> intersect (const MyArc&, const LineSegment&) throw (std::bad_alloc);
std::vector<Vec> intersect (const Circle&, const Quadrangle&) throw (std::bad_alloc);
inline std::vector<Vec> intersect (const Quadrangle& q, const Circle& c) throw (std::bad_alloc) { return intersect (c,q); }
std::vector<Vec> intersect (const LineSegment&, const Quadrangle&) throw (std::bad_alloc);
inline std::vector<Vec> intersect (const Quadrangle& q, const LineSegment& c) throw (std::bad_alloc) { return intersect (c,q); }
std::vector<Vec> intersect (const Line&, const Quadrangle&) throw (std::bad_alloc);
inline std::vector<Vec> intersect (const Quadrangle& q, const Line& c) throw (std::bad_alloc) { return intersect (c,q); }
std::vector<Vec> tangent_point (const Circle&, const Vec&) throw (std::bad_alloc, std::invalid_argument);


Vec intercept_theorem(const Vec& point, double b, double h);

class Area {
public:
	virtual ~Area() throw () {;}
	virtual bool is_inside (const Vec) const throw () =0;
	virtual void draw(std::ostream&) const throw () =0;
	virtual Area* clone() const =0;
};

class Curve {
public:
	virtual ~Curve() throw () {;}
	virtual Vec closest_point (const Vec) const throw () =0;
	virtual double distance (const Vec p) const throw () { return (p-closest_point (p)).length(); }
	virtual void draw(std::ostream&) const throw () =0;
};

class CurveAndArea : public Curve, public Area {
};

class Line : public Curve {
protected:
	friend Vec intersect (const Line&, const Line&) throw (std::invalid_argument);
	friend std::vector<Vec> intersect (const Line&, const Circle&) throw (std::bad_alloc);
	friend std::vector<Vec> intersect (const Line&, const MyArc&) throw (std::bad_alloc);
	friend Vec perpendicular_point (const Vec&, const Line&) throw ();
	friend Line operator* (const Frame2d&, const Line&) throw ();

public:
	Vec p1;    
	Vec p2;
	Line () throw ();
	Line (const Vec, const Vec) throw (std::invalid_argument);
	Line (const Line&) throw ();
	const Line& operator= (const Line&) throw ();
	~Line () throw () {;}

	double distance (const Vec) const throw ();
	int side (const Vec) throw ();
	Vec perpendicular_point (const Vec) const throw ();
	Vec closest_point (const Vec) const throw ();

	Line& s_rotate   (const Angle&) throw();
	Line& s_translate(const Vec&) throw();

	Line rotate (const Angle) const throw ();
	Line translate (const Vec) const throw ();

	void draw(std::ostream&) const throw ();
};


class LineSegment : public Curve {
	friend std::vector<Vec> intersect (const LineSegment&, const MyArc&) throw (std::bad_alloc);
	friend std::vector<Vec> intersect (const LineSegment&, const Line&) throw (std::bad_alloc);
	friend std::vector<Vec> intersect (const LineSegment&, const LineSegment&) throw (std::bad_alloc);
	friend LineSegment operator* (const Frame2d&, const LineSegment&) throw ();

public:
	Vec p1;   // Anfangspunkt
	Vec p2;   // Endpunkt

	LineSegment () throw ();
	LineSegment (const Vec, const Vec) throw (std::invalid_argument);
	LineSegment (const LineSegment&) throw ();
	const LineSegment& operator= (const LineSegment&) throw ();
	~LineSegment () throw () {;}

	Vec closest_point (const Vec) const throw ();

	const Vec& getStart() const throw();

	const Vec& getEnd() const throw();

	LineSegment& s_rotate   (const Angle&) throw();
	LineSegment& s_translate(const Vec&) throw();
	LineSegment rotate (const Angle) const throw ();
	LineSegment translate (const Vec) const throw ();

	void draw(std::ostream&) const throw ();
};


class MyArc : public Curve {
private:
	friend std::vector<Vec> intersect (const Line&, const MyArc&) throw (std::bad_alloc);
	friend std::vector<Vec> intersect (const LineSegment&, const MyArc&) throw (std::bad_alloc);
	friend MyArc operator* (const Frame2d&, const MyArc&) throw ();

public:
	Vec center;    
	double radius;   
	Angle start;   
	Angle end;   

	MyArc () throw ();
	MyArc (Vec, double, Angle, Angle) throw ();
	MyArc (const MyArc&) throw ();
	const MyArc& operator= (const MyArc&) throw ();
	~MyArc () throw () {;}

	MyArc& s_rotate   (const Angle&) throw();
	MyArc& s_translate(const Vec&) throw();

	MyArc rotate (const Angle) const throw ();
	MyArc translate (const Vec) const throw ();

	Vec closest_point (const Vec) const throw ();

	void draw(std::ostream&) const throw ();
};


class Circle : public CurveAndArea {
private:
	friend std::vector<Vec> intersect (const Line&, const Circle&) throw (std::bad_alloc);
	friend std::vector<Vec> intersect (const Circle&, const Circle&) throw (std::bad_alloc);
	friend std::vector<Vec> tangent_point (const Circle&, const Vec&) throw (std::bad_alloc, std::invalid_argument);
	friend Circle operator* (const Frame2d&, const Circle&) throw ();

public:
	Vec center;     ///< Mittelpunkt
	double radius;  ///< Radius

	Circle () throw ();
	Circle (const Vec, double) throw ();
	Circle (const Vec, const Vec, const Vec) throw (std::invalid_argument);
	Circle (const Circle&) throw ();
	const Circle& operator= (const Circle&) throw ();

	Area* clone() const {  return new Circle(*this); }

	const Vec& get_center () const throw ();
	double get_radius () const throw ();

	bool is_inside (const Vec) const throw ();  
	double distance (const Vec) const throw ();
	Vec closest_point (const Vec) const throw ();
	void draw(std::ostream&) const throw ();
};


class Triangle : public Area {
public:
	Triangle () throw ();
	Triangle (Vec,Vec,Vec) throw ();
	Triangle (const Triangle&) throw ();
	Area* clone() const { return new Triangle(*this); }
	bool is_inside (const Vec) const throw ();
	double area () const throw ();   
	void draw(std::ostream&) const throw ();
	Vec p1, p2, p3;

	friend Triangle operator* (const Frame2d&, const Triangle&) throw ();
};


class XYRectangle : public Area {
public:
	XYRectangle () throw ();
	XYRectangle (const XYRectangle&) throw ();
	XYRectangle (Vec,Vec) throw ();
	Area* clone() const { return new XYRectangle(*this); }
	bool is_inside (const Vec) const throw ();
	void draw(std::ostream&) const throw ();
	Vec p1;
	Vec p2;
};


class Quadrangle : public CurveAndArea {
public:
	Quadrangle () throw ();
	Quadrangle (const Quadrangle&) throw ();
	Quadrangle (Vec, Vec) throw ();
	Quadrangle (const Vec&, const Vec&, double) throw ();    
	Quadrangle (const Vec&, const Vec&, double,double) throw ();    
	Quadrangle (Vec, Vec, Vec, Vec) throw ();
	Area* clone() const { return new Quadrangle(*this); }
	bool is_inside (const Vec) const throw ();
	Vec closest_point (const Vec) const throw ();
	void draw(std::ostream&) const throw ();
	Vec p1, p2, p3, p4;

	friend Quadrangle operator* (const Frame2d&, const Quadrangle&) throw ();
}; 


class Halfplane : public Area {
public:
	Halfplane () throw ();
	Halfplane (const Halfplane&) throw ();
	Halfplane (Vec, Vec) throw ();
	Halfplane (Vec, Angle) throw ();
	Area* clone() const { return new Halfplane(*this); }
	bool is_inside (const Vec) const throw ();
	void draw(std::ostream&) const throw ();
private:
	Vec p1;
	Vec norm;

	friend Halfplane operator* (const Frame2d&, const Halfplane&) throw ();
};

class UnionArea : public Area {
public:
	UnionArea () throw ();
	virtual ~UnionArea() throw ();
	virtual void add (const Area&) throw ();  
	virtual bool is_inside (const Vec) const throw ();
	virtual void draw(std::ostream&) const throw ();   
	virtual Area* clone() const;
private:
	std::vector<Area*> elements;
};



std::ostream& operator<< (std::ostream&, const Curve&);
std::ostream& operator<< (std::ostream&, const Area&);
std::ostream& operator<< (std::ostream&, const CurveAndArea&);

#endif

