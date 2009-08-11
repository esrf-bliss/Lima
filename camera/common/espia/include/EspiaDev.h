#ifndef ESPIADEV_H
#define ESPIADEV_H

#include <map>
#include "Espia.h"

#include "ThreadUtils.h"
#include "HwFrameCallback.h"

namespace lima
{

namespace Espia
{


extern const std::string OPT_DEBUG_LEVEL, 
                         OPT_NO_FIFO_RESET;

extern std::map<std::string, int> EspiaDrvOptMap;


class Dev
{
 public:
	Dev(int dev_nb);
	~Dev();

	operator espia_t();

	bool isMeta();

	void registerCallback(struct espia_cb_data& cb_data, int& cb_nr);
	void unregisterCallback(int& cb_nr);

	void resetLink();
	void getCcdStatus(int& ccd_status);

	AutoMutex acqLock();

	void getDrvOption( const std::string &opt_name, int &val );
	void setDrvOption( const std::string &opt_name, int new_val, 
	                                                        int &old_val );

 private:
	static const double ResetLinkTime;

	void open(int dev_nb);
	void close();

	void initEspiaDrvOptMap();

	int m_dev_nb;
	espia_t m_dev;

	Mutex m_acq_mutex;
};

inline Dev::operator espia_t()
{ 
	return m_dev; 
}

inline bool Dev::isMeta()
{
	return (m_dev_nb == MetaDev);
}

inline AutoMutex Dev::acqLock()
{
	return AutoMutex(m_acq_mutex, AutoMutex::Locked);
}


} // namespace Espia

} // namespace lima

#endif // ESPIADEV_H
