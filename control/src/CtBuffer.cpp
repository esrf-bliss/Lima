#include "CtBuffer.h"

using namespace lima;

CtBuffer::CtBuffer(HwInterface *hw)
{
	if (!hw->getHwCtrlObj(m_hw_buffer))
                throw LIMA_CTL_EXC(Error, "Cannot get hardware buffer object");
}

CtBuffer::~CtBuffer()
{
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

void CtBuffer::setup(CtAcquisition *ct_acq)
{
	AcqMode mode;
	int acq_nframes, acc_nframes, concat_nframes;

	ct_acq->getAcqMode(mode);
	ct_acq->getAcqNbFrames(acq_nframes);

	switch (mode) {
		case Single:
			acc_nframes= 0;
			concat_nframes= 0;
			break;
		case Accumulation:
			ct_acq->getAccNbFrames(acc_nframes);
			concat_nframes= 0;
			break;
		case Concatenation:
			acc_nframes= 0;
			ct_acq->getConcatNbFrames(concat_nframes);
			break;
	}
	m_hw_buffer->setNbAccFrames(acc_nframes);
	m_hw_buffer->setNbConcatFrames(concat_nframes);
	m_hw_buffer->setNbBuffers(acq_nframes);
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
