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
}

SoftOpBackgroundSubstraction::~SoftOpBackgroundSubstraction()
{
  m_opt->unref();
}

void SoftOpBackgroundSubstraction::setBackgroundImage(Data &anImage)
{
  m_opt->setBackgroundImageData(anImage);
}

void SoftOpBackgroundSubstraction::addTo(TaskMgr &aMgr,int stage)
{
  aMgr.setLinkTask(stage,m_opt);
}
//-------------------- BINNING --------------------
				   
/** @brief small wrapper around Binning Task
 */
SoftOpBinning::SoftOpBinning() : 
  SoftOpBaseClass()
{
  m_opt = new Tasks::Binning();
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

void SoftOpBinning::addTo(TaskMgr &aMgr,int stage)
{
  aMgr.setLinkTask(stage,m_opt);
}

//-------------------- BPM --------------------
				   
/** @brief small wrapper around Bpm Task
 */
SoftOpBpm::SoftOpBpm() : 
  SoftOpBaseClass()
{
  m_manager = new Tasks::BpmManager(DEFAULT_HISTORY_SIZE);
  m_task = new Tasks::BpmTask(*m_manager);
}

SoftOpBpm::~SoftOpBpm()
{
  m_task->unref();
  delete m_manager;		///@todo bad should also use ref unref
}


void SoftOpBpm::addTo(TaskMgr &aMgr,int stage)
{
  aMgr.addSinkTask(stage,m_task);
}

void SoftOpBpm::prepare()
{
  m_manager->resetHistory();
}

//-------------------- FLATFIELDCORRECTION --------------------
				   
/** @brief small wrapper around FlatfieldCorrection Task
 */
SoftOpFlatfieldCorrection::SoftOpFlatfieldCorrection() : 
  SoftOpBaseClass()
{
  m_opt = new Tasks::FlatfieldCorrection();
}

SoftOpFlatfieldCorrection::~SoftOpFlatfieldCorrection()
{
  m_opt->unref();
}

void SoftOpFlatfieldCorrection::setFlatFieldImage(Data &aData)
{
  m_opt->setFlatFieldImageData(aData);
}

void SoftOpFlatfieldCorrection::addTo(TaskMgr &aMgr,int stage)
{
  aMgr.setLinkTask(stage,m_opt);
}

//-------------------- FLIP --------------------
				   
/** @brief small wrapper around Flip Task
 */
SoftOpFlip::SoftOpFlip() : 
  SoftOpBaseClass()
{
  m_opt = new Tasks::Flip();
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

void SoftOpFlip::addTo(TaskMgr &aMgr,int stage)
{
  aMgr.setLinkTask(stage,m_opt);
}

//-------------------- MASK --------------------
				   
/** @brief small wrapper around Mask Task
 */
SoftOpMask::SoftOpMask() : 
  SoftOpBaseClass()
{
  m_opt = new Tasks::Mask();
}

SoftOpMask::~SoftOpMask()
{
  m_opt->unref();
}

void SoftOpMask::setMaskImage(Data &mask)
{
  m_opt->setMaskImageData(mask);
}

void SoftOpMask::addTo(TaskMgr &aMgr,int stage)
{
  aMgr.setLinkTask(stage,m_opt);
}

//-------------------- ROI COUNTERS --------------------

SoftOpRoiCounter::SoftOpRoiCounter() : 
  SoftOpBaseClass(),
  m_history_size(DEFAULT_HISTORY_SIZE),
  m_counter_status(-1)
{
}

SoftOpRoiCounter::~SoftOpRoiCounter()
{
  for(std::list<ManagerNCounter>::iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();++i)
    {
      i->second->unref();
      delete i->first;
    }
}

void SoftOpRoiCounter::add(const std::list<Roi> &rois)
{
  AutoMutex aLock(m_cond.mutex());
  for(std::list<Roi>::const_iterator i = rois.begin();
      i != rois.end();++i)
    {
      Tasks::RoiCounterManager *aCounterMgrPt = new Tasks::RoiCounterManager(m_history_size);
      Tasks::RoiCounterTask *aCounterTaskPt = new Tasks::RoiCounterTask(*aCounterMgrPt);

      const Point &aOri = i->getTopLeft();
      const Size &aSize = i->getSize();
      aCounterTaskPt->setRoi(aOri.x,aOri.y,aSize.getWidth(),aSize.getHeight());
      aCounterTaskPt->setMask(m_mask);

      m_manager_tasks.push_back(ManagerNCounter(aCounterMgrPt,aCounterTaskPt));
    }
}
void SoftOpRoiCounter::set(const std::list<Roi> &rois)
{
  clearAllRoi();
  add(rois);
}
/** @brief return the list of roi set
 */
void SoftOpRoiCounter::get(std::list<Roi> &aReturnList) const
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
void SoftOpRoiCounter::del(const std::list<int> &roiIds)
{
  std::list<int> aTmpList = roiIds;
  aTmpList.sort();
  
  AutoMutex aLock(m_cond.mutex());
  std::list<int>::iterator i = aTmpList.begin();
  std::list<ManagerNCounter>::iterator k = m_manager_tasks.begin();
  for(int index = 1;i != aTmpList.end() && k != m_manager_tasks.end();++i)
    {
      while(index != *i && k != m_manager_tasks.end())
	++k,++index;

      if(index == *i)
	{
	  k->second->unref();
	  delete k->first;
	  k = m_manager_tasks.erase(k),++index;
	}
    }
}

/** @brief remove all roi
 */
void SoftOpRoiCounter::clearAllRoi()
{
  AutoMutex aLock(m_cond.mutex());
  for(std::list<ManagerNCounter>::iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();i = m_manager_tasks.erase(i))
    {
      i->second->unref();
      delete i->first;
    }
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
  for(std::list<ManagerNCounter>::iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();i = m_manager_tasks.erase(i))
      i->second->setMask(aMask);
  m_mask = aMask;
}

void SoftOpRoiCounter::setBufferSize(int size)
{
  AutoMutex aLock(m_cond.mutex());
  for(std::list<ManagerNCounter>::iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();++i)
    i->first->resizeHistory(size);
  m_history_size = size;
}

void SoftOpRoiCounter::getBufferSize(int &size) const
{
  AutoMutex aLock(m_cond.mutex());
  size = m_history_size;
}

void SoftOpRoiCounter::readCounters(int from,std::list<RoiIdAndResults> &result) const
{
  AutoMutex aLock(m_cond.mutex());
  int roiIndex = 1;
  for(std::list<ManagerNCounter>::const_iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();++i,++roiIndex)
    {
      result.push_back(RoiIdAndResults(roiIndex,std::list<Tasks::RoiCounterResult>()));
      RoiIdAndResults &roiAndResults = result.back();
      i->first->getHistory(roiAndResults.second,from);
    }
}

void SoftOpRoiCounter::addTo(TaskMgr &aMgr,int stage)
{
  AutoMutex aLock(m_cond.mutex());
  for(std::list<ManagerNCounter>::iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();++i)
    aMgr.addSinkTask(stage,i->second);
  ++m_counter_status;
}

void SoftOpRoiCounter::prepare()
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
}

SoftOpSoftRoi::~SoftOpSoftRoi()
{
  m_opt->unref();
}

void SoftOpSoftRoi::setRoi(int x,int y,int width,int height)
{
  m_opt->setRoi(x,x+width,y,y+width);
}

void SoftOpSoftRoi::addTo(TaskMgr &aMgr,int stage)
{
  aMgr.setLinkTask(stage,m_opt);
}
