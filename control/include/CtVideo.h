#ifndef CTVIDEO_H
#define CTVIDEO_H

#include "CtControl.h"

namespace lima
{
  class CtVideo
  {
    DEB_CLASS_NAMESPC(DebModControl,"Control","Control");
  public:
    CtVideo(CtControl&);
    ~CtVideo();
    
    enum VideoMode {UNDEF,Y8,Y16,Y32,
		    RGB555,RGB565,
		    RGB24,RGB32,
		    BGR24,BGR32,
		    BAYER_RG8,BAYER_RG16,
		    I420,YUV411,YUV422,YUV444};

    struct Parameters
    {
      DEB_CLASS_NAMESPC(DebModControl,"Video::Parameters","Control");
    public:
      Parameters();
      void reset();
      
      bool 	live;
      double 	framerate;	///< frame/second
      double 	exposure;	///< exposure time in second
      double 	brightness;	///< % of brightness (0. <= brightness <= 1.)
      double 	gain;		///< % of gain (0. <= gain <= 1.)
      VideoMode mode;
      Roi 	roi;
      Bin 	bin;
    };
    struct Image
    {
      int 			height;
      int 			width;
      CtVideo::VideoMode 	mode;
      char*			buffer;
    };

    class ImageCallback
    {
      DEB_CLASS_NAMESPC(DebModControl,"Video::ImageCallback", 
			"Control");

    public:
      ImageCallback() {}
      virtual ~ImageCallback() {}
    protected:
      virtual void newImage(const Image&) = 0;
    };
    // --- parameters
    void setParameters(const Parameters &pars);
    void getParameters(Parameters &pars) const;

    void setLive(bool liveFlag);
    void getLive(bool &liveFlag) const;

    void setFrameRate(double aFrameRate);
    void getFrameRate(double &aFrameRate) const;

    void setBrightness(double aBrightness);
    void getBrightness(double &aBrightness) const;

    void setGain(double aGain);
    void getGain(double &aGain) const;

    void setMode(VideoMode aMode);
    void getMode(VideoMode &aMode) const;

    void setRoi(const Roi &aRoi);
    void getRoi(Roi &aRoi) const;

    void setBin(const Bin &aBin);
    void getBin(Bin &aBin) const;

    // --- images
    void getLastImage(Image &anImage) const;
    void getLastImageCounter(int &anImageCounter) const;

    void registerImageCallback(ImageCallback &cb);
    void unregisterImageCallback(ImageCallback &cb);

    // --- video mode
    void getSupportedVideoMode(std::list<VideoMode> &modeList);
  private:
    Parameters m_pars;
  };
}
#endif
