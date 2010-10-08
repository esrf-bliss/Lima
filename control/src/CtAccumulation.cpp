#include "CtAccumulation.h"
#include "CtAcquisition.h"

using namespace lima;
//	     ******** CtAccumulation::Parameters ********
CtAccumulation::Parameters::Parameters() : 
  pixelThresholdValue(2^16),
  buffers_size(64),
  savingFlag(false),
  savePrefix("saturated_")
{
  reset();
}
void CtAccumulation::Parameters::reset()
{
  active = false;
}
//		   ******** CtAccumulation ********
CtAccumulation::CtAccumulation(CtControl &ct) : 
  m_ct(ct),m_lock(MutexAttr::Normal) {}

CtAccumulation::~CtAccumulation()
{
}

void CtAccumulation::setParameters(const Parameters &pars)
{
  AutoMutex aLock(m_lock);
  m_pars = pars;
}

void CtAccumulation::getParameters(Parameters& pars) const
{
  AutoMutex aLock(m_lock);
  pars = m_pars;
}

void CtAccumulation::setActive(bool activeFlag)
{
  AutoMutex aLock(m_lock);
  m_pars.active = activeFlag;
}

void CtAccumulation::getActive(bool &activeFlag)
{
  AutoMutex aLock(m_lock);
  activeFlag = m_pars.active;
}

void CtAccumulation::setPixelThresholdValue(int pixelThresholdValue)
{
  AutoMutex aLock(m_lock);
  m_pars.pixelThresholdValue = pixelThresholdValue;
}

void CtAccumulation::getPixelThresholdValue(int &pixelThresholdValue) const
{
  AutoMutex aLock(m_lock);
  pixelThresholdValue = m_pars.pixelThresholdValue;
}

void CtAccumulation::setBufferSize(int aBufferSize)
{
  AutoMutex aLock(m_lock);
  m_pars.buffers_size = aBufferSize;
}

void CtAccumulation::getBufferSize(int &aBufferSize) const
{
  AutoMutex aLock(m_lock);
  aBufferSize = m_pars.buffers_size;
}

void CtAccumulation::setSavingFlag(bool savingFlag)
{
  AutoMutex aLock(m_lock);
  m_pars.savingFlag = savingFlag;
}

void CtAccumulation::getSavingFlag(bool &savingFlag) const
{
  AutoMutex aLock(m_lock);
  savingFlag = m_pars.savingFlag;
}

void CtAccumulation::setSavePrefix(const std::string &savePrefix)
{
  AutoMutex aLock(m_lock);
  m_pars.savePrefix = savePrefix;
}
void CtAccumulation::getSavePrefix(std::string &savePrefix) const
{
  AutoMutex aLock(m_lock);
  savePrefix = m_pars.savePrefix;
}

/** @brief read the saturated image of accumulated image which id is frameNumber
    @parameters saturatedImage the saturated image conter
    @parameters frameNumber the frame acquisition id
 */
void CtAccumulation::readSaturatedImageCounter(Data &saturatedImage,long frameNumber)
{
  
}
/** @brief read the saturated counters
    @parameters from is the start frame acquisition id
    @parameters result It's a list of list of saturated counters. 
    i.e: from == 5 result == [[2,3,2],[4,3,2],...] : so first list [2,3,2] is the saturated counters of image 5
*/
void CtAccumulation::readSaturatedSumCounter(int from,CtAccumulation::saturatedCounterResult &result)
{
}

/** @brief set the mask for saturation calculation
 */
void CtAccumulation::setMask(Data&)
{
}

void CtAccumulation::registerThresholdCallback(ThresholdCallback &cb)
{

}

void CtAccumulation::unregisterThresholdCallback(ThresholdCallback &cb)
{

}

void CtAccumulation::_calcSaturatedImage(Data &src)
{
}
/** @brief prepare all stuff for a new acquisition
 */
void CtAccumulation::prepare()
{
  AutoMutex aLock(m_lock);
  m_datas.clear();
  m_saturated_images.clear();
}
/** @brief this is an intergnal call from CtBuffer in case of accumulation
 */
bool CtAccumulation::newFrameReady(Data &aData)
{
  CtAcquisition *acq = m_ct.acquisition();
  int nb_acc_frame;
  acq->getAccNbFrames(nb_acc_frame);
  AutoMutex aLock(m_lock);
  if(!(aData.frameNumber % nb_acc_frame)) // new Data has to be created
    {
      int nextFrameNumber;
      if(m_datas.empty())	// Init (first Frame)
	nextFrameNumber = 0;
      else
	nextFrameNumber = m_datas.back().frameNumber + 1;

      Data newData;
      newData.type = Data::INT32;
      newData.width = aData.width;
      newData.height = aData.height;
      newData.frameNumber = nextFrameNumber;
      newData.timestamp = aData.timestamp;
      newData.buffer = new Buffer(newData.size());
      memset(newData.data(),0,newData.size());
      m_datas.push_back(newData);
      
      if(m_datas.size() > m_pars.buffers_size)
	m_datas.pop_front();
    }
  bool active = m_pars.active;
  Data &accFrame = m_datas.back();
  aLock.unlock();

  _accFrame(aData,accFrame);
  if(active)
    _calcSaturatedImage(aData);

  if(!((aData.frameNumber + 1) % nb_acc_frame))
    return m_ct.newFrameReady(accFrame);
  else
    return true;			// Always continue @see if it may have an overun
}
/** @brief retrived the image from the buffer
    @parameters frameNumber == acquisition image id
    @return aReturnData the associated data
 */
void CtAccumulation::getFrame(Data &aReturnData,int frameNumber)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(frameNumber);
  AutoMutex aLock(m_lock);
  if(m_datas.empty())		// something weard append!
    THROW_CTL_ERROR(InvalidValue) << "Something weard append should be never in that case";
  
  if(frameNumber < 0)		// means last
    aReturnData = m_datas.back();
  else
    {
      int oldestFrameNumber = m_datas.front().frameNumber;
      int lastFrameId = m_datas.back().frameNumber;
      // No more into buffer list
      if(frameNumber < oldestFrameNumber || frameNumber > lastFrameId)
	THROW_CTL_ERROR(Error) << "Frame " << frameNumber << " not available";
      else
	aReturnData = m_datas[frameNumber - oldestFrameNumber];
    }
}


template <class SrcType, class DstType> 
void accumulateFrame(void *src_ptr,void *dst_ptr,int nb_items)
{
  SrcType *sp  = (SrcType *) src_ptr;
  DstType *dp = (DstType *) dst_ptr;
	
  for(int i = nb_items;i;--i,++sp,++dp)
    *dp += *sp;
}

void CtAccumulation::_accFrame(Data &src,Data &dst)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(src,dst);

  int nb_items = src.width * src.height;
  switch(src.type)
    {
    case Data::UINT8: 	accumulateFrame<unsigned char,int>(src.data(),dst.data(),nb_items);break;
    case Data::INT8: 	accumulateFrame<char,int>(src.data(),dst.data(),nb_items);break;
    case Data::UINT16: 	accumulateFrame<unsigned short,int>(src.data(),dst.data(),nb_items);break;
    case Data::INT16: 	accumulateFrame<short,int>(src.data(),dst.data(),nb_items);break;
    case Data::UINT32: 	accumulateFrame<unsigned int,int>(src.data(),dst.data(),nb_items);break;
    case Data::INT32: 	accumulateFrame<int,int>(src.data(),dst.data(),nb_items);break;
    default:
      THROW_CTL_ERROR(Error) << "Data type for accumulation is not yet managed";
    }
}
