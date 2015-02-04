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
#ifndef SIMUHWINTERFACE_H
#define SIMUHWINTERFACE_H

#include "SimulatorCompatibility.h"
#include "lima/HwInterface.h"
#include "SimulatorCamera.h"
#include "SimulatorSyncCtrlObj.h"
#include "SimulatorDetInfoCtrlObj.h"
#include "SimulatorShutterCtrlObj.h"

namespace lima
{

class HwInterface;

namespace Simulator
{

/*******************************************************************
 * \class BinCtrlObj
 * \brief Control object providing simulator binning interface
 *******************************************************************/

class LIBSIMULATOR_API BinCtrlObj : public HwBinCtrlObj
{
 public:
	BinCtrlObj(Camera& simu);
	virtual ~BinCtrlObj();

	virtual void setBin(const Bin& bin);
	virtual void getBin(Bin& bin);
	virtual void checkBin(Bin& bin);

 private:
	Camera& m_simu;
};


/*******************************************************************
 * \class HwInterface
 * \brief Simulator hardware interface
 *******************************************************************/

class LIBSIMULATOR_API Interface : public HwInterface
{
 public:
	Interface(Camera& simu);
	virtual ~Interface();

	virtual void getCapList(CapList&) const;

	virtual void reset(ResetLevel reset_level);
	virtual void prepareAcq();
	virtual void startAcq();
	virtual void stopAcq();
	virtual void getStatus(StatusType& status);
	virtual int getNbHwAcquiredFrames();
	
    //! get the camera object to access it directly from client
    Camera& getCamera() { return m_simu;}	

 private:
	Camera& 	m_simu;
	CapList 	m_cap_list;
	DetInfoCtrlObj 	m_det_info;
	SyncCtrlObj    	m_sync;
	BinCtrlObj     	m_bin;
    ShutterCtrlObj     	m_shutter;
};

}
}

#endif // SIMUHWINTERFACE_H
