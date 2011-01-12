#ifndef HWVIDEOCTRLOBJ_H
#define HWVIDEOCTRLOBJ_H
#include <list>
#include "Debug.h"
#include "Constants.h"
#include "SizeUtils.h"

namespace lima
{
  class HwVideoCtrlObj
  {
    DEB_CLASS(DebModHardware,"HwVideoCtrlObj");
  public:
 class ImageCallback
    {
      DEB_CLASS_NAMESPC(DebModControl,"Video::ImageCallback", 
			"Control");

    public:
      ImageCallback() {}
      virtual ~ImageCallback() {}
    protected:
      virtual void newImage(char *,int width,int height,VideoMode) = 0;
    };

    virtual ~HwVideoCtrlObj() {}
 
    virtual void getSupportedVideoMode(std::list<VideoMode> &aList) const = 0;
    virtual void checkBin(Bin& bin) = 0;
    virtual void checkRoi(const Roi& set_roi, Roi& hw_roi) = 0;

    void registerImageCallback(ImageCallback &cb);
    void unregisterImageCallback(ImageCallback &cb);
  };
}
#endif
