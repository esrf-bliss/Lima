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
#ifndef __SOFTOPID_H
#define __SOFTOPID_H


#include "lima/SizeUtils.h"

#include "processlib/TaskMgr.h"

#include "processlib/BackgroundSubstraction.h"
#include "processlib/Binning.h"
#include "processlib/Bpm.h"
#include "processlib/FlatfieldCorrection.h"
#include "processlib/Flip.h"
#include "processlib/Mask.h"
#include "processlib/RoiCounter.h"
#include "processlib/Roi2Spectrum.h"
#include "processlib/SoftRoi.h"
#include "processlib/PeakFinder.h"

namespace lima
{
  class LIMACORE_API SoftOpBaseClass
  {
    friend class SoftOpExternalMgr;
  protected:
    static const int DEFAULT_HISTORY_SIZE = 128;

    virtual ~SoftOpBaseClass() {};

    virtual bool addTo(TaskMgr&,int stage) = 0;
    virtual void prepare() = 0;

  public:
    // By default the task is active. Some Op may be in the pipeline but not actually active, e.g.
    // RoiCounters, Roi2Spectrum without Rois defined.
    virtual bool isActive() const { return true; }
  };

  enum SoftOpId
    {
      UNDEF,
      BACKGROUNDSUBSTRACTION,
      BINNING,
      BPM,
      FLATFIELDCORRECTION,
      FLIP,
      MASK,
      ROICOUNTERS,
      ROI2SPECTRUM,
      SOFTROI,
      USER_LINK_TASK,
      USER_SINK_TASK,
      PEAKFINDER,
    };

  struct LIMACORE_API SoftOpKey
  {
    SoftOpKey() : m_id(UNDEF),m_name(NULL) {};
    SoftOpKey(SoftOpId id,const char *name) : m_id(id),m_name(name) {}

    SoftOpId	m_id;
    const char *m_name;
  };

  struct LIMACORE_API SoftOpInstance
  {
    SoftOpInstance() : m_linkable(false),
		       m_opt(NULL) {};

    SoftOpInstance(const SoftOpKey &akey,
		   const std::string &anAlias) :
      m_key(akey),
      m_alias(anAlias),m_linkable(false),m_opt(NULL) {}

    bool isActive() const { return m_opt->isActive(); }

    SoftOpKey		m_key;
    std::string		m_alias;
    bool		m_linkable;
    SoftOpBaseClass*	m_opt;
  };
  
  class LIMACORE_API SoftOpBackgroundSubstraction : public SoftOpBaseClass
  {
  public:
    SoftOpBackgroundSubstraction();
    virtual ~SoftOpBackgroundSubstraction();
    
    void setBackgroundImage(Data &aData);
    void setOffset(int value);
    void getOffset(int& value) const;
  protected:
    virtual bool addTo(TaskMgr&,int stage);
    virtual void prepare() {};
  private:
    Tasks::BackgroundSubstraction *m_opt;
  };

  class LIMACORE_API SoftOpBinning : public SoftOpBaseClass
  {
  public:
    SoftOpBinning();
    virtual ~SoftOpBinning();
    
    void setBinning(int x,int y);
    
  protected:
    virtual bool addTo(TaskMgr&,int stage);
    virtual void prepare() {};
  private:
    Tasks::Binning *m_opt;
  };

  class LIMACORE_API SoftOpBpm : public SoftOpBaseClass
  {
  public:
    SoftOpBpm();
    virtual ~SoftOpBpm();
    
    Tasks::BpmTask* 	getTask() 	{return m_task;}
    Tasks::BpmManager* 	getManager() 	{return m_manager;}

  protected:
    virtual bool addTo(TaskMgr&,int stage);
    virtual void prepare();
  private:
    Tasks::BpmManager	*m_manager;
    Tasks::BpmTask	*m_task;
  };

  class LIMACORE_API SoftOpFlatfieldCorrection : public SoftOpBaseClass
  {
  public:
    SoftOpFlatfieldCorrection();
    virtual ~SoftOpFlatfieldCorrection();
    
    void setFlatFieldImage(Data &aData,bool normalize = true);
    
  protected:
    virtual bool addTo(TaskMgr&,int stage);
    virtual void prepare() {};
  private:
    Tasks::FlatfieldCorrection *m_opt;
  };

  class LIMACORE_API SoftOpFlip : public SoftOpBaseClass
  {
  public:
    SoftOpFlip();
    virtual ~SoftOpFlip();
    
    void setFlip(bool x,bool y);
    
  protected:
    virtual bool addTo(TaskMgr&,int stage);
    virtual void prepare() {};
  private:
    Tasks::Flip *m_opt;
  };

  class LIMACORE_API SoftOpMask : public SoftOpBaseClass
  {
  public:
    enum Type {STANDARD,DUMMY};
    SoftOpMask();
    virtual ~SoftOpMask();

