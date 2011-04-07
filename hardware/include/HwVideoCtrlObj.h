#ifndef HWVIDEOCTRLOBJ_H
#define HWVIDEOCTRLOBJ_H
#include <list>
#include "Debug.h"
#include "Constants.h"
#include "SizeUtils.h"
#include "HwBufferMgr.h"
#include "HwBufferCtrlObj.h"

namespace lima
{
  class HwVideoCtrlObj
  {
    DEB_CLASS(DebModHardware,"HwVideoCtrlObj");
  public:
    class ImageCallback
    {
      DEB_CLASS_NAMESPC(DebModControl,"Video::ImageCallback", 
			"Hardware");

    public:
      ImageCallback() {}
      virtual ~ImageCallback() {}
    protected:
      virtual void newImage(char *,int width,int height,VideoMode) = 0;
    };

    HwVideoCtrlObj() : 
      m_buffer_cb_mgr(m_buffer_alloc_mgr),
      m_buffer_ctrl_mgr(m_buffer_cb_mgr),
      m_hw_buffer_ctrl_mgr(m_buffer_ctrl_mgr) {}

      virtual ~HwVideoCtrlObj() {}
 
      virtual void getSupportedVideoMode(std::list<VideoMode> &aList) const = 0;
      virtual void setVideoMode(VideoMode) = 0;

      virtual void setLive(bool) = 0;

      virtual void getBrightness(double&) const = 0;
      virtual void setBrightness(double) = 0;

      virtual void getGain(double&) const = 0;
      virtual void setGain(double) = 0;

      virtual void checkBin(Bin& bin) = 0;
      virtual void checkRoi(const Roi& set_roi, Roi& hw_roi) = 0;

      void registerImageCallback(ImageCallback &cb);
      void unregisterImageCallback(ImageCallback &cb);

      HwBufferCtrlObj& getHwBufferCtrlObj() {return m_hw_buffer_ctrl_mgr;}
  private:
      class _BufferCtrlMgr : public  HwBufferCtrlObj
      {
      public:
	_BufferCtrlMgr(BufferCtrlMgr &buffer_mgr) :
	  HwBufferCtrlObj(),
	  m_mgr(buffer_mgr) {}

	  virtual void setFrameDim(const FrameDim& frame_dim) {m_mgr.setFrameDim(frame_dim);}
	  virtual void getFrameDim(FrameDim& frame_dim) {m_mgr.getFrameDim(frame_dim);}

	  virtual void setNbBuffers(int  nb_buffers) {m_mgr.setNbBuffers(nb_buffers);}
	  virtual void getNbBuffers(int& nb_buffers) {m_mgr.getNbBuffers(nb_buffers);}

	  virtual void setNbConcatFrames(int nb_concat_frames) {m_mgr.setNbConcatFrames(nb_concat_frames);}
	  virtual void getNbConcatFrames(int& nb_concat_frames) {m_mgr.getNbConcatFrames(nb_concat_frames);}

	  virtual void getMaxNbBuffers(int& max_nb_buffers) {m_mgr.getMaxNbBuffers(max_nb_buffers);}

	  virtual void *getBufferPtr(int buffer_nb,int concat_frame_nb = 0)
	  {return m_mgr.getBufferPtr(buffer_nb,concat_frame_nb);}

	  virtual void *getFramePtr(int acq_frame_nb)
	  {return m_mgr.getFramePtr(acq_frame_nb);}

	  virtual void getStartTimestamp(Timestamp& start_ts) 
	  {m_mgr.getStartTimestamp(start_ts);}
	  virtual void getFrameInfo(int acq_frame_nb, HwFrameInfoType& info)
	  {m_mgr.getFrameInfo(acq_frame_nb,info);}

	  virtual void   registerFrameCallback(HwFrameCallback& frame_cb)
	  {m_mgr.registerFrameCallback(frame_cb);}
	  virtual void unregisterFrameCallback(HwFrameCallback& frame_cb) 
	  {m_mgr.unregisterFrameCallback(frame_cb);}
      private:
	  BufferCtrlMgr  m_mgr;
      };

      SoftBufferAllocMgr 	m_buffer_alloc_mgr;
      StdBufferCbMgr 		m_buffer_cb_mgr;
      BufferCtrlMgr		m_buffer_ctrl_mgr;
      _BufferCtrlMgr 		m_hw_buffer_ctrl_mgr;
  };
}
#endif
