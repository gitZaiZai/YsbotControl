#include "stdafx.h"
#include "VisionProcessing.h"
#include "WorldModel.h"

VisionProcessing::VisionProcessing(const ConfigReader& vread) throw() : ThreadTaskTypeBase("vision process"), vtans(NULL),data_number(4),
	vis_object_num(0),vis_object_ang(0),use_vision(false),dist_file(""),
	use_marker(false)
{
 	vread.get("Vision::use_vision", use_vision );
	if ( use_vision )
	{
		vector<double> params;
		vector<Vec> vis;
		vector<CartPos> rob;
		char label[20];
 	
		std::string img2world_type = "";
		if ( vread.get("Vision::image2world_type", img2world_type ) > 0 )
		{
			if ( img2world_type == "Transform" )
			{
				std::string home_path = vread.getenv();
				dist_file = home_path + "config_files/" + "coord_transform.cfg";
				vtans = new VisionCoordTransform(dist_file.c_str());
		    } 
			else if ( img2world_type == "Marker")
			{
				vtans = new VisionCoordTransform();

				for( int i =0; i < 3; i++ )
				{
					sprintf(label,"Vision::marker_%d",i+1);
					if ( vread.get(label, params) && params.size()>=9) 
					{
						vis.push_back(Vec(params[0],params[1]));
						rob.push_back(CartPos(params[2],params[3],params[4],
							deg2rad(params[5]),deg2rad(params[6]),deg2rad(params[7]), 
							static_cast<int>(params[8]) ) );
					}	
					params.clear();
				}
				use_marker = true;
				vtans->vision_calibration(vis,rob);
 			}
			else
				vtans = new VisionCoordTransform();

		}

 		int port = 8000;
		vread.get("Vision::port", port );

		vistcp.init_as_server(port);

 		start_task_thread();

		vis.clear();
		rob.clear();
    } 
}
 
VisionProcessing::~VisionProcessing() throw()
{
	if ( vtans )
	{
		delete vtans;
		vtans = NULL;
	}
}

unsigned int VisionProcessing::thread_task( ThreadTaskTypeBase* pParam)
{
	VisionProcessing *pvis = ( VisionProcessing* )pParam;

	Ysbot::Time current_time;
	const long int loop_time = 200;

	char recvBuf[100];  
	memset(recvBuf, 0, sizeof(recvBuf));  
 	unsigned int bchSTX[] = {0xa1,0x01,0x01};

	const int total_bytes = pvis->data_number * 4 + 5;
	unsigned int len;

	while( !exitFlag )
	{
		memset( recvBuf, '\0', sizeof(recvBuf) );  
		if ( pvis->vistcp.receive( recvBuf, len, 100 ) ) 
		{
			if ( len == total_bytes &&
				(bchSTX[0] == static_cast<unsigned int>(static_cast<unsigned char>(*(recvBuf+0)))) &&
				(bchSTX[1] == static_cast<unsigned int>(static_cast<unsigned char>(*(recvBuf+1)))) &&
				(bchSTX[2] == static_cast<unsigned int>(static_cast<unsigned char>(*(recvBuf+2)))) ) // a1 01 01 **** **** ****  02 01
			{
				int total = pvis->byte_to_long(recvBuf+3);
				int posx  = pvis->byte_to_long(recvBuf+7);
				int posy  = pvis->byte_to_long(recvBuf+11);
				int ang   = pvis->byte_to_long(recvBuf+15);
				
				Lock();
				pvis->vis_object_num  = total;
				pvis->vis_object_pos  = Vec( static_cast<double>(posx), static_cast<double>(posy) ) * 0.01;
 				pvis->vis_object_ang  = static_cast<double>(ang) * 0.01;
 				UnLock();
				static int ii = 0;
//  				cout << "vision normal 1-" << ii++ << endl;
 			}
			else
			{
				Lock();
				pvis->vis_object_num  = 0;
				pvis->vis_object_pos  = Vec::zero_vector;
				pvis->vis_object_ang  = 0;
				UnLock();
				cout << "vision normal2\n";
 			}
		}
		else
			cout << "vision receive failed!\n";

		Sleep(1); // important for another thread to read variable.
	}
 
 	return 0;
}

int VisionProcessing::byte_to_long(char *pbch)
{
	int iValue = 0;
	int ihigh = ( static_cast<unsigned char>(*(pbch+0)) << 8 ) + static_cast<unsigned char>(*(pbch+1));
	int ilow  = ( static_cast<unsigned char>(*(pbch+2)) << 8 ) + static_cast<unsigned char>(*(pbch+3));

	iValue = MAKELONG(ilow,ihigh); //(ihigh << 16) | ilow; 	
	return iValue;
}

void VisionProcessing::process_vision()
{
	VisionPos objpos;
	if ( use_vision )
	{
		objpos.simulator_vision = false;
// 		Lock();
		int total_num  = vis_object_num;
		Vec vpos   = vis_object_pos;
		double ang = vis_object_ang;
//  		UnLock();
		static int dd = 0;
		if ( total_num == 0 )
		{
			objpos.pos_known = false;
  		}
		else
		{
			objpos.pos_known = true;
			if ( vtans )
			{
				const CartPos& cp = vtans->vision_mapto_world( vpos );
				objpos.cpos = cp;
				if ( !use_marker )
				{
					objpos.cpos.ang.x = M_PI;
					objpos.cpos.ang.y = 0;
					objpos.cpos.fig   = 5;
				}
				objpos.cpos.ang.z = valid_angle( M_PI - ( deg2rad(ang)), M_PI, -M_PI );
					
			}

			objpos.absPosRefWorkCoord.pos = vpos;
			objpos.absPosRefWorkCoord.ang = Vec3D(deg2rad(ang),0,0); // zyz
 		}
// 		cout << " vis dest = " << total_num << " robot = "<< vpos << " known = " << objpos.pos_known  << dd++ << '\n';
	}
	else
	{
		objpos.pos_known = false;
		objpos.simulator_vision = true;
	}

	if ( MWM.is_main_world_model_available() )
	{
		MWM.set_vision_pos(objpos);
	}
}

Vec VisionProcessing::get_object_vispos()
{
	return vis_object_pos;
}

bool VisionProcessing::calibration_vision(const std::vector<Vec>& vis, const std::vector<CartPos>& rob)
{
	if ( vtans )
	{
		return vtans->vision_calibration(vis,rob);
	}
	return false;
}

void VisionProcessing::save_calibration_file()
{
	if ( vtans )
	{
		vtans->save(dist_file);
	}
}