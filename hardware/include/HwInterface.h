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
#ifndef HWINTERFACE_H
#define HWINTERFACE_H

#include "LimaCompatibility.h"
#include "HwCap.h"
#include <vector>
#include <ostream>

namespace lima
{

class LIMACORE_API HwInterface
{
	DEB_CLASS(DebModHardware, "HwInterface");

 public:
	typedef std::vector<HwCap> CapList;

	typedef struct Status {
	  enum Basic {Ready,Exposure,Readout,Latency,Config};
	  inline void set(Basic);

		AcqStatus acq;
		DetStatus det;
		DetStatus det_mask;
	} StatusType;

	enum ResetLevel {
		SoftReset, HardReset,
	};

	HwInterface();
	virtual ~HwInterface();

	virtual void getCapList(CapList &) const = 0;

	template <class CtrlObj>
	bool getHwCtrlObj(CtrlObj *& ctrl_obj_ptr) const;

	virtual void reset(ResetLevel reset_level) = 0;
	virtual void prepareAcq() = 0;
	virtual void startAcq() = 0;
	virtual void stopAcq() = 0;
	virtual void getStatus(StatusType& status) = 0;

	virtual int getNbAcquiredFrames();
	virtual int getNbHwAcquiredFrames() = 0;
};

template <class CtrlObj>
bool HwInterface::getHwCtrlObj(CtrlObj *& ctrl_obj) const
{
        CapList cap_list;
	getCapList(cap_list);
	typedef CapList::const_iterator It;
	for (It i = cap_list.begin(); i != cap_list.end(); ++i)
		if (i->getCtrlObj(ctrl_obj))
			return true;

	ctrl_obj = NULL;
	return false;
}

LIMACORE_API std::ostream& operator <<(std::ostream& os, 
				       const HwInterface::StatusType& status);
LIMACORE_API std::ostream& operator <<(std::ostream& os, 
				       HwInterface::ResetLevel reset_level);

void HwInterface::StatusType::set(HwInterface::StatusType::Basic basic_status)
{
  switch (basic_status) 
    {
    case HwInterface::StatusType::Ready:
      acq = AcqReady;
      det = DetIdle;
      break;
    case HwInterface::StatusType::Config:
      acq = AcqConfig;
      det = DetIdle;
      break;
    case HwInterface::StatusType::Exposure:
      det = DetExposure;
      goto Running;
    case HwInterface::StatusType::Readout:
      det = DetReadout;
      goto Running;
    case HwInterface::StatusType::Latency:
      det = DetLatency;
    Running:
      acq = AcqRunning;
      break;
    }
  det_mask = DetExposure | DetReadout | DetLatency;
}

} // namespace lima

#endif // HWINTERFACE_H
