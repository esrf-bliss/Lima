#include <list>
#include "HwVideoCtrlObj.h"
#include "CtVideo.h"
#include "CtAcquisition.h"
#include "CtImage.h"

#include "PoolThreadMgr.h"
#include "SinkTask.h"
#include "TaskMgr.h"
#include "Binning.h"
#include "SoftRoi.h"

using namespace lima;
enum ParModifyMask
  {
    PARMODIFYMASK_EXPOSURE 	= 1U << 0,
    PARMODIFYMASK_GAIN 		= 1U << 1,
    PARMODIFYMASK_MODE	 	= 1U << 2,
    PARMODIFYMASK_ROI 		= 1U << 3,
    PARMODIFYMASK_BIN 		= 1U << 4
  };
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
    DEB_TRACE() << DEB_VAR1(*anImage);
    while(anImage->inused)
      m_cnt.m_cond.wait();
    anImage->inused = -1;	// Write Mode
    aLock.unlock();
    
    data2Image(aData,*anImage);
    
    aLock.lock();
    anImage->inused = 0;	// Unlock
    ++m_cnt.m_image_counter;

    // if read Image is not use, swap
    if(!m_cnt.m_read_image->inused)
      {
	m_cnt.m_write_image = m_cnt.m_read_image;
	m_cnt.m_write_image->frameNumber = -1;
	m_cnt.m_read_image = anImage;

	if(m_cnt.m_image_callback)
	  {
	    CtVideo::Image anImageWrapper(&m_cnt,anImage);
	    ImageCallback *cb = m_cnt.m_image_callback;
	    aLock.unlock();
	    cb->newImage(anImageWrapper);
	  }
      }
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
  virtual void finished(Data &aData)
  {
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(aData);

    m_video._data2image_finnished(aData);
  }
private:
  CtVideo &m_video;
};
// --- CtVideo::_InternalImageCBK
class CtVideo::_InternalImageCBK : public HwVideoCtrlObj::ImageCallback
{
  DEB_CLASS_NAMESPC(DebModControl,"CtVideo::_InternalImageCBK","Control");
public:
  _InternalImageCBK(CtVideo &video) : 
    HwVideoCtrlObj::ImageCallback(),
    m_video(video),m_buffer(video.m_video->getBuffer()) {}
  virtual ~_InternalImageCBK() {}
  
protected:
  virtual bool newImage(char * data,int width,int height,VideoMode mode);
private:
  CtVideo& 		m_video;
  StdBufferCbMgr& 	m_buffer;
};

bool CtVideo::_InternalImageCBK::newImage(char * data,int width,int height,VideoMode mode)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR4((void*)data,width,height,mode);

  bool liveFlag;
  AutoMutex aLock(m_video.m_cond.mutex());
  if(m_video.m_stopping_live) return false;

  liveFlag = m_video.m_pars.live;
  int image_counter = m_video.m_image_counter + 1;
  aLock.unlock();
    
  if(!liveFlag)			// Classic acquisition
    {
      int buffer_nb, concat_frame_nb;
      m_buffer.acqFrameNb2BufferNb(image_counter, buffer_nb, 
				     concat_frame_nb);
      void *ptr = m_buffer.getBufferPtr(buffer_nb,
					concat_frame_nb);
      try
	{
	  lima::image2YUV((unsigned char*)data,width,height,mode,(unsigned char*)ptr);
	  HwFrameInfoType frame_info;
	  frame_info.acq_frame_nb = image_counter;
	  m_buffer.newFrameReady(frame_info);
	}
      // Should happen only when video format is not implemented (Debug mode)
      catch(Exception &exc)
	{
	  std::cerr << exc.getErrMsg() << std::endl;
	  return false;
	}
    }

  aLock.lock();
  ++m_video.m_image_counter;
  VideoImage *anImage = m_video.m_write_image;
  if(anImage->inused) return true;			// Skip it (Should never happen!)
  anImage->inused = -1;		// Write Mode
  aLock.unlock();
  
  anImage->setParams(image_counter,width,height,mode);
  memcpy(anImage->buffer,data,int(anImage->size()));

  aLock.lock();
  anImage->inused = 0;
  // if read Image is not use, swap
  if(!m_video.m_read_image->inused)
    {
      m_video.m_write_image = m_video.m_read_image;
      m_video.m_write_image->frameNumber = -1;
      m_video.m_read_image = anImage;
   
      if(m_video.m_image_callback)
	{
	  //TODO should be done in background
	}
    }
  return true;
}
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

const char* CtVideo::Image::buffer() const
{
  return m_image ? m_image->buffer : NULL;
}

int CtVideo::Image::width() const
{
  return m_image ? m_image->width : -1;
}

int CtVideo::Image::height() const
{
  return m_image ? m_image->height : -1;
}

VideoMode CtVideo::Image::mode() const
{
  return m_image ? m_image->mode : Y8;
}

int CtVideo::Image::size() const
{
  return m_image ? int(m_image->size() + 0.5) : 0;
}

long long CtVideo::Image::frameNumber() const
{
  return m_image ? m_image->frameNumber : -1;
}
// --- CtVideo class
CtVideo::CtVideo(CtControl &ct) :
  m_pars_modify_mask(0),
  m_ready_flag(true),
  m_image_counter(0),
  m_read_image(new VideoImage()),
  m_write_image(new VideoImage()),
  m_image_callback(NULL),
  m_internal_image_callback(NULL),
  m_ct(ct),
  m_stopping_live(false)
{
  HwInterface *hw = ct.hwInterface();
  m_has_video = hw->getHwCtrlObj(m_video);
  hw->getHwCtrlObj(m_sync);
  m_data_2_image_task = new _Data2ImageTask(*this);

  m_data_2_image_cb = new _Data2ImageCBK(*this);
  m_data_2_image_task->setEventCallback(m_data_2_image_cb);

  // Params init
  if(m_has_video)
    {
      m_internal_image_callback = new _InternalImageCBK(*this);
      m_video->getGain(m_pars.gain);
      m_video->getVideoMode(m_pars.mode);
      m_video->registerImageCallback(*m_internal_image_callback);
    }
}

CtVideo::~CtVideo()
{
  m_data_2_image_task->unref();
  m_data_2_image_cb->unref();
  delete m_read_image;
  delete m_write_image;
  delete m_internal_image_callback;
}

// --- parameters
void CtVideo::setParameters(const Parameters &pars)
{
  //All check
  _check_video_mode(pars.mode);

  AutoMutex aLock(m_cond.mutex());

  if(m_pars.exposure != pars.exposure) 		m_pars_modify_mask |= PARMODIFYMASK_EXPOSURE;
  if(m_pars.gain != pars.gain) 			m_pars_modify_mask |= PARMODIFYMASK_GAIN;
  if(m_pars.mode != pars.mode) 			m_pars_modify_mask |= PARMODIFYMASK_MODE;
  if(m_pars.roi != pars.roi) 			m_pars_modify_mask |= PARMODIFYMASK_ROI;
  if(m_pars.bin != pars.bin) 			m_pars_modify_mask |= PARMODIFYMASK_BIN;

  m_pars = pars;
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

  while(m_stopping_live) m_cond.wait();

  if(m_pars.live == liveFlag)	// nothing to do
    return;

  CtControl::Status status;
  m_ct.getStatus(status);
  if(liveFlag && status.AcquisitionStatus != AcqReady)
    throw LIMA_CTL_EXC(Error, "Can't set live mode if an acquisition is running");

  _apply_params(liveFlag);
  
  if(m_has_video)
    {
      if(!liveFlag)
	{
	  m_stopping_live = true;
	  aLock.unlock();
	}

      m_video->setLive(liveFlag);

      if(!liveFlag)
	{
	  aLock.lock();
	  m_stopping_live = false;
	  m_cond.signal();
	}
    }
  else
    {
      if(liveFlag)
	{
	  CtAcquisition *acqPt = m_ct.acquisition();
	  acqPt->setAcqNbFrames(0);	// Live
	  m_ct.prepareAcq();
	  m_ct.startAcq();
	}
      else
	m_ct.stopAcq();
    }
  m_pars.live = liveFlag;
}
void CtVideo::getLive(bool &liveFlag) const
{
  AutoMutex aLock(m_cond.mutex());
  liveFlag = m_pars.live;
}

void CtVideo::setExposure(double anExposure)
{
  AutoMutex aLock(m_cond.mutex());
  m_pars.exposure = anExposure,m_pars_modify_mask |= PARMODIFYMASK_EXPOSURE;
  _apply_params();
}

void CtVideo::getExposure(double &anExposure) const
{
  AutoMutex aLock(m_cond.mutex());
  anExposure = m_pars.exposure;
}

void CtVideo::setGain(double aGain)
{
  if(!m_has_video)
    throw LIMA_CTL_EXC(Error,"Can't change the gain on Scientific camera");
  if(aGain < 0. || aGain > 1.)
    throw LIMA_CTL_EXC(InvalidValue,"Gain should be between 0. and 1.");

  AutoMutex aLock(m_cond.mutex());
  m_pars.gain = aGain,m_pars_modify_mask |= PARMODIFYMASK_GAIN;
  _apply_params();
}
void CtVideo::getGain(double &aGain) const
{
  AutoMutex aLock(m_cond.mutex());
  aGain = m_pars.gain;
}

void CtVideo::setMode(VideoMode aMode)
{
  _check_video_mode(aMode);
  AutoMutex aLock(m_cond.mutex());
  m_pars.mode = aMode,m_pars_modify_mask |= PARMODIFYMASK_MODE;
  _apply_params();
}
void CtVideo::getMode(VideoMode &aMode) const
{
  AutoMutex aLock(m_cond.mutex());
  aMode = m_pars.mode;
}

void CtVideo::setRoi(const Roi &aRoi)
{
  AutoMutex aLock(m_cond.mutex());
  m_pars.roi = aRoi,m_pars_modify_mask |= PARMODIFYMASK_ROI;
  _apply_params();
}
void CtVideo::getRoi(Roi &aRoi) const
{
  AutoMutex aLock(m_cond.mutex());
  aRoi = m_pars.roi;
}

