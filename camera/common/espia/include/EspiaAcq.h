#ifndef ESPIAACQ_H
#define ESPIAACQ_H

#include "EspiaDev.h"
#include "SizeUtils.h"

namespace lima
{

namespace Espia
{

class Acq
{
 public:
	Acq(Dev& dev);
	~Acq();

	typedef struct AcqStatus {
		bool	acq_started;
		bool	acq_running;
		int	acq_run_nb;
		int	last_acq_frame_nb;
	} AcqStatusType;

	void bufferAlloc(const FrameDim& frame_dim, int& nb_buffers,
			 int buffer_frames);
	void bufferFree();

	void getFrameDim(FrameDim& frame_dim);
	void getNbBuffers(int& nb_buffers);
	void getBufferFrames(int& buffer_frames);

	void *getBufferFramePtr(int buffer_nb, int frame_nb = 0);
	void *getAcqFramePtr(int acq_frame_nb);
	void getFrameInfo(int acq_frame_nb, HwFrameInfoType& info);

	void setNbFrames(int  nb_frames);
	void getNbFrames(int& nb_frames);

	void startAcq();
	void stopAcq();
	void getAcqStatus(AcqStatusType& acq_status);

	void getStartTimestamp(Timestamp& start_ts);

 private:
	bool hasVirtualBuffers();
	int realBufferNb(int virt_buffer, int virt_frame);
	int realFrameNb (int virt_buffer, int virt_frame);
	int virtBufferNb(int real_buffer, int real_frame);
	int virtFrameNb (int real_buffer, int real_frame);

	void real2virtFrameInfo(const struct img_frame_info& real_info, 
				HwFrameInfoType& virt_info);
	void resetFrameInfo(struct img_frame_info& frame_info);

	static int dispatchFrameCb(struct espia_cb_data *cb_data);

	void registerLastFrameCb();
	void unregisterLastFrameCb();
	void lastFrameCb(struct espia_cb_data *cb_data);

	AutoMutex acqLock();

	Dev& m_dev;

	FrameDim m_frame_dim;
	int m_nb_buffers;
	int m_buffer_frames;
	int m_real_frame_factor;
	int m_real_frame_size;

	int m_nb_frames;
	bool m_started;
	Timestamp m_start_ts;
	struct img_frame_info m_last_frame_info;
	int m_last_frame_cb_nr;
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
		real_frame / m_buffer_frames);
}

inline int Acq::virtFrameNb (int real_buffer, int real_frame)
{
	return real_frame % m_buffer_frames;
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
