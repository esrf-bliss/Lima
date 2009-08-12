#ifndef CTBUFFER_H
#define CTBUFFER_H

#include "Constants.h"
#include "SizeUtils.h"
#include "CtAcquisition.h"
#include "CtImage.h"
#include "HwInterface.h"
#include "HwFrameCallback.h"

#include "Data.h"

namespace lima {

class CtBufferFrameCB : public HwFrameCallback
{
    public:
	CtBufferFrameCB(CtControl *ct): m_ct(ct) {}
    protected:
        bool newFrameReady(const HwFrameInfoType& frame_info);
    private:
	CtControl *m_ct;
};

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

	void registerFrameCallback(CtControl *ct);
        void unregisterFrameCallback();

	void setup(CtControl *ct);

    private:
	HwBufferCtrlObj	*m_hw_buffer;
	CtBufferFrameCB *m_frame_cb;
	Parameters	m_pars;

};
} // namespace lima

#endif // CTBUFFER_H