    void setMaskImage(Data &aData);
    void getType(Type&) const;
    void setType(Type);
  protected:
    virtual bool addTo(TaskMgr&,int stage);
    virtual void prepare() {};
  private:
    Tasks::Mask *m_opt;
  };

  template <class Manager, class Task>
  class NameTaskMap
  {
  public:
    typedef std::string Name;
    typedef std::pair<Name, Task *> NameAndTask;
    typedef std::list<Name> NameList;
    typedef std::list<NameAndTask> NameAndTaskList;
    typedef std::pair<Manager *, Task *> ManagerAndTask;
    typedef std::map<Name, ManagerAndTask> Name2ManagerAndTask;
    typedef typename Name2ManagerAndTask::iterator NameMapIterator;
    typedef typename Name2ManagerAndTask::const_iterator NameMapConstIterator;

    NameTaskMap()
    {
      clearCounterStatus();
      m_compat_format = "%d";
    }

    ~NameTaskMap()
    {
      for(NameMapIterator i = begin(); i != end(); ++i) {
	i->second.second->unref();
	i->second.first->unref();
      }
    }

    void setCompatFormat(std::string compat_format)
    {
      m_compat_format = compat_format;
    }

    Name getCompatName(int id) const
    {
      char buffer[256];
      snprintf(buffer, sizeof(buffer), m_compat_format.c_str(), id);
      return Name(buffer);
    }

    bool getCompatId(Name name, int& id) const
    {
      if (sscanf(name.c_str(), m_compat_format.c_str(), &id) != 1)
	return false;
      return true;
    }

    int size() const
    {
      return m_manager_tasks.size();
    }

    bool empty() const
    {
      return m_manager_tasks.empty();
    }

    NameMapIterator begin()
    {
      return m_manager_tasks.begin();
    }

    NameMapConstIterator begin() const
    {
      return m_manager_tasks.begin();
    }

    NameMapIterator end()
    {
      return m_manager_tasks.end();
    }

    NameMapConstIterator end() const
    {
      return m_manager_tasks.end();
    }

    NameMapIterator find(const Name& name)
    {
      return m_manager_tasks.find(name);
    }

    NameMapConstIterator find(const Name& name) const
    {
      return m_manager_tasks.find(name);
    }

    NameMapIterator findCompat(int id)
    {
      return m_manager_tasks.find(getCompatName(id));
    }

    NameMapConstIterator findCompat(int id) const
    {
      return m_manager_tasks.find(getCompatName(id));
    }


    void insert(const Name& name, ManagerAndTask man_task)
    {
      m_manager_tasks[name] = man_task;
    }

    void getTasks(NameAndTaskList& l) const
    {
      for (NameMapConstIterator i = begin(); i != end(); ++i) {
	const Name& name = i->first;
	Task *task = i->second.second;
	l.push_back(NameAndTask(name, task));
      }
    }

    void getNames(NameList& l) const
    {
      for (NameMapConstIterator i = begin(); i != end(); ++i)
	l.push_back(i->first);
    }

    void remove(const NameList& l)
    {
      for (NameList::const_iterator i = l.begin(); i != l.end(); ++i) {
	NameMapIterator named_roi = m_manager_tasks.find(*i);
	if (named_roi != end()) {
	  named_roi->second.second->unref();
	  named_roi->second.first->unref();
	  m_manager_tasks.erase(named_roi);
	}
      }
    }

    void clearAll()
    {
      for (NameMapIterator i = begin(); i != end(); ++i) {
	i->second.second->unref();
	i->second.first->unref();
      }
      m_manager_tasks.clear();
    }

    bool addTo(TaskMgr& aMgr, int stage)
    {
      for (NameMapIterator i = begin(); i != end(); ++i)
	aMgr.addSinkTask(stage, i->second.second);
      ++m_counter_status;
      return !m_manager_tasks.empty();
    }

    int getCounterStatus() const
    {
      return m_counter_status;
    }

    void prepareCounterStatus()
    {
      m_counter_status = -1;
    }

    void clearCounterStatus()
    {
      m_counter_status = -2;
    }

  private:
    Name2ManagerAndTask m_manager_tasks;
    int	m_counter_status;
    std::string m_compat_format;
  };

  class LIMACORE_API SoftOpRoiCounter : public SoftOpBaseClass
  {
    DEB_CLASS_NAMESPC(DebModControl,"SoftwareOperation","SoftOpRoiCounter");
  public:
    typedef std::pair<int,std::list<Tasks::RoiCounterResult> > RoiIdAndResults;
    typedef std::pair<int,Roi> RoiIdAndRoi;
    typedef std::pair<std::string,std::list<Tasks::RoiCounterResult> > RoiNameAndResults;
    typedef std::pair<std::string,Roi> RoiNameAndRoi;
    typedef std::pair<std::string,ArcRoi> RoiNameAndArcRoi;
    typedef std::pair<std::string,int> RoiNameAndType;
    typedef std::pair<std::string,Tasks::RoiCounterTask*> RoiNameAndTask;
    typedef std::list<RoiNameAndTask> RoiNameAndTaskList;

