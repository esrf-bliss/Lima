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
#include "SoftOpId.h"
using namespace lima;
#include "BackgroundSubstraction.h"

//-------------------- BACKGROUND SUBSTRACTION --------------------
				   
/** @brief small wrapper around BackgroundSubstraction Task
 */
SoftOpBackgroundSubstraction::SoftOpBackgroundSubstraction() : 
  SoftOpBaseClass()
{
  m_opt = new Tasks::BackgroundSubstraction();
  m_opt->setProcessingInPlace(false);
}

SoftOpBackgroundSubstraction::~SoftOpBackgroundSubstraction()
{
  m_opt->unref();
}

void SoftOpBackgroundSubstraction::setBackgroundImage(Data &anImage)
{
  m_opt->setBackgroundImageData(anImage);
}

bool SoftOpBackgroundSubstraction::addTo(TaskMgr &aMgr,int stage)
{
  aMgr.setLinkTask(stage,m_opt);
  return true;
}
//-------------------- BINNING --------------------
				   
/** @brief small wrapper around Binning Task
 */
SoftOpBinning::SoftOpBinning() : 
  SoftOpBaseClass()
{
  m_opt = new Tasks::Binning();
  m_opt->setProcessingInPlace(false);
}

SoftOpBinning::~SoftOpBinning()
{
  m_opt->unref();
}

void SoftOpBinning::setBinning(int x,int y)
{
  m_opt->mXFactor = x;
  m_opt->mYFactor = y;
}

bool SoftOpBinning::addTo(TaskMgr &aMgr,int stage)
{
  aMgr.setLinkTask(stage,m_opt);
  return true;
}

//-------------------- BPM --------------------
				   
/** @brief small wrapper around Bpm Task
 */
SoftOpBpm::SoftOpBpm() : 
  SoftOpBaseClass()
{
#ifndef WITHOUT_GSL
  m_manager = new Tasks::BpmManager(DEFAULT_HISTORY_SIZE);
  m_task = new Tasks::BpmTask(*m_manager);
#else
  throw Exception(Control,Error, "Bpm not available because it wasn't compiled with gsl support ",
		  __FILE__, __FUNCTION__, __LINE__,NULL);
#endif
 
}

SoftOpBpm::~SoftOpBpm()
{
#ifndef WITHOUT_GSL
  m_task->unref();
  m_manager->unref();
#endif
}


bool SoftOpBpm::addTo(TaskMgr &aMgr,int stage)
{
#ifndef WITHOUT_GSL
  aMgr.addSinkTask(stage,m_task);
  return true;
#endif
  return false;
}

void SoftOpBpm::prepare()
{
#ifndef WITHOUT_GSL
  m_manager->resetHistory();
#endif
}

//-------------------- FLATFIELDCORRECTION --------------------
				   
/** @brief small wrapper around FlatfieldCorrection Task
 */
SoftOpFlatfieldCorrection::SoftOpFlatfieldCorrection() : 
  SoftOpBaseClass()
{
  m_opt = new Tasks::FlatfieldCorrection();
  m_opt->setProcessingInPlace(false);
}

SoftOpFlatfieldCorrection::~SoftOpFlatfieldCorrection()
{
  m_opt->unref();
}

void SoftOpFlatfieldCorrection::setFlatFieldImage(Data &aData,bool normalize)
{
  m_opt->setFlatFieldImageData(aData,normalize);
}

bool SoftOpFlatfieldCorrection::addTo(TaskMgr &aMgr,int stage)
{
  aMgr.setLinkTask(stage,m_opt);
  return true;
}

//-------------------- FLIP --------------------
				   
/** @brief small wrapper around Flip Task
 */
SoftOpFlip::SoftOpFlip() : 
  SoftOpBaseClass()
{
  m_opt = new Tasks::Flip();
  m_opt->setProcessingInPlace(false);
}

SoftOpFlip::~SoftOpFlip()
{
  m_opt->unref();
}

