
namespace Frelon
{


class Camera
{

%TypeHeaderCode
#include "FrelonCamera.h"
#include "RegEx.h"
#include "MiscUtils.h"
#include <sstream>
%End

 public:
	Camera(Espia::SerialLine& espia_ser_line);
	~Camera();

	Frelon::SerialLine& getSerialLine();

	void writeRegister(Frelon::Reg reg, int  val);
	void readRegister (Frelon::Reg reg, int& val /Out/);

	void hardReset();
	void getVersion(std::string& ver /Out/);

	void getComplexSerialNb(int& complex_ser_nb /Out/);
	void getSerialNb(int& ser_nb /Out/);
	void isFrelon2k16(bool& is_frelon_2k16 /Out/);
	void isFrelon4M(bool& is_frelon_4m /Out/);
	void hasTaper(bool& has_taper /Out/);

	void setInputChan(Frelon::InputChan  input_chan);
	void getInputChan(Frelon::InputChan& input_chan /Out/);

	void setFrameTransferMode(Frelon::FrameTransferMode  ftm);
	void getFrameTransferMode(Frelon::FrameTransferMode& ftm /Out/);

	void getFrameDim(FrameDim& frame_dim /Out/);

	bool isChanActive(Frelon::InputChan curr, Frelon::InputChan chan);

	void checkFlip(Flip& flip /InOut/);
	void setFlip(const Flip& flip);
	void getFlip(Flip& flip /Out/);

	void checkBin(Bin& bin /InOut/);
	void setBin(const Bin& bin);
	void getBin(Bin& bin /Out/);

	void setRoiMode(Frelon::RoiMode  roi_mode);
	void getRoiMode(Frelon::RoiMode& roi_mode /Out/);

	void checkRoi(const Roi& set_roi, Roi& hw_roi /Out/);
	void setRoi(const Roi& set_roi);
	void getRoi(Roi& hw_roi /Out/);

	void setTrigMode(TrigMode  trig_mode);
	void getTrigMode(TrigMode& trig_mode /Out/);
	
	void setExpTime(double  exp_time);
	void getExpTime(double& exp_time /Out/);

	void setLatTime(double  lat_time);
	void getLatTime(double& lat_time /Out/);

	void setNbFrames(int  nb_frames);
	void getNbFrames(int& nb_frames /Out/);

	void getStatus(Frelon::Status& status /Out/);
	bool waitStatus(Frelon::Status& status /InOut/, double timeout = 0);

	void start();
	void stop();

 protected:
	virtual void setMaxImageSizeCallbackActive(bool cb_active);

};


}; // namespace Frelon
