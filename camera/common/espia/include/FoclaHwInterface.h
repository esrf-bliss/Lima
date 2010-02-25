/***************************************************************//**
 * @file   FoclaHwInterface.h
 * @brief  This file contains Focla Hardware Interface classes
 *
 * @author A.Kirov
 * @date   15/07/2009
 *******************************************************************/

#ifndef FOCLAHWINTERFACE_H
#define FOCLAHWINTERFACE_H

#include "HwInterface.h"
#include "EspiaBufferMgr.h"
#include "EspiaFocla.h"

namespace lima
{
namespace Espia
{
namespace Focla
{


extern const FrameDim MaxFrameDim;


/***************************************************************//**
 * @class DetInfoCtrlObj
 * @brief Focla detector info control object
 *******************************************************************/
class DetInfoCtrlObj : public HwDetInfoCtrlObj 
{
  public:
	DetInfoCtrlObj( Dev &focla );
	virtual ~DetInfoCtrlObj();

	virtual void getMaxImageSize(Size& max_image_size);
	virtual void getDetectorImageSize(Size& det_image_size);

	virtual void getDefImageType(ImageType& def_image_type);
	virtual void getCurrImageType(ImageType& curr_image_type);
	virtual void setCurrImageType(ImageType  curr_image_type);

	virtual void getPixelSize(double& pixel_size);
	virtual void getDetectorType(std::string& det_type);
	virtual void getDetectorModel(std::string& det_model);

	virtual void registerMaxImageSizeCallback(
					HwMaxImageSizeCallback& cb);
	virtual void unregisterMaxImageSizeCallback(
					HwMaxImageSizeCallback& cb);

  private:
	class MaxImageSizeCallbackGen: public HwMaxImageSizeCallbackGen
	{
	protected:
		virtual void setMaxImageSizeCallbackActive(bool cb_active);
	};

	Espia::Focla::Dev &m_focla;
	MaxImageSizeCallbackGen m_mis_cb_gen;
};


/***************************************************************//**
 * @class BufferCtrlObj
 * @brief Focla buffer control object
 *******************************************************************/
class BufferCtrlObj : public HwBufferCtrlObj
{
  public:
	BufferCtrlObj( BufferCtrlMgr &buffer_mgr );
	virtual ~BufferCtrlObj();

	virtual void setFrameDim(const FrameDim &frame_dim);
	virtual void getFrameDim(      FrameDim &frame_dim);

	virtual void setNbBuffers(int  nb_buffers);
	virtual void getNbBuffers(int &nb_buffers);

	virtual void setNbConcatFrames(int  nb_concat_frames);
	virtual void getNbConcatFrames(int &nb_concat_frames);

	virtual void setNbAccFrames(int  nb_acc_frames);
	virtual void getNbAccFrames(int &nb_acc_frames);

	virtual void getMaxNbBuffers(int &max_nb_buffers);

	virtual void *getBufferPtr(int buffer_nb, int concat_frame_nb=0);
	virtual void *getFramePtr(int acq_frame_nb);

	virtual void getStartTimestamp(Timestamp &start_ts);
	virtual void getFrameInfo(int acq_frame_nb, HwFrameInfoType &info);

	virtual void   registerFrameCallback(HwFrameCallback &frame_cb);
	virtual void unregisterFrameCallback(HwFrameCallback &frame_cb);

  private:
	BufferCtrlMgr& m_buffer_mgr;
};


/***************************************************************//**
 * @class SyncCtrlObj
 * @brief Focla synchronization control object
 *******************************************************************/
class SyncCtrlObj : public HwSyncCtrlObj
{
  public:
	SyncCtrlObj( Espia::Acq &acq, Dev &focla, 
		     HwBufferCtrlObj& buffer_ctrl );
	virtual ~SyncCtrlObj();

	virtual void setTrigMode(TrigMode  trig_mode);
	virtual void getTrigMode(TrigMode &trig_mode);

	virtual void setExpTime(double  exp_time);
	virtual void getExpTime(double &exp_time);

	virtual void setLatTime(double  lat_time);
	virtual void getLatTime(double &lat_time);

	virtual void setNbHwFrames(int  nb_frames);
	virtual void getNbHwFrames(int &nb_frames);

	virtual void getValidRanges(ValidRangesType &valid_ranges);

  private:
	Espia::Acq &m_acq;
	Dev        &m_focla;
};


/***************************************************************//**
 * @class Interface
 * @brief Focla hardware interface
 *******************************************************************/
class Interface : public HwInterface
{
 public:
	Interface( Espia::Acq &acq, BufferCtrlMgr &buffer_mgr, 
	           Espia::Focla::Dev &focla );
	virtual ~Interface();

	virtual void getCapList(CapList &) const;

	virtual void reset(ResetLevel reset_level);
	virtual void prepareAcq();
	virtual void startAcq();
	virtual void stopAcq();
	virtual void getStatus(StatusType& status);
	virtual int  getNbHwAcquiredFrames();

 private:
	Espia::Acq        &m_acq;
	BufferCtrlMgr     &m_buffer_mgr;
	Espia::Focla::Dev &m_focla;

	CapList           m_cap_list;
	DetInfoCtrlObj    m_det_info;
	BufferCtrlObj     m_buffer;
	SyncCtrlObj       m_sync;
};


} // namespace Focla
} // namespace Espia
} // namespace lima


#endif /* FOCLAHWINTERFACE_H */
