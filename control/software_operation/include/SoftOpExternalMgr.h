#ifndef __SOFTOPEXTERNALMGR_H
#define __SOFTOPEXTERNALMGR_H
#include <string>
#include <list>
#include <map>
#include "SoftOpId.h"
#include "TaskMgr.h"

namespace lima
{
  class SoftOpExternalMgr
  {
  public:
    typedef std::string alias;
    typedef int stage;
    SoftOpExternalMgr();
    ~SoftOpExternalMgr();
    
    void getAvailableOp(const SoftOpKey*&) const;
    void getActiveOp(std::list<stage,std::list<alias> >&) const;
    void getActiveOp(std::list<stage,std::list<SoftOpInstance> >&);
    void getActiveStageOp(stage,std::list<alias>&) const;

    void addOp(const SoftOpKey&,
	       const alias&,int stage);
    void delOp(const alias&);
    void getOpClass(const alias&,
		    SoftOpInstance&) const;
    void setEndLinkTaskCallback(TaskEventCallback *aCbk);
    void setEndSinkTaskCallback(TaskEventCallback *aCbk);

    void addTo(TaskMgr&,int begin_stage,int &last_link_task,int &last_sink_task);
  private:
    typedef std::map<stage,std::list<SoftOpInstance> > Stage2Instance;
    Stage2Instance	m_stage2instance;
  };
}
#endif