    SoftOpRoiCounter();
    virtual ~SoftOpRoiCounter();

    void updateRois(const std::list<RoiNameAndRoi>&);
    void updateArcRois(const std::list<RoiNameAndArcRoi>&);

    void getRois(std::list<RoiNameAndRoi>& names_rois) const;
    void getArcRois(std::list<RoiNameAndArcRoi>& names_rois) const;

    void setLut(const std::string& name,
		const Point& origin,Data &lut);
    void setLutMask(const std::string& name,
		    const Point& origin,Data &lut);

    void getNames(std::list<std::string>& roi_names) const;
    void getTypes(std::list<RoiNameAndType>& names_types) const;
    void getTasks(RoiNameAndTaskList&);

    void readCounters(int from,std::list<RoiNameAndResults> &result) const;

    void removeRois(const std::list<std::string>& names);
    void clearAllRois();		/* clear all roi */

    void clearCounterStatus();
    int  getCounterStatus() const;

    void setMask(Data &aMask);

    void setBufferSize(int size);
    void getBufferSize(int &size) const;

    /*override*/ bool isActive() const { return !m_task_manager.empty(); }

    void getOverflowThreshold(unsigned long long& threshold);
    void setOverflowThreshold(unsigned long long threshold);
    
  protected:
    virtual bool addTo(TaskMgr&,int stage);
    virtual void prepare();
  private:
    typedef Tasks::RoiCounterTask SoftTask;
    typedef Tasks::RoiCounterManager SoftManager;
    typedef NameTaskMap<SoftManager, SoftTask> TaskMap;
    typedef TaskMap::NameMapIterator NameMapIterator;
    typedef TaskMap::NameMapConstIterator NameMapConstIterator;

    void _get_or_create(const std::string& roi_name,
			SoftManager *&, SoftTask *&);

    template <SoftTask::type roi_type, class R>
    void _get_rois_of_type(std::list<std::pair<std::string, R> >& names_rois) const;
    template <class R>
    void _get_task_roi(SoftTask *task, R& roi) const;

    TaskMap			m_task_manager;
    int				m_history_size;
    Data			m_mask;
    mutable Cond		m_cond;
    unsigned long long 		m_overflow_threshold;
  };

  template <SoftOpRoiCounter::SoftTask::type type, class R>
  void SoftOpRoiCounter::_get_rois_of_type(std::list<std::pair<std::string, R> >& names_rois) const
  {
    for(NameMapConstIterator i = m_task_manager.begin();
	i != m_task_manager.end();++i) {
      SoftTask *task = i->second.second;
      SoftTask::type roi_type;
      task->getType(roi_type);
      if (roi_type != type)
	continue;
      R roi;
      _get_task_roi(task, roi);
      names_rois.push_back(std::pair<std::string, R>(i->first, roi));
    }
  }

  template <>
  inline void SoftOpRoiCounter::_get_task_roi(SoftTask *task, Roi& roi) const
  {
    int x,y,width,height;
    task->getRoi(x,y,width,height);
    roi = Roi(x,y,width,height);
  }

  template <>
  inline void SoftOpRoiCounter::_get_task_roi(SoftTask *task, ArcRoi& roi) const
  {
    double x,y,r1,r2,a1,a2;
    task->getArcMask(x,y,r1,r2,a1,a2);
    roi = ArcRoi(x,y,r1,r2,a1,a2);
  }

  class LIMACORE_API SoftOpRoi2Spectrum : public SoftOpBaseClass
  {
    DEB_CLASS_NAMESPC(DebModControl,"SoftwareOperation","SoftOpRoi2Spectrum");
  public:
    typedef std::pair<int,std::list<Tasks::Roi2SpectrumResult> > RoiIdAndResults;
    typedef std::pair<int,Roi> RoiIdAndRoi;
    typedef std::pair<std::string,std::list<Tasks::Roi2SpectrumResult> > RoiNameAndResults;
    typedef std::pair<std::string,Roi> RoiNameAndRoi;
    typedef std::pair<std::string,Tasks::Roi2SpectrumTask*> RoiNameAndTask;
    typedef std::pair<std::string,int> RoiNameAndMode;
    typedef std::list<RoiNameAndTask> RoiNameAndTaskList;

    SoftOpRoi2Spectrum();
    virtual ~SoftOpRoi2Spectrum();

    /** New methods set */
    void updateRois(const std::list<RoiNameAndRoi>&);
    void getRois(std::list<RoiNameAndRoi>&) const;

