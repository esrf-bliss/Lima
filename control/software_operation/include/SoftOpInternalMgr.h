#ifndef __SOFTOPINTERNAL_H
#define __SOFTOPINTERNAL_H

#include "TaskMgr.h"
#include "LinkTask.h"

namespace lima
{
  class SoftOpInternalMgr
  {
  public:
    struct Bin
    {
      Bin();
      int bin_x;
      int bin_y;
    };

    struct Roi
    {
      Roi();
      bool active;
      int x,y;
      int width,height;
    };

    struct Flip
    {
      Flip();
      bool flip_x;
      bool flip_y;
    };

    SoftOpInternalMgr();
    ~SoftOpInternalMgr();
    
    void setBin(const Bin &);
    void getBin(Bin &) const;

    void setRoi(const Roi &);
    void getRoi(Roi &) const;

    void setFlip(const Flip&);
    void getFlip(Flip&) const;
    
    void setReconstructionTask(LinkTask*);

    void addTo(TaskMgr&,int&) const;

    void setEndCallback(TaskEventCallback *aCbk)
    {
      if(m_end_callback)
	m_end_callback->unref();
      m_end_callback = aCbk;
      m_end_callback->ref();
    }
  private:
    class _EndCbk;
    friend class _EndCbk;

    Bin			m_bin;
    Flip		m_flip;
    Roi			m_roi;
    mutable LinkTask	*m_reconstruction_task;
    TaskEventCallback	*m_end_callback;
  };
}
#endif
