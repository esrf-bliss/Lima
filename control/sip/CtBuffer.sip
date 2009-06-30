class CtBuffer {
%TypeHeaderCode
#include <CtBuffer.h>
using namespace lima;
%End
    public:
        enum Mode {Linear,Circular};
	struct Parameters {
		CtBuffer::Mode	mode;
		long	nbBuffers;
		short	maxMemory;
	};

	CtBuffer(HwInterface *hw);
	~CtBuffer();

	void setMode(BufferMode mode);
	BufferMode getMode() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipCpp->getMode(sipRes);
Py_END_ALLOW_THREADS
%End

	void setNumber(long nb_buffers);
	long getNumber() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipCpp->getNumber(sipRes);
Py_END_ALLOW_THREADS
%End

	void setMaxMemory(short max_memory);
	short getMaxMemory() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipCpp->getMaxMemory(sipRes);
Py_END_ALLOW_THREADS
%End
};
