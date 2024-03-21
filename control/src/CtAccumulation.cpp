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
#include "lima/CtAccumulation.h"
#include "lima/CtAcquisition.h"
#include "lima/CtBuffer.h"
#include "processlib/SinkTask.h"
#include "processlib/SinkTaskMgr.h"
#include <algorithm>
#include <type_traits>

#ifdef __unix
#include <malloc.h>
#endif

using namespace lima;

const long CtAccumulation::ACC_MIN_BUFFER_SIZE;
const int CtAccumulation::ACC_MAX_PARALLEL_PROC;


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
Algorithm
*********************************************************************************/
struct pixel_accumulate
{
    template <class SrcType, class DstType>
    void operator()(SrcType const& src, DstType& dst) const {
        dst += src;
    }
};

struct pixel_accumulate_threshold
{
    pixel_accumulate_threshold(long long threshold) :
        threshold_(threshold) {}

    template <class SrcType, class DstType>
    void operator()(SrcType const& src, DstType& dst) const {
        if (src > threshold_)
            dst += src;
    }

    long long threshold_ = 0;
};

struct pixel_accumulate_offset_threshold
{
    pixel_accumulate_offset_threshold(long long offset, long long threshold) :
        offset_(offset), threshold_(threshold) {}

    template <class SrcType, class DstType>
    void operator()(SrcType const& src, DstType& dst) const {
        DstType tmp_d = src - offset_;
        if (!std::is_signed<DstType>::value && (src < offset_))
            tmp_d = 0;
        if (tmp_d > threshold_)
            dst += tmp_d;
    }

    long long threshold_ = 0;
    long long offset_ = 0;
};

struct pixel_cast
{
    template <class SrcType, class DstType>
    void operator()(SrcType const& src, DstType& dst) const {
        dst = src;
    }
};

struct pixel_divide
{
    pixel_divide(unsigned int denom) : denom_(denom) {}

    template <class DstType>
    void operator()(unsigned int src, DstType& dst) const {
        dst = src / denom_;
    }

    unsigned int denom_ = 1;
};

template <class SrcType, class DstType, class Func>
void transform_pixel(void const* const src_ptr, void* const dst_ptr, int nb_items, Func fn)
{
    SrcType* sp = (SrcType*)src_ptr;
    DstType* dp = (DstType*)dst_ptr;

    for (int i = nb_items; i; --i, ++sp, ++dp)
        fn(*sp, *dp);
}

template <class Func>
void transform_pixel(Data& src, Data& dst, Func fn)
{
    int nb_items = src.dimensions[0] * src.dimensions[1];
    switch (src.type)
    {
    case Data::UINT8:
        switch (dst.type)
        {
        case Data::UINT16: 	return transform_pixel<unsigned char, unsigned short>(src.data(), dst.data(), nb_items, fn);
        case Data::INT16: 	return transform_pixel<unsigned char, short>(src.data(), dst.data(), nb_items, fn);
        case Data::UINT32: 	return transform_pixel<unsigned char, unsigned int>(src.data(), dst.data(), nb_items, fn);
        case Data::INT32: 	return transform_pixel<unsigned char, int>(src.data(), dst.data(), nb_items, fn);
        }
        break;

    case Data::INT8:
        switch (dst.type)
        {
        case Data::INT16: 	return transform_pixel<char, short>(src.data(), dst.data(), nb_items, fn);
        case Data::INT32: 	return transform_pixel<char, int>(src.data(), dst.data(), nb_items, fn);
        }
        break;

    case Data::UINT16:
        switch (dst.type)
        {
        case Data::UINT16: 	return transform_pixel<unsigned short, unsigned short>(src.data(), dst.data(), nb_items, fn);
        case Data::INT16: 	return transform_pixel<unsigned short, short>(src.data(), dst.data(), nb_items, fn);
        case Data::UINT32: 	return transform_pixel<unsigned short, unsigned int>(src.data(), dst.data(), nb_items, fn);
        case Data::INT32: 	return transform_pixel<unsigned short, int>(src.data(), dst.data(), nb_items, fn);
        }
        break;

    case Data::INT16:
        switch (dst.type)
        {
        case Data::INT16: 	return transform_pixel<short, short>(src.data(), dst.data(), nb_items, fn);
        case Data::INT32: 	return transform_pixel<short, int>(src.data(), dst.data(), nb_items, fn);
        }
        break;

    case Data::UINT32:
        switch (dst.type)
        {
        case Data::UINT16: 	return transform_pixel<unsigned int, unsigned short>(src.data(), dst.data(), nb_items, fn);
        case Data::INT16: 	return transform_pixel<unsigned int, short>(src.data(), dst.data(), nb_items, fn);
        case Data::UINT32:  return transform_pixel<unsigned int, unsigned int>(src.data(), dst.data(), nb_items, fn);
        case Data::INT32:   return transform_pixel<unsigned int, int>(src.data(), dst.data(), nb_items, fn);        
        }
        break;

    case Data::INT32:
        switch (dst.type)
        {
        case Data::INT16: 	return transform_pixel<int, short>(src.data(), dst.data(), nb_items, fn);
        case Data::INT32:   return transform_pixel<int, int>(src.data(), dst.data(), nb_items, fn);
        }
        break;
    }

    throw LIMA_CTL_EXC(Error, "Output data type for accumulation is not supported for input type ")
        << "SRC=" << convert_2_string(src.type) << " DST=" << convert_2_string(dst.type);
}

