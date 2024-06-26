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
#include "lima/CtBuffer.h"
#include "lima/CtAccumulation.h"
#include "lima/CtSaving.h"
#include "lima/SidebandData.h"

#ifdef __unix
#include <malloc.h>
#endif

using namespace lima;

static const double WAIT_BUFFERS_RELEASED_TIMEOUT = 5.0;

class CtBuffer::_DataBuffer : public MappedBuffer
{
public:
  _DataBuffer(CtBuffer &buffer, void *p)
    : MappedBuffer(p, [&](void *) { buffer._release(this); }), m_map_ref(NULL)
  {}

  const char *type() const override
  {
    return "Managed";
  }

private:
  friend class CtBuffer;
  void *m_map_ref;
};

bool CtBufferFrameCB::newFrameReady(const HwFrameInfoType& frame_info)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(frame_info);

  Data fdata;
  m_ct->buffer()->getDataFromHwFrameInfo(fdata,frame_info);
  if(m_ct_accumulation)
    return m_ct_accumulation->_newFrameReady(fdata);
  else
    return m_ct->newFrameReady(fdata);
}

CtBuffer::CtBuffer(HwInterface *hw)
  : m_frame_cb(NULL),m_ct_accumulation(NULL),m_nb_buffers(0),m_mapped_frames(0)
{
  DEB_CONSTRUCTOR();

  if (!hw->getHwCtrlObj(m_hw_buffer))
    THROW_CTL_ERROR(Error) <<  "Cannot get hardware buffer object";

  m_params.initMem = true;
  m_params.reqMemSizePercent = 70.0;
  m_hw_buffer->setAllocParameters(m_params);

  m_hw_buffer_cb = m_hw_buffer->getBufferCallback();
}

CtBuffer::~CtBuffer()
{
  DEB_DESTRUCTOR();

  unregisterFrameCallback();
}

void CtBuffer::registerFrameCallback(CtControl *ct) 
{
  DEB_MEMBER_FUNCT();

  if (m_frame_cb == NULL) {
    m_frame_cb= new CtBufferFrameCB(ct);
    m_hw_buffer->registerFrameCallback(*m_frame_cb);
  }
}

void CtBuffer::unregisterFrameCallback() 
{
  DEB_MEMBER_FUNCT();
  
  if (m_frame_cb != NULL) {
    delete m_frame_cb;
    m_frame_cb= NULL;
  }
}

void CtBuffer::setAllocParameters(const Parameters& params)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(params);

  double max_memory = params.reqMemSizePercent;
  if ((max_memory < 0.0) || (max_memory >= 100.0))
    THROW_CTL_ERROR(InvalidValue) <<  "Max memory usage outside [0,100) range";

  m_hw_buffer->setAllocParameters(params);
  m_params = params;
}

void CtBuffer::getAllocParameters(Parameters& params) const
{
  DEB_MEMBER_FUNCT();
  params = m_params;
  DEB_RETURN() << DEB_VAR1(params);
}

void CtBuffer::getNumber(long& nb_buffers) const
{
  DEB_MEMBER_FUNCT();
  nb_buffers = m_nb_buffers;
  DEB_RETURN() << DEB_VAR1(nb_buffers);
}

void CtBuffer::getMaxHwNumber(long& nb_buffers) const
{
  int max_nb_buffers;
  m_hw_buffer->getMaxNbBuffers(max_nb_buffers);
  nb_buffers = max_nb_buffers;
}

void CtBuffer::getMaxAccNumber(long& nb_buffers) const
{
  int max_nb_buffers = 0;
  if(m_ct_accumulation)
    m_ct_accumulation->getMaxNbBuffers(max_nb_buffers);
  nb_buffers = max_nb_buffers;
}

void CtBuffer::getMaxNumber(long& nb_buffers) const
{
  if(m_ct_accumulation)
    getMaxAccNumber(nb_buffers);
  else
    getMaxHwNumber(nb_buffers);
}

void CtBuffer::getFrame(Data &aReturnData,int frameNumber,int readBlockLen)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(frameNumber, readBlockLen);

  int concat_frames;
  m_hw_buffer->getNbConcatFrames(concat_frames);
  if (readBlockLen != 1) {
    if (concat_frames == 1)
      throw LIMA_CTL_EXC(InvalidValue, "Cannot read block of frames "
			 "if not in Concatenation mode");
    else if (frameNumber % concat_frames + readBlockLen > concat_frames)
      throw LIMA_CTL_EXC(InvalidValue, "Reading block of frames cannot cross "
			 "boundaries given by specified nb_concat_frames");
  }

  if(m_ct_accumulation)
    m_ct_accumulation->getFrame(aReturnData,frameNumber);
  else
    {
      HwFrameInfo info;
      m_hw_buffer->getFrameInfo(frameNumber,info);
      getDataFromHwFrameInfo(aReturnData,info,readBlockLen);
    }
  DEB_RETURN() << DEB_VAR1(aReturnData);
}

void CtBuffer::reset()
{
  DEB_MEMBER_FUNCT();
}

