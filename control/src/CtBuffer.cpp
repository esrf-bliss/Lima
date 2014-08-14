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
#include "CtBuffer.h"
#include "CtAccumulation.h"

using namespace lima;

class CtBuffer::_DataDestroyCallback : public Buffer::Callback
{
public:
  _DataDestroyCallback(CtBuffer &buffer) : m_buffer(buffer) {}

  virtual void destroy(void *dataPt)
  {
    m_buffer.m_hw_buffer_cb->release(dataPt);
  }
private:
  CtBuffer& m_buffer;
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
  : m_frame_cb(NULL),m_ct_accumulation(NULL),
    m_data_destroy_callback(NULL)

{
  DEB_CONSTRUCTOR();

  if (!hw->getHwCtrlObj(m_hw_buffer))
    THROW_CTL_ERROR(Error) <<  "Cannot get hardware buffer object";

  m_hw_buffer_cb = m_hw_buffer->getBufferCallback();
  if(m_hw_buffer_cb)
    m_data_destroy_callback = new _DataDestroyCallback(*this);
}

CtBuffer::~CtBuffer()
{
  DEB_DESTRUCTOR();

  unregisterFrameCallback();
  delete m_data_destroy_callback;
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

void CtBuffer::setPars(Parameters pars) 
{
  DEB_MEMBER_FUNCT();

  setMode(pars.mode);
  setNumber(pars.nbBuffers);
  setMaxMemory(pars.maxMemory);
}

void CtBuffer:: getPars(Parameters& pars) const
{
  DEB_MEMBER_FUNCT();

  pars= m_pars;

  DEB_RETURN() << DEB_VAR1(pars);
}

void CtBuffer:: setMode(BufferMode mode)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(mode);

  m_pars.mode= mode;
}

void CtBuffer:: getMode(BufferMode& mode) const
{
  DEB_MEMBER_FUNCT();

  mode= m_pars.mode;

  DEB_RETURN() << DEB_VAR1(mode);
}

void CtBuffer:: setNumber(long nb_buffers)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(nb_buffers);

  m_pars.nbBuffers= nb_buffers;
}

void CtBuffer:: getNumber(long& nb_buffers) const
{
  DEB_MEMBER_FUNCT();

  nb_buffers= m_pars.nbBuffers;

  DEB_RETURN() << DEB_VAR1(nb_buffers);
}

void CtBuffer:: setMaxMemory(short max_memory)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(max_memory);

  if ((max_memory<1)||(max_memory>100))
    THROW_CTL_ERROR(InvalidValue) <<  "Max memory usage between 1 and 100";

  m_pars.maxMemory= max_memory;
}
	
void CtBuffer::getMaxMemory(short& max_memory) const
{
  DEB_MEMBER_FUNCT();

  max_memory= m_pars.maxMemory;

  DEB_RETURN() << DEB_VAR1(max_memory);
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

  m_pars.reset();
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

  img= ct->image();
  img->getHwImageDim(fdim);

  int hwNbBuffer = acq_nframes,nbuffers = acq_nframes;
  m_ct_accumulation = NULL;
  if(!acq_nframes)		// continous mode
    hwNbBuffer = nbuffers = 16;

  switch (mode) {
  case Single:
    concat_nframes= 1;
    break;
  case Accumulation:
    concat_nframes= 1;
    m_ct_accumulation = ct->accumulation();
    hwNbBuffer = 16;
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

  int max_nbuffers;
  m_hw_buffer->getMaxNbBuffers(max_nbuffers);
  if (hwNbBuffer > max_nbuffers)
    hwNbBuffer = max_nbuffers;
  m_hw_buffer->setNbBuffers(hwNbBuffer);

  if(nbuffers > max_nbuffers)
    nbuffers = max_nbuffers;
  m_pars.nbBuffers = nbuffers;
  registerFrameCallback(ct);
  m_frame_cb->m_ct_accumulation = m_ct_accumulation;

  if(m_hw_buffer_cb)
    m_hw_buffer_cb->releaseAll();
}

void CtBuffer::transformHwFrameInfoToData(Data &fdata,
					  const HwFrameInfoType& frame_info,
					  int readBlockLen)
{
  DEB_STATIC_FUNCT();
  DEB_PARAM() << DEB_VAR2(frame_info, readBlockLen);

  ImageType ftype;
  Size fsize;

  ftype= frame_info.frame_dim.getImageType();
  switch (ftype) {
  case Bpp8:
    fdata.type= Data::UINT8; break;
  case Bpp8S:
    fdata.type = Data::INT8; break;
  case Bpp10:
  case Bpp12:
  case Bpp14:
  case Bpp16:
    fdata.type= Data::UINT16; break;
  case Bpp16S:
    fdata.type = Data::INT16; break;
  case Bpp32:
    fdata.type= Data::UINT32; break;
  case Bpp32S:
    fdata.type = Data::INT32; break;
  default:
    THROW_CTL_ERROR(InvalidValue) << "Data type not yet managed" << DEB_VAR1(ftype);
  }

  fsize= frame_info.frame_dim.getSize();
  fdata.dimensions.push_back(fsize.getWidth());
  fdata.dimensions.push_back(fsize.getHeight());
  if (readBlockLen > 1)
    fdata.dimensions.push_back(readBlockLen);
  fdata.frameNumber= frame_info.acq_frame_nb;
  fdata.timestamp = frame_info.frame_timestamp;

  Buffer *fbuf = new Buffer();
  fbuf->data = frame_info.frame_ptr;
  if(frame_info.buffer_owner_ship == HwFrameInfoType::Managed)
    fbuf->owner = Buffer::MAPPED;
  else
    fbuf->owner = Buffer::SHARED;

  fdata.setBuffer(fbuf);
  fbuf->unref();
}

void CtBuffer::getDataFromHwFrameInfo(Data &fdata,
				      const HwFrameInfoType& frame_info,
				      int readBlockLen)
{
  DEB_MEMBER_FUNCT();

  transformHwFrameInfoToData(fdata,frame_info,readBlockLen);
  // Manage Buffer callback
  if(m_hw_buffer_cb)
    {
      m_hw_buffer_cb->map(frame_info.frame_ptr);
      fdata.buffer->callback = m_data_destroy_callback;
    }
  DEB_RETURN() << DEB_VAR1(fdata);
}
// -----------------
// struct Parameters
// -----------------
CtBuffer::Parameters::Parameters()
{
  DEB_CONSTRUCTOR();

  reset();
}

void CtBuffer::Parameters::reset()
{
  DEB_MEMBER_FUNCT();

  mode= Linear;
  nbBuffers= 1;
  maxMemory= 75;

  DEB_TRACE() << *this;
}
