
namespace Frelon
{


//class Interface;


class DetInfoCtrlObj : HwDetInfoCtrlObj
{

%TypeHeaderCode
#include "FrelonInterface.h"
#include <algorithm>
%End

 public:
	DetInfoCtrlObj(Frelon::Camera& cam);
	virtual ~DetInfoCtrlObj();

	virtual void getMaxImageSize(Size& max_image_size /Out/);
	virtual void getDetectorImageSize(Size& det_image_size /Out/);

	virtual void getDefImageType(ImageType& def_image_type /Out/);
	virtual void getCurrImageType(ImageType& curr_image_type /Out/);
	virtual void setCurrImageType(ImageType  curr_image_type);

	virtual void getPixelSize(double& pixel_size /Out/);
	virtual void getDetectorType(std::string& det_type /Out/);
	virtual void getDetectorModel(std::string& det_model /Out/);

 protected:
	virtual void setMaxImageSizeCallbackActive(bool cb_active);

};


class BufferCtrlObj : HwBufferCtrlObj
{

%TypeHeaderCode
#include "FrelonInterface.h"
#include <algorithm>
%End

 public:
	BufferCtrlObj(BufferCtrlMgr& buffer_mgr);
	virtual ~BufferCtrlObj();

	virtual void setFrameDim(const FrameDim& frame_dim);
	virtual void getFramedim(      FrameDim& frame_dim /Out/);

	virtual void setNbBuffers(int  nb_buffers);
	virtual void getNbBuffers(int& nb_buffers /Out/);

	virtual void setNbConcatFrames(int  nb_concat_frames);
	virtual void getNbConcatFrames(int& nb_concat_frames /Out/);

	virtual void setNbAccFrames(int  nb_acc_frames);
	virtual void getNbAccFrames(int& nb_acc_frames /Out/);

	virtual void getMaxNbBuffers(int& max_nb_buffers /Out/);

	virtual void *getBufferPtr(int buffer_nb, int concat_frame_nb = 0);
	virtual void *getFramePtr(int acq_frame_nb);

	virtual void getStartTimestamp(Timestamp& start_ts /Out/);
	virtual void getFrameInfo(int acq_frame_nb, HwFrameInfoType& info /Out/);

	virtual void   registerFrameCallback(HwFrameCallback& frame_cb);
	virtual void unregisterFrameCallback(HwFrameCallback& frame_cb);

};


class SyncCtrlObj : HwSyncCtrlObj
{

%TypeHeaderCode
#include "FrelonInterface.h"
#include <algorithm>
%End

 public:
	SyncCtrlObj(Espia::Acq& acq, Frelon::Camera& cam, 
	            Frelon::BufferCtrlObj& buffer_ctrl);
	virtual ~SyncCtrlObj();

	virtual void setTrigMode(TrigMode  trig_mode);
	virtual void getTrigMode(TrigMode& trig_mode /Out/);

	virtual void setExpTime(double  exp_time);
	virtual void getExpTime(double& exp_time /Out/);

	virtual void setLatTime(double  lat_time);
	virtual void getLatTime(double& lat_time /Out/);

	virtual void setNbHwFrames(int  nb_frames);
	virtual void getNbHwFrames(int& nb_frames /Out/);

	virtual void getValidRanges(ValidRangesType& valid_ranges /Out/);

};


class BinCtrlObj : HwBinCtrlObj
{

%TypeHeaderCode
#include "FrelonInterface.h"
#include <algorithm>
%End

 public:
	BinCtrlObj(Frelon::Camera& cam);
	virtual ~BinCtrlObj();

	virtual void setBin(const Bin& bin);
	virtual void getBin(Bin& bin /Out/);
	virtual void checkBin(Bin& bin /InOut/);
};


class RoiCtrlObj : HwRoiCtrlObj
{

%TypeHeaderCode
#include "FrelonInterface.h"
#include <algorithm>
%End

 public:
	RoiCtrlObj(Frelon::Camera& cam);
	virtual ~RoiCtrlObj();

	virtual void checkRoi(const Roi& set_roi, Roi& hw_roi /Out/);
	virtual void setRoi(const Roi& set_roi);
	virtual void getRoi(Roi& hw_roi /Out/);
};


class Interface : HwInterface
{

%TypeHeaderCode
#include "FrelonInterface.h"
#include <algorithm>
%End

 public:
	Interface(Espia::Acq& acq, BufferCtrlMgr& buffer_mgr, Frelon::Camera& cam);
	virtual ~Interface();

//	virtual const HwInterface::CapList& getCapList() const;
	virtual const std::vector<HwCap>& getCapList() const;

	virtual void reset(HwInterface::ResetLevel reset_level);
	virtual void prepareAcq();
	virtual void startAcq();
	virtual void stopAcq();
	virtual void getStatus(HwInterface::StatusType& status /Out/);
	virtual int getNbHwAcquiredFrames();
};


}; // namespace Frelon