void SoftOpFlip::setFlip(bool x,bool y)
{
  Tasks::Flip::FLIP_MODE flip_mode = Tasks::Flip::FLIP_NONE;
  if(x && y)
    flip_mode = Tasks::Flip::FLIP_ALL;
  else if(x)
    flip_mode = Tasks::Flip::FLIP_X;
  else if(y)
    flip_mode = Tasks::Flip::FLIP_Y;
    
  m_opt->setFlip(flip_mode);
}

bool SoftOpFlip::addTo(TaskMgr &aMgr,int stage)
{
  aMgr.setLinkTask(stage,m_opt);
  return true;
}

//-------------------- MASK --------------------
				   
/** @brief small wrapper around Mask Task
 */
SoftOpMask::SoftOpMask() : 
  SoftOpBaseClass()
{
  m_opt = new Tasks::Mask();
  m_opt->setProcessingInPlace(false);
}

SoftOpMask::~SoftOpMask()
{
  m_opt->unref();
}

void SoftOpMask::setMaskImage(Data &mask)
{
  m_opt->setMaskImageData(mask);
}

void SoftOpMask::setType(SoftOpMask::Type aType)
{
  m_opt->setType(aType == SoftOpMask::STANDARD ? 
		 Tasks::Mask::STANDARD : Tasks::Mask::DUMMY);
}

void SoftOpMask::getType(Type &aType) const
{
  Tasks::Mask::Type aMaskType;
  m_opt->getType(aMaskType);
  aType = aMaskType == Tasks::Mask::STANDARD ?
    SoftOpMask::STANDARD : SoftOpMask::DUMMY;
}

bool SoftOpMask::addTo(TaskMgr &aMgr,int stage)
{
  aMgr.setLinkTask(stage,m_opt);
  return true;
}

//-------------------- ROI COUNTERS --------------------

SoftOpRoiCounter::SoftOpRoiCounter() : 
  SoftOpBaseClass(),
  m_history_size(DEFAULT_HISTORY_SIZE),
  m_counter_status(-2)
{
}

SoftOpRoiCounter::~SoftOpRoiCounter()
{
  for(Name2ManagerNCounter::iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();++i)
    {
      i->second.second->unref();
      i->second.first->unref();
    }
}

void SoftOpRoiCounter::update(const std::list<RoiNameAndRoi> &named_rois)
{
  AutoMutex aLock(m_cond.mutex());
  for(std::list<RoiNameAndRoi>::const_iterator i = named_rois.begin();
      i != named_rois.end();++i)
    {
      Tasks::RoiCounterManager *aCounterMgrPt;
      Tasks::RoiCounterTask *aCounterTaskPt;
      _get_or_create(i->first,aCounterMgrPt,aCounterTaskPt);
      //update
      const Point &aOri = i->second.getTopLeft();
      const Size &aSize = i->second.getSize();
      aCounterTaskPt->setRoi(aOri.x,aOri.y,aSize.getWidth(),aSize.getHeight());
      aCounterTaskPt->setMask(m_mask);
    }
}
void SoftOpRoiCounter::update(const std::list<RoiNameAndArcRoi>& named_arc)
{
  AutoMutex aLock(m_cond.mutex());
  for(std::list<RoiNameAndArcRoi>::const_iterator i = named_arc.begin();
      i != named_arc.end();++i)
    {
      if(i->second.isEmpty()) continue;
      Tasks::RoiCounterManager* aCounterMgrPt;
      Tasks::RoiCounterTask* aCounterTaskPt;
      _get_or_create(i->first,aCounterMgrPt,aCounterTaskPt);
      //update
      double x,y;
      i->second.getCenter(x,y);
      double rayon1,rayon2;
      i->second.getRayons(rayon1,rayon2);
      double start,end;
      i->second.getAngles(start,end);
      aCounterTaskPt->setArcMask(x,y,
				 rayon1,rayon2,
				 start,end);
      aCounterTaskPt->setMask(m_mask);
    }
}
void SoftOpRoiCounter::setLut(const std::string& name,
			      const Point& origin,Data& lut)
{
  AutoMutex aLock(m_cond.mutex());
  Tasks::RoiCounterManager* aCounterMgrPt;
  Tasks::RoiCounterTask* aCounterTaskPt;
  _get_or_create(name,aCounterMgrPt,aCounterTaskPt);
  aCounterTaskPt->setLut(origin.x,origin.y,lut);
  aCounterTaskPt->setMask(m_mask);
}
void SoftOpRoiCounter::setLutMask(const std::string& name,
				  const Point& origin,Data& mask)
{
  AutoMutex aLock(m_cond.mutex());
  Tasks::RoiCounterManager* aCounterMgrPt;
  Tasks::RoiCounterTask* aCounterTaskPt;
  _get_or_create(name,aCounterMgrPt,aCounterTaskPt);
  aCounterTaskPt->setLutMask(origin.x,origin.y,mask);
  aCounterTaskPt->setMask(m_mask);
}
void SoftOpRoiCounter::getTasks(RoiNameAndTaskList& l)
{
  for(Name2ManagerNCounter::iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();++i)
    {
      const std::string& name = i->first;
      Tasks::RoiCounterTask* task = i->second.second;
      l.push_back(RoiNameAndTask(name,task));
    }
}
void SoftOpRoiCounter::names(std::list<std::string>& roi_names) const
{
  AutoMutex aLock(m_cond.mutex());
  for(Name2ManagerNCounter::const_iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();++i)
    roi_names.push_back(i->first);
}
void SoftOpRoiCounter::remove(const std::list<std::string>& names)
{
  AutoMutex aLock(m_cond.mutex());
  for(std::list<std::string>::const_iterator i = names.begin();
      i != names.end();++i)
    {
      Name2ManagerNCounter::iterator named_roi = m_manager_tasks.find(*i);
      if(named_roi != m_manager_tasks.end())
	{
	  named_roi->second.second->unref();
	  named_roi->second.first->unref();
	  m_manager_tasks.erase(named_roi);
	}
    }
}

