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

#include "lima/LimaCompatibility.h"
#include "lima/HwCap.h"
#include <vector>
#include <ostream>

namespace lima
{

/// As an interface to the Control Layer, this class exports the capabilities provided by the hardware.
/// It is implemented by every camera plugins.
class LIMACORE_API HwInterface
{
	DEB_CLASS(DebModHardware, "HwInterface");

 public:
	typedef std::vector<HwCap> CapList;

	/// A tuple of status with acquisition and detector status / mask
	typedef struct Status
	{
		/// Basic detector states (some detectors may have additional states)
		enum Basic {
			Fault,	//!< Fault
			Ready,	//!< Ready for acquisition
			Exposure,	//!< Counting photons
			Readout,	//!< Reading data from the chip
			Latency,	//!< Latency between exposures
			Config	//!< Fault
		};
		inline void set(Basic);

		AcqStatus acq;			//!< Global acquisition status.
		DetStatus det;			//!< Compound bit flags specifying the current detector status.
		DetStatus det_mask;		//!< A mask specifying the detector status bits that are supported by the hardware.
	} StatusType;

	enum ResetLevel {
		SoftReset, HardReset,
	};

	HwInterface();
	virtual ~HwInterface();

  /// Returns a list of capabilities
	virtual void getCapList(CapList &) const = 0;

	template <class CtrlObj>
	bool getHwCtrlObj(CtrlObj *& ctrl_obj_ptr) const;

	/// Reset the hardware interface
	virtual void reset(ResetLevel reset_level) = 0;

	/// Prepare the acquisition and make sure the camera is properly configured.
	/// This member function is always called before the acquisition is started.
	virtual void prepareAcq() = 0;

	/// Start the acquisition
	virtual void startAcq() = 0;

	/// Stop the acquisition
	virtual void stopAcq() = 0;

	/// Returns the current state of the hardware
	virtual void getStatus(StatusType& status) = 0;

	/// Returns the number of acquired frames
	virtual int getNbAcquiredFrames();

	/// Returns the number of acquired frames returned by the hardware (may differ from getNbAcquiredFrames if accumulation is on)
	virtual int getNbHwAcquiredFrames() = 0;

	virtual bool firstProcessingInPlace() const {return true;}
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
    case HwInterface::StatusType::Fault:
      det = DetFault;
      acq = AcqFault;
      break;
    }
  det_mask = DetExposure | DetReadout | DetLatency;
}

} // namespace lima

#endif // HWINTERFACE_H
