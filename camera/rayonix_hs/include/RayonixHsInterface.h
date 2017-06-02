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
#ifndef RayonixHsINTERFACE_H
#define RayonixHsINTERFACE_H

#include "HwInterface.h"

namespace lima {
namespace RayonixHs {

class Camera;
class DetInfoCtrlObj;
class BufferCtrlObj;
class SyncCtrlObj;
class BinCtrlObj;
//class RoiCtrlObj;

class Interface: public HwInterface {
	DEB_CLASS_NAMESPC(DebModCamera, "Interface", "RayonixHs");

	public:
		Interface(Camera*);
		virtual ~Interface();

		virtual void getCapList(CapList &) const;

		virtual void reset(ResetLevel reset_level);
		virtual void prepareAcq();
		virtual void startAcq();
		virtual void stopAcq();
		virtual void getStatus(StatusType& status);

		virtual int getNbAcquiredFrames();
		virtual int getNbHwAcquiredFrames();
	private:
		Camera* m_cam;
		DetInfoCtrlObj* m_det_info;
		BufferCtrlObj* m_buffer;
		SyncCtrlObj* m_sync;
		BinCtrlObj *m_bin;
		//RoiCtrlObj *m_roi;
};

} // namespace RayonixHs
} // namespace lima

#endif // RayonixHsINTERFACE_H
