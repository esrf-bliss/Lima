#ifndef CTBUFFER_H
#define CTBUFFER_H

#include "Constants.h"
#include "CtAcquisition.h"
#include "HwInterface.h"
#include "HwCap.h"

namespace lima {

class CtBuffer {

    public:
	struct Parameters {
		Parameters();
		void reset();
		BufferMode mode;
		long	nbBuffers;
		short	maxMemory;
	};

	CtBuffer(HwInterface *hw);
	~CtBuffer();

	void setPars(Parameters pars);
	void getPars(Parameters& pars) const;

	void setMode(BufferMode mode);
	void getMode(BufferMode& mode) const;

	void setNumber(long nb_buffers);
	void getNumber(long& nb_buffers) const;

	void setMaxMemory(short max_memory);
	void getMaxMemory(short& max_memory) const;

	void setup(CtAcquisition *ct_acq, FrameDim& fdim);

    private:
	HwBufferCtrlObj	*m_hw_buffer;
	Parameters	m_pars;

};
} // namespace lima

#endif // CTBUFFER_H
