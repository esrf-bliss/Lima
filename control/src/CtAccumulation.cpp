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
#include "CtAccumulation.h"
#include "CtAcquisition.h"
#include "CtBuffer.h"
#include "SinkTask.h"
#ifndef __unix
#include "SinkTaskMgr.i"
#endif
using namespace lima;
/****************************************************************************
CtAccumulation::_ImageReady4AccCallback
****************************************************************************/
CtAccumulation::_ImageReady4AccCallback::_ImageReady4AccCallback(CtAccumulation &acc) :
  TaskEventCallback(),m_acc(acc) {}

void CtAccumulation::_ImageReady4AccCallback::finished(Data &aData)
{
  m_acc._newBaseFrameReady(aData);
}

/*********************************************************************************
			   calculation task
*********************************************************************************/
template<class INPUT>
static long long _calc_saturated_image_n_counter(const Data &src,Data &dst,
						 const Data &mask,long long pixelThresholdValue)
{
  const INPUT *aSrcPt,*aEndPt;
  aSrcPt = (const INPUT*)src.data();
  int pixelnb = src.dimensions[0] * src.dimensions[1];
  aEndPt = aSrcPt + pixelnb;
  long long saturatedCounter = 0;
  unsigned short *saturatedImagePt = (unsigned short*)dst.data();
  const char *maskPt = (const char*)mask.data();
  while(aSrcPt < aEndPt)
    {
      INPUT pixelValue = *aSrcPt;
      if((!maskPt || *maskPt) &&
	 pixelValue > pixelThresholdValue)
	{
	  ++(*saturatedImagePt);
	  ++saturatedCounter;
	}
      ++aSrcPt,++saturatedImagePt;
      if(maskPt) ++maskPt;
    }
  return saturatedCounter;
}

class CtAccumulation::_CalcSaturatedTask : public SinkTask<CtAccumulation::_CounterResult>
{
  DEB_CLASS_NAMESPC(DebModControl,"Calculate saturated pixel","Control");
public:
  _CalcSaturatedTask(_CalcSaturatedTaskMgr &mgr,CtAccumulation &acc_cnt,Data &dst) :
    SinkTask<CtAccumulation::_CounterResult>(mgr),
    m_cnt(acc_cnt),
    m_dst(dst)
 {}

  virtual void process(Data &aData)
  {
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(aData);
    AutoMutex aLock(m_cnt.m_cond.mutex());
    long long pixelThresholdValue = m_cnt.m_pars.pixelThresholdValue;
    aLock.unlock();

    CtAccumulation::_CounterResult result(aData.frameNumber);
    Data &mask = m_cnt.m_calc_mask;

    if(aData.dimensions != m_dst.dimensions)
      {
      	DEB_ERROR() << "Saturated image size is != form data src";
	return;
      }

    if(!mask.empty())
      {
	if(mask.depth() != 1)
	  {
	    DEB_ERROR() << "mask should by an unsigned/signed char";
	    return;
	  }
	if(mask.dimensions != aData.dimensions)
	  {
	    DEB_ERROR() << "mask size is != with data size";
	    return;
	  }
      }

    switch(aData.type)
      {
      case Data::UINT8:
	result.value = 
	  _calc_saturated_image_n_counter<unsigned char>(aData,m_dst,
							  mask,pixelThresholdValue);
	break;
      case Data::INT8:
	result.value = 
	  _calc_saturated_image_n_counter<char>(aData,m_dst,
						mask,pixelThresholdValue);
	break;
      case Data::UINT16:
	result.value = 
	  _calc_saturated_image_n_counter<unsigned short>(aData,m_dst,
							  mask,pixelThresholdValue);
	break;
      case Data::INT16:
	result.value = 
	  _calc_saturated_image_n_counter<short>(aData,m_dst,
						 mask,pixelThresholdValue);
	break;
      case Data::UINT32:
	result.value = 
	  _calc_saturated_image_n_counter<unsigned int>(aData,m_dst,
							mask,pixelThresholdValue);
	break;
      case Data::INT32:
	result.value = 
	  _calc_saturated_image_n_counter<int>(aData,m_dst,
					       mask,pixelThresholdValue);
	break;
      default:
	DEB_ERROR() << "Wrong image type to calculate saturated image";
	return;
      }
    _mgr.setResult(result);
    m_cnt._callIfNeedThresholdCallback(aData,result.value);
  }
private:
  CtAccumulation& 	m_cnt;
  Data			m_dst;
};

