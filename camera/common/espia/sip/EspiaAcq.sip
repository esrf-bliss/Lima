
namespace Espia
{

class Acq
{
%TypeHeaderCode
#include <EspiaDev.h>
#include <EspiaAcq.h>
using namespace lima;
%End
 public:
	Acq(Espia::Dev& dev);
	~Acq();

//	typedef struct Status {
//		bool	running;
//		int	run_nb;
//		int	last_frame_nb;
//	} StatusType;

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
//	void getStatus(StatusType& status);

	void getStartTimestamp(Timestamp& start_ts);

 protected:
	virtual void setFrameCallbackActive(bool cb_active);
};

}; // namespace Espia

