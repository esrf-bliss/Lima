#ifndef HWVIDEOCTRLOBJ_H
#define HWVIDEOCTRLOBJ_H

namespace lima
{
  class HwVideoCtrlObj
  {
    DEB_CLASS(DebModHardware,"HwVideoCtrlObj");
  public:
    virtual void getSupportedVideoMode(std::list<VideoMode> &aList) const = 0;
    virtual void checkBin(Bin& bin) = 0;
    virtual void checkRoi(const Roi& set_roi, Roi& hw_roi) = 0;

    void registerImageCallback(ImageCallback &cb);
    void unregisterImageCallback(ImageCallback &cb);
  };
}
#endif