/*********************************************************************************
			   accumulation task
*********************************************************************************/
class CtAccumulation::_ProcAccTask : public SinkTaskBase
{
public:
  _ProcAccTask(CtAccumulation &acc_cnt) : m_cnt(acc_cnt)
  {}

  virtual void process(Data &aData)
  {
    m_cnt._newBaseFrameReady(aData);
  }

private:
  CtAccumulation& m_cnt;
};


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
    if((!maskPt || *maskPt) && pixelValue > pixelThresholdValue)
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
    long long pixelThresholdValue;
    {
      AutoMutex aLock(m_cnt.m_cond.mutex());
      pixelThresholdValue = m_cnt.m_pars.pixelThresholdValue;
    }

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
      result.value = _calc_saturated_image_n_counter<unsigned char>(
        aData,m_dst,mask,pixelThresholdValue);
      break;
    case Data::INT8:
      result.value =  _calc_saturated_image_n_counter<char>(
        aData,m_dst,mask,pixelThresholdValue);
      break;
    case Data::UINT16:
      result.value =  _calc_saturated_image_n_counter<unsigned short>(
        aData,m_dst,mask,pixelThresholdValue);
      break;
    case Data::INT16:
      result.value = _calc_saturated_image_n_counter<short>(
        aData,m_dst,mask,pixelThresholdValue);
      break;
    case Data::UINT32:
      result.value = _calc_saturated_image_n_counter<unsigned int>(
        aData,m_dst,mask,pixelThresholdValue);
      break;
    case Data::INT32:
      result.value = _calc_saturated_image_n_counter<int>(
        aData,m_dst,mask,pixelThresholdValue);
      break;
    default:
      DEB_ERROR() << "Wrong image type to calculate saturated image";
      return;
    }
    _mgr.setResult(result);
    m_cnt._callIfNeedThresholdCallback(aData,result.value);
  }
private:
  CtAccumulation& m_cnt;
  Data            m_dst;
};

class CtAccumulation::_CalcEndCBK : public TaskEventCallback
{
  DEB_CLASS_NAMESPC(DebModControl,"_CalcEndCBK","Control");
public:
  _CalcEndCBK(CtAccumulation &cnt) : m_cnt(cnt) {}
  virtual void finished(Data &/*aData*/)
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

//       ******** CtAccumulation::Parameters ********
CtAccumulation::Parameters::Parameters() :
  pixelThresholdValue(1 << 16),
  pixelOutputType(Bpp32S),
  savingFlag(false),
  savePrefix("saturated_"),
  mode(CtAccumulation::Parameters::STANDARD),
  operation(CtAccumulation::ACC_SUM),
  filter(CtAccumulation::FILTER_NONE),
  thresholdB4Acc(0),
  offsetB4Acc(0)
{
  reset();
}
void CtAccumulation::Parameters::reset()
{
  active = false;
}

#ifdef WITH_CONFIG
//       ******** _ConfigHandler ********
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

//       ******** CtAccumulation ********
CtAccumulation::CtAccumulation(CtControl &ct) :
  m_buffers_size(1),
  m_ct(ct),
  m_calc_ready(true),
  m_acc_nb_frames(0),
  m_threshold_cb(NULL),
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

void CtAccumulation::setOutputType(ImageType pixelOutputType)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(pixelOutputType);

