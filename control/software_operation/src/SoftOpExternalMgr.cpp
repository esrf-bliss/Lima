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
#include "SoftOpExternalMgr.h"
using namespace lima;

static SoftOpKey SoftOpTable[] = {
  SoftOpKey(BACKGROUNDSUBSTRACTION,"Background substraction"),
  SoftOpKey(BINNING,"Binning"),
  SoftOpKey(BPM,"Bpm"),
  SoftOpKey(FLATFIELDCORRECTION,"Flat field correction"),
  SoftOpKey(FLIP,"Flip"),
  SoftOpKey(MASK,"Mask"),
  SoftOpKey(ROICOUNTERS,"Roi counters"),
  SoftOpKey(SOFTROI,"Software roi"),
  SoftOpKey(USER_LINK_TASK,"User link task"),
  SoftOpKey(USER_SINK_TASK,"User sink task"),
  SoftOpKey()
};

static SoftOpKey getSoftOpKey(SoftOpId anId)
{
  for(unsigned int i = 0;i < sizeof(SoftOpTable);++i)
    {
      if(SoftOpTable[i].m_id == anId)
	return SoftOpTable[i];
    }
  return SoftOpKey();
}

SoftOpExternalMgr::SoftOpExternalMgr() :
  m_end_link_callback(NULL),
  m_end_sink_callback(NULL)
{
}

SoftOpExternalMgr::~SoftOpExternalMgr()
{
  if(m_end_link_callback)
    m_end_link_callback->unref();
  if(m_end_sink_callback)
    m_end_sink_callback->unref();

  for(Stage2Instance::iterator i = m_stage2instance.begin();
      i != m_stage2instance.end();++i)
    for(std::list<SoftOpInstance>::iterator k = i->second.begin();
	k != i->second.end();k = i->second.erase(k))
      delete k->m_opt;
}

void SoftOpExternalMgr::getAvailableOp(const SoftOpKey* &available) const
{
  available = SoftOpTable;
}

void SoftOpExternalMgr::getActiveOp(std::map<stage,std::list<alias> > &activeOp) const
{
  AutoMutex aLock(m_cond.mutex());
  for(Stage2Instance::const_iterator i = m_stage2instance.begin();
      i != m_stage2instance.end();++i)
    {
      std::pair<std::map<stage,std::list<alias> >::iterator,bool> result =
	activeOp.insert(std::pair<stage,std::list<alias> >(i->first,std::list<alias>()));
      std::list<alias> &aliasList = result.first->second;

      for(std::list<SoftOpInstance>::const_iterator k = i->second.begin();
	  k != i->second.end();++k)
	aliasList.push_back(k->m_alias);
    }
}

void SoftOpExternalMgr::getActiveStageOp(stage aStage,std::list<alias> &activeOp) const
{
  AutoMutex aLock(m_cond.mutex());
  Stage2Instance::const_iterator i = m_stage2instance.find(aStage);
  if(i != m_stage2instance.end())
    for(std::list<SoftOpInstance>::const_iterator k = i->second.begin();
	  k != i->second.end();++k)
      activeOp.push_back(k->m_alias);
}

void SoftOpExternalMgr::addOp(SoftOpId aSoftOpId,
			      const alias &anAlias,int aStage,
			      SoftOpInstance &anInstance)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR3(aSoftOpId,anAlias,aStage);

  AutoMutex aLock(m_cond.mutex());
  _checkIfPossible(aSoftOpId,aStage);
  SoftOpInstance newInstance(getSoftOpKey(aSoftOpId),anAlias);
  
  switch(aSoftOpId)
    {
    case ROICOUNTERS:
      newInstance.m_opt = new SoftOpRoiCounter();
      break;
    case BPM:
      newInstance.m_opt = new SoftOpBpm();
      break;
    case BACKGROUNDSUBSTRACTION:
      newInstance.m_opt = new SoftOpBackgroundSubstraction();
      newInstance.m_linkable = true;
      break;
    case BINNING:
      newInstance.m_opt = new SoftOpBinning();
      newInstance.m_linkable = true;
      break;
    case FLATFIELDCORRECTION:
      newInstance.m_opt = new SoftOpFlatfieldCorrection();
      newInstance.m_linkable = true;
      break;
    case FLIP:
      newInstance.m_opt = new SoftOpFlip();
      newInstance.m_linkable = true;
      break;
    case MASK:
      newInstance.m_opt = new SoftOpMask();
      newInstance.m_linkable = true;
      break;
    case SOFTROI:
      newInstance.m_opt = new SoftOpSoftRoi();
      newInstance.m_linkable = true;
      break;
    case USER_LINK_TASK:
      newInstance.m_opt = new SoftUserLinkTask();
      newInstance.m_linkable = true;
      break;
    case USER_SINK_TASK:
      newInstance.m_opt = new SoftUserSinkTask();
      break;
    default:
      throw LIMA_CTL_EXC(InvalidValue,"Not yet managed");
    }
  std::pair<Stage2Instance::iterator,bool> aResult = 
    m_stage2instance.insert(std::pair<stage,std::list<SoftOpInstance> >(aStage,std::list<SoftOpInstance>()));
  aResult.first->second.push_back(newInstance);
  anInstance = newInstance;
}

