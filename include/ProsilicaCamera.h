#ifndef PROSILICACAMERA_H
#define PROSILICACAMERA_H
#include "Prosilica.h"
#include "Debug.h"
#include "Constants.h"
#include "HwMaxImageSizeCallback.h"

namespace lima
{
  namespace Prosilica
  {
    class SyncCtrlObj;
    class VideoCtrlObj;
    class Camera : public HwMaxImageSizeCallbackGen
    {
      friend class Interface;
      DEB_CLASS_NAMESPC(DebModCamera,"Camera","Prosilica");
    public:
      Camera(const char*);
      ~Camera();
      
      bool isMonochrome() const;
      tPvHandle& getHandle() {return m_handle;}
      void getMaxWidthHeight(tPvUint32& width,tPvUint32& height)
      {width = m_maxwidth, height = m_maxheight;}
      int getNbAcquiredFrames() const {return m_acq_frame_nb;}

      VideoMode getVideoMode() const;
      void 	setVideoMode(VideoMode);
      
      void	getCameraName(std::string& name);
	
      void 	startAcq();
      void	reset();

    private:
      void 		_allocBuffer();
      static void 	_newFrameCBK(tPvFrame*);
      void		_newFrame(tPvFrame*);

      bool 		m_cam_connected;
      tPvHandle		m_handle;
      char		m_camera_name[128];
      char		m_sensor_type[64];
      tPvUint32		m_ufirmware_maj, m_ufirmware_min;
      tPvUint32		m_maxwidth, m_maxheight;
      tPvUint32		m_uid;
      tPvFrame		m_frame[2];
      
      SyncCtrlObj*	m_sync;
      VideoCtrlObj*	m_video;
      VideoMode		m_video_mode;
      int		m_acq_frame_nb;
      bool		m_continue_acq;
    };
  }
}
#endif
