class CtBuffer {
%TypeHeaderCode
#include <CtBuffer.h>
#include <Constants.h>
#include <sstream>
using namespace lima;
%End
    public:
	struct Parameters {
		BufferMode mode;
		long	nbBuffers;
		short	maxMemory;

		const char* __repr__();
%MethodCode
	std::ostringstream str;
	str << *sipCpp;	
	sipRes = str.str().c_str();
%End
	};

	CtBuffer(HwInterface *hw);
	~CtBuffer();

	void setMode(BufferMode mode);
	void getMode(BufferMode& mode /Out/) const;

	void setNumber(long nb_buffers);
	void getNumber(long& nb_buffers /Out/) const;

	void setMaxMemory(short max_memory);
	void getMaxMemory(short& max_memory /Out/) const;

	void registerFrameCallback(CtControl *ct);
        void unregisterFrameCallback();

	void getFrame(Data &data /Out/,int frameNumber);

	void reset();
	void setup(CtControl *ct);

	static void getDataFromHwFrameInfo(Data&,const HwFrameInfoType&);
};