class CtAccumulation::_CalcEndCBK : public TaskEventCallback
{
  DEB_CLASS_NAMESPC(DebModControl,"_CalcEndCBK","Control");
public:
  _CalcEndCBK(CtAccumulation &cnt) : m_cnt(cnt) {}
  virtual void finished(Data &aData)
  {
    AutoMutex Lock(m_cnt.m_cond.mutex());
    if(m_cnt.m_calc_pending_data.empty())
      {
	m_cnt.m_calc_ready = true;
	m_cnt.m_cond.broadcast(); // in case of waiting for m_calc_ready
      }
    else
      {
	std::pair<Data,Data> values = m_cnt.m_calc_pending_data.front();
	m_cnt.m_calc_pending_data.pop_front();

	_CalcSaturatedTask *calcTaskPt = new _CalcSaturatedTask(*m_cnt.m_calc_mgr,
								m_cnt,values.second);
	calcTaskPt->setEventCallback(m_cnt.m_calc_end);
	TaskMgr *aCalcMgrPt = new TaskMgr();
	aCalcMgrPt->addSinkTask(0,calcTaskPt);
	calcTaskPt->unref();
	aCalcMgrPt->setInputData(values.first);
	PoolThreadMgr::get().addProcess(aCalcMgrPt);
      }
  }
private:
  CtAccumulation& m_cnt;
};

//	     ******** CtAccumulation::Parameters ********
CtAccumulation::Parameters::Parameters() : 
  pixelThresholdValue(2^16),
  savingFlag(false),
  savePrefix("saturated_")
{
  reset();
}
void CtAccumulation::Parameters::reset()
{
  active = false;
}

#ifdef WITH_CONFIG
//		   ******** _ConfigHandler ********
class CtAccumulation::_ConfigHandler : public CtConfig::ModuleTypeCallback
{
public:
  _ConfigHandler(CtAccumulation& acc) :
    CtConfig::ModuleTypeCallback("Accumulation"),
    m_acc(acc) {}

  virtual void store(Setting& accumulation_setting)
  {
    CtAccumulation::Parameters pars;
    m_acc.getParameters(pars);
  
    accumulation_setting.set("active",pars.active);
    accumulation_setting.set("pixelThresholdValue",pars.pixelThresholdValue);
    accumulation_setting.set("savingFlag",pars.savingFlag);
    accumulation_setting.set("savePrefix",pars.savePrefix);
  }
  virtual void restore(const Setting& accumulation_setting)
  {
    CtAccumulation::Parameters pars;
    m_acc.getParameters(pars);
  
    accumulation_setting.get("active",pars.active);
    accumulation_setting.get("pixelThresholdValue",
			     pars.pixelThresholdValue);
    accumulation_setting.get("savingFlag",pars.savingFlag);
    accumulation_setting.get("savePrefix",pars.savePrefix);

    m_acc.setParameters(pars);
  }
private:
  CtAccumulation& m_acc;
};
#endif //WITH_CONFIG

//		   ******** CtAccumulation ********
CtAccumulation::CtAccumulation(CtControl &ct) : 
  m_buffers_size(16),
  m_ct(ct),
  m_calc_ready(true),
  m_threshold_cb(NULL),
  m_last_acc_frame_nb(-1),
  m_last_continue_flag(true)
{
  m_calc_end = new _CalcEndCBK(*this);
  m_calc_mgr = new _CalcSaturatedTaskMgr();
}

CtAccumulation::~CtAccumulation()
{
  AutoMutex aLock(m_cond.mutex());
  while(!m_calc_ready)
    m_cond.wait();

  m_calc_end->unref();
  m_calc_mgr->unref();
}

