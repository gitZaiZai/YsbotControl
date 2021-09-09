#include "stdafx.h"
#include "VisionCoordTransform.h"
#include "YsibotsException.h"

VisionCoordTransform::VisionCoordTransform() throw()
{
	init_homotrans();
}

VisionCoordTransform::VisionCoordTransform(std::string filename) throw()
{
	init_homotrans();
	load(filename);
// 	("test"+image_handler+".lut").c_str()
	std::string bakname = filename+".bak";
	save(bakname);
}

VisionCoordTransform::VisionCoordTransform( const Matrix& homo )throw()
{
	init_homotrans();
	homography = homo;
}

VisionCoordTransform::~VisionCoordTransform() throw()
{
	;
}

void VisionCoordTransform::init_homotrans()
{
	homography.ReSize(4,4);
	homography << fourbyfourident;
}

bool VisionCoordTransform::vision_calibration( const std::vector<Vec>& vision_pos, const std::vector<CartPos>& robot_pos )
{
	if( vision_pos.size() != 3 || robot_pos.size() != 3 )
	{
		return false;
	}

	Matrix  image_point(4,4);
	Matrix  world_point(4,4);
	 
	image_point = 0;
 	world_point = 0;

	for ( int i = 0; i < 3; i++ )
	{
		image_point(1,i+1) = vision_pos[i].x;
		image_point(2,i+1) = vision_pos[i].y;
		image_point(3,i+1) = 0.0;
		image_point(4,i+1) = 1.0;

		world_point(1,i+1) = robot_pos[i].pos.x;
		world_point(2,i+1) = robot_pos[i].pos.y;
		world_point(3,i+1) = robot_pos[i].pos.z;
		world_point(4,i+1) = 1.0;
	}

	image_point(1,4) = 0.0;
	image_point(2,4) = 0.0;
	image_point(3,4) = 1.0;
	image_point(4,4) = 0.0;

	world_point(1,4) = 0.0;
	world_point(2,4) = 0.0;
	world_point(3,4) = 1.0;
	world_point(4,4) = 0.0;

	Matrix inv_image = pinv(image_point);
	homography = world_point * inv_image;

	// 求正交 A = N × O 矢量积
	double nx,ny,nz; 
	double ox,oy,oz;
	double ax,ay,az;
	double norm = 0;
	nx = homography(1,1);
	ny = homography(2,1);
	nz = homography(3,1);
	ox = homography(1,2);
	oy = homography(2,2);
	oz = homography(3,2);
	ax = ny * oz - nz * oy;
	ay = nz * ox - nx * oz;
	az = nx * oy - ny * ox;
	norm = -1 * sqrt( ax*ax + ay*ay + az*az ); // left hand coordinate rule  reference Denso  by lrd
	if( norm == 0 )
	{
		homography(1,3) = 0;
		homography(2,3) = 0;
		homography(3,3) = 1;
	}
	else
	{
		homography(1,3) = ax / norm; // normalize vector
		homography(2,3) = ay / norm;
		homography(3,3) = az / norm;
	}

	measure_point = robot_pos[0];

	cout << homography << endl;

	return true;
}

const CartPos& VisionCoordTransform::vision_mapto_world( const Vec& visvec )
{
	double img_point_vec[] = { visvec.x, visvec.y, 0 ,1 };
	Matrix image_point(4,1);
	image_point << img_point_vec;
 
	Matrix world_point(4,1);
	world_point = 0;

	world_point = homography * image_point;

	visobject_pos = measure_point;

	visobject_pos.pos.x = world_point(1,1);
	visobject_pos.pos.y = world_point(2,1);
    visobject_pos.pos.z = world_point(3,1);

	return visobject_pos;
}

void VisionCoordTransform::save(std::string filename) const
{
	ofstream out (filename.c_str());
	if (!out) 
	{
		throw YsbotsException(__FILE__ ": Could not write vision coordinate transform to disk.");
	}

	out << homography << '\n';

	out.close();    
}

void VisionCoordTransform::load(std::string filename)
{
	ifstream in (filename.c_str());
	if (!in) 
	{
		throw YsbotsException(__FILE__ ": Could not read vision coordinate transform from disk.");
	}
	double elementval[16] = {0};
	int i = 0;
	do 
	{
		in >> elementval[i++];
 		if (in.fail() || in.eof()) break;
   	} 
	while ( !(in.fail() || in.eof()) );
 	homography << elementval; 
	in.close();  

	cout << "load from file\n";
	cout << homography;
}