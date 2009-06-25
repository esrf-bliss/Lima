#ifndef ESPIAACQ_H
#define ESPIAACQ_H

#include "EspiaDev.h"
#include "SizeUtils.h"
#include "HwFrameCallback.h"

namespace lima
{

namespace Espia
{

class Acq : public HwFrameCallbackGen
{
 public:
	Acq(Dev& dev);
	~Acq();

	typedef struct Status {
		bool	running;
		int	run_nb;
		int	last_frame_nb;
	} StatusType;

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
	void getStatus(StatusType& status);

	void getStartTimestamp(Timestamp& start_ts);

 protected:
	virtual void setFrameCallbackActive(bool cb_active);

 private:
	enum FrameCallback {
		Last, User,
	};

	bool hasVirtualBuffers();
	int realBufferNb(int virt_buffer, int virt_frame);
	int realFrameNb (int virt_buffer, int virt_frame);
	int virtBufferNb(int real_buffer, int real_frame);
	int virtFrameNb (int real_buffer, int real_frame);

	void real2virtFrameInfo(const struct img_frame_info& real_info, 
				HwFrameInfoType& virt_info);
	void resetFrameInfo(struct img_frame_info& frame_info);

	static int dispatchFrameCallback(struct espia_cb_data *cb_data);

	void enableFrameCallback(FrameCallback frame_cb);
	void disableFrameCallback(FrameCallback frame_cb);
	int& getFrameCallbackNb(FrameCallback frame_cb);
	void lastFrameCallback(struct espia_cb_data *cb_data);
	void userFrameCallback(struct espia_cb_data *cb_data);

	AutoMutex acqLock();

	Dev& m_dev;

	FrameDim m_frame_dim;
	int m_nb_buffers;
	int m_nb_buffer_frames;
	int m_real_frame_factor;
	int m_real_frame_size;

	int m_nb_frames;
	bool m_started;
	Timestamp m_start_ts;
	struct img_frame_info m_last_frame_info;
	int m_last_frame_cb_nr;
	int m_user_frame_cb_nr;
};

inline bool Acq::hasVirtualBuffers()
{
	return m_real_frame_factor != 1;
}

inline int Acq::realBufferNb(int virt_buffer, int virt_frame)
{
	return virt_buffer / m_real_frame_factor;
}

inline int Acq::realFrameNb (int virt_buffer, int virt_frame)
{
	return virt_buffer % m_real_frame_factor + virt_frame;
}

inline int Acq::virtBufferNb(int real_buffer, int real_frame)
{
	return (real_buffer * m_real_frame_factor + 
		real_frame / m_nb_buffer_frames);
}

inline int Acq::virtFrameNb (int real_buffer, int real_frame)
{
	return real_frame % m_nb_buffer_frames;
}

inline void Acq::getStartTimestamp(Timestamp& start_ts)
{
	start_ts = m_start_ts;
}

inline AutoMutex Acq::acqLock()
{
	return m_dev.acqLock();
}


} // namespace Espia

} // namespace lima


#endif // ESPIAACQ_H