  AutoMutex aLock(m_cond.mutex());
  m_pars.pixelOutputType = pixelOutputType;
}

void CtAccumulation::getOutputType(ImageType& pixelOutputType) const
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  pixelOutputType = m_pars.pixelOutputType;
  DEB_RETURN() << DEB_VAR1(pixelOutputType);
}

void CtAccumulation::getMaxNbBuffers(int &max_nb_buffers) const
{
  DEB_MEMBER_FUNCT();

  CtImage* image = m_ct.image();
  FrameDim hw_image_dim;
  image->getHwImageDim(hw_image_dim);
  FrameDim acc_image_dim;
  image->getImageDim(acc_image_dim);

  CtBuffer *buffer = m_ct.buffer();
  long max_hw_nb_buffers = 0;
  buffer->getMaxHwNumber(max_hw_nb_buffers);

  bool is_sat_active;
  {
    AutoMutex aLock(m_cond.mutex());
    is_sat_active = m_pars.active;
  }

  DEB_TRACE() << DEB_VAR4(hw_image_dim, acc_image_dim, max_hw_nb_buffers,
                          is_sat_active);

  int hw_image_depth = hw_image_dim.getDepth();
  int acc_image_depth = acc_image_dim.getDepth();
  if(is_sat_active) acc_image_depth += 2;
  DEB_TRACE() << DEB_VAR2(hw_image_depth, acc_image_depth);
  max_nb_buffers = long(max_hw_nb_buffers * double(hw_image_depth) /
                        acc_image_depth);

  DEB_RETURN() << DEB_VAR1(max_nb_buffers);
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

void CtAccumulation::getMode(Parameters::Mode &mode) const
{
  AutoMutex aLock(m_cond.mutex());
  mode = m_pars.mode;
}

void CtAccumulation::setMode(Parameters::Mode mode)
{
  AutoMutex aLock(m_cond.mutex());
  m_pars.mode = mode;

  switch (mode)
  {
  case Parameters::STANDARD:
    m_pars.filter = Filter::FILTER_NONE;
    m_pars.operation = Operation::ACC_SUM;
    break;

  case Parameters::THRESHOLD_BEFORE:
    m_pars.filter = Filter::FILTER_THRESHOLD_MIN;
    m_pars.operation = Operation::ACC_SUM;
    break;

  case Parameters::OFFSET_THEN_THRESHOLD_BEFORE:
    m_pars.filter = Filter::FILTER_OFFSET_THEN_THRESHOLD_MIN;
    m_pars.operation = Operation::ACC_SUM;
    break;
  }
}

void CtAccumulation::getFilter(Filter& filter) const
{
  AutoMutex aLock(m_cond.mutex());
  filter = m_pars.filter;
}

void CtAccumulation::setFilter(Filter filter)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(filter);

  AutoMutex aLock(m_cond.mutex());
  m_pars.filter = filter;
}

void CtAccumulation::getOperation(Operation& operation) const
{
  AutoMutex aLock(m_cond.mutex());
  operation = m_pars.operation;
}

void CtAccumulation::setOperation(Operation operation)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(operation);

  if (operation == ACC_MEDIAN)
    THROW_CTL_ERROR(Error) << "Median accumulator not implemented";

  AutoMutex aLock(m_cond.mutex());
  m_pars.operation = operation;
}

void CtAccumulation::getThresholdBefore(long long& threshold) const
{
  AutoMutex aLock(m_cond.mutex());
  threshold = m_pars.thresholdB4Acc;
}

void CtAccumulation::setThresholdBefore(const long long& threshold)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(threshold);

  AutoMutex aLock(m_cond.mutex());
  m_pars.thresholdB4Acc = threshold;
}

void CtAccumulation::getOffsetBefore(long long& offset) const
{
  AutoMutex aLock(m_cond.mutex());
  offset = m_pars.offsetB4Acc;
}

