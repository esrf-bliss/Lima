#ifndef PROSILICABUFFERCTRLOBJ_H
#define PROSILICABUFFERCTRLOBJ_H

#include "Prosilica.h"

#include "HwBufferMgr.h"

namespace lima
{
  namespace Prosilica
  {
    class Camera;
    class SyncCtrlObj;
    class Interface;

    class BufferCtrlObj : public SoftBufferCtrlMgr
    {
      friend class Interface;
      DEB_CLASS_NAMESPC(DebModCamera,"BufferCtrlObj","Prosilica");
    public:
      BufferCtrlObj(Camera *cam);
      void startAcq();
    private:
      static void _newFrame(tPvFrame*);
      
      tPvHandle&      	m_handle;
      tPvFrame        	m_frame[2];
      SyncCtrlObj* 	m_sync;
    };
  }
}
#endif
