#ifndef ESPIADEV_H
#define ESPIADEV_H

#include "Espia.h"

#include "ThreadUtils.h"
#include "HwFrameCallback.h"

namespace lima
{

class EspiaDev : public Espia
{
 public:
	EspiaDev(int dev_nb);
	~EspiaDev();

	operator espia_t();

	void registerCallback(struct espia_cb_data& cb_data, int& cb_nr);
	void unregisterCallback(int& cb_nr);

 private:
	void open(int dev_nb);
	void close();

	AutoMutex acqLock();

	int m_dev_nb;
	espia_t m_dev;

	Mutex m_acq_mutex;
};

inline EspiaDev::operator espia_t()
{ 
	return m_dev; 
}


inline AutoMutex EspiaDev::acqLock()
{
	return AutoMutex(m_acq_mutex, AutoMutex::Locked);
}

} // namespace lima

#endif // ESPIADEV_H
