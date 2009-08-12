#include "SoftOpExternalMgr.h"
using namespace lima;

static SoftOpKey SoftOpTable[] = {
  {BACKGROUNDSUBSTRACTION,"Background substraction"},
  {BINNING,"Binning"},
  {BPM,"Bpm"},
  {FLATFIELDCORRECTION,"Flat field correction"},
  {FLIP,"Flip"},
  {MASK,"Mask"},
  {ROICOUNTER,"Roi counter"},
  {SOFTROI,"Software roi"},
  {UNDEF,NULL}
};

SoftOpExternalMgr::SoftOpExternalMgr()
{
}

SoftOpExternalMgr::~SoftOpExternalMgr()
{
}

void SoftOpExternalMgr::getAvailableOp(const SoftOpKey* &available) const
{
  available = SoftOpTable;
}

void SoftOpExternalMgr::getActiveOp(std::list<stage,std::list<alias> >&) const
{
}

void SoftOpExternalMgr::getActiveOp(std::list<stage,std::list<SoftOpInstance> >&)
{
}

void SoftOpExternalMgr::getActiveStageOp(stage,std::list<alias>&) const
{
}

void SoftOpExternalMgr::addOp(const SoftOpKey &aSoftOpKey,
			      const alias &anAlias,int stage)
{
  
}

void SoftOpExternalMgr::delOp(const alias&)
{

}

void SoftOpExternalMgr::getOpClass(const alias&,SoftOpInstance&) const
{

}

void SoftOpExternalMgr::setEndLinkTaskCallback(TaskEventCallback *aCbk)
{

}

void SoftOpExternalMgr::setEndSinkTaskCallBback(TaskEventCallback *aCbk)
{

}

void SoftOpExternalMgr::addTo(TaskMgr&,
			      int begin_stage,
			      int &last_link_task,int &last_sink_task)
{
  last_link_task = last_sink_task = begin_stage; // TODO
}