void CtVideo::setBin(const Bin &aBin)
{
  AutoMutex aLock(m_cond.mutex());
  m_pars.bin = aBin,m_pars_modify_mask |= PARMODIFYMASK_BIN;
  _apply_params();
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
  DEB_MEMBER_FUNCT();

  if(m_has_video)
    m_video->getSupportedVideoMode(modeList);
  else				// TODO
    {
      CtImage* image = m_ct.image();
      ImageType anImageType;
      image->getImageType(anImageType);
      switch(anImageType)
	{
	case Bpp8:
	case Bpp8S:
	  modeList.push_back(Y8); break;
	case Bpp10:
	case Bpp10S: 
	case Bpp12:
	case Bpp12S:
	case Bpp14:
	case Bpp14S:
	case Bpp16:
	case Bpp16S:
	  modeList.push_back(Y16); break;
	case Bpp32:
	case Bpp32S:
	  modeList.push_back(Y32); break;
	default:
	  DEB_ERROR() << "Image type not yet managed";
	  throw LIMA_CTL_EXC(Error, "Image type not yet managed");
	}
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
  DEB_MEMBER_FUNCT();
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
  DEB_MEMBER_FUNCT();
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

void CtVideo::_apply_params(bool aForceLiveFlag)
{
  if(aForceLiveFlag && !m_pars.live)
      m_read_image->frameNumber = m_write_image->frameNumber = m_image_counter = -1;
  
  if(aForceLiveFlag || m_pars.live)
    {
      if(m_has_video)
	{
	  if(m_pars_modify_mask & PARMODIFYMASK_MODE)
	    m_video->setVideoMode(m_pars.mode);
	  if(m_pars_modify_mask & PARMODIFYMASK_GAIN)
	    m_video->setGain(m_pars.gain);
	  if(m_pars_modify_mask & PARMODIFYMASK_BIN)
	    {
	      m_hw_bin = m_pars.bin;
	      m_video->checkBin(m_hw_bin);
	      m_video->setBin(m_hw_bin);
	      // Synchronisation with standard acquisition
	      CtImage* image = m_ct.image();
	      image->setBin(m_hw_bin);
	    }
	  if(m_pars_modify_mask & PARMODIFYMASK_ROI)
	    {
	      m_video->checkRoi(m_pars.roi,m_hw_roi);
	      m_video->setRoi(m_hw_roi);
	      // Synchronisation with standard acquisition
	      CtImage* image = m_ct.image();
	      image->setRoi(m_hw_roi);
	    }
	  if(m_pars_modify_mask & PARMODIFYMASK_EXPOSURE)
	    {
	      m_sync->setExpTime(m_pars.exposure);
	      CtAcquisition* acquisition = m_ct.acquisition();
	      acquisition->setAcqExpoTime(m_pars.exposure);
	    }
	}
      else			// Scientific Camera
	{
          if(m_pars_modify_mask & PARMODIFYMASK_EXPOSURE)
            {
              CtAcquisition* acquisition = m_ct.acquisition();
              acquisition->setAcqExpoTime(m_pars.exposure);
	      if(m_pars.live)
	        {
		  m_ct.stopAcq();
		  m_ct.prepareAcq();
		  m_ct.startAcq();
		}
            }
	}
      m_pars_modify_mask = 0;	// reset
    }
}

void CtVideo::_read_hw_params()
{
  CtAcquisition *acquisition = m_ct.acquisition();
  acquisition->getAcqExpoTime(m_pars.exposure);

  CtImage* image = m_ct.image();
  image->getRoi(m_hw_roi);
  if(!m_hw_roi.containsRoi(m_pars.roi))
    m_pars.roi = m_hw_roi;
  
  image->getBin(m_hw_bin);
  if(m_hw_bin.getX() > m_pars.bin.getX() ||
     m_hw_bin.getY() > m_pars.bin.getY())
    m_pars.bin = m_hw_bin;
}

void CtVideo::_check_video_mode(VideoMode aMode)
{
  std::list<VideoMode> aModeList;
  getSupportedVideoMode(aModeList);
  bool findMode = false;
  for(std::list<VideoMode>::iterator i = aModeList.begin();
      !findMode && i != aModeList.end();++i)
    findMode = aMode == *i;

  if(!findMode)
    throw LIMA_CTL_EXC(Error,"Video mode is not available for this camera");
}

/** @brief an Acquisition will start so,
 *  we have to stop video mode
 */ 
void CtVideo::_prepareAcq()
{
  DEB_MEMBER_FUNCT();
  DEB_TRACE() << DEB_VAR1(m_stopping_live);

  AutoMutex aLock(m_cond.mutex());

  while(m_stopping_live) m_cond.wait();

  m_stopping_live = true;
  aLock.unlock();

  if(m_has_video)
    m_video->setLive(false);
  
  aLock.lock();
  m_stopping_live = false;
  m_cond.signal();

  m_pars.live = false;
  m_image_counter = -1;
  _read_hw_params();
  
  m_read_image->frameNumber = -1;
  m_write_image->frameNumber = -1;
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
  exposure = 1.;
  gain = -1.;
  mode = Y8;
  roi.reset();
  bin.reset();
}