void CtAccumulation::setParameters(const Parameters &pars)
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  while(!m_calc_ready)
    m_cond.wait();

  m_pars = pars;
}

void CtAccumulation::getParameters(Parameters& pars) const
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  pars = m_pars;
}

void CtAccumulation::setActive(bool activeFlag)
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  m_pars.active = activeFlag;
}

void CtAccumulation::getActive(bool &activeFlag)
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  activeFlag = m_pars.active;
}

void CtAccumulation::setPixelThresholdValue(long long pixelThresholdValue)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(pixelThresholdValue);

  AutoMutex aLock(m_cond.mutex());
  m_pars.pixelThresholdValue = pixelThresholdValue;
}

void CtAccumulation::getPixelThresholdValue(long long &pixelThresholdValue) const
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  pixelThresholdValue = m_pars.pixelThresholdValue;
  DEB_RETURN() << DEB_VAR1(pixelThresholdValue);
}

void CtAccumulation::getBufferSize(int &aBufferSize) const
{ 
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  aBufferSize = m_buffers_size;
  DEB_RETURN() << DEB_VAR1(aBufferSize);
}

void CtAccumulation::setSavingFlag(bool savingFlag)
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  m_pars.savingFlag = savingFlag;
}

void CtAccumulation::getSavingFlag(bool &savingFlag) const
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  savingFlag = m_pars.savingFlag;
}

void CtAccumulation::setSavePrefix(const std::string &savePrefix)
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  m_pars.savePrefix = savePrefix;
}
void CtAccumulation::getSavePrefix(std::string &savePrefix) const
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  savePrefix = m_pars.savePrefix;
}

/** @brief read the saturated image of accumulated image which id is frameNumber
    @param saturatedImage the saturated image conter (empty if not yet available)
    @param frameNumber the frame acquisition id
 */
void CtAccumulation::readSaturatedImageCounter(Data &saturatedImage,long frameNumber)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(frameNumber);

  CtAcquisition *acquisition = m_ct.acquisition();
  int acc_nframes;
  acquisition->getAccNbFrames(acc_nframes);

  AutoMutex aLock(m_cond.mutex());
  if(frameNumber < 0)
    {
      if(!m_saturated_images.empty())
	frameNumber = m_saturated_images.back().frameNumber;
      else
	frameNumber = 0;
    }
  aLock.unlock();

  //ask the last accumulated frame for a frameNumber
  int counterId = ((frameNumber + 1) * acc_nframes) - 1;
  CtAccumulation::_CounterResult result = m_calc_mgr->getResult(0.,counterId);

  if(result.errorCode == _CalcSaturatedTaskMgr::NO_MORE_AVAILABLE)
    THROW_CTL_ERROR(Error) << "Frame " << frameNumber << " not more available";

  // Counter is available => saturated image calc finnished
  if(result.errorCode == _CalcSaturatedTaskMgr::OK)
    {
      aLock.lock();
      int oldestFrameNumber = m_saturated_images.front().frameNumber;
      int lastFrameId = m_saturated_images.back().frameNumber;
      //No more into buffer list
      if(frameNumber < oldestFrameNumber || frameNumber > lastFrameId)
	THROW_CTL_ERROR(Error) << "Frame " << frameNumber << " not more available";
      else
	saturatedImage = m_saturated_images[frameNumber - oldestFrameNumber];
    }
  DEB_RETURN() << DEB_VAR1(saturatedImage);
}
/** @brief read the saturated counters
    @param from is the start frame acquisition id
    @param result It's a list of list of saturated counters. 
    i.e: from == 5 result == [[2,3,2],[4,3,2],...] : so first list [2,3,2] is the saturated counters of image 5
*/
void CtAccumulation::readSaturatedSumCounter(CtAccumulation::saturatedCounterResult &result,int from)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(from);

  CtAcquisition *acquisition = m_ct.acquisition();
  int acc_nframes;
  acquisition->getAccNbFrames(acc_nframes);
  if(acc_nframes > 0)
    {
      AutoMutex aLock(m_cond.mutex());
      if(from < 0)
	{
	  if(!m_saturated_images.empty())
	    from = m_saturated_images.back().frameNumber;
	  else
	    from = 0;
	}
      aLock.unlock();
      std::list<CtAccumulation::_CounterResult> resultList;
      int fromCounterId = from * acc_nframes;
      m_calc_mgr->getHistory(resultList,fromCounterId);

      for(std::list<CtAccumulation::_CounterResult>::iterator i = resultList.begin();
	  i != resultList.end();++i)
	{
	  if(!(i->frameNumber % acc_nframes))
	    result.push_back(std::list<long long>());
	  
	  std::list<long long> &satImgCounters = result.back();
	  satImgCounters.push_back(i->value);
	}
      /* Check if last image has the same number of counters
	 if not remove */
      if(!result.empty() &&
	 result.front().size() != result.back().size())
	result.pop_back();
    }
}

