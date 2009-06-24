#ifndef FRELONCAMERA_H
#define FRELONCAMERA_H

#include "FrelonSerialLine.h"

namespace lima
{

namespace Frelon
{

class Camera
{
 public:
	Camera(Espia::SerialLine& espia_ser_line);

	SerialLine& getSerialLine();

	void writeRegister(Reg reg, int  val);
	void readRegister (Reg reg, int& val);

	void hardReset();
	void getVersion(std::string& ver);

	void getComplexSerialNb(int& complex_ser_nb);
	void getCameraType(int& type);

	void setInputChan(InputChan  input_chan);
	void getInputChan(InputChan& input_chan);

	void setFrameTransferMode(FrameTransferMode  ftm);
	void getFrameTransferMode(FrameTransferMode& ftm);

	void getFrameDim(FrameDim& frame_dim);

	bool isChanActive(InputChan chan);

	void setFlip(const Point& flip);
	void getFlip(Point& flip);

	void setBin(const Bin& bin);
	void getBin(Bin& bin);

	void setRoiMode(RoiMode  roi_mode);
	void getRoiMode(RoiMode& roi_mode);

	void checkRoi(const Roi& set_roi, Roi& hw_roi);
	void setRoi(const Roi& set_roi);
	void getRoi(Roi& hw_roi);

	void setTriggerMode(TrigMode  trig_mode);
	void getTriggerMode(TrigMode& trig_mode);
	
	void setExpTime(double  exp_time);
	void getExpTime(double& exp_time);

	void setLatTime(double  lat_time);
	void getLatTime(double& lat_time);

	void setNbFrames(int  nb_frames);
	void getNbFrames(int& nb_frames);

	void start();
	void stop();

 private:
	static const double HorzBinSleepTime;

	void sendCmd(Cmd cmd);
	void getSerialNbParam(SerNbParam param, int& val);

	void setChanMode(int  chan_mode);
	void getChanMode(int& chan_mode);
	
	void getBaseChanMode(FrameTransferMode ftm, int& base_chan_mode);
	void getInputChanMode(FrameTransferMode ftm, InputChan input_chan,
			      int& chan_mode);

	void setFlipMode(int  flip_mode);
	void getFlipMode(int& flip_mode);

	void getMirror(Point& mirror);
	void getNbChan(Point& nb_chan);
	void getCcdSize(Size& ccd_size);
	void getChanSize(Size& chan_size);
	void xformChanCoords(const Point& point, Point& chan_point, 
			     Corner& ref_corner);
	void getImageRoi(const Roi& chan_roi, Roi& image_roi);
	void getFinalRoi(const Roi& image_roi, const Point& roi_offset,
			 Roi& final_roi);
	void getChanRoi(const Roi& image_roi, Roi& chan_roi);
	void getImageRoiOffset(const Roi& req_roi, const Roi& image_roi,
			       Point& roi_offset);
        void checkRoiMode(const Roi& roi);
	void processSetRoi(const Roi& req_roi, Roi& hw_roi, Roi& chan_roi, 
			   Point& roi_offset);

	void setTimeUnitFactor(TimeUnitFactor  time_unit_factor);
	void getTimeUnitFactor(TimeUnitFactor& time_unit_factor);

	SerialLine m_ser_line;
	Point m_roi_offset;
	TrigMode m_trig_mode;
	int m_nb_frames;
};

inline bool Camera::isChanActive(InputChan chan)
{
	InputChan curr_chan;
	getInputChan(curr_chan);
	return (curr_chan & chan) == chan;
};



} // namespace Frelon

} // namespace lima


#endif // FRELONCAMERA_H