/** @brief remove all roi
 */
void SoftOpRoiCounter::clearAllRoi()
{
  AutoMutex aLock(m_cond.mutex());
  for(Name2ManagerNCounter::iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();++i)
    {
      i->second.second->unref();
      i->second.first->unref();
    }
  m_manager_tasks.clear();
}

void SoftOpRoiCounter::clearCounterStatus()
{
  AutoMutex aLock(m_cond.mutex());
  m_counter_status = -2;
}
/** @brief get the counter status
 *  counter status indicate the status of roi counters
 *  if counterStatus == -2 acquisition didn't started
 *  if counterStatus == -1 acquisition has started
 *  counterStatus > -1 == nb image pending :
 *  i.e: if counterStatus == 10, it's mean image id 10 is in progress or finnished
 */
int SoftOpRoiCounter::getCounterStatus() const
{
  AutoMutex aLock(m_cond.mutex());
  return m_counter_status;
}
void SoftOpRoiCounter::setMask(Data &aMask)
{
  AutoMutex aLock(m_cond.mutex());
  for(Name2ManagerNCounter::iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();++i)
      i->second.second->setMask(aMask);
  m_mask = aMask;
}

void SoftOpRoiCounter::setBufferSize(int size)
{
  AutoMutex aLock(m_cond.mutex());
  for(Name2ManagerNCounter::iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();++i)
    i->second.first->resizeHistory(size);
  m_history_size = size;
}

void SoftOpRoiCounter::getBufferSize(int &size) const
{
  AutoMutex aLock(m_cond.mutex());
  size = m_history_size;
}

void SoftOpRoiCounter::readCounters(int from,std::list<RoiNameAndResults> &result) const
{
  AutoMutex aLock(m_cond.mutex());
  for(Name2ManagerNCounter::const_iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();++i)
    {
      result.push_back(RoiNameAndResults(i->first,std::list<Tasks::RoiCounterResult>()));
      RoiNameAndResults &roiAndResults = result.back();
      i->second.first->getHistory(roiAndResults.second,from);
    }
}

bool SoftOpRoiCounter::addTo(TaskMgr &aMgr,int stage)
{
  AutoMutex aLock(m_cond.mutex());
  for(Name2ManagerNCounter::iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();++i)
    aMgr.addSinkTask(stage,i->second.second);
  ++m_counter_status;
  return !m_manager_tasks.empty();
}

