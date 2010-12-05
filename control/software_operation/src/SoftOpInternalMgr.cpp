#include "SoftOpInternalMgr.h"
using namespace lima;

#include "Flip.h"
#include "Binning.h"
#include "SoftRoi.h"


SoftOpInternalMgr::SoftOpInternalMgr() :
  m_reconstruction_task(NULL),m_end_callback(NULL)
{
}

SoftOpInternalMgr::~SoftOpInternalMgr()
{
  if(m_reconstruction_task)
    m_reconstruction_task->unref();
  if(m_end_callback)
    m_end_callback->unref();
}


void SoftOpInternalMgr::setBin(const Bin &aBin)
{
  m_bin = aBin;
}

void SoftOpInternalMgr::getBin(Bin &aBin) const
{
  aBin = m_bin;
}

void SoftOpInternalMgr::setRoi(const Roi &aRoi)
{
  m_roi = aRoi;
}

void SoftOpInternalMgr::getRoi(Roi &aRoi) const
{
  aRoi = m_roi;
}

void SoftOpInternalMgr::setFlip(const Flip &aFlip)
{
  m_flip = aFlip;
}

void SoftOpInternalMgr::getFlip(Flip &aFlip) const
{
  aFlip = m_flip;
}

/** @brief set the reconstruction task.
 *  reconstruction task will be the first task
 */
void SoftOpInternalMgr::setReconstructionTask(LinkTask *aTask)
{
  if(aTask)
    aTask->ref();
  if(m_reconstruction_task)
    m_reconstruction_task->unref();
      
  m_reconstruction_task = aTask;
}

bool SoftOpInternalMgr::hasReconstructionTask()
{
  return (m_reconstruction_task!=NULL);
}

void SoftOpInternalMgr::addTo(TaskMgr &aTaskMgr,
			      int &aLastStage) const
{
  aLastStage = 0;
  if(m_reconstruction_task)
    {
      aTaskMgr.setLinkTask(aLastStage,m_reconstruction_task);
      ++aLastStage;
    }
  Tasks::Flip *aFlipTaskPt = NULL;
  if(m_flip.x || m_flip.y)
    {
      Tasks::Flip::FLIP_MODE aMode = Tasks::Flip::FLIP_NONE;
      if(m_flip.x && m_flip.y)
	aMode = Tasks::Flip::FLIP_ALL;
      else if(m_flip.x)
	aMode = Tasks::Flip::FLIP_X;
      else
	aMode = Tasks::Flip::FLIP_Y;
      
      aFlipTaskPt = new Tasks::Flip();
      aFlipTaskPt->setFlip(aMode);
      aTaskMgr.setLinkTask(aLastStage,aFlipTaskPt);
      aFlipTaskPt->unref();
      ++aLastStage;
    }
  
  Tasks::Binning *aBinTaskPt = NULL;
  if(m_bin.getX() > 1 || m_bin.getY() > 1)
    {
      aBinTaskPt = new Tasks::Binning();
      aBinTaskPt->mXFactor = m_bin.getX();
      aBinTaskPt->mXFactor = m_bin.getY();
      aTaskMgr.setLinkTask(aLastStage,aBinTaskPt);
      aBinTaskPt->unref();
      ++aLastStage;
    }
  
  Tasks::SoftRoi *aSoftRoiTaskPt = NULL;
  if(m_roi.isActive())
    {
      Point topl= m_roi.getTopLeft();
      Point botr= m_roi.getBottomRight();
      Size  roi_size= m_roi.getSize();
      aSoftRoiTaskPt = new Tasks::SoftRoi();
      aSoftRoiTaskPt->setRoi(topl.x, botr.x, topl.y, botr.y);
      aTaskMgr.setLinkTask(aLastStage,aSoftRoiTaskPt);
      aSoftRoiTaskPt->unref();
      ++aLastStage;
    }
  bool removeReconstructionTaskCallback = true;
  //Check now what is the last task to add a callback
  if(aSoftRoiTaskPt)
    aSoftRoiTaskPt->setEventCallback(m_end_callback);
  else if(aBinTaskPt)
    aBinTaskPt->setEventCallback(m_end_callback);
  else if(aFlipTaskPt)
    aFlipTaskPt->setEventCallback(m_end_callback);
  else if(m_reconstruction_task)
    m_reconstruction_task->setEventCallback(m_end_callback),removeReconstructionTaskCallback = false;

  //Clear eventCallback for reconstruction task
  if(m_reconstruction_task && removeReconstructionTaskCallback)
    m_reconstruction_task->setEventCallback(NULL);
}