void CtAccumulation::setOffsetBefore(const long long& offset)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(offset);

  AutoMutex aLock(m_cond.mutex());
  m_pars.offsetB4Acc = offset;
}

/** @brief read the saturated image of accumulated image which id is frameNumber
    @param saturatedImage the saturated image conter (empty if not yet available)
    @param frameNumber the frame acquisition id
 */
void CtAccumulation::readSaturatedImageCounter(Data &saturatedImage,long frameNumber)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(frameNumber);

  int acc_nframes;
  {
    AutoMutex aLock(m_cond.mutex());
    if(frameNumber < 0)
    {
      if(!m_saturated_images.empty())
        frameNumber = m_saturated_images.back().frameNumber;
      else
        frameNumber = 0;
    }
    acc_nframes = m_acc_nb_frames;
  }

  //ask the last accumulated frame for a frameNumber
  int counterId = ((frameNumber + 1) * acc_nframes) - 1;
  CtAccumulation::_CounterResult result = m_calc_mgr->getResult(0.,counterId);

  if(result.errorCode == _CalcSaturatedTaskMgr::NO_MORE_AVAILABLE)
    THROW_CTL_ERROR(Error) << "Frame " << frameNumber << " not more available";

  // Counter is available => saturated image calc finnished
  if(result.errorCode == _CalcSaturatedTaskMgr::OK)
  {
    AutoMutex aLock(m_cond.mutex());
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

  int acc_nframes;
  {
    AutoMutex aLock(m_cond.mutex());
    if(m_acc_nb_frames <= 0)
      return;
    if(from < 0)
    {
      if(!m_saturated_images.empty())
        from = m_saturated_images.back().frameNumber;
      else
        from = 0;
    }
    acc_nframes = m_acc_nb_frames;
  }

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

  /* Check if last image has the same number of counters if not remove */
  if(!result.empty() && result.front().size() != result.back().size())
    result.pop_back();
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
/** @brief clear all buffers
 */
void CtAccumulation::clear()
{
    AutoMutex aLock(m_cond.mutex());
    m_proc_info_map.clear();
    m_datas.clear();
    m_saturated_images.clear();

#ifdef __unix
    bool use_malloc_trim = true;
    if (use_malloc_trim)
        malloc_trim(0);
#endif
}

/** @brief prepare all stuff for a new acquisition
 */
void CtAccumulation::prepare()
{
  DEB_MEMBER_FUNCT();

  // Gather acq. parameters
  CtAcquisition *acquisition = m_ct.acquisition();
  int acc_nframes = 0;
  acquisition->getAccNbFrames(acc_nframes);
  if(acc_nframes < 0) acc_nframes = 1;

  // Buffer parameters
  CtBuffer *buffer = m_ct.buffer();
  long nb_buffers = 0;
  buffer->getNumber(nb_buffers);

  // Adjust number of acc image
  CtImage* image = m_ct.image();
  FrameDim hw_image_dim;
  image->getHwImageDim(hw_image_dim);
  int hw_image_depth = hw_image_dim.getDepth();
  FrameDim acc_image_dim;
  image->getImageDim(acc_image_dim);
  int acc_image_depth = acc_image_dim.getDepth();

  if ((hw_image_depth > acc_image_depth) || (hw_image_dim.isSigned() && !acc_image_dim.isSigned()))
    throw LIMA_CTL_EXC(Error, "Output data type for accumulation is not supported for input type ")
      << "SRC=" << convert_2_string(hw_image_dim.getImageType())
      << " DST=" << convert_2_string(acc_image_dim.getImageType());

  AutoMutex aLock(m_cond.mutex());
  m_acc_nb_frames = acc_nframes;
  m_buffers_size = std::max(1, int(nb_buffers));
  m_signed_data = hw_image_dim.isSigned();

  // Allocate the temporary data (if needed)
  switch (m_pars.operation) {
  case ACC_SUM:
    break;

  case ACC_MEAN:
    break;

  case ACC_MEDIAN:
    THROW_CTL_ERROR(NotSupported) << "ACC_MEDIAN mode is not supported yet";
  }

  m_calc_mgr->resizeHistory(m_buffers_size * acc_nframes);
  m_last_continue_flag = true;
}
/** @brief this is an internal call from CtBuffer in case of accumulation
 */
bool CtAccumulation::_newFrameReady(Data &aData)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(aData);

  bool stop = false;
  {
    AutoMutex aLock(m_cond.mutex());
    if(!m_last_continue_flag)
      return false;

    if(!m_proc_info_map.empty()) {
      _ProcAccInfo& old_proc = m_proc_info_map.begin()->second;
      if(!old_proc.new_pending_data.empty()) {
        Data& oData = old_proc.new_pending_data.begin()->second;
        if(aData.data() == oData.data()) {
          DEB_ERROR() << "Accumulation overrun: " << DEB_VAR2(aData, oData);
          m_last_continue_flag = false;
          stop = true;
        }
      }
    }
  }
  if (stop) {
    m_ct.stopAcqAsync(AcqFault, CtControl::ProcessingOverun, aData);
    return false;
  }

  TaskMgr *mgr = new TaskMgr();
  mgr->setEventCallback(m_ct.getSoftOpErrorHandler());
  mgr->setInputData(aData);

  // First check for SoftOpInternal (reconstruction, bin/roi/flip/rot)
  int internal_stage = 0;
  m_ct.m_op_int->addTo(*mgr,internal_stage);

  // If nothing is needed, just call the standard Processing
  if(!internal_stage) {
    SinkTaskBase *task = new _ProcAccTask(*this);
    mgr->addSinkTask(internal_stage,task);
    task->unref();
  }

  PoolThreadMgr::get().addProcess(mgr);

  AutoMutex aLock(m_cond.mutex());
  return m_last_continue_flag;
}
/** @brief this is an internal call at the end of internal process or from CtBuffer
 */
