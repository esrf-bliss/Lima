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
  };
}
#endif
