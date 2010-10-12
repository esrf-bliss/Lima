#ifndef MPXINTERFACE_H
#define MPXINTERFACE_H

#include "HwInterface.h"
#include "EspiaBufferMgr.h"
#include "MaxipixDet.h"
#include "PriamAcq.h"

namespace lima
{

namespace Maxipix
{

/*******************************************************************
 * \class DetInfoCtrlObj
 * \brief Control object providing Maxipix detector info interface
 *******************************************************************/

class DetInfoCtrlObj : public HwDetInfoCtrlObj
{
  DEB_CLASS_NAMESPC(DebModCamera, "DetInfoCtrlObj", "Maxipix");

  public:
    DetInfoCtrlObj(MaxipixDet& det);
    virtual ~DetInfoCtrlObj();

    virtual void getMaxImageSize(Size& size);
    virtual void getDetectorImageSize(Size& size);

    virtual void getDefImageType(ImageType& image_type);
    virtual void getCurrImageType(ImageType& image_type);
    virtual void setCurrImageType(ImageType image_type);

    virtual void getPixelSize(double& size);
    virtual void getDetectorType(std::string& type);
    virtual void getDetectorModel(std::string& model);

    virtual void registerMaxImageSizeCallback(HwMaxImageSizeCallback& cb);
    virtual void unregisterMaxImageSizeCallback(HwMaxImageSizeCallback& cb);

  private:
    MaxipixDet& m_det;
};


/*******************************************************************
 * \class BufferCtrlObj
 * \brief Control object providing Maxipix buffering interface
 *******************************************************************/

class BufferCtrlObj : public HwBufferCtrlObj
{
  DEB_CLASS_NAMESPC(DebModCamera, "BufferCtrlObj", "Maxipix");

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
 * \brief Control object providing Maxipix synchronization interface
 *******************************************************************/

class SyncCtrlObj : public HwSyncCtrlObj
{
    DEB_CLASS_NAMESPC(DebModCamera, "SyncCtrlObj", "Maxipix");

  public:
    SyncCtrlObj(Espia::Acq& acq, PriamAcq& priam, BufferCtrlObj& buffer_mgr);
    virtual ~SyncCtrlObj();

    virtual bool checkTrigMode(TrigMode trig_mode);
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
    Espia::Acq& m_acq;
    PriamAcq& m_priam;
};

/*******************************************************************
 * \class ShutterCtrlObj
 * \brief Control object providing Frelon shutter interface
 *******************************************************************/

class ShutterCtrlObj : public HwShutterCtrlObj
{
	DEB_CLASS(DebModCamera, "ShutterCtrlObj");

public:
	ShutterCtrlObj(PriamAcq& priam);
	virtual ~ShutterCtrlObj();

	virtual bool checkMode(ShutterMode shut_mode) const;
	virtual void getModeList(ShutterModeList&  mode_list) const;
	virtual void setMode(ShutterMode  shut_mode);
	virtual void getMode(ShutterMode& shut_mode) const;

	virtual void setState(bool  shut_open);
	virtual void getState(bool& shut_open) const;

	virtual void setOpenTime (double  shut_open_time);
	virtual void getOpenTime (double& shut_open_time) const;
	virtual void setCloseTime(double  shut_close_time);
	virtual void getCloseTime(double& shut_close_time) const;

 private:
	PriamAcq& m_priam;
};

/*******************************************************************
 * \class Interface
 * \brief Maxipix hardware interface
 *******************************************************************/
class Interface : public HwInterface
{
	DEB_CLASS_NAMESPC(DebModCamera, "Interface", "Maxipix");

 public:
	Interface(Espia::Acq& acq, BufferCtrlMgr& buffer_mgr, 
		  PriamAcq& priam, MaxipixDet& det);
	virtual ~Interface();

	virtual void getCapList(CapList&) const;

	virtual void reset(ResetLevel reset_level);
	virtual void prepareAcq();
	virtual void startAcq();
	virtual void stopAcq();
	virtual void getStatus(StatusType& status);
	virtual int getNbHwAcquiredFrames();

 private:
	class AcqEndCallback : public Espia::AcqEndCallback
	{
		DEB_CLASS_NAMESPC(DebModCamera, "Interface::AcqEndCallback", 
				  "Maxipix");

	public:
		AcqEndCallback(PriamAcq& priam);
		virtual ~AcqEndCallback();

	protected:
		virtual void acqFinished(const HwFrameInfoType& /*finfo*/);
	private:
		PriamAcq& m_priam;
	};

	Espia::Acq&	m_acq;
	BufferCtrlMgr&	m_buffer_mgr;
	PriamAcq&	m_priam;
	AcqEndCallback  m_acq_end_cb;

	CapList m_cap_list;
	DetInfoCtrlObj m_det_info;
	BufferCtrlObj  m_buffer;
	SyncCtrlObj    m_sync;
	ShutterCtrlObj m_shutter;
};

} // namespace Maxipix

} // namespace lima

#endif // MPXINTERFACE_H