void CtAccumulation::_newBaseFrameReady(Data &aData)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(aData);

  AutoMutex aLock(m_cond.mutex());

  typedef std::map<int,_ProcAccInfo> ProcInfoMap;

  ProcInfoMap& procs = m_proc_info_map;
  int abs_frame = aData.frameNumber / m_acc_nb_frames;
  int rel_frame = aData.frameNumber % m_acc_nb_frames;
  ProcInfoMap::iterator proc_it = procs.find(abs_frame);
  if(proc_it == procs.end()) {
    if(procs.size() == ACC_MAX_PARALLEL_PROC)
      THROW_CTL_ERROR(Error) << "Too many parallel accumulation processings";
    proc_it = procs.emplace(std::make_pair(abs_frame,
                                           _ProcAccInfo(abs_frame))).first;
  }
  _ProcAccInfo& info = proc_it->second;
  std::map<int,Data>& pending = info.new_pending_data;
  
  if(rel_frame < info.acc_frames) {
    THROW_CTL_ERROR(Error) << "Frame already accumulated: " << aData;
  } else if(rel_frame > info.acc_frames) {
    pending.insert(std::make_pair(rel_frame, aData));
    return;
  }

  _processBaseFrame(info, aData, aLock);

  std::map<int,Data>::iterator it;
  while(!pending.empty() && (it = pending.begin())->first == info.acc_frames)
  {
    Data& oData = it->second;
    _processBaseFrame(info, oData, aLock);
    pending.erase(it);
  }

  if(info.acc_frames == m_acc_nb_frames)
    procs.erase(proc_it);
}

