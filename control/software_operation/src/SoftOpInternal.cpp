#include "SoftOpInternalMgr.cpp"

SoftOpInternalMgr::SoftOpInternalMgr() :
  m_reconstruction_task(NULL)
{
}

SoftOpInternalMgr::~SoftOpInternalMgr()
{
  if(m_reconstruction_task)
    m_reconstruction_task->unref();
}


void SoftOpInternalMgr::setBin(const Bin &aBin)
{
  m_bin = aBin;
}

void SoftOpInternalMgr::getBin(const Bin &aBin) const
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
  if(m_reconstruction_task)
    m_reconstruction_task->unref();
      
  m_reconstruction_task = aTask;
  if(m_reconstruction_task)
    m_reconstruction_task->ref();
}

void SoftOpInternalMgr::addTo(TaskMgr &aTaskMgr,
			      int &aLastStage) const
{
  aLastStage = 0;
  if(m_reconstruction_task)
    {
      aTaskMgr.addSinkTask(aLastStage,m_reconstruction_task);
      ++aLastStage;
    }
  Flip *aFlipTaskPt = NULL;
  if(m_flip.flip_x || m_flip.flip_y)
    {
      Flip::FLIP_MODE aMode = Flip::FLIP_NONE;
      if(m_flip.flip_x && m_flip.flip_y)
	aMode = Flip::FLIP_ALL;
      else if(m_flip.flip_x)
	aMode = Flip::FLIP_X;
      else
	aMode = Flip::FLIP_Y;
      
      aFlipTaskPt = new Flip();
      aFlipTaskPt.setFlip(aMode);
      aTaskMgr.setLinkTask(aLastStage,aFlipTaskPt);
      aFlipTaskPt->unref();
      ++aLastStage;
    }
  
  Binning *aBinTaskPt = NULL;
  if(m_bin.bin_x > 1 || m_bin.bin_y > 1)
    {
      aBinTaskPt = new Binning();
      aBinTaskPt->mXFactor = m_bin.bin_x;
      aBinTaskPt->mXFactor = m_bin.bin_y;
      aTaskMgr.setLinkTask(aLastStage,aBinTaskPt);
      aBinTaskPt->unref();
      ++aLastStage;
    }
  
  SoftRoi *aSoftRoiTaskPt = NULL;
  if(m_roi.active)
    {
      aSoftRoiTaskPt = new SoftRoi();
      aSoftRoiTaskPt.setRoi(m_roi.x, m_roi.x + m_roi.width,
			    m_roi.y,m_roi.y + m_roi.height);
      aTaskMgr.setLinkTask(aLastStage,aSoftRoiTaskPt);
      aSoftRoiTaskPt->unref();
      ++aLastStage;
    }

  //Check now what is the last task to add a callback
  if(aSoftRoiTaskPt)
    {
    }
  else if(aBinTaskPt)
    {
    }
  else if(aFlipTaskPt)
    {
    }
  else if(m_reconstruction_task)
    {
    }
}


//BIN
SoftOpInternalMgr::Bin::Bin() :
  bin_x(1),bin_y(1)
{}

//ROI
SoftOpInternalMgr::Roi::Roi() :
  active(false),x(0),y(0),width(-1),height(-1)
{}

SoftOpInternalMgr::Flip::Flip() :
  flip_x(false),flip_y(false)
{}

