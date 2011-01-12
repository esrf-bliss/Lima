#include <list>
#include "HwVideoCtrlObj.h"
#include "CtVideo.h"

#include "PoolThreadMgr.h"
#include "SinkTask.h"
#include "TaskMgr.h"
#include "Binning.h"
#include "SoftRoi.h"

using namespace lima;

// --- CtVideo::Data2Imagetask
class CtVideo::_Data2ImageTask : public SinkTaskBase
{
  DEB_CLASS_NAMESPC(DebModControl,"Data to image","Control");
public:
  _Data2ImageTask(CtVideo &cnt) : SinkTaskBase(),m_cnt(cnt) {}

  virtual void process(Data &aData)
  {
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(aData);
    
    AutoMutex aLock(m_cnt.m_cond.mutex());
    int nextImageNumber = m_cnt.m_image_counter;
    VideoImage &anImage = m_cnt.m_last_image[nextImageNumber & 0x1];
    aLock.unlock();
    
    
    data2Image(aData,anImage);
    //@todo call the image callback....
  }
private:
  CtVideo &m_cnt;
};

CtVideo::CtVideo(CtControl &ct) :
  m_image_counter(0)
{
  HwInterface *hw = ct.interface();
  m_has_video = hw->getHwCtrlObj(m_video);

  m_data_2_image_task = new _Data2ImageTask(*this);
}

CtVideo::~CtVideo()
{
  m_data_2_image_task->unref();
  delete [] m_last_image[0].buffer;
  delete [] m_last_image[1].buffer;
}

// --- parameters
void CtVideo::setParameters(const Parameters &pars)
{
  AutoMutex aLock(m_cond.mutex());
  m_pars = pars;
}
void CtVideo::getParameters(Parameters &pars) const
{
  AutoMutex aLock(m_cond.mutex());
  pars = m_pars;
}

void CtVideo::setLive(bool liveFlag)
{
  AutoMutex aLock(m_cond.mutex());
  m_pars.live = liveFlag;
}
void CtVideo::getLive(bool &liveFlag) const
{
  AutoMutex aLock(m_cond.mutex());
  liveFlag = m_pars.live;
}

void CtVideo::setFrameRate(double aFrameRate)
{
  AutoMutex aLock(m_cond.mutex());
  m_pars.framerate = aFrameRate;
}
void CtVideo::getFrameRate(double &aFrameRate) const
{
  AutoMutex aLock(m_cond.mutex());
  aFrameRate = m_pars.framerate;
}

void CtVideo::setBrightness(double aBrightness)
{
  AutoMutex aLock(m_cond.mutex());
  m_pars.brightness = aBrightness;
}
void CtVideo::getBrightness(double &aBrightness) const
{
  AutoMutex aLock(m_cond.mutex());
  aBrightness = m_pars.brightness;
}

void CtVideo::setGain(double aGain)
{
  AutoMutex aLock(m_cond.mutex());
  m_pars.gain = aGain;
}
void CtVideo::getGain(double &aGain) const
{
  AutoMutex aLock(m_cond.mutex());
  aGain = m_pars.gain;
}

void CtVideo::setMode(VideoMode aMode)
{
  AutoMutex aLock(m_cond.mutex());
  m_pars.mode = aMode;
}
void CtVideo::getMode(VideoMode &aMode) const
{
  AutoMutex aLock(m_cond.mutex());
  aMode = m_pars.mode;
}

void CtVideo::setRoi(const Roi &aRoi)
{
  AutoMutex aLock(m_cond.mutex());
  m_pars.roi = aRoi;
}
void CtVideo::getRoi(Roi &aRoi) const
{
  AutoMutex aLock(m_cond.mutex());
  aRoi = m_pars.roi;
}

void CtVideo::setBin(const Bin &aBin)
{
  AutoMutex aLock(m_cond.mutex());
  m_pars.bin = aBin;
}
void CtVideo::getBin(Bin &aBin) const
{
  AutoMutex aLock(m_cond.mutex());
  aBin = m_pars.bin;
}

// --- images
void CtVideo::getLastImage(VideoImage &anImage) const
{
}
void CtVideo::getLastImageCounter(int &anImageCounter) const
{
}

void CtVideo::registerImageCallback(ImageCallback &cb)
{
}
void CtVideo::unregisterImageCallback(ImageCallback &cb)
{
}

// --- video mode
void CtVideo::getSupportedVideoMode(std::list<VideoMode> &modeList)
{
  if(m_has_video)
    m_video->getSupportedVideoMode(modeList);
  else				// TODO
    {
    }
}

// --- callback from CtControl
void CtVideo::frameReady(Data &aData)
{
  DEB_MEMBER_FUNCT();
  if(!m_has_video)
    {
      AutoMutex aLock(m_cond.mutex());
      if(m_ready_flag)
	{
	  m_ready_flag = false;
	  Bin aBin = m_pars.bin;
	  Roi aRoi = m_pars.roi;
	  aLock.unlock();
	  _data_2_image(aData,aBin,aRoi);
	}
      else
	m_last_data = aData;
    }
}

void CtVideo::_data_2_image(Data &aData,Bin &aBin,Roi &aRoi)
{
  TaskMgr *anImageCopy = new TaskMgr();
  int runLevel = 0;
  if(aBin.getX() > 1 || aBin.getY() > 1)
    {
      Tasks::Binning *aBinTaskPt = new Tasks::Binning();
      aBinTaskPt->mXFactor = aBin.getX();
      aBinTaskPt->mYFactor = aBin.getY();
      anImageCopy->setLinkTask(runLevel,aBinTaskPt);
      aBinTaskPt->unref();
      ++runLevel;
    }
  if(aRoi.isActive())
    {
      Point topl= aRoi.getTopLeft();
      Point botr= aRoi.getBottomRight();
      Tasks::SoftRoi *aSoftRoiTaskPt = new Tasks::SoftRoi();
      aSoftRoiTaskPt->setRoi(topl.x, botr.x, topl.y, botr.y);
      anImageCopy->setLinkTask(runLevel,aSoftRoiTaskPt);
      aSoftRoiTaskPt->unref();
      ++runLevel;
    }
  anImageCopy->addSinkTask(runLevel,m_data_2_image_task);
  anImageCopy->setInputData(aData);
  
  PoolThreadMgr::get().addProcess(anImageCopy);
}
//============================================================================
//			 CtVideo::Parameters
//============================================================================
CtVideo::Parameters::Parameters()
{
  reset();
}
void CtVideo::Parameters::reset()
{
  live = false;
  framerate = -1.;
  exposure = 1;
  brightness = .5;
  gain = .5;
  mode = Y8;
  roi.reset();
  bin.reset();
}
