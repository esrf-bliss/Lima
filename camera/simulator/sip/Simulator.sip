
class Simulator
{
%TypeHeaderCode
#include "Simulator.h"
#include <string>
#include <time.h>
#include <cmath>
%End

 public:
	enum Status {
		Ready, Exposure, Readout, Latency,
	};

	Simulator();
	~Simulator();

	BufferCtrlMgr& getBufferMgr();
	
	void startAcq();
	void stopAcq();

	void setNbFrames(int  nb_frames);
	void getNbFrames(int& nb_frames /Out/);

	void setExpTime(double  exp_time);
	void getExpTime(double& exp_time /Out/);

	void setLatTime(double  lat_time);
	void getLatTime(double& lat_time /Out/);

	void setBin(const Bin& bin);
	void getBin(Bin& bin /Out/);
	void checkBin(Bin& bin /InOut/);

	void setFrameDim(const FrameDim& frame_dim);
	void getFrameDim(FrameDim& frame_dim /Out/);
	
	Status getStatus();
	int getNbAcquiredFrames();

	void getMaxImageSize(Size& max_image_size /Out/);

	void reset();

};

