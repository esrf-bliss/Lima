class CtBuffer {
%TypeHeaderCode
#include <CtBuffer.h>
#include <Constants.h>
using namespace lima;
%End
    public:
	struct Parameters {
		BufferMode mode;
		long	nbBuffers;
		short	maxMemory;
	};

	CtBuffer(HwInterface *hw);
	~CtBuffer();

	void setMode(BufferMode mode);
	void getMode(BufferMode& mode /Out/) const;

	void setNumber(long nb_buffers);
	void getNumber(long& nb_buffers /Out/) const;

	void setMaxMemory(short max_memory);
	void getMaxMemory(short& max_memory /Out/) const;
};
