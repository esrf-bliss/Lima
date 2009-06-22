#include "SoftOpExternalMgr.h"
using namespace lima;

SoftOpExternalMgr::SoftOpExternalMgr()
{
}

SoftOpExternalMgr::~SoftOpExternalMgr()
{
}

void SoftOpExternalMgr::getAvailableOp(const SoftOpKey[]&) const
{
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