void CtAccumulation::_processBaseFrame(_ProcAccInfo &info, Data &aData,
                                       AutoMutex &aLock)
{
  bool active = m_pars.active;
  int nb_acc_frame = m_acc_nb_frames;
  Operation op = m_pars.operation;
  Data& acc_data = info.acc_data;
  Data& sat_data = info.sat_data;
  Data& tmp_data = info.tmp_data;

  if(info.acc_frames == 0) // new Data has to be created
  {
    int nextFrameNumber = info.frame_nb;

    // Release old data(s) before crossing the size limit
    if(long(m_datas.size()) == m_buffers_size)
      m_datas.pop_front();
    if(active && (long(m_saturated_images.size()) == m_buffers_size))
      m_saturated_images.pop_front();

    ImageType acc_type = m_pars.pixelOutputType;
    {
      AutoMutexUnlock u(aLock);
      acc_data.type = convert_imagetype_to_datatype(acc_type);
      acc_data.dimensions = aData.dimensions;
      acc_data.frameNumber = nextFrameNumber;
      acc_data.timestamp = aData.timestamp;
      acc_data.buffer = new Buffer(acc_data.size());
      memset(acc_data.data(),0,acc_data.size());

      // create also the new image for saturated counters
      if(active)
      {
        sat_data.type = Data::UINT16;
        sat_data.dimensions = aData.dimensions;
        sat_data.frameNumber = nextFrameNumber;
        sat_data.timestamp = aData.timestamp;
        sat_data.buffer = new Buffer(sat_data.size());
        memset(sat_data.data(),0,sat_data.size());
      }

      if(op == ACC_MEAN) {
        tmp_data.type = m_signed_data ? Data::INT32 : Data::UINT32;
        tmp_data.dimensions = aData.dimensions;
        tmp_data.buffer = new Buffer(tmp_data.size());
        memset(tmp_data.data(), 0, tmp_data.size());
      }
    }
    m_datas.push_back(acc_data);
    if(active)
      m_saturated_images.push_back(sat_data);
  }

  bool last = ((info.acc_frames + 1) == nb_acc_frame);
  {
    AutoMutexUnlock u(aLock);

    if(active)
      _calcSaturatedImageNCounters(aData,sat_data);

    // Accumulate
    switch (op) {
    case ACC_SUM:
      _accFrame(aData, acc_data);
      break;

    case ACC_MEAN:
      _accFrame(aData, tmp_data);
      break;

    case ACC_MEDIAN:
      m_tmp_datas.push_back(aData);
      break;
    }

    // If accumulated frame is cleared for takeoff
    if(last)
    {
      // Reduction
      switch (op) {
      case ACC_SUM:
        break;

      case ACC_MEAN:
        transform_pixel(tmp_data, acc_data, pixel_divide(nb_acc_frame));
        break;

      case ACC_MEDIAN:
        // TODO compute the median from m_tmp_datas
        break;
      }
    }
  }

  ++info.acc_frames;
  if(!last)
    return;

  bool cont_flag;
  {
    AutoMutexUnlock u(aLock);
    cont_flag = m_ct.newFrameReady(acc_data);
  }

  m_last_continue_flag &= cont_flag;
}
/** @brief stops the current integration
 */
void CtAccumulation::stop()
{
  DEB_MEMBER_FUNCT();
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
  if(m_datas.empty())
    THROW_CTL_ERROR(Error) << "Frame " << frameNumber << " not available";

  if(frameNumber < 0)    // means last
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

inline void acc_frame(Data &src,Data &dst)
{
  pixel_accumulate fn;
  transform_pixel(src, dst, fn);
}

inline void acc_frame_with_threshold(Data &src,Data &dst,long long threshold_value)
{
  pixel_accumulate_threshold fn(threshold_value);
  transform_pixel(src, dst, fn);
}

inline void acc_frame_with_offset_then_threshold(Data &src,Data &dst,long long offset_value, long long threshold_value)
{
  pixel_accumulate_offset_threshold fn(offset_value, threshold_value);
  transform_pixel(src, dst, fn);
}

void CtAccumulation::_accFrame(Data& src, Data& dst) const
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(src, dst);

  long long threshold_value = m_pars.thresholdB4Acc;
  long long offset_value = m_pars.offsetB4Acc;

  DEB_TRACE() << DEB_VAR3(m_pars.filter, threshold_value, offset_value);

  switch (m_pars.filter)
  {
  case Filter::FILTER_NONE:
    acc_frame(src, dst); break;
  case Filter::FILTER_THRESHOLD_MIN:
    acc_frame_with_threshold(src, dst, threshold_value); break;
  case Filter::FILTER_OFFSET_THEN_THRESHOLD_MIN:
    acc_frame_with_offset_then_threshold(src, dst, offset_value, threshold_value); break;
  }
}

#ifdef WITH_CONFIG
CtConfig::ModuleTypeCallback* CtAccumulation::_getConfigHandler()
{
  return new _ConfigHandler(*this);
}
#endif //WITH_CONFIG
