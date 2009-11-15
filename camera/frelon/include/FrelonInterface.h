#ifndef FRELONINTERFACE_H
#define FRELONINTERFACE_H

#include "HwInterface.h"
#include "EspiaBufferMgr.h"
#include "FrelonCamera.h"

namespace lima
{

namespace Frelon
{

class Interface;

/*******************************************************************
 * \class DetInfoCtrlObj
 * \brief Control object providing Frelon detector info interface
 *******************************************************************/

class DetInfoCtrlObj : public HwDetInfoCtrlObj
{
	DEB_CLASS_NAMESPC(DebModCamera, "DetInfoCtrlObj", "Frelon");

 public:
	DetInfoCtrlObj(Camera& cam);
	virtual ~DetInfoCtrlObj();

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
	Camera& m_cam;
	bool m_iscb_act;
};


/*******************************************************************
 * \class BufferCtrlObj
 * \brief Control object providing Frelon buffering interface
 *******************************************************************/

class BufferCtrlObj : public HwBufferCtrlObj
{
	DEB_CLASS_NAMESPC(DebModCamera, "BufferCtrlObj", "Frelon");

 public:
	BufferCtrlObj(BufferCtrlMgr& buffer_mgr);
	virtual ~BufferCtrlObj();

	virtual void setFrameDim(const FrameDim& frame_dim);
	virtual void getFrameDim(      FrameDim& frame_dim);

	virtual void setNbBuffers(int  nb_buffers);
	virtual void getNbBuffers(int& nb_buffers);

	virtual void setNbConcatFrames(int  nb_concat_frames);
	virtual void getNbConcatFrames(int& nb_concat_frames);

	virtual void setNbAccFrames(int  nb_acc_frames);
	virtual void getNbAccFrames(int& nb_acc_frames);

	virtual void getMaxNbBuffers(int& max_nb_buffers);

	virtual void *getBufferPtr(int buffer_nb, int concat_frame_nb = 0);
	virtual void *getFramePtr(int acq_frame_nb);

	virtual void getStartTimestamp(Timestamp& start_ts);
	virtual void getFrameInfo(int acq_frame_nb, HwFrameInfoType& info);

	virtual void   registerFrameCallback(HwFrameCallback& frame_cb);
	virtual void unregisterFrameCallback(HwFrameCallback& frame_cb);

 private:
	BufferCtrlMgr& m_buffer_mgr;
};


/*******************************************************************
 * \class SyncCtrlObj
 * \brief Control object providing Frelon synchronization interface
 *******************************************************************/

class SyncCtrlObj : public HwSyncCtrlObj
{
	DEB_CLASS_NAMESPC(DebModCamera, "SyncCtrlObj", "Frelon");

 public:
	SyncCtrlObj(Espia::Acq& acq, Camera& cam, BufferCtrlObj& buffer_ctrl);
	virtual ~SyncCtrlObj();

	virtual void setTrigMode(TrigMode  trig_mode);
	virtual void getTrigMode(TrigMode& trig_mode);

	virtual void setExpTime(double  exp_time);
	virtual void getExpTime(double& exp_time);

	virtual void setLatTime(double  lat_time);
	virtual void getLatTime(double& lat_time);

	virtual void setNbHwFrames(int  nb_frames);
	virtual void getNbHwFrames(int& nb_frames);

	virtual void getValidRanges(ValidRangesType& valid_ranges);

 private:
	class AcqEndCallback : public Espia::AcqEndCallback
	{
		DEB_CLASS_NAMESPC(DebModCamera, "SyncCtrlObj::AcqEndCallback", 
				  "Frelon");

	public:
		AcqEndCallback(Camera& cam);
		virtual ~AcqEndCallback();

	protected:
		virtual void acqFinished(const HwFrameInfoType& /*finfo*/);
	private:
		Camera& m_cam;
	};

	Espia::Acq& m_acq;
	Camera& m_cam;
	AcqEndCallback m_acq_end_cb;
};


/*******************************************************************
 * \class BinCtrlObj
 * \brief Control object providing Frelon binning interface
 *******************************************************************/

class BinCtrlObj : public HwBinCtrlObj
{
	DEB_CLASS_NAMESPC(DebModCamera, "BinCtrlObj", "Frelon");

 public:
	BinCtrlObj(Camera& cam);
	virtual ~BinCtrlObj();

	virtual void setBin(const Bin& bin);
	virtual void getBin(Bin& bin);
	virtual void checkBin(Bin& bin);

 private:
	Camera& m_cam;
};


/*******************************************************************
 * \class RoiCtrlObj
 * \brief Control object providing Frelon Roi interface
 *******************************************************************/

class RoiCtrlObj : public HwRoiCtrlObj
{
	DEB_CLASS_NAMESPC(DebModCamera, "RoiCtrlObj", "Frelon");

 public:
	RoiCtrlObj(Camera& cam);
	virtual ~RoiCtrlObj();

	virtual void checkRoi(const Roi& set_roi, Roi& hw_roi);
	virtual void setRoi(const Roi& set_roi);
	virtual void getRoi(Roi& hw_roi);

 private:
	Camera& m_cam;
};


/*******************************************************************
 * \class Interface
 * \brief Frelon hardware interface
 *******************************************************************/

class Interface : public HwInterface
{
	DEB_CLASS_NAMESPC(DebModCamera, "Interface", "Frelon");

 public:
	Interface(Espia::Acq& acq, BufferCtrlMgr& buffer_mgr, Camera& cam);
	virtual ~Interface();

	virtual const CapList& getCapList() const;

	virtual void reset(ResetLevel reset_level);
	virtual void prepareAcq();
	virtual void startAcq();
	virtual void stopAcq();
	virtual void getStatus(StatusType& status);
	virtual int getNbHwAcquiredFrames();

 private:
	Espia::Acq&    m_acq;
	BufferCtrlMgr& m_buffer_mgr;
	Camera&        m_cam;

	CapList m_cap_list;
	DetInfoCtrlObj m_det_info;
	BufferCtrlObj  m_buffer;
	SyncCtrlObj    m_sync;
	BinCtrlObj     m_bin;
	RoiCtrlObj     m_roi;
};




} // namespace Frelon

} // namespace lima

#endif // FRELONINTERFACE_H
