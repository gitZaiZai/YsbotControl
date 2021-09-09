#ifndef _vision_processing_h
#define _vision_processing_h

#include "ConfigReader.h"
#include "VisionCoordTransform.h"
#include "TcpSocket.h"
#include "ThreadTaskTypeBase.h"

class VisionProcessing : public ThreadTaskTypeBase
{
public:
	VisionProcessing (const ConfigReader&) throw ();
	virtual ~VisionProcessing () throw ();
  
	void process_vision();
	Vec  get_object_vispos();

	bool calibration_vision(const std::vector<Vec>&, const std::vector<CartPos>&);
	void save_calibration_file();

private:
	VisionCoordTransform* vtans;
	TcpSocket vistcp;

	unsigned int thread_task(ThreadTaskTypeBase*);

	int byte_to_long(char *pbch);
	int data_number;

	int vis_object_num;
    Vec vis_object_pos;
	double vis_object_ang;

	bool use_vision;

	std::string dist_file;
    bool use_marker;	 
};

#endif
