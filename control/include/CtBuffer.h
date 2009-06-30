#ifndef CTBUFFER_H
#define CTBUFFER_H

#include "Constants.h"
#include "HwInterface.h"
#include "HwCap.h"

namespace lima {

class CtBuffer {

    public:
        enum Mode {Linear,Circular};
	struct Parameters {
		Mode	mode;
		long	nbBuffers;
		short	maxMemory;
	};

	CtBuffer(HwInterface *hw);
	~CtBuffer();

	void setMode(BufferMode mode);
	void getMode(BufferMode& mode) const;

	void setNumber(long nb_buffers);
	void getNumber(long& nb_buffers) const;

	void setMaxMemory(short max_memory);
	void getMaxMemory(short& max_memory) const;

    private:
	HwBufferCtrlObj	*m_hw_buffer;
	Parameters	m_pars;

};
} // namespace lima

#endif // CTBUFFER_H
