#ifndef CTVIDEO_H
#define CTVIDEO_H

#include "CtControl.h"
#include "VideoUtils.h"

namespace lima
{
  class HwVideoCtrlObj;
  class HwSyncCtrlObj;

  class CtVideo
  {
    DEB_CLASS_NAMESPC(DebModControl,"Video","Control");
    friend class CtControl;

    class _Data2ImageTask;
    friend class _Data2ImageTask;

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
      double 	exposure;	///< exposure time in second
      double 	gain;		///< % of gain (0. <= gain <= 1.)
      VideoMode mode;
      Roi 	roi;
      Bin 	bin;
    };

    class Image
    {
      friend class CtVideo;
      friend class _Data2ImageTask;
      friend std::ostream& operator<<(std::ostream &os,CtVideo::Image& im);
    public:
      Image();
      ~Image();
      Image(const Image&);
      Image& operator=(const Image&);

      const char* 	buffer() 	const;
      int 		width() 	const;
      int 		height() 	const;
      VideoMode 	mode() 		const;
      int		size()		const;
      long long		frameNumber()	const;
      
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
    void setActive(bool aFlag);
    bool isActive() const;

    // --- parameters
    void setParameters(const Parameters &pars);
    void getParameters(Parameters &pars) const;

    void startLive() {_setLive(true);}
    void stopLive() {_setLive(false);}
    void getLive(bool &liveFlag) const;

    void setExposure(double);
    void getExposure(double&) const;

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
    void getLastImageCounter(long long &anImageCounter) const;

    void registerImageCallback(ImageCallback &cb);
    void unregisterImageCallback(ImageCallback &cb);

    // --- video mode
    void getSupportedVideoMode(std::list<VideoMode> &modeList);
  private:
    class _Data2ImageCBK;
    friend class _Data2ImageCBK;
    class _InternalImageCBK;
    friend class _InternalImageCBK;

    void frameReady(Data&);	// callback from CtControl

    void _setLive(bool);
    void _data_2_image(Data &aData,Bin &aBin,Roi &aRoi);
    void _data2image_finnished(Data&);
    void _apply_params(AutoMutex &,bool = false);
    void _read_hw_params();
    void _check_video_mode(VideoMode);
    void _prepareAcq();

    Parameters		m_pars;
    int			m_pars_modify_mask;
    bool 		m_has_video;
    bool		m_ready_flag;
    Data		m_last_data;
    _Data2ImageTask*	m_data_2_image_task;
    _Data2ImageCBK*	m_data_2_image_cb;
    HwVideoCtrlObj* 	m_video;
    HwSyncCtrlObj*      m_sync;
    mutable Cond	m_cond;
    long long		m_image_counter;
    mutable VideoImage*	m_read_image;
    mutable VideoImage*	m_write_image;
    ImageCallback*	m_image_callback;
    _InternalImageCBK*	m_internal_image_callback;
    CtControl&		m_ct;
    Roi			m_hw_roi;
    Bin			m_hw_bin;
    bool		m_stopping_live; ///< variable to avoid deadlock when stopping live
    bool		m_active_flag; ///< flag if video is active
  };

  inline std::ostream& operator<<(std::ostream &os,
				  CtVideo::Image& im)
  {
    if(im.m_image)
      os << *(im.m_image);
    else
      os << "<No image>";
    return os;
  }

}
#endif
