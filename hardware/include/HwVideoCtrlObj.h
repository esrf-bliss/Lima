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
      friend class HwVideoCtrlObj;
      DEB_CLASS_NAMESPC(DebModControl,"Video::ImageCallback", 
			"Hardware");

    public:
      ImageCallback() {}
      virtual ~ImageCallback() {}
    protected:
      virtual bool newImage(char *,int width,int height,VideoMode) = 0;
    };

    HwVideoCtrlObj() : m_image_cbk(NULL) {}
      
      virtual ~HwVideoCtrlObj() {}
 
      virtual void getSupportedVideoMode(std::list<VideoMode> &aList) const = 0;
      virtual void setVideoMode(VideoMode) = 0;

      virtual void setLive(bool) = 0;
      virtual void getLive(bool&) const = 0;

      virtual void getGain(double&) const = 0;
      virtual void setGain(double) = 0;

      virtual void checkBin(Bin& bin) = 0;
      virtual void checkRoi(const Roi& set_roi, Roi& hw_roi) = 0;

      void registerImageCallback(ImageCallback &cb);
      void unregisterImageCallback(ImageCallback &cb);

      HwBufferCtrlObj& getHwBufferCtrlObj() {return m_hw_buffer_ctrl_mgr;}
      StdBufferCbMgr& getBuffer() {return m_hw_buffer_ctrl_mgr.getBuffer();}

      bool callNewImage(char *data,int width,int height,VideoMode aVideoMode)
      {
	bool continueFlag = false;
	if(m_image_cbk)
	  continueFlag = m_image_cbk->newImage(data,width,height,aVideoMode);
	return continueFlag;
      }
  protected:
      ImageCallback* m_image_cbk;
  private:
      SoftBufferCtrlMgr		m_hw_buffer_ctrl_mgr;
  };
}
#endif