void SoftOpRoiCounter::prepare()
{
  AutoMutex aLock(m_cond.mutex());
   for(Name2ManagerNCounter::iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();++i)
     i->second.first->resetHistory();
   m_counter_status = -1;
}

void SoftOpRoiCounter::_get_or_create(const std::string& roi_name,
				      Tasks::RoiCounterManager*& aCounterMgrPt,
				      Tasks::RoiCounterTask*& aCounterTaskPt)
{
  Name2ManagerNCounter::iterator named_roi = m_manager_tasks.find(roi_name);
  if(named_roi == m_manager_tasks.end())
    {
      aCounterMgrPt = new Tasks::RoiCounterManager(m_history_size);
      aCounterTaskPt = new Tasks::RoiCounterTask(*aCounterMgrPt);
      m_manager_tasks[roi_name] = ManagerNCounter(aCounterMgrPt,aCounterTaskPt);
    }
  else
    {
      aCounterMgrPt = named_roi->second.first;
      aCounterTaskPt = named_roi->second.second;
    }
}
//-------------------- ROI TO SPECTRUM --------------------

SoftOpRoi2Spectrum::SoftOpRoi2Spectrum() : 
  SoftOpBaseClass(),
  m_history_size(DEFAULT_HISTORY_SIZE),
  m_counter_status(-2)
{
}

SoftOpRoi2Spectrum::~SoftOpRoi2Spectrum()
{
  for(std::list<ManagerNCounter>::iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();++i)
    {
      i->second->unref();
      i->first->unref();
    }
}

void SoftOpRoi2Spectrum::add(const std::list<Roi> &rois)
{
  AutoMutex aLock(m_cond.mutex());
  for(std::list<Roi>::const_iterator i = rois.begin();
      i != rois.end();++i)
    {
      Tasks::Roi2SpectrumManager *aCounterMgrPt = new Tasks::Roi2SpectrumManager(m_history_size);
      Tasks::Roi2SpectrumTask *aCounterTaskPt = new Tasks::Roi2SpectrumTask(*aCounterMgrPt);

      const Point &aOri = i->getTopLeft();
      const Size &aSize = i->getSize();
      aCounterTaskPt->setRoi(aOri.x,aOri.y,aSize.getWidth(),aSize.getHeight());
      //aCounterTaskPt->setMask(m_mask);

      m_manager_tasks.push_back(ManagerNCounter(aCounterMgrPt,aCounterTaskPt));
    }
}
void SoftOpRoi2Spectrum::set(const std::list<Roi> &rois)
{
  clearAllRoi();
  add(rois);
}
/** @brief return the list of roi set
 */
void SoftOpRoi2Spectrum::get(std::list<Roi> &aReturnList) const
{
  AutoMutex aLock(m_cond.mutex());
  for(std::list<ManagerNCounter>::const_iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();++i)
    {
      int x,y,width,height;
      i->second->getRoi(x,y,width,height);
      aReturnList.push_back(Roi(x,y,width,height));
    }
}
/** @brief remove roi with roi index.
    roi index start at 1
*/
void SoftOpRoi2Spectrum::del(const std::list<int> &roiIds)
{
  std::list<int> aTmpList = roiIds;
  aTmpList.sort();
  
  AutoMutex aLock(m_cond.mutex());
  std::list<int>::iterator i = aTmpList.begin();
  std::list<ManagerNCounter>::iterator k = m_manager_tasks.begin();
  for(int index = 0;i != aTmpList.end() && k != m_manager_tasks.end();++i)
    {
      while(index != *i && k != m_manager_tasks.end())
	++k,++index;

      if(index == *i)
	{
	  k->second->unref();
	  k->first->unref();
	  k = m_manager_tasks.erase(k),++index;
	}
    }
}

/** @brief remove all roi
 */
void SoftOpRoi2Spectrum::clearAllRoi()
{
  AutoMutex aLock(m_cond.mutex());
  for(std::list<ManagerNCounter>::iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();i = m_manager_tasks.erase(i))
    {
      i->second->unref();
      i->first->unref();
    }
}

