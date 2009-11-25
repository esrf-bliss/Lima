
namespace Espia
{

class AcqEndCallback
{
%TypeHeaderCode
#include <EspiaDev.h>
#include <EspiaAcq.h>
using namespace lima;
%End
 public:
	AcqEndCallback();
	virtual ~AcqEndCallback();

	Espia::Acq *getAcq() const;

 protected:
	virtual void acqFinished(const HwFrameInfoType& frame_info) = 0;

};


class Acq
{
%TypeHeaderCode
#include <EspiaDev.h>
#include <EspiaAcq.h>
using namespace lima;
%End
 public:
	struct StatusType {  // Instead ot "typedef struct Status {} StatusType"
		bool	running;
		int	run_nb;
		int	last_frame_nb;
		const char* __repr__();
%MethodCode
	std::ostringstream str;
	str << *sipCpp;	
	sipRes = str.str().c_str();
%End
	};

	Acq(Espia::Dev& dev);
	~Acq();

	Espia::Dev& getDev();

	void bufferAlloc(int& nb_buffers, int nb_buffer_frames, 
			 const FrameDim& frame_dim);
	void bufferFree();

	const FrameDim& getFrameDim();
	void getNbBuffers(int& nb_buffers);
	void getNbBufferFrames(int& nb_buffer_frames);

	void *getBufferFramePtr(int buffer_nb, int frame_nb = 0);
	void *getAcqFramePtr(int acq_frame_nb);
	void getFrameInfo(int acq_frame_nb, HwFrameInfoType& info);

	void setNbFrames(int  nb_frames);
	void getNbFrames(int& nb_frames);

	void start();
	void stop();
	void getStatus(StatusType& status /Out/);

	void getStartTimestamp(Timestamp& start_ts);

	void registerAcqEndCallback(Espia::AcqEndCallback& acq_end_cb);
	void unregisterAcqEndCallback(Espia::AcqEndCallback& acq_end_cb);

 protected:
	virtual void setFrameCallbackActive(bool cb_active);
};

}; // namespace Espia