void CtBuffer::setup(CtControl *ct)
{
  DEB_MEMBER_FUNCT();

  CtAcquisition *acq;
  CtImage *img;
  AcqMode mode;
  FrameDim fdim;
  int acq_nframes, concat_nframes;

  acq= ct->acquisition();
  acq->getAcqMode(mode);
  acq->getAcqNbFrames(acq_nframes);
  
  CtSaving* saving = ct->saving();
  CtSaving::ManagedMode saving_mode;
  saving->getManagedMode(saving_mode);
  
  img= ct->image();
  img->getHwImageDim(fdim);

  int hwNbBuffer = acq_nframes,nbuffers = acq_nframes;
  m_ct_accumulation = NULL;
  if(!acq_nframes)		// continous mode
    hwNbBuffer = nbuffers = 16;
  else if(saving_mode != CtSaving::Software)
    hwNbBuffer = nbuffers = 1;

  switch (mode) {
  case Single:
    concat_nframes= 1;
    break;
  case Accumulation:
    concat_nframes= 1;
    m_ct_accumulation = ct->accumulation();
    m_ct_accumulation->getHwNbBuffers(hwNbBuffer);
    break;
  case Concatenation:
    acq->getConcatNbFrames(concat_nframes);
    if (acq_nframes > 0)
      hwNbBuffer = (acq_nframes + concat_nframes - 1) / concat_nframes;
    nbuffers = hwNbBuffer;
    break;
  }
  m_hw_buffer->setFrameDim(fdim);
  m_hw_buffer->setNbConcatFrames(concat_nframes);

  long max_hw_nb_buffers, max_nb_buffers;
  getMaxHwNumber(max_hw_nb_buffers);
  getMaxNumber(max_nb_buffers);

  if (hwNbBuffer > max_hw_nb_buffers) {
    if(m_ct_accumulation)
      THROW_CTL_ERROR(Error) << "Invalid acc_hw_nb_buffers: max is "
			     << max_hw_nb_buffers;
    hwNbBuffer = max_hw_nb_buffers;
  }
  m_hw_buffer->prepareAlloc(hwNbBuffer);
  m_hw_buffer->setNbBuffers(hwNbBuffer);

  if(nbuffers > max_nb_buffers)
    nbuffers = max_nb_buffers;
  m_nb_buffers = nbuffers;
  registerFrameCallback(ct);
  m_frame_cb->m_ct_accumulation = m_ct_accumulation;

  if(m_hw_buffer_cb) {
    const double& timeout = WAIT_BUFFERS_RELEASED_TIMEOUT;
    if (!waitBuffersReleased(timeout))
      THROW_CTL_ERROR(Error) << "Buffers still in use!";
    m_hw_buffer_cb->releaseAll();
  }

#ifdef __unix
  bool use_malloc_trim = true;
  if (use_malloc_trim)
    malloc_trim(0);
#endif
}

void CtBuffer::_initDataFromHwFrameInfo(Data &fdata, const HwFrameInfoType& frame_info,
					int readBlockLen)
{
  DEB_STATIC_FUNCT();
  DEB_PARAM() << DEB_VAR2(frame_info, readBlockLen);

  ImageType ftype;
  Size fsize;

  ftype= frame_info.frame_dim.getImageType();
  fdata.type = convert_imagetype_to_datatype(ftype);
  if (fdata.type == Data::UNDEF)
    THROW_CTL_ERROR(InvalidValue) << "Data type not yet supported" << DEB_VAR1(ftype);

  fsize= frame_info.frame_dim.getSize();
  fdata.dimensions.push_back(fsize.getWidth());
  fdata.dimensions.push_back(fsize.getHeight());
  if (readBlockLen > 1)
    fdata.dimensions.push_back(readBlockLen);
  fdata.frameNumber= frame_info.acq_frame_nb;
  fdata.timestamp = frame_info.frame_timestamp;

  if(!frame_info.sideband_data.empty())
    fdata.sideband = frame_info.sideband_data;
}

void CtBuffer::getDataFromHwFrameInfo(Data &fdata,
				      const HwFrameInfoType& frame_info,
				      int readBlockLen)
{
  DEB_MEMBER_FUNCT();

  bool managed= (frame_info.buffer_owner_ship == HwFrameInfoType::Managed);
  if(!managed || !m_hw_buffer_cb) {
    std::function<void(void *)> empty_deleter;
    getDataFromAnonymousHwFrameInfo(fdata, frame_info, empty_deleter, readBlockLen);
    return;
  }

  _initDataFromHwFrameInfo(fdata,frame_info,readBlockLen);

  // Manage Buffer callback
  _DataBuffer *fbuf= new _DataBuffer(*this,frame_info.frame_ptr);
  fdata.setBuffer(fbuf);
  fbuf->unref();
  fbuf->m_map_ref= m_hw_buffer_cb->map(fbuf->data);

  AutoMutex l(m_cond.mutex());
  ++m_mapped_frames;

  DEB_RETURN() << DEB_VAR1(fdata);
}

void CtBuffer::_release(_DataBuffer *fbuf)
{
  DEB_MEMBER_FUNCT();
  m_hw_buffer_cb->release(fbuf->m_map_ref);
  AutoMutex l(m_cond.mutex());
  if(--m_mapped_frames == 0)
    m_cond.signal();
}

bool CtBuffer::waitBuffersReleased(double timeout)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(timeout);

  if(!m_hw_buffer_cb)
    return true;

  AutoMutex l(m_cond.mutex());
  Timestamp end = Timestamp::now() + Timestamp(timeout);
  double t;
  while((m_mapped_frames > 0) && ((t = end - Timestamp::now()) > 0))
    m_cond.wait(t);
  bool all_released = (m_mapped_frames == 0);
  DEB_RETURN() << DEB_VAR2(all_released, m_mapped_frames);
  return all_released;
}
