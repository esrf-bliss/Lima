#ifndef CTVIDEO_H
#define CTVIDEO_H

#include "CtControl.h"
#include "VideoUtils.h"

namespace lima
{
  class HwVideoCtrlObj;

  class CtVideo
  {
    DEB_CLASS_NAMESPC(DebModControl,"Control","Control");
    friend class CtControl;

  public:
    CtVideo(CtControl&);
    ~CtVideo();
    
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

    class Image
    {
      friend class CtVideo;
    public:
      Image();
      ~Image();
      Image(const Image&);
      Image& operator=(const Image&);

      const unsigned char* 	buffer() 	const;
      int 			width() 	const;
      int 			height() 	const;
      VideoMode 		mode() 		const;
    private:
      Image(const CtVideo*,VideoImage*);

      const CtVideo* 	m_video;
      VideoImage* 	m_image;
    };

    friend class Image;

    class ImageCallback
    {
      DEB_CLASS_NAMESPC(DebModControl,"Video::ImageCallback", 
			"Control");

    public:
      ImageCallback() {}
      virtual ~ImageCallback() {}

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
    class _Data2ImageTask;
    friend class _Data2ImageTask;
    class _Data2ImageCBK;
    friend class _Data2ImageCBK;

    void frameReady(Data&);	// callback from CtControl

    void _data_2_image(Data &aData,Bin &aBin,Roi &aRoi);
    void _data2image_finnished(Data&);
    void _apply_params();

    Parameters		m_pars;
    bool 		m_has_video;
    bool		m_ready_flag;
    Data		m_last_data;
    _Data2ImageTask*	m_data_2_image_task;
    _Data2ImageCBK*	m_data_2_image_cb;
    HwVideoCtrlObj* 	m_video;
    mutable Cond	m_cond;
    int			m_image_counter;
    mutable VideoImage*	m_read_image;
    mutable VideoImage*	m_write_image;
    ImageCallback*	m_image_callback;
  };
}
#endif
