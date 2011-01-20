//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2011
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################
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


extern const std::string PARAM_CHAN_UP_LED;

extern std::map<std::string, int> EspiaParamMap;


class Dev
{
	DEB_CLASS_NAMESPC(DebModEspia, "Dev", "Espia");

 public:
	Dev(int dev_nb);
	~Dev();

	operator espia_t();

	int getDevNb();
	bool isMeta();

	void registerCallback(struct espia_cb_data& cb_data, int& cb_nr);
	void unregisterCallback(int& cb_nr);

	void resetLink();
	void getCcdStatus(int& ccd_status);

	AutoMutex acqLock();

	void getDrvOption(const std::string& opt_name, int& val);
	void setDrvOption(const std::string& opt_name, int  val);

	void getParam(const std::string& param_name, int& val);
	void setParam(const std::string& param_name, int  val);

	void getChanUpLed(int& chan_up_led);

 private:
	static const double ResetLinkTime;

	void open(int dev_nb);
	void close();

	void initDrvOptMap();
	void initParamMap();

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

inline int Dev::getDevNb()
{
	return m_dev_nb;
}

inline AutoMutex Dev::acqLock()
{
	return AutoMutex(m_acq_mutex, AutoMutex::Locked);
}


} // namespace Espia

} // namespace lima

#endif // ESPIADEV_H
