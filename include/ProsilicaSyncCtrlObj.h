#ifndef PROSILICASYNCCTRLOBJ_H
#define PROSILICASYNCCTRLOBJ_H

#include "Prosilica.h"

#include "HwSyncCtrlObj.h"

namespace lima
{
  namespace Prosilica
  {
    class Camera;
    class BufferCtrlObj;

    class SyncCtrlObj : public HwSyncCtrlObj
    {
      DEB_CLASS_NAMESPC(DebModCamera,"SyncCtrlObj","Prosilica");
    public:
      SyncCtrlObj(Camera*,BufferCtrlObj*);
      virtual ~SyncCtrlObj();

      virtual bool checkTrigMode(TrigMode trig_mode);
      virtual void setTrigMode(TrigMode  trig_mode);
      virtual void getTrigMode(TrigMode& trig_mode);

      virtual void setExpTime(double  exp_time);
      virtual void getExpTime(double& exp_time);

      virtual void setLatTime(double  lat_time);
      virtual void getLatTime(double& lat_time);

      virtual void setNbFrames(int  nb_frames);
      virtual void getNbFrames(int& nb_frames);

      virtual void setNbHwFrames(int  nb_frames);
      virtual void getNbHwFrames(int& nb_frames);

      virtual void getValidRanges(ValidRangesType& valid_ranges);

      void startAcq();
      void stopAcq();
    private:
      Camera*		m_cam;
      tPvHandle&	m_handle;
      TrigMode		m_trig_mode;
      BufferCtrlObj*	m_buffer;
      int		m_nb_frames;
    };

  } // namespace Prosilica
} // namespace lima

#endif // PROSILICASYNCCTRLOBJ_H
