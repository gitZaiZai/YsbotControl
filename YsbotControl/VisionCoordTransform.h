#ifndef _VisionCoordTransform_h_
#define _VisionCoordTransform_h_

#include <vector>
#include "CartPos.h"

class VisionCoordTransform
{
public:
	VisionCoordTransform () throw ();
	VisionCoordTransform (std::string filename) throw ();
	VisionCoordTransform (const Matrix& homo ) throw ();
	virtual ~VisionCoordTransform () throw ();

	bool vision_calibration(const std::vector<Vec>& vision_pos, const std::vector<CartPos>& robot_pos);
	const CartPos& vision_mapto_world( const Vec& visvec );

    void load(std::string filename);
    void save(std::string filename) const;

private:
	void init_homotrans();

 	Matrix homography;
	CartPos visobject_pos;
	CartPos measure_point;
};

#endif
