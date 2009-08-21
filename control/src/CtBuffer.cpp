#include "CtBuffer.h"

using namespace lima;

bool CtBufferFrameCB::newFrameReady(const HwFrameInfoType& frame_info)
{
  Data fdata;
  CtBuffer::getDataFromHwFrameInfo(fdata,frame_info);
  m_ct->newFrameReady(fdata);
  return true;
}

CtBuffer::CtBuffer(HwInterface *hw)
{
	if (!hw->getHwCtrlObj(m_hw_buffer))
                throw LIMA_CTL_EXC(Error, "Cannot get hardware buffer object");
}

CtBuffer::~CtBuffer()
{
	unregisterFrameCallback();
}

void CtBuffer::registerFrameCallback(CtControl *ct) {
	m_frame_cb= new CtBufferFrameCB(ct);
	m_hw_buffer->registerFrameCallback(*m_frame_cb);
}

void CtBuffer::unregisterFrameCallback() {
	if (m_frame_cb != NULL) {
		m_hw_buffer->unregisterFrameCallback(*m_frame_cb);
		delete m_frame_cb;
		m_frame_cb= NULL;
	}
}

void CtBuffer::setPars(Parameters pars) {
	setMode(pars.mode);
	setNumber(pars.nbBuffers);
	setMaxMemory(pars.maxMemory);
}

void CtBuffer:: getPars(Parameters& pars) const
{
	pars= m_pars;
}

void CtBuffer:: setMode(BufferMode mode)
{
	m_pars.mode= mode;
}

void CtBuffer:: getMode(BufferMode& mode) const
{
	mode= m_pars.mode;
}

void CtBuffer:: setNumber(long nb_buffers)
{
	m_pars.nbBuffers= nb_buffers;
}

void CtBuffer:: getNumber(long& nb_buffers) const
{
	nb_buffers= m_pars.nbBuffers;
}

void CtBuffer:: setMaxMemory(short max_memory)
{
	if ((max_memory<1)||(max_memory>100))
		throw LIMA_CTL_EXC(InvalidValue, "Max memory usage between 1 and 100");

	m_pars.maxMemory= max_memory;
}
	
void CtBuffer::getMaxMemory(short& max_memory) const
{
	max_memory= m_pars.maxMemory;
}

void CtBuffer::getFrame(Data &aReturnData,int frameNumber)
{
  HwFrameInfo info;
  m_hw_buffer->getFrameInfo(frameNumber,info);
  getDataFromHwFrameInfo(aReturnData,info);
}
void CtBuffer::setup(CtControl *ct)
{
	CtAcquisition *acq;
	CtImage *img;
	AcqMode mode;
	FrameDim fdim;
	int acq_nframes, acc_nframes, concat_nframes;

	acq= ct->acquisition();
	acq->getAcqMode(mode);
	acq->getAcqNbFrames(acq_nframes);

	img= ct->image();
	img->getHwImageDim(fdim);

	switch (mode) {
		case Single:
			acc_nframes= 0;
			concat_nframes= 1;
			break;
		case Accumulation:
			acq->getAccNbFrames(acc_nframes);
			concat_nframes= 0;
			break;
		case Concatenation:
			acc_nframes= 0;
			acq->getConcatNbFrames(concat_nframes);
			break;
	}
	m_hw_buffer->setFrameDim(fdim);
	m_hw_buffer->setNbAccFrames(acc_nframes);
	m_hw_buffer->setNbConcatFrames(concat_nframes);
	m_hw_buffer->setNbBuffers(acq_nframes);

	registerFrameCallback(ct);
}

void CtBuffer::getDataFromHwFrameInfo(Data &fdata,
				      const HwFrameInfoType& frame_info)
{
  ImageType ftype;
  Size fsize;

  ftype= frame_info.frame_dim->getImageType();
  switch (ftype) {
  case Bpp8:
    fdata.type= Data::UINT8; break;
  case Bpp10:
  case Bpp12:
  case Bpp14:
  case Bpp16:
    fdata.type= Data::UINT16; break;
  case Bpp32:
    fdata.type= Data::UINT32; break;
  }

  fsize= frame_info.frame_dim->getSize();
  fdata.width= fsize.getWidth();
  fdata.height= fsize.getHeight();
  fdata.frameNumber= frame_info.acq_frame_nb;
  fdata.timestamp = frame_info.frame_timestamp;

  Buffer *fbuf = new Buffer();
  fbuf->owner = Buffer::MAPPED;	
  fbuf->data = frame_info.frame_ptr;
  fdata.setBuffer(fbuf);
  fbuf->unref();
}
// -----------------
// struct Parameters
// -----------------
CtBuffer::Parameters::Parameters()
{
	reset();
}

void CtBuffer::Parameters::reset()
{
	mode= Linear;
	nbBuffers= 1;
	maxMemory= 75;
}
