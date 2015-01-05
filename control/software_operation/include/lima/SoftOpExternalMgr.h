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
#ifndef __SOFTOPEXTERNALMGR_H
#define __SOFTOPEXTERNALMGR_H
#include <string>
#include <list>
#include <map>
#include "SoftOpId.h"
#include "TaskMgr.h"

namespace lima
{
  class LIMACORE_API SoftOpExternalMgr
  {
    DEB_CLASS_NAMESPC(DebModControl,"External Operation","Control");
  public:
    typedef std::string alias;
    typedef int stage;
    SoftOpExternalMgr();
    ~SoftOpExternalMgr();
    
    void getAvailableOp(const SoftOpKey*&) const;
    void getActiveOp(std::map<stage,std::list<alias> >&) const;
    void getActiveStageOp(stage,std::list<alias>&) const;

    void addOp(SoftOpId,const alias&,int stage,SoftOpInstance&);
    void delOp(const alias&);
    void getOpClass(const alias&,
		    SoftOpInstance&) const;
    void setEndLinkTaskCallback(TaskEventCallback *aCbk);
    void setEndSinkTaskCallback(TaskEventCallback *aCbk);

    void addTo(TaskMgr&,int begin_stage,int &last_link_task,int &last_sink_task);
    
    void isTaskActive(bool &linkTaskFlag,bool &sinkTaskFlag) const;
    void prepare();

  private:
    typedef std::map<stage,std::list<SoftOpInstance> > Stage2Instance;
    Stage2Instance	m_stage2instance;
    
    TaskEventCallback	*m_end_link_callback;
    TaskEventCallback   *m_end_sink_callback;

    void _checkIfPossible(SoftOpId aSoftOpId,
			  int stage);
    mutable Cond	m_cond;
  };
}
#endif
