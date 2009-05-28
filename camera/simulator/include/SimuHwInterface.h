#ifndef SIMUHWINTERFACE_H
#define SIMUHWINTERFACE_H

#include "HwInterface.h"
#include "Simulator.h"

namespace lima
{

class SimuHwInterface;

/*******************************************************************
 * \class SimuDetInfoCtrlObj
 * \brief Control object providing simulator detector info interface
 *******************************************************************/

class SimuDetInfoCtrlObj : public HwDetInfoCtrlObj
{
 public:
	SimuDetInfoCtrlObj(Simulator& simu);
	virtual ~SimuDetInfoCtrlObj();

	virtual void getMaxImageSize(Size& max_image_size);
	virtual void getDetectorImageSize(Size& det_image_size);

	virtual void getDefImageType(ImageType& def_image_type);
	virtual void getCurrImageType(ImageType& curr_image_type);
	virtual void setCurrImageType(ImageType  curr_image_type);

	virtual void getPixelSize(double& pixel_size);
	virtual void getDetectorType(std::string& det_type);
	virtual void getDetectorModel(std::string& det_model);

 protected:
	virtual void setMaxImageSizeCallbackActive(bool cb_active);

 private:
	Simulator& m_simu;
	bool m_iscb_act;
};


/*******************************************************************
 * \class SimuBufferCtrlObj
 * \brief Control object providing simulator buffering interface
 *******************************************************************/

class SimuBufferCtrlObj : public HwBufferCtrlObj
{
 public:
	SimuBufferCtrlObj(Simulator& simu);
	virtual ~SimuBufferCtrlObj();

	virtual void setFrameDim(const FrameDim& frame_dim);
	virtual void getFramedim(      FrameDim& frame_dim);

	virtual void setNbBuffers(int  nb_buffers);
	virtual void getNbBuffers(int& nb_buffers);

	virtual void setNbConcatFrames(int  nb_concat_frames);
	virtual void getNbConcatFrames(int& nb_concat_frames);

	virtual void setNbAccFrames(int  nb_acc_frames);
	virtual void getNbAccFrames(int& nb_acc_frames);

	virtual void getMaxNbBuffers(int& max_nb_buffers);

	virtual void *getBufferPtr(int buffer_nb);
	virtual void *getFramePtr(int acq_frame_nb);

	virtual void getStartTimestamp(Timestamp& start_ts);
	virtual void getFrameInfo(int acq_frame_nb, HwFrameInfoType& info);

	virtual void   registerFrameCallback(HwFrameCallback *frame_cb);
	virtual void unregisterFrameCallback(HwFrameCallback *frame_cb);

 private:
	Simulator& m_simu;
	BufferCtrlMgr& m_buffer_mgr;
};


/*******************************************************************
 * \class SimuSyncCtrlObj
 * \brief Control object providing simulator synchronization interface
 *******************************************************************/

class SimuSyncCtrlObj : public HwSyncCtrlObj
{
 public:
	SimuSyncCtrlObj(Simulator& simu);
	virtual ~SimuSyncCtrlObj();

	virtual void setTrigMode(TrigMode  trig_mode);
	virtual void getTrigMode(TrigMode& trig_mode);

	virtual void setExpTime(double  exp_time);
	virtual void getExpTime(double& exp_time);

	virtual void setLatTime(double  lat_time);
	virtual void getLatTime(double& lat_time);

	virtual void setNbFrames(int  nb_frames);
	virtual void getNbFrames(int& nb_frames);

	virtual void getValidRanges(ValidRangesType& valid_ranges);

 private:
	Simulator& m_simu;
};


/*******************************************************************
 * \class SimuHwInterface
 * \brief Simulator hardware interface
 *******************************************************************/

class SimuHwInterface : public HwInterface
{
 public:
	SimuHwInterface(Simulator& simu);
	virtual ~SimuHwInterface();

	virtual const CapList& getCapList() const;

	virtual void reset(ResetLevel reset_level);
	virtual void prepareAcq();
	virtual void startAcq();
	virtual void stopAcq();
	virtual void getStatus(StatusType& status);
	virtual int getNbAcquiredFrames();

 private:
	Simulator& m_simu;
	CapList m_cap_list;
	SimuDetInfoCtrlObj m_det_info;
	SimuBufferCtrlObj  m_buffer;
	SimuSyncCtrlObj    m_sync;
};

}

#endif // SIMUHWINTERFACE_H
