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
    VideoImage *anImage = m_cnt.m_write_image;
    while(anImage->inused)
      m_cnt.m_cond.wait();
    anImage->inused = -1;	// Write Mode
    aLock.unlock();
    
    
    data2Image(aData,*anImage);

    aLock.lock();
    anImage->inused = 0;	// Unlock
    // if read Image is not use, swap
    if(!m_cnt.m_read_image->inused)
      {
	m_cnt.m_write_image = m_cnt.m_read_image;
	m_cnt.m_write_image->frameNumber = -1;
	m_cnt.m_read_image = anImage;
      }

    ++m_cnt.m_image_counter;

    if(m_cnt.m_image_callback)
      {
	CtVideo::Image anImageWrapper(&m_cnt,anImage);
	ImageCallback *cb = m_cnt.m_image_callback;
	aLock.unlock();
	cb->newImage(anImageWrapper);
      }
    else
      m_cnt.m_cond.broadcast();
  }
private:
  CtVideo &m_cnt;
};

// --- CtVideo::_Data2ImageCBK 
class CtVideo::_Data2ImageCBK : public TaskEventCallback
{
  DEB_CLASS_NAMESPC(DebModControl,"CtVideo::_Data2ImageCBK","Control");
public:
  _Data2ImageCBK(CtVideo &aCtVideo) : m_video(aCtVideo) {}
  virtual void finnished(Data &aData)
  {
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(aData);

    m_video._data2image_finnished(aData);
  }
private:
  CtVideo &m_video;
};
// --- CtVideo::Image
CtVideo::Image::Image() : m_video(NULL),m_image(NULL) {}

CtVideo::Image::~Image()
{
  if(m_video)
    {
      AutoMutex aLock(m_video->m_cond.mutex());
      --(m_image->inused);
      m_video->m_cond.broadcast();
    }
}

CtVideo::Image::Image(const Image &anOther) :
  m_video(anOther.m_video),
  m_image(anOther.m_image)
{
  if(m_video)
    {
      AutoMutex aLock(m_video->m_cond.mutex());
      ++(m_image->inused);
    }
}

CtVideo::Image& CtVideo::Image::operator=(const CtVideo::Image &other)
{
  if(this != &other)
    {
      if(other.m_video)
	{
	  AutoMutex aLock(other.m_video->m_cond.mutex());
	  ++(other.m_image->inused);
	}

      if(m_video)
	{
	  AutoMutex aLock(m_video->m_cond.mutex());
	  --(m_image->inused);
	  m_video->m_cond.broadcast();
	}

      m_video = other.m_video;
      m_image = other.m_image;
    }
  return *this;
}
/** @brief an other contructor
 *  This methode should be call under Lock
 */
CtVideo::Image::Image(const CtVideo *video,VideoImage *image) :
  m_video(video),
  m_image(image)
{
  ++(m_image->inused);
}

// --- CtVideo class
CtVideo::CtVideo(CtControl &ct) :
  m_ready_flag(true),
  m_image_counter(0),
  m_read_image(new VideoImage()),
  m_write_image(new VideoImage()),
  m_image_callback(NULL)
{
  HwInterface *hw = ct.interface();
  m_has_video = hw->getHwCtrlObj(m_video);

  m_data_2_image_task = new _Data2ImageTask(*this);

  m_data_2_image_cb = new _Data2ImageCBK(*this);
  m_data_2_image_task->setEventCallback(m_data_2_image_cb);

  // Params init
  if(m_has_video)
    {
      m_video->getBrightness(m_pars.brightness);
      m_video->getGain(m_pars.gain);
    }
}

CtVideo::~CtVideo()
{
  m_data_2_image_task->unref();
  m_data_2_image_cb->unref();
  delete m_read_image;
  delete m_write_image;
}

// --- parameters
void CtVideo::setParameters(const Parameters &pars)
{
  AutoMutex aLock(m_cond.mutex());
  m_pars = pars;
  if(m_pars.live)
    _apply_params();
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
  if(liveFlag)
    _apply_params();
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
void CtVideo::getLastImage(CtVideo::Image &anImage) const
{
  AutoMutex aLock(m_cond.mutex());
  if(m_write_image->inused >= 0 && // No writter
     m_write_image->frameNumber >= m_read_image->frameNumber)
    {
      VideoImage *tmp = m_read_image;
      m_read_image = m_write_image;
      m_write_image = tmp;
      m_write_image->frameNumber = -1;
    }
  CtVideo::Image tmpImage(this,m_read_image);
  aLock.unlock();
  
  anImage = tmpImage;
}

void CtVideo::getLastImageCounter(int &anImageCounter) const
{
  AutoMutex aLock(m_cond.mutex());
  anImageCounter = m_image_counter;
}

void CtVideo::registerImageCallback(ImageCallback &cb)
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  DEB_PARAM() << DEB_VAR2(&cb, m_image_callback);
  
  if(m_image_callback)
    {
      DEB_ERROR() << "ImageCallback already registered";
      throw LIMA_CTL_EXC(InvalidValue, "ImageCallback already registered");
    }

  m_image_callback = &cb;
}

void CtVideo::unregisterImageCallback(ImageCallback &cb)
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  DEB_PARAM() << DEB_VAR2(&cb, m_image_callback);
  if(m_image_callback != &cb)
    {
      DEB_ERROR() << "ImageCallback not registered";
      throw LIMA_CTL_EXC(InvalidValue, "ImageCallback not registered"); 
    }

  m_image_callback = NULL;
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

void CtVideo::_data2image_finnished(Data&)
{
  AutoMutex aLock(m_cond.mutex());
  if(!m_last_data.empty())
    {
      Data aData = m_last_data;
      m_last_data = Data();
      Bin aBin = m_pars.bin;
      Roi aRoi = m_pars.roi;
      aLock.unlock();
      _data_2_image(aData,aBin,aRoi);
    }
  else
    m_ready_flag = true;
}

void CtVideo::_apply_params()
{
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
