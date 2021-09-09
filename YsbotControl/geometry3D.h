
#ifndef _geometry_3D_h_
#define _geometry_3D_h_

#include "Vec.h"
#include "geometry.h"
#include "Vec3D.h"
#include <stdexcept>
#include <vector>


class Line3D;
class LineSegment3D;


Vec3D intersectSkewLines(Vec3D L1Start, Vec3D L1Rel, Vec3D L2Start, Vec3D L2Rel);
Vec3D intersectSkewLines(Line3D& l1, Line3D& l2);

Vec3D intersect (const Line3D&, const Line3D&) throw (std::invalid_argument);
std::vector<Vec3D> intersect (const LineSegment3D&, const Line3D&) throw (std::bad_alloc);
std::vector<Vec3D> intersect (const Line3D&, const LineSegment3D&) throw (std::bad_alloc);
std::vector<Vec3D> intersect (const LineSegment3D&, const LineSegment3D&) throw (std::bad_alloc);

class Line3D {
protected:
	friend Vec3D intersect (const Line3D&, const Line3D&) throw (std::invalid_argument);
	friend Vec3D perpendicular_point (const Vec3D&, const Line3D&) throw ();

public:
	Vec3D p1;   // p1, p2: 2 Punkte auf der Linie (nicht identisch!)
	Vec3D p2;

	Line3D (const Vec3D, const Vec3D) throw (std::invalid_argument);
	Line3D (const Line&) throw ();
	Line3D (const Line3D&) throw ();
	Line3D () throw();
	const Line3D& operator= (const Line3D&) throw ();

	Vec3D intersectZPlane(const double z) const;


	Vec3D intersectYPlane(const double y) const;

	double distance (const Vec3D) throw ();

	Vec3D perpendicular_point (const Vec3D&) throw ();

	/** Rotation um Ursprung (um Z-Achse)*/
	Line3D& s_rotate   (const Angle&) throw();
	/** Translation */
	Line3D& s_translate(const Vec3D&) throw();

	/** Rotation um Ursprung (um z-Achse)*/
	Line3D rotate (const Angle) const throw ();
	/** Translation */
	Line3D translate (const Vec3D) const throw ();
};

class LineSegment3D {
	friend std::vector<Vec3D> intersect (const LineSegment3D&, const Line3D&) throw (std::bad_alloc);
	friend std::vector<Vec3D> intersect (const LineSegment3D&, const LineSegment3D&) throw (std::bad_alloc);

public:
	Vec3D p1;   // Anfangspunkt
	Vec3D p2;   // Endpunkt

	LineSegment3D (const Vec3D, const Vec3D) throw (std::invalid_argument);
 	LineSegment3D (const LineSegment&) throw ();
 	LineSegment3D (const LineSegment3D&) throw ();
 	const LineSegment3D& operator= (const LineSegment3D&) throw ();

 	double distance (const Vec3D) throw ();

 	const Vec3D& getStart() const throw();

 	const Vec3D& getEnd() const throw();

 	LineSegment3D& s_rotate   (const Angle&) throw();
 	LineSegment3D& s_translate(const Vec3D&) throw();
 	LineSegment3D rotate (const Angle) const throw ();
 	LineSegment3D translate (const Vec3D) const throw ();
};    


std::ostream& operator<< (std::ostream& os, const Line3D& v);

#endif





