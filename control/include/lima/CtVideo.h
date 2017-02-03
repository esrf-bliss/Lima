#ifndef CTVIDEO_H
#define CTVIDEO_H

#include "lima/LimaCompatibility.h"
#include "lima/CtControl.h"
#include "lima/CtConfig.h"
#include "lima/VideoUtils.h"

namespace lima
{
  class HwVideoCtrlObj;
  class HwSyncCtrlObj;

  class LIMACORE_API CtVideo
  {
    DEB_CLASS_NAMESPC(DebModControl,"Video","Control");
    friend class CtControl;

    class _Data2ImageTask;
    friend class _Data2ImageTask;

  public:
    enum AutoGainMode {
      OFF,	  ///< Always off
      ON,	  ///< Always on
      ON_LIVE	  ///< ON during live and OFF for standard Acquisition
    };
    typedef std::vector<AutoGainMode> AutoGainModeList;

    enum VideoSource {
      BASE_IMAGE,		///< video image source is BaseImage
      LAST_IMAGE		///< video image source is the last image (after all processing)
    };

    CtVideo(CtControl&);
    ~CtVideo();
    
    struct LIMACORE_API Parameters
    {
      DEB_CLASS_NAMESPC(DebModControl,"Video::Parameters","Control");
    public:
      Parameters();
      void reset();
      
      bool		live;
      double		exposure;	///< exposure time in second
      double		gain;		///< % of gain (0. <= gain <= 1.)
      AutoGainMode	auto_gain_mode;
      VideoSource	video_source;
      VideoMode		mode;
      Roi		roi;
      Bin		bin;
    };
    class _InternalImageCBK;
    class LIMACORE_API Image
    {
      friend class CtVideo;
      friend class CtVideo::_InternalImageCBK;
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

    class LIMACORE_API ImageCallback
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
    bool checkAutoGainMode(AutoGainMode mode) const;
    void getAutoGainModeList(AutoGainModeList& modes) const;
    void setAutoGainMode(AutoGainMode mode);
    void getAutoGainMode(AutoGainMode& mode) const;

    void setVideoSource(VideoSource source);
    void getVideoSource(VideoSource& source) const;

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
    void getSupportedVideoMode(std::list<VideoMode> &modeList) const;
  private:
    class _Data2ImageCBK;
    friend class _Data2ImageCBK;
    friend class _InternalImageCBK;
    class _videoBackgroundCallback;

    void frameReady(Data&);	// callback from CtControl

    void _setLive(bool);
    void _data_2_image(Data &aData,Bin &aBin,Roi &aRoi);
    void _data2image_finnished(Data&);
    void _apply_params(AutoMutex &,bool = false);
    void _read_hw_params();
    void _check_video_mode(VideoMode);
    void _prepareAcq();
    void _startAcqTime();
#ifdef WITH_CONFIG
    class _ConfigHandler;
    CtConfig::ModuleTypeCallback* _getConfigHandler();
#endif //WITH_CONFIG

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
  
  inline const char* convert_2_string(CtVideo::AutoGainMode mode)
  {
    const char *name;
    switch(mode)
      {
      case CtVideo::ON: name = "ON";break;
      case CtVideo::OFF: name = "OFF";break;
      case CtVideo::ON_LIVE: name = "ON LIVE";break;
      default:
	name = "UNKNOWN";
	break;
      }
    return name;
  }
  inline void convert_from_string(const std::string& val,
				  CtVideo::AutoGainMode& mode)
  {
    std::string buffer = val;
    std::transform(buffer.begin(),buffer.end(),
		   buffer.begin(),::tolower);
    
    if(buffer == "off") mode = CtVideo::OFF;
    else if(buffer == "on") mode = CtVideo::ON;
    else if(buffer == "on live") mode = CtVideo::ON_LIVE;
    else
      {
	std::ostringstream msg;
	msg << "AutoExposureMode can't be:" << DEB_VAR1(val);
	throw LIMA_EXC(Common,InvalidValue,msg.str());
      }
  }
  inline std::ostream& operator<<(std::ostream &os,
				  CtVideo::AutoGainMode mode)
  {
    return os << convert_2_string(mode);
  }
  inline const char* convert_2_string(CtVideo::VideoSource source)
  {
    const char *name;
    switch(source)
      {
      case CtVideo::BASE_IMAGE: name = "BASE_IMAGE";break;
      case CtVideo::LAST_IMAGE: name = "LAST_IMAGE";break;
      default:
	name = "UNKNOWN";
	break;
      }
    return name;
  }
  inline void convert_from_string(const std::string& val,
				  CtVideo::VideoSource& source)
  {
    std::string buffer = val;
    std::transform(buffer.begin(),buffer.end(),
		   buffer.begin(),::tolower);
    
    if(buffer == "base_image") source = CtVideo::BASE_IMAGE;
    else if(buffer == "last_image") source = CtVideo::LAST_IMAGE;
    else
      {
	std::ostringstream msg;
	msg << "VideoSource can't be:" << DEB_VAR1(val);
	throw LIMA_EXC(Common,InvalidValue,msg.str());
      }
  }
  inline std::ostream& operator<<(std::ostream &os,
				  CtVideo::VideoSource source)
  {
    return os << convert_2_string(source);
  }
  inline std::ostream& operator<<(std::ostream &os,
				  const CtVideo::Parameters& params)
    {
      os << "<"
	 << "live=" << (params.live ? "Yes" : "No") << ", "
	 << "exposure=" << params.exposure << ", "
	 << "gain=" << params.gain << ", "
	 << "auto_gain_mode=" << convert_2_string(params.auto_gain_mode) << ", "
	 << "mode=" << convert_2_string(params.mode) << ", "
	 << "roi=" << params.roi << ", "
	 << "bin=" << params.bin << ", "
	 << "source=" << params.video_source
	 << ">";
      return os;
    }
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