/** @brief set the mask for saturation calculation
 *  @param mask the mask data image, empty mask == unset
 */
void CtAccumulation::setMask(Data &mask)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(mask);

  AutoMutex aLock(m_cond.mutex());
  while(!m_calc_ready)
    m_cond.wait();

  m_calc_mask = mask.mask();
}

void CtAccumulation::registerThresholdCallback(ThresholdCallback &cb)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(&cb, m_threshold_cb);

  AutoMutex aLock(m_cond.mutex());
  while(!m_calc_ready)
    m_cond.wait();

  if(m_threshold_cb)
    THROW_CTL_ERROR(InvalidValue) <<  "ThresholdCallback already registered";

  m_threshold_cb = &cb;
}

void CtAccumulation::unregisterThresholdCallback(ThresholdCallback &cb)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(&cb, m_threshold_cb);

  AutoMutex aLock(m_cond.mutex());
  while(!m_calc_ready)
    m_cond.wait();
  
  if(m_threshold_cb != &cb)
    THROW_CTL_ERROR(InvalidValue) <<  "ThresholdCallback not registered";

  m_threshold_cb = NULL;
}

void CtAccumulation::_calcSaturatedImageNCounters(Data &src,Data &dst)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(src,dst);

  Data copiedSrc = src.copy();
  AutoMutex Lock(m_cond.mutex());
  if(m_calc_ready)
    {
      m_calc_ready = false;

      _CalcSaturatedTask *calcTaskPt = new _CalcSaturatedTask(*m_calc_mgr,*this,dst);
      calcTaskPt->setEventCallback(m_calc_end);
      TaskMgr *aCalcMgrPt = new TaskMgr();
      aCalcMgrPt->addSinkTask(0,calcTaskPt);
      calcTaskPt->unref();
      aCalcMgrPt->setInputData(copiedSrc);
      PoolThreadMgr::get().addProcess(aCalcMgrPt);
    }
  else
    {
      std::pair<Data,Data> values(copiedSrc,dst);
      m_calc_pending_data.push_back(values);
    }
}
void CtAccumulation::_callIfNeedThresholdCallback(Data &aData,long long value)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(aData,value);

  if(m_threshold_cb && value > m_threshold_cb->m_max)
    m_threshold_cb->aboveMax(aData,value);
}
/** @brief prepare all stuff for a new acquisition
 */
void CtAccumulation::prepare()
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  m_datas.clear();
  m_saturated_images.clear();
  CtBuffer *buffer = m_ct.buffer();
  buffer->getNumber(m_buffers_size);
  CtAcquisition *acquisition = m_ct.acquisition();
  int acc_nframes;
  acquisition->getAccNbFrames(acc_nframes);
  if(acc_nframes < 0) acc_nframes = 1;

  m_calc_mgr->resizeHistory(m_buffers_size * acc_nframes);
  m_last_continue_flag = true;
  m_last_acc_frame_nb = -1;
}
/** @brief this is an internal call from CtBuffer in case of accumulation
 */
