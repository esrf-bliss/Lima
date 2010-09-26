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
  for(std::list<ManagerNCounter>::iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();i = m_manager_tasks.erase(i))
    {
      i->second->unref();
      delete i->first;
    }
}

void SoftOpRoiCounter::clearCounterStatus()
{
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
  return m_counter_status;
}
void SoftOpRoiCounter::setMask(Data &aMask)
{
  for(std::list<ManagerNCounter>::iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();i = m_manager_tasks.erase(i))
      i->second->setMask(aMask);
  m_mask = aMask;
}

void SoftOpRoiCounter::setBufferSize(int size)
{
  for(std::list<ManagerNCounter>::iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();++i)
    i->first->resizeHistory(size);
  m_history_size = size;
}

void SoftOpRoiCounter::getBufferSize(int &size) const
{
  size = m_history_size;
}

void SoftOpRoiCounter::readCounters(int from,std::list<RoiIdAndResults> &result) const
{
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
  for(std::list<ManagerNCounter>::iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();++i)
    aMgr.addSinkTask(stage,i->second);
  ++m_counter_status;
}

void SoftOpRoiCounter::prepare()
{
   for(std::list<ManagerNCounter>::iterator i = m_manager_tasks.begin();
      i != m_manager_tasks.end();++i)
     i->first->resetHistory();
   m_counter_status = -1;
}