void SoftOpRoi2Spectrum::getRoiMode(std::list<int> &aReturnList) const
{
  AutoMutex aLock(m_cond.mutex());
  for(std::list<ManagerNCounter>::const_iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();++i)
      aReturnList.push_back(i->second->getMode());
}

void SoftOpRoi2Spectrum::setRoiMode(int roiId,int mode)
{
  AutoMutex aLock(m_cond.mutex());
  int rId = 0;
  for(std::list<ManagerNCounter>::const_iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();++i,++rId)
    {
      if(rId == roiId)
	{
	  i->second->setMode(mode == Tasks::Roi2SpectrumTask::COLUMN_SUM ?
			     Tasks::Roi2SpectrumTask::COLUMN_SUM :
			     Tasks::Roi2SpectrumTask::LINES_SUM);
	  break;
	}
    }
}
void SoftOpRoi2Spectrum::clearCounterStatus()
{
  AutoMutex aLock(m_cond.mutex());
  m_counter_status = -2;
}
/** @brief get the counter status
 *  counter status indicate the status of roi counters
 *  if counterStatus == -2 acquisition didn't started
 *  if counterStatus == -1 acquisition has started
 *  counterStatus > -1 == nb image pending :
 *  i.e: if counterStatus == 10, it's mean image id 10 is in progress or finnished
 */
int SoftOpRoi2Spectrum::getCounterStatus() const
{
  AutoMutex aLock(m_cond.mutex());
  return m_counter_status;
}

// void SoftOpRoi2Spectrum::setMask(Data &aMask)
// {
//   AutoMutex aLock(m_cond.mutex());
//   for(std::list<ManagerNCounter>::iterator i = m_manager_tasks.begin();
//       i != m_manager_tasks.end();i = m_manager_tasks.erase(i))
//       i->second->setMask(aMask);
//   m_mask = aMask;
// }

void SoftOpRoi2Spectrum::setBufferSize(int size)
{
  AutoMutex aLock(m_cond.mutex());
  for(std::list<ManagerNCounter>::iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();++i)
    i->first->resizeHistory(size);
  m_history_size = size;
}

void SoftOpRoi2Spectrum::getBufferSize(int &size) const
{
  AutoMutex aLock(m_cond.mutex());
  size = m_history_size;
}

void SoftOpRoi2Spectrum::readCounters(int from,std::list<RoiIdAndResults> &result) const
{
  AutoMutex aLock(m_cond.mutex());
  int roiIndex = 0;
  for(std::list<ManagerNCounter>::const_iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();++i,++roiIndex)
    {
      result.push_back(RoiIdAndResults(roiIndex,std::list<Tasks::Roi2SpectrumResult>()));
      RoiIdAndResults &roiAndResults = result.back();
      i->first->getHistory(roiAndResults.second,from);
    }
}

void SoftOpRoi2Spectrum::createImage(int roiId,int &from,Data &aData) const
{
  AutoMutex aLock(m_cond.mutex());
  int roiIndex = 0;
  for(std::list<ManagerNCounter>::const_iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();++i,++roiIndex)
    {
      if(roiIndex == roiId)
	{
	  std::list<Tasks::Roi2SpectrumResult> aResult;
	  i->first->getHistory(aResult,from);
	  if(!aResult.empty())
	    {
	      Tasks::Roi2SpectrumResult &firstResult = aResult.front();
	      int aSize = firstResult.spectrum.size();
	      from = firstResult.frameNumber;
	      if(aSize)
		{
		  Buffer *aBuffer = new Buffer(aSize * aResult.size());
		  char *dataPt = (char*)aBuffer->data;
		  for(std::list<Tasks::Roi2SpectrumResult>::iterator k = aResult.begin();
		      k != aResult.end();++k,dataPt += aSize)
		    memcpy(dataPt,k->spectrum.data(),aSize);
		  aData.type = firstResult.spectrum.type;
		  aData.dimensions = firstResult.spectrum.dimensions;
		  aData.dimensions.push_back(aResult.size());
		  aData.setBuffer(aBuffer);
		  aBuffer->unref();
		}
	    }
	  break;
	}
    }
}
bool SoftOpRoi2Spectrum::addTo(TaskMgr &aMgr,int stage)
{
  AutoMutex aLock(m_cond.mutex());
  for(std::list<ManagerNCounter>::iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();++i)
    aMgr.addSinkTask(stage,i->second);
  ++m_counter_status;
  return !m_manager_tasks.empty();
}