bool CtAccumulation::_newFrameReady(Data &aData)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(aData);

  TaskMgr *mgr = new TaskMgr();
  mgr->setInputData(aData);
  int internal_stage = 0;
  m_ct.m_op_int->addTo(*mgr,internal_stage);

  if(internal_stage)
    PoolThreadMgr::get().addProcess(mgr);
  else
    {
      delete mgr;
      m_last_continue_flag = m_last_continue_flag && _newBaseFrameReady(aData);
    }
  return m_last_continue_flag;
}
/** @brief this is an internal call at the end of internal process or from CtBuffer
 */
bool CtAccumulation::_newBaseFrameReady(Data &aData)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(aData);

  CtAcquisition *acq = m_ct.acquisition();
  int nb_acc_frame;
  acq->getAccNbFrames(nb_acc_frame);
  AutoMutex aLock(m_cond.mutex());
  while(aData.frameNumber != (m_last_acc_frame_nb + 1))
    m_cond.wait();

  bool active = m_pars.active;
  if(!(aData.frameNumber % nb_acc_frame)) // new Data has to be created
    {
      int nextFrameNumber;
      if(m_datas.empty())	// Init (first Frame)
	nextFrameNumber = 0;
      else
	nextFrameNumber = m_datas.back().frameNumber + 1;

      Data newData;
      newData.type = Data::INT32;
      newData.dimensions = aData.dimensions;
      newData.frameNumber = nextFrameNumber;
      newData.timestamp = aData.timestamp;
      newData.buffer = new Buffer(newData.size());
      memset(newData.data(),0,newData.size());
      m_datas.push_back(newData);

      if(long(m_datas.size()) > m_buffers_size)
	m_datas.pop_front();

      // create also the new image for saturated counters
      if(active)
	{
	  Data newSatImg;
	  newSatImg.type = Data::UINT16;
	  newSatImg.dimensions = aData.dimensions;
	  newSatImg.frameNumber = nextFrameNumber;
	  newSatImg.timestamp = aData.timestamp;
	  newSatImg.buffer = new Buffer(newSatImg.size());
	  memset(newSatImg.data(),0,newSatImg.size());
	  m_saturated_images.push_back(newSatImg);

	  if(long(m_saturated_images.size()) > m_buffers_size)
	    m_saturated_images.pop_front();
	}

    }
  Data accFrame = m_datas.back();
  Data saturatedImg;
  if(active)
    saturatedImg = m_saturated_images.back();
  aLock.unlock();

  if(active)
    _calcSaturatedImageNCounters(aData,saturatedImg);

  _accFrame(aData,accFrame);

  if(!((aData.frameNumber + 1) % nb_acc_frame))
    m_last_continue_flag = m_ct.newFrameReady(accFrame);

  aLock.lock();
  m_last_acc_frame_nb = aData.frameNumber;
  m_cond.broadcast();

  return m_last_continue_flag;
}
/** @brief retrived the image from the buffer
    @param frameNumber == acquisition image id
    @return aReturnData the associated data
 */
void CtAccumulation::getFrame(Data &aReturnData,int frameNumber)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(frameNumber);
  AutoMutex aLock(m_cond.mutex());
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

  int nb_items = src.dimensions[0] * src.dimensions[1];
  switch(src.type)
    {
    case Data::UINT8: 	accumulateFrame<unsigned char,int>	(src.data(),dst.data(),nb_items);break;
    case Data::INT8: 	accumulateFrame<char,int>		(src.data(),dst.data(),nb_items);break;
    case Data::UINT16: 	accumulateFrame<unsigned short,int>	(src.data(),dst.data(),nb_items);break;
    case Data::INT16: 	accumulateFrame<short,int>		(src.data(),dst.data(),nb_items);break;
    case Data::UINT32: 	accumulateFrame<unsigned int,int>	(src.data(),dst.data(),nb_items);break;
    case Data::INT32: 	accumulateFrame<int,int>		(src.data(),dst.data(),nb_items);break;
    default:
      THROW_CTL_ERROR(Error) << "Data type for accumulation is not yet managed";
    }
}

#ifdef WITH_CONFIG
CtConfig::ModuleTypeCallback* CtAccumulation::_getConfigHandler()
{
  return new _ConfigHandler(*this);
}
#endif //WITH_CONFIG