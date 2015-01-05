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
#include "lima/SoftOpId.h"
using namespace lima;
#include "processlib/BackgroundSubstraction.h"

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
  m_history_size(DEFAULT_HISTORY_SIZE)
{
  m_task_manager.setCompatFormat("roi_%d");
}

SoftOpRoiCounter::~SoftOpRoiCounter()
{
}

void SoftOpRoiCounter::updateRois(const std::list<RoiNameAndRoi> &named_rois)
{
  AutoMutex aLock(m_cond.mutex());
  for(std::list<RoiNameAndRoi>::const_iterator i = named_rois.begin();
      i != named_rois.end();++i)
    {
      SoftManager *aCounterMgrPt;
      SoftTask *aCounterTaskPt;
      _get_or_create(i->first,aCounterMgrPt,aCounterTaskPt);
      //update
      const Point &aOri = i->second.getTopLeft();
      const Size &aSize = i->second.getSize();
      aCounterTaskPt->setRoi(aOri.x,aOri.y,aSize.getWidth(),aSize.getHeight());
      aCounterTaskPt->setMask(m_mask);
    }
}
void SoftOpRoiCounter::updateArcRois(const std::list<RoiNameAndArcRoi>& named_arc)
{
  AutoMutex aLock(m_cond.mutex());
  for(std::list<RoiNameAndArcRoi>::const_iterator i = named_arc.begin();
      i != named_arc.end();++i)
    {
      if(i->second.isEmpty()) continue;
      SoftManager *aCounterMgrPt;
      SoftTask *aCounterTaskPt;
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
  SoftManager *aCounterMgrPt;
  SoftTask *aCounterTaskPt;
  _get_or_create(name,aCounterMgrPt,aCounterTaskPt);
  aCounterTaskPt->setLut(origin.x,origin.y,lut);
  aCounterTaskPt->setMask(m_mask);
}
void SoftOpRoiCounter::setLutMask(const std::string& name,
				  const Point& origin,Data& mask)
{
  AutoMutex aLock(m_cond.mutex());
  SoftManager *aCounterMgrPt;
  SoftTask *aCounterTaskPt;
  _get_or_create(name,aCounterMgrPt,aCounterTaskPt);
  aCounterTaskPt->setLutMask(origin.x,origin.y,mask);
  aCounterTaskPt->setMask(m_mask);
}
void SoftOpRoiCounter::getRois(std::list<RoiNameAndRoi>& names_rois) const
{
  AutoMutex aLock(m_cond.mutex());
  _get_rois_of_type<SoftTask::SQUARE, Roi>(names_rois);
}
void SoftOpRoiCounter::getArcRois(std::list<RoiNameAndArcRoi>& names_rois) const
{
  AutoMutex aLock(m_cond.mutex());
  _get_rois_of_type<SoftTask::ARC, ArcRoi>(names_rois);
}
void SoftOpRoiCounter::getTypes(std::list<RoiNameAndType>& names_types) const
{
  AutoMutex aLock(m_cond.mutex());
  for(NameMapConstIterator i = m_task_manager.begin();
      i != m_task_manager.end();++i) {
    SoftTask *task = i->second.second;
    SoftTask::type roi_type;
    task->getType(roi_type);
    names_types.push_back(RoiNameAndType(i->first, roi_type));
  }
}
void SoftOpRoiCounter::getTasks(RoiNameAndTaskList& l)
{
  AutoMutex aLock(m_cond.mutex());
  m_task_manager.getTasks(l);
}
void SoftOpRoiCounter::getNames(std::list<std::string>& roi_names) const
{
  AutoMutex aLock(m_cond.mutex());
  m_task_manager.getNames(roi_names);
}
void SoftOpRoiCounter::removeRois(const std::list<std::string>& names)
{
  AutoMutex aLock(m_cond.mutex());
  m_task_manager.remove(names);
}

/** @brief remove all roi
 */
void SoftOpRoiCounter::clearAllRois()
{
  AutoMutex aLock(m_cond.mutex());
  m_task_manager.clearAll();
}

void SoftOpRoiCounter::clearCounterStatus()
{
  AutoMutex aLock(m_cond.mutex());
  m_task_manager.clearCounterStatus();
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
  return m_task_manager.getCounterStatus();
}
void SoftOpRoiCounter::setMask(Data& aMask)
{
  AutoMutex aLock(m_cond.mutex());
  for(NameMapIterator i = m_task_manager.begin();
       i != m_task_manager.end();++i)
      i->second.second->setMask(aMask);
  m_mask = aMask;
}

void SoftOpRoiCounter::setBufferSize(int size)
{
  AutoMutex aLock(m_cond.mutex());
  for(NameMapIterator i = m_task_manager.begin();
      i != m_task_manager.end();++i)
    i->second.first->resizeHistory(size);
  m_history_size = size;
}

void SoftOpRoiCounter::getBufferSize(int& size) const
{
  AutoMutex aLock(m_cond.mutex());
  size = m_history_size;
}

void SoftOpRoiCounter::readCounters(int from,
				    std::list<RoiNameAndResults>& result) const
{
  AutoMutex aLock(m_cond.mutex());
  for(NameMapConstIterator i = m_task_manager.begin();
      i != m_task_manager.end();++i) {
    typedef std::list<Tasks::RoiCounterResult> RoiCounterResults;
    result.push_back(RoiNameAndResults(i->first, RoiCounterResults()));
    RoiNameAndResults& roiAndResults = result.back();
    i->second.first->getHistory(roiAndResults.second, from);
  }
}

bool SoftOpRoiCounter::addTo(TaskMgr &aMgr,int stage)
{
  AutoMutex aLock(m_cond.mutex());
  return m_task_manager.addTo(aMgr, stage);
}

void SoftOpRoiCounter::prepare()
{
  AutoMutex aLock(m_cond.mutex());
  for(NameMapIterator i = m_task_manager.begin();
      i != m_task_manager.end();++i)
     i->second.first->resetHistory();
  m_task_manager.prepareCounterStatus();
}

void SoftOpRoiCounter::_get_or_create(const std::string& roi_name,
				      SoftManager *& aCounterMgrPt,
				      SoftTask *& aCounterTaskPt)
{
  NameMapIterator i = m_task_manager.find(roi_name);
  if (i == m_task_manager.end()) {
    aCounterMgrPt = new SoftManager(m_history_size);
    aCounterTaskPt = new SoftTask(*aCounterMgrPt);
    TaskMap::ManagerAndTask man_task(aCounterMgrPt, aCounterTaskPt);
    m_task_manager.insert(roi_name, man_task);
  } else {
    aCounterMgrPt = i->second.first;
    aCounterTaskPt = i->second.second;
  }
}
//-------------------- ROI TO SPECTRUM --------------------

SoftOpRoi2Spectrum::SoftOpRoi2Spectrum() : 
  SoftOpBaseClass(),
  m_history_size(DEFAULT_HISTORY_SIZE)
{
  m_task_manager.setCompatFormat("roi_%d");
}

SoftOpRoi2Spectrum::~SoftOpRoi2Spectrum()
{
}

void SoftOpRoi2Spectrum::updateRois(const std::list<RoiNameAndRoi> &named_rois)
{
  AutoMutex aLock(m_cond.mutex());
  for(std::list<RoiNameAndRoi>::const_iterator i = named_rois.begin();
      i != named_rois.end();++i)
    {
      SoftManager *aRoi2SpectrumMgrPt;
      SoftTask *aRoi2SpectrumTaskPt;
      _get_or_create(i->first,aRoi2SpectrumMgrPt,aRoi2SpectrumTaskPt);
      //update
      const Point &aOri = i->second.getTopLeft();
      const Size &aSize = i->second.getSize();
      aRoi2SpectrumTaskPt->setRoi(aOri.x,aOri.y,aSize.getWidth(),aSize.getHeight());
      //aRoi2SpectrumTaskPt->setMask(m_mask);
    }
}
void SoftOpRoi2Spectrum::getRois(std::list<RoiNameAndRoi>& named_rois) const
{
  AutoMutex aLock(m_cond.mutex());
  for(NameMapConstIterator i = m_task_manager.begin();
      i != m_task_manager.end();++i)
    {
      int x,y,width,height;
      i->second.second->getRoi(x,y,width,height);
      RoiNameAndRoi name_roi(i->first,  Roi(x,y,width,height));
      named_rois.push_back(name_roi);
    }
}
void SoftOpRoi2Spectrum::getTasks(RoiNameAndTaskList& l)
{
  AutoMutex aLock(m_cond.mutex());
  m_task_manager.getTasks(l);
}
void SoftOpRoi2Spectrum::getNames(std::list<std::string>& roi_names) const
{
  AutoMutex aLock(m_cond.mutex());
  m_task_manager.getNames(roi_names);
}
void SoftOpRoi2Spectrum::removeRois(const std::list<std::string>& names)
{
  AutoMutex aLock(m_cond.mutex());
  m_task_manager.remove(names);
}

/** @brief remove all roi
 */
void SoftOpRoi2Spectrum::clearAllRois()
{
  AutoMutex aLock(m_cond.mutex());
  m_task_manager.clearAll();
}

void SoftOpRoi2Spectrum::getRoiModes(std::list<RoiNameAndMode>& roi_modes) const
{
  AutoMutex aLock(m_cond.mutex());
  for(NameMapConstIterator i = m_task_manager.begin();
      i != m_task_manager.end();++i)
    {
      RoiNameAndMode name_mode(i->first, i->second.second->getMode());
      roi_modes.push_back(name_mode);
    }
}

void SoftOpRoi2Spectrum::setRoiModes(const std::list<RoiNameAndMode>& roi_modes)
{
  AutoMutex aLock(m_cond.mutex());
  for(std::list<RoiNameAndMode>::const_iterator i = roi_modes.begin();
      i != roi_modes.end();++i)
    {
      SoftManager *aRoi2SpectrumMgrPt;
      SoftTask *aRoi2SpectrumTaskPt;
      _get_or_create(i->first,aRoi2SpectrumMgrPt,aRoi2SpectrumTaskPt);
      aRoi2SpectrumTaskPt->setMode((i->second == SoftTask::COLUMN_SUM) ?
			      SoftTask::COLUMN_SUM : SoftTask::LINES_SUM);
    }
}
void SoftOpRoi2Spectrum::clearCounterStatus()
{
  AutoMutex aLock(m_cond.mutex());
  m_task_manager.clearCounterStatus();
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
  return m_task_manager.getCounterStatus();
}

// void SoftOpRoi2Spectrum::setMask(Data &aMask)
// {
//   AutoMutex aLock(m_cond.mutex());
//   for(NameMapIterator i = m_task_manager.begin();
//       i != m_task_manager.end();++i)
//       i->second.second->setMask(aMask);
//   m_mask = aMask;
// }

void SoftOpRoi2Spectrum::setBufferSize(int size)
{
  AutoMutex aLock(m_cond.mutex());
  for(NameMapIterator i = m_task_manager.begin();
      i != m_task_manager.end();++i)
    i->second.first->resizeHistory(size);
  m_history_size = size;
}

void SoftOpRoi2Spectrum::getBufferSize(int &size) const
{
  AutoMutex aLock(m_cond.mutex());
  size = m_history_size;
}

void SoftOpRoi2Spectrum::readCounters(int from,
				      std::list<RoiNameAndResults> &result) const
{
  AutoMutex aLock(m_cond.mutex());
  for(NameMapConstIterator i = m_task_manager.begin();
      i != m_task_manager.end();++i)
    {
      typedef std::list<Tasks::Roi2SpectrumResult> ResultList;
      result.push_back(RoiNameAndResults(i->first, ResultList()));
      RoiNameAndResults &name_res = result.back();
      i->second.first->getHistory(name_res.second, from);
    }
}

void SoftOpRoi2Spectrum::createImage(std::string roi_name, int& from,
				     Data& aData) const
{
  AutoMutex aLock(m_cond.mutex());
  NameMapConstIterator i = m_task_manager.find(roi_name);
  if(i == m_task_manager.end())
    return;

  std::list<Tasks::Roi2SpectrumResult> aResult;
  i->second.first->getHistory(aResult,from);
  if(aResult.empty())
    return;

  Tasks::Roi2SpectrumResult &firstResult = aResult.front();
  int aSize = firstResult.spectrum.size();
  if(!aSize)
    return;

  from = firstResult.frameNumber;
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
bool SoftOpRoi2Spectrum::addTo(TaskMgr &aMgr,int stage)
{
  AutoMutex aLock(m_cond.mutex());
  return m_task_manager.addTo(aMgr, stage);
}

void SoftOpRoi2Spectrum::prepare()
{
  AutoMutex aLock(m_cond.mutex());
  for(NameMapIterator i = m_task_manager.begin();
      i != m_task_manager.end();++i)
    i->second.first->resetHistory();
  m_task_manager.prepareCounterStatus();
}

void SoftOpRoi2Spectrum::_get_or_create(const std::string& roi_name,
					SoftManager *& aRoi2SpectrumMgrPt,
					SoftTask *& aRoi2SpectrumTaskPt)
{
  NameMapIterator i = m_task_manager.find(roi_name);
  if (i == m_task_manager.end()) {
    aRoi2SpectrumMgrPt = new SoftManager(m_history_size);
    aRoi2SpectrumTaskPt = new SoftTask(*aRoi2SpectrumMgrPt);
    TaskMap::ManagerAndTask man_task(aRoi2SpectrumMgrPt, aRoi2SpectrumTaskPt);
    m_task_manager.insert(roi_name, man_task);
  } else {
    aRoi2SpectrumMgrPt = i->second.first;
    aRoi2SpectrumTaskPt = i->second.second;
  }
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

//-------------------- PEAK FINDER --------------------

SoftOpPeakFinder::SoftOpPeakFinder() : 
  SoftOpBaseClass(),
  m_history_size(DEFAULT_HISTORY_SIZE)
{
  SoftManager *aCounterMgrPt;
  SoftTask *aCounterTaskPt;
  aCounterMgrPt = new SoftManager(m_history_size);
  aCounterTaskPt = new SoftTask(*aCounterMgrPt);
  TaskMap::ManagerAndTask man_task(aCounterMgrPt, aCounterTaskPt);
  m_task_manager.insert("my_name",man_task);
}

SoftOpPeakFinder::~SoftOpPeakFinder()
{
  m_opt->unref();
}
void SoftOpPeakFinder::setMask(Data& aMask)
{
}

bool SoftOpPeakFinder::addTo(TaskMgr &aMgr,int stage)
{
  AutoMutex aLock(m_cond.mutex());
  return m_task_manager.addTo(aMgr, stage);
}

void SoftOpPeakFinder::clearCounterStatus()
{
  AutoMutex aLock(m_cond.mutex());
  m_task_manager.clearCounterStatus();
}

/** @brief get the counter status
 *  counter status indicate the status of roi counters
 *  if counterStatus == -2 acquisition didn't started
 *  if counterStatus == -1 acquisition has started
 *  counterStatus > -1 == nb image pending :
 *  i.e: if counterStatus == 10, it's mean image id 10 is in progress or finnished
 */
int SoftOpPeakFinder::getCounterStatus() const
{
  AutoMutex aLock(m_cond.mutex());
  return m_task_manager.getCounterStatus();
}

void SoftOpPeakFinder::setBufferSize(int size)
{
  AutoMutex aLock(m_cond.mutex());
  for(NameMapIterator i = m_task_manager.begin();
      i != m_task_manager.end();++i)
    i->second.first->resizeHistory(size);
  m_history_size = size;
}

void SoftOpPeakFinder::getBufferSize(int& size) const
{
  AutoMutex aLock(m_cond.mutex());
  size = m_history_size;
}

void SoftOpPeakFinder::readPeaks(std::list<Tasks::PeakFinderResult>& result) const
{
  AutoMutex aLock(m_cond.mutex());
  for(NameMapConstIterator i = m_task_manager.begin(); i != m_task_manager.end();++i) {
    int from = 0;
    i->second.first->getHistory(result, from);
  }
}

void SoftOpPeakFinder::setComputingMode(ComputingMode aComputingMode)
{
  for(NameMapConstIterator i = m_task_manager.begin(); i != m_task_manager.end();++i) {
    i->second.second->setComputingMode(aComputingMode == SoftOpPeakFinder::MAXIMUM ?
				       Tasks::PeakFinderTask::MAXIMUM : Tasks::PeakFinderTask::CM );
  }
}

void SoftOpPeakFinder::getComputingMode(ComputingMode &aComputingMode) const
{
  Tasks::PeakFinderTask::ComputingMode aMode;
  for(NameMapConstIterator i = m_task_manager.begin(); i != m_task_manager.end();++i) {
    i->second.second->getComputingMode(aMode); 
  }
  aComputingMode = aMode == Tasks::PeakFinderTask::MAXIMUM ?
    SoftOpPeakFinder::MAXIMUM : SoftOpPeakFinder::CM;
}
