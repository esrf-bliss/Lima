//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2011
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################
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

    SoftOpInternalMgr();
    ~SoftOpInternalMgr();
    
    void setBin(const Bin &);
    void getBin(Bin &) const;

    void setRoi(const Roi &);
    void getRoi(Roi &) const;

    void setFlip(const Flip&);
    void getFlip(Flip&) const;

    void setRotation(RotationMode);
    void getRotation(RotationMode&) const;

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
    RotationMode	m_rotation;
    mutable LinkTask	*m_reconstruction_task;
    TaskEventCallback	*m_end_callback;
  };
}
#endif