void SoftOpRoi2Spectrum::prepare()
{
  AutoMutex aLock(m_cond.mutex());
   for(std::list<ManagerNCounter>::iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();++i)
     i->first->resetHistory();
   m_counter_status = -1;
}

//-------------------- SOFTROI --------------------
				   
/** @brief small wrapper around SoftRoi Task
 */
SoftOpSoftRoi::SoftOpSoftRoi() : 
  SoftOpBaseClass()
{
  m_opt = new Tasks::SoftRoi();
  m_opt->setProcessingInPlace(false);
}

SoftOpSoftRoi::~SoftOpSoftRoi()
{
  m_opt->unref();
}

void SoftOpSoftRoi::setRoi(int x,int y,int width,int height)
{
  m_opt->setRoi(x,x+width,y,y+height);
}

bool SoftOpSoftRoi::addTo(TaskMgr &aMgr,int stage)
{
  aMgr.setLinkTask(stage,m_opt);
  return true;
}

//-------------------------- SOFTCALLBACK --------------------------
SoftCallback::~SoftCallback()
{
  if(m_callback_gen)
    m_callback_gen->unregisterCallback(*this);
}

//--------------------- SOFTPREPARECALLBACKGEN ---------------------

SoftPrepareCallbackGen::SoftPrepareCallbackGen() : m_cb(NULL)
{
  DEB_CONSTRUCTOR();
}

SoftPrepareCallbackGen::~SoftPrepareCallbackGen()
{
  DEB_DESTRUCTOR();
}

void SoftPrepareCallbackGen::registerCallback(SoftCallback& cb)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(&cb,m_cb);

  if(m_cb)
    THROW_CTL_ERROR(InvalidValue) << "Software callback has already registered";

  m_cb = &cb;
  cb.m_callback_gen = this;
}

void SoftPrepareCallbackGen::unregisterCallback(SoftCallback& cb)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(&cb,m_cb);

  if(m_cb != &cb)
    THROW_CTL_ERROR(InvalidValue) << "Requested Callback not registered";

  m_cb = NULL;
  cb.m_callback_gen = NULL;
}

void SoftPrepareCallbackGen::prepare_cb()
{
  DEB_MEMBER_FUNCT();
  if(!m_cb)
    DEB_TRACE() << "No cb registered";
  else
    m_cb->prepare();
}

//------------------------ SOFTUSERLINKTASK -------------------------

SoftUserLinkTask::SoftUserLinkTask() :
  SoftOpBaseClass(),
  SoftPrepareCallbackGen(),
  m_link_task(NULL)
{
}

SoftUserLinkTask::~SoftUserLinkTask()
{
  setLinkTask(NULL);
}

void SoftUserLinkTask::setLinkTask(LinkTask *aTaskPt)
{
  if(aTaskPt)
    aTaskPt->ref();

  if(m_link_task)
    m_link_task->unref();

  m_link_task = aTaskPt;
}

bool SoftUserLinkTask::addTo(TaskMgr &aMgr,int stage)
{
  if(m_link_task)
    aMgr.setLinkTask(stage,m_link_task);
  return !!m_link_task;
}

//------------------------ SoftUserSinkTask ------------------------

SoftUserSinkTask::SoftUserSinkTask() :
  SoftOpBaseClass(),
  SoftPrepareCallbackGen(),
  m_sink_task(NULL)
{
}

SoftUserSinkTask::~SoftUserSinkTask()
{
  setSinkTask(NULL);
}

void SoftUserSinkTask::setSinkTask(SinkTaskBase *aTaskPt)
{
  if(aTaskPt)
    aTaskPt->ref();

  if(m_sink_task)
    m_sink_task->unref();

  m_sink_task = aTaskPt;
}

bool SoftUserSinkTask::addTo(TaskMgr &aMgr,int stage)
{
  if(m_sink_task)
    aMgr.addSinkTask(stage,m_sink_task);
  return !!m_sink_task;
}