    void setRoiModes(const std::list<RoiNameAndMode>& names_modes);
    void getRoiModes(std::list<RoiNameAndMode>& roi_modes) const;

    void getNames(std::list<std::string>& roi_names) const;
    void getTasks(RoiNameAndTaskList&);

    void readCounters(int from,std::list<RoiNameAndResults> &result) const;
    void createImage(std::string roi_name,int &from,Data &aData) const;

    void removeRois(const std::list<std::string>& names);
    void clearAllRois();		/* clear all roi */
    // end of new set

    void clearCounterStatus();
    int  getCounterStatus() const;

    // probably needed in future
    //void setMask(Data &aMask);

    void setBufferSize(int size);
    void getBufferSize(int &size) const;

    /*override*/ bool isActive() const { return !m_task_manager.empty(); }
    
  protected:
    virtual bool addTo(TaskMgr&,int stage);
    virtual void prepare();
  private:
    typedef Tasks::Roi2SpectrumTask SoftTask;
    typedef Tasks::Roi2SpectrumManager SoftManager;
    typedef NameTaskMap<SoftManager, SoftTask> TaskMap;
    typedef TaskMap::NameMapIterator NameMapIterator;
    typedef TaskMap::NameMapConstIterator NameMapConstIterator;

    void _get_or_create(const std::string& roi_name,
			SoftManager *&, SoftTask *&);

    TaskMap			m_task_manager;
    int				m_history_size;
    //Data			m_mask;
    mutable Cond		m_cond;
  };

  class LIMACORE_API SoftOpSoftRoi : public SoftOpBaseClass
  {
  public:
    SoftOpSoftRoi();
    virtual ~SoftOpSoftRoi();
    
    void setRoi(int x,int y,int width,int height);
    
  protected:
    virtual bool addTo(TaskMgr&,int stage);
    virtual void prepare() {};
  private:
    Tasks::SoftRoi *m_opt;
  };

  class SoftPrepareCallbackGen;
  class LIMACORE_API SoftCallback
  {
    friend class SoftPrepareCallbackGen;
  public:
    SoftCallback() : m_callback_gen(NULL) {}
    virtual ~SoftCallback();
  protected:
    virtual void prepare() = 0;
  private:
    SoftPrepareCallbackGen* m_callback_gen;
  };

  class LIMACORE_API SoftPrepareCallbackGen
  {
    DEB_CLASS(DebModControl, "SoftPrepareCallbackGen");
  public:
    SoftPrepareCallbackGen();
    virtual ~SoftPrepareCallbackGen();

    void registerCallback(SoftCallback&);
    void unregisterCallback(SoftCallback&);
  protected:
    virtual void prepare_cb();
  private:
    SoftCallback* m_cb;
  };

  class LIMACORE_API SoftUserLinkTask : public SoftOpBaseClass,
					public SoftPrepareCallbackGen
  {
  public:
    SoftUserLinkTask();
    virtual ~SoftUserLinkTask();

    void setLinkTask(LinkTask*);
  protected:
    virtual bool addTo(TaskMgr&,int stage);
    virtual void prepare() {prepare_cb();}
  private:
    LinkTask*	m_link_task;
  };

  class LIMACORE_API SoftUserSinkTask : public SoftOpBaseClass,
					public SoftPrepareCallbackGen
  {
  public:
    SoftUserSinkTask();
    virtual ~SoftUserSinkTask();

    void setSinkTask(SinkTaskBase*);

  protected:
    virtual bool addTo(TaskMgr&,int stage);
    virtual void prepare() {prepare_cb();}
  private:
    SinkTaskBase* m_sink_task;
  };

  class LIMACORE_API SoftOpPeakFinder : public SoftOpBaseClass
  {
  public:
    enum ComputingMode {MAXIMUM,CM};
    SoftOpPeakFinder();
    virtual ~SoftOpPeakFinder();
    
    void setMask(Data &aData);
    void clearCounterStatus();
    int  getCounterStatus() const;

    void setBufferSize(int size);
    void getBufferSize(int &size) const;

    void readPeaks(std::list<Tasks::PeakFinderResult> &result) const;

    void setComputingMode( ComputingMode);
    void getComputingMode( ComputingMode&) const;

  protected:
    virtual bool addTo(TaskMgr&,int stage);
    virtual void prepare() {};
  private:
    typedef Tasks::PeakFinderTask SoftTask;
    typedef Tasks::PeakFinderManager SoftManager;
    typedef NameTaskMap<SoftManager, SoftTask> TaskMap;
    typedef TaskMap::NameMapIterator NameMapIterator;
    typedef TaskMap::NameMapConstIterator NameMapConstIterator;

    TaskMap			m_task_manager;
    int				m_history_size;
    //    LinkTask *m_opt;
    Tasks::PeakFinderTask *m_opt;
    mutable Cond		m_cond;
    
  };



}
#endif
