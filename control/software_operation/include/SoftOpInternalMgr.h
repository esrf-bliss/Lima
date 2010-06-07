#ifndef __SOFTOPINTERNAL_H
#define __SOFTOPINTERNAL_H

#include "TaskMgr.h"
#include "LinkTask.h"
#include "SizeUtils.h"

namespace lima
{
  class SoftOpInternalMgr
  {
  public:

    struct Flip
    {
      Flip() { flip_x=0; flip_y=0; } ;
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
    bool hasReconstructionTask();

    void addTo(TaskMgr&,int&) const;

    void setEndCallback(TaskEventCallback *aCbk)
    {
      if(aCbk)
	aCbk->ref();
      if(m_end_callback)
	m_end_callback->unref();
      m_end_callback = aCbk;
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
