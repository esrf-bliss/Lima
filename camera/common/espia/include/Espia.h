#ifndef ESPIA_H
#define ESPIA_H

#include "SizeUtils.h"
#include "Exceptions.h"
#include "HwFrameInfo.h"

#include "espia_lib.h"

#include <string>

namespace lima
{

class Espia
{
 public:
	enum {
		Invalid = -1,
	};

	Espia(int dev_nr);
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

	void serWrite(const std::string& buffer, 
		      int block_size = 0, double block_delay = 0, 
		      bool no_wait = false);
	void serRead(std::string& buffer, int len, double timeout);
	void serReadStr(std::string& buffer, int len, 
			const std::string& term, double timeout);
	void serFlush();

	static void throwError(int ret, std::string file, std::string func, 
			       int line);

 private:
	void open(int dev_nr);
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

	int m_dev_nr;
	espia_t m_dev;
	FrameDim m_frame_dim;
	int m_nb_buffers;
	int m_buffer_frames;
	int m_real_frame_factor;
	int m_real_frame_size;
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

} // namespace lima

#endif // ESPIA_H
