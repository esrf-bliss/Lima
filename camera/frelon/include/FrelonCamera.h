#ifndef FRELONCAMERA_H
#define FRELONCAMERA_H

#include "FrelonSerialLine.h"
#include "FrelonModel.h"
#include "HwMaxImageSizeCallback.h"

namespace lima
{

namespace Frelon
{

class Camera : public HwMaxImageSizeCallbackGen
{
	DEB_CLASS_NAMESPC(DebModCamera, "Camera", "Frelon");

 public:
	Camera(Espia::SerialLine& espia_ser_line);
	~Camera();

	SerialLine& getSerialLine();

	void writeRegister(Reg reg, int  val);
	void readRegister (Reg reg, int& val);

	void hardReset();
	void getVersion(std::string& ver);
	void getComplexSerialNb(int& complex_ser_nb);
	Model& getModel();

	void setInputChan(InputChan  input_chan);
	void getInputChan(InputChan& input_chan);

	void setFrameTransferMode(FrameTransferMode  ftm);
	void getFrameTransferMode(FrameTransferMode& ftm);

	void getFrameDim(FrameDim& frame_dim);

	bool isChanActive(InputChan curr, InputChan chan);

	void checkFlip(Flip& flip);
	void setFlip(const Flip& flip);
	void getFlip(Flip& flip);

	void checkBin(Bin& bin);
	void setBin(const Bin& bin);
	void getBin(Bin& bin);

	void setRoiMode(RoiMode  roi_mode);
	void getRoiMode(RoiMode& roi_mode);

	void checkRoi(const Roi& set_roi, Roi& hw_roi);
	void setRoi(const Roi& set_roi);
	void getRoi(Roi& hw_roi);

	void setRoiBinOffset(const Point& roi_bin_offset);
	void getRoiBinOffset(Point& roi_bin_offset);

	void setTrigMode(TrigMode  trig_mode);
	void getTrigMode(TrigMode& trig_mode);
	
	void setExpTime(double  exp_time);
	void getExpTime(double& exp_time);

	void setShutCloseTime(double  shut_time);
	void getShutCloseTime(double& shut_time);

	void setLatTime(double  lat_time);
	void getLatTime(double& lat_time);

	void setNbFrames(int  nb_frames);
	void getNbFrames(int& nb_frames);

	void getStatus(Status& status);
	bool waitStatus(Status& status, double timeout = 0);

	void start();
	void stop();

 protected:
	virtual void setMaxImageSizeCallbackActive(bool cb_active);

 private:
	static const double BinChangeTime;
	static const double MaxReadoutTime;

	Espia::Dev& getEspiaDev();

	void sync();

	void sendCmd(Cmd cmd);

	void setChanMode(int  chan_mode);
	void getChanMode(int& chan_mode);
	
	void getBaseChanMode(FrameTransferMode ftm, int& base_chan_mode);
	void getInputChanMode(FrameTransferMode ftm, InputChan input_chan,
			      int& chan_mode);

	void setFlipMode(int  flip_mode);
	void getFlipMode(int& flip_mode);

	Flip  getMirror();
	Point getNbChan();
	Size  getCcdSize();
	Size  getChanSize();
        Flip  getRoiInsideMirror();

	void writeChanRoi(const Roi& chan_roi);
	void readChanRoi(Roi& chan_roi);

	void xformChanCoords(const Point& point, Point& chan_point, 
			     Corner& ref_corner);
	void calcImageRoi(const Roi& chan_roi, const Flip& roi_inside_mirror,
			  Roi& image_roi, Point& roi_bin_offset);
	void calcFinalRoi(const Roi& image_roi, const Point& roi_offset,
			  Roi& final_roi);
	void calcChanRoi(const Roi& image_roi, Roi& chan_roi,
			 Flip& roi_inside_mirror);
	void calcImageRoiOffset(const Roi& req_roi, const Roi& image_roi,
				Point& roi_offset);
        void checkRoiMode(const Roi& roi);
	void processSetRoi(const Roi& req_roi, Roi& hw_roi, Roi& chan_roi, 
			   Point& roi_offset);


	void setTimeUnitFactor(TimeUnitFactor  time_unit_factor);
	void getTimeUnitFactor(TimeUnitFactor& time_unit_factor);

	SerialLine m_ser_line;
	Model m_model;
	Point m_roi_offset;
	TrigMode m_trig_mode;
	int m_nb_frames;
	bool m_mis_cb_act;
};

inline bool Camera::isChanActive(InputChan curr, InputChan chan)
{
	return (curr & chan) == chan;
};



} // namespace Frelon

} // namespace lima


#endif // FRELONCAMERA_H
