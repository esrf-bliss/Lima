#ifndef ESPIA_H
#define ESPIA_H

#include "SizeUtils.h"
#include "Exceptions.h"
#include "ThreadUtils.h"
#include "HwFrameCallback.h"

#include "espia_lib.h"

#include <string>

namespace lima
{

class Espia
{
 public:
	enum {
		Invalid = SCDXIPCI_INVALID,
		NoBlock = SCDXIPCI_NO_BLOCK,
		BlockForever = SCDXIPCI_BLOCK_FOREVER,
	};

	typedef struct AcqStatus {
		bool	acq_started;
		bool	acq_running;
		int	acq_run_nb;
		int	last_acq_frame_nb;
	} AcqStatusType;

	Espia(int dev_nb);
	~Espia();

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

	void serWrite(const std::string& buffer, 
		      int block_size = 0, double block_delay = 0, 
		      bool no_wait = false);
	void serRead(std::string& buffer, int len, double timeout);
	void serReadStr(std::string& buffer, int len, 
			const std::string& term, double timeout);
	void serFlush();
	void serGetAvailableBytes(int& available_bytes);

	static void throwError(int ret, std::string file, std::string func, 
			       int line);

 private:
	void open(int dev_nb);
	void close();
	unsigned long sec2usec(double sec);
	double usec2sec(unsigned long usec);

	bool hasVirtualBuffers();
	int realBufferNb(int virt_buffer, int virt_frame);
	int realFrameNb (int virt_buffer, int virt_frame);
	int virtBufferNb(int real_buffer, int real_frame);
	int virtFrameNb (int real_buffer, int real_frame);

	void real2virtFrameInfo(const struct img_frame_info& real_info, 
				HwFrameInfoType& virt_info);
	void resetFrameInfo(struct img_frame_info& frame_info);

	static int cbDispatch(struct espia_cb_data *cb_data);

	void registerLastFrameCb();
	void unregisterLastFrameCb();
	void lastFrameCb(struct espia_cb_data *cb_data);

	AutoMutex acqLock();

	int m_dev_nb;
	espia_t m_dev;

	FrameDim m_frame_dim;
	int m_nb_buffers;
	int m_buffer_frames;
	int m_real_frame_factor;
	int m_real_frame_size;

	Mutex m_acq_mutex;
	int m_nb_frames;
	bool m_started;
	Timestamp m_start_ts;
	struct img_frame_info m_last_frame_info;
	int m_last_frame_cb_nr;
};

#define ESPIA_CHECK_CALL(ret)						\
	do {								\
		int aux_ret = (ret);					\
		if (aux_ret < 0)					\
			Espia::throwError(aux_ret, __FILE__,		\
					  __FUNCTION__, __LINE__);	\
	} while (0)


inline unsigned long Espia::sec2usec(double sec)
{
	return (unsigned long) (sec * 1e6);
}

inline double Espia::usec2sec(unsigned long  usec)
{
	return usec * 1e-6;
}

inline bool Espia::hasVirtualBuffers()
{
	return m_real_frame_factor != 1;
}

inline int Espia::realBufferNb(int virt_buffer, int virt_frame)
{
	return virt_buffer / m_real_frame_factor;
}

inline int Espia::realFrameNb (int virt_buffer, int virt_frame)
{
	return virt_buffer % m_real_frame_factor + virt_frame;
}

inline int Espia::virtBufferNb(int real_buffer, int real_frame)
{
	return (real_buffer * m_real_frame_factor + 
		real_frame / m_buffer_frames);
}

inline int Espia::virtFrameNb (int real_buffer, int real_frame)
{
	return real_frame % m_buffer_frames;
}

inline void Espia::getStartTimestamp(Timestamp& start_ts)
{
	start_ts = m_start_ts;
}

inline AutoMutex Espia::acqLock()
{
	return AutoMutex(m_acq_mutex, AutoMutex::Locked);
}

} // namespace lima

#endif // ESPIA_H