void SoftOpExternalMgr::delOp(const alias &anAlias)
{
   AutoMutex aLock(m_cond.mutex());
   for(Stage2Instance::iterator i = m_stage2instance.begin();
      i != m_stage2instance.end();++i)
    {
      for(std::list<SoftOpInstance>::iterator k = i->second.begin();
	  k != i->second.end();++k)
	{
	  if(k->m_alias == anAlias)
	    {
	      delete k->m_opt;
	      i->second.erase(k);
	      if(i->second.empty())
		m_stage2instance.erase(i);
	      return;
	    }
	}
    }
}

void SoftOpExternalMgr::getOpClass(const alias &anAlias,
				   SoftOpInstance &aSoftOpInstance) const
{
  AutoMutex aLock(m_cond.mutex());
  for(Stage2Instance::const_iterator i = m_stage2instance.begin();
      i != m_stage2instance.end();++i)
    {
      for(std::list<SoftOpInstance>::const_iterator k = i->second.begin();
	  k != i->second.end();++k)
	{
	  if(k->m_alias == anAlias)
	    {
	      aSoftOpInstance = *k;
	      return;
	    }
	}
    }
}

void SoftOpExternalMgr::setEndLinkTaskCallback(TaskEventCallback *aCbk)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(aCbk);

  if(m_end_link_callback)
    m_end_link_callback->unref();
  m_end_link_callback = aCbk;
  if(m_end_link_callback)
    m_end_link_callback->ref();
}

void SoftOpExternalMgr::setEndSinkTaskCallback(TaskEventCallback *aCbk)
{
  if(m_end_sink_callback)
    m_end_sink_callback->unref();
  m_end_sink_callback = aCbk;
  if(m_end_sink_callback)
    m_end_sink_callback->ref();
}

void SoftOpExternalMgr::addTo(TaskMgr &aTaskMgr,
			      int begin_stage,
			      int &last_link_task,int &last_sink_task)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(begin_stage);

  AutoMutex aLock(m_cond.mutex());
  last_link_task = last_sink_task = -1;
  int nextStage = begin_stage;
  for(Stage2Instance::iterator i = m_stage2instance.begin();
      i != m_stage2instance.end();++i,++nextStage)
    {
      for(std::list<SoftOpInstance>::const_iterator k = i->second.begin();
	  k != i->second.end();++k)
	{
	  if(k->m_linkable)
	    last_link_task = nextStage;
	  else
	    last_sink_task = nextStage;
	  k->m_opt->addTo(aTaskMgr,nextStage);
	}
    }
  std::pair<int,LinkTask*> aLastLink(0,NULL);
  std::pair<int,SinkTaskBase*> aLastSink(0,NULL);

  aTaskMgr.getLastTask(aLastLink,aLastSink);

  if(aLastLink.first >= begin_stage)
    aLastLink.second->setEventCallback(m_end_link_callback);
  if(aLastSink.first >= begin_stage)
    {
      SinkTaskBase *aDummyPt = new SinkTaskBase();
      aDummyPt->setEventCallback(m_end_sink_callback);
      aTaskMgr.addSinkTask(aLastSink.first + 1,aDummyPt);
      aDummyPt->unref();
    }
  DEB_RETURN() << DEB_VAR2(last_link_task,last_sink_task);
}

void SoftOpExternalMgr::_checkIfPossible(SoftOpId aSoftOpId,
					 int stage)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(aSoftOpId,stage);

  bool checkLinkable = false;
  switch(aSoftOpId) 
    {
    case ROICOUNTERS:
    case BPM:
    case USER_SINK_TASK:
      break;			// always possible
    case BACKGROUNDSUBSTRACTION:
    case BINNING:
    case FLATFIELDCORRECTION:
    case FLIP:
    case MASK:
    case SOFTROI:
    case USER_LINK_TASK:
      checkLinkable = true;
      break;
    default:
      throw LIMA_CTL_EXC(InvalidValue,"Not yet managed");
    }

  DEB_TRACE() << DEB_VAR1(checkLinkable);

  if(checkLinkable)
    {
      Stage2Instance::iterator i = m_stage2instance.find(stage);
      if(i != m_stage2instance.end())
	{
	  for(std::list<SoftOpInstance>::iterator k = i->second.begin();
	      k != i->second.end();++k)
	    {
	      if(k->m_linkable)
		{
		  char buffer[256];
		  snprintf(buffer,sizeof(buffer),"%s task  %s is already active on that level",
			   k->m_key.m_name,k->m_alias.c_str());
		  throw LIMA_CTL_EXC(Error,buffer);
		}
	    }
	}
    }
}

void SoftOpExternalMgr::isTaskActive(bool &linkTaskFlag,bool &sinkTaskFlag) const
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  linkTaskFlag = sinkTaskFlag = false;
  for(Stage2Instance::const_iterator i = m_stage2instance.begin();
      i != m_stage2instance.end() && (!linkTaskFlag || !sinkTaskFlag);++i)
    {
      for(std::list<SoftOpInstance>::const_iterator k = i->second.begin();
	  k != i->second.end() && (!linkTaskFlag || !sinkTaskFlag);++k)
	{
	 if(k->m_linkable)
	    linkTaskFlag = true;
	  else
	    sinkTaskFlag = true;
	}
    }
  DEB_RETURN() << DEB_VAR2(linkTaskFlag,sinkTaskFlag);
}


void SoftOpExternalMgr::prepare()
{
  AutoMutex aLock(m_cond.mutex());
  for(Stage2Instance::iterator i = m_stage2instance.begin();
      i != m_stage2instance.end();++i)
    for(std::list<SoftOpInstance>::iterator k = i->second.begin();
	k != i->second.end();++k)
      k->m_opt->prepare();
}
