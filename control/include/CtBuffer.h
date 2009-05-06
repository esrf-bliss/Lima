#ifndef CTBUFFER_H
#define CTBUFFER_H

#include "Constants.h"

namespace lima {

class CtBuffer {

    public:

	struct Parameters {
		Mode	mode;
		long	nbBuffers;
		short	maxMemory;
	};

	CtBuffer();
	~CtBuffer();

	void setMode(BufferMode mode);
	void getMode(BufferMode& mode) const;

	void setNumber(long nb_buffers);
	void getNumber(long& nb_buffers) const;

	void setMaxMemory(short max_memory);
	void getMaxMemory(short& max_memory) const;

    private:
	Parameters m_pars;

} // namespace lima

#endif // CTBUFFER_H
