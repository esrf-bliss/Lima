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
#include "SimulatorInterface.h"

using namespace lima;
using namespace lima::Simulator;
using namespace std;

/*******************************************************************
 * \brief DetInfoCtrlObj constructor
 *******************************************************************/

DetInfoCtrlObj::DetInfoCtrlObj(Camera& simu)
	: m_simu(simu)
{
}

DetInfoCtrlObj::~DetInfoCtrlObj()
{
}

void DetInfoCtrlObj::getMaxImageSize(Size& max_image_size)
{
	FrameDim fdim;
	m_simu.getFrameDim(fdim);
	max_image_size = fdim.getSize();
}

void DetInfoCtrlObj::getDetectorImageSize(Size& det_image_size)
{
	m_simu.getMaxImageSize(det_image_size);
}

void DetInfoCtrlObj::getDefImageType(ImageType& def_image_type)
{
	FrameDim fdim;
	m_simu.getFrameDim(fdim);
	def_image_type = fdim.getImageType();
}

void DetInfoCtrlObj::setCurrImageType(ImageType curr_image_type)
{
	FrameDim fdim;
	m_simu.getFrameDim(fdim);
	fdim.setImageType(curr_image_type);
	m_simu.setFrameDim(fdim);
}

void DetInfoCtrlObj::getCurrImageType(ImageType& curr_image_type)
{
	FrameDim fdim;
	m_simu.getFrameDim(fdim);
	curr_image_type = fdim.getImageType();
}

void DetInfoCtrlObj::getPixelSize(double& pixel_size)
{
	pixel_size = 1e-6;
}

void DetInfoCtrlObj::getDetectorType(string& det_type)
{
	det_type = "Simulator";
}

void DetInfoCtrlObj::getDetectorModel(string& det_model)
{
	det_model = "PeakGenerator";
}

void DetInfoCtrlObj::registerMaxImageSizeCallback(
						HwMaxImageSizeCallback& cb)
{
	m_mis_cb_gen.registerMaxImageSizeCallback(cb);
}

void DetInfoCtrlObj::unregisterMaxImageSizeCallback(
						HwMaxImageSizeCallback& cb)
{
	m_mis_cb_gen.unregisterMaxImageSizeCallback(cb);
}


void DetInfoCtrlObj::
     MaxImageSizeCallbackGen::setMaxImageSizeCallbackActive(bool cb_active)
{
}

/*******************************************************************
 * \brief SyncCtrlObj constructor
 *******************************************************************/

SyncCtrlObj::SyncCtrlObj(Camera& simu)
	: HwSyncCtrlObj(), m_simu(simu)
{
}

SyncCtrlObj::~SyncCtrlObj()
{
}

bool SyncCtrlObj::checkTrigMode(TrigMode trig_mode)
{
	return (trig_mode == IntTrig);
}

void SyncCtrlObj::setTrigMode(TrigMode trig_mode)
{
	if (!checkTrigMode(trig_mode))
		throw LIMA_HW_EXC(InvalidValue, "Invalid (external) trigger");
}

void SyncCtrlObj::getTrigMode(TrigMode& trig_mode)
{
	trig_mode = IntTrig;
}

void SyncCtrlObj::setExpTime(double exp_time)
{
	m_simu.setExpTime(exp_time);
}

void SyncCtrlObj::getExpTime(double& exp_time)
{
	m_simu.getExpTime(exp_time);
}

void SyncCtrlObj::setLatTime(double lat_time)
{
	m_simu.setLatTime(lat_time);
}

void SyncCtrlObj::getLatTime(double& lat_time)
{
	m_simu.getLatTime(lat_time);
}

void SyncCtrlObj::setNbHwFrames(int nb_frames)
{
	m_simu.setNbFrames(nb_frames);
}

void SyncCtrlObj::getNbHwFrames(int& nb_frames)
{
	m_simu.getNbFrames(nb_frames);
}

void SyncCtrlObj::getValidRanges(ValidRangesType& valid_ranges)
{
	double min_time = 10e-9;
	double max_time = 1e6;
	valid_ranges.min_exp_time = min_time;
	valid_ranges.max_exp_time = max_time;
	valid_ranges.min_lat_time = min_time;
	valid_ranges.max_lat_time = max_time;
}


/*******************************************************************
 * \brief BinCtrlObj constructor
 *******************************************************************/

BinCtrlObj::BinCtrlObj(Camera& simu)
	: m_simu(simu)
{
}

BinCtrlObj::~BinCtrlObj()
{
}

void BinCtrlObj::setBin(const Bin& bin)
{
	m_simu.setBin(bin);
}

void BinCtrlObj::getBin(Bin& bin)
{
	m_simu.getBin(bin);
}

void BinCtrlObj::checkBin(Bin& bin)
{
	m_simu.checkBin(bin);
}


/*******************************************************************
 * \brief Interface constructor
 *******************************************************************/

Interface::Interface(Camera& simu)
	: m_simu(simu), m_det_info(simu),
	  m_sync(simu), m_bin(simu)
{
	HwDetInfoCtrlObj *det_info = &m_det_info;
	m_cap_list.push_back(HwCap(det_info));

	m_cap_list.push_back(HwCap(simu.getBufferMgr()));

	HwSyncCtrlObj *sync = &m_sync;
	m_cap_list.push_back(HwCap(sync));

	HwBinCtrlObj *bin = &m_bin;
	m_cap_list.push_back(HwCap(bin));
}

Interface::~Interface()
{
}

void Interface::getCapList(HwInterface::CapList &aReturnCapList) const
{
  aReturnCapList = m_cap_list;
}

void Interface::reset(ResetLevel reset_level)
{
	m_simu.reset();
}

void Interface::prepareAcq()
{
}

void Interface::startAcq()
{
	m_simu.startAcq();
}

void Interface::stopAcq()
{
	m_simu.stopAcq();
}

void Interface::getStatus(StatusType& status)
{
  status.set(m_simu.getStatus());
}

int Interface::getNbHwAcquiredFrames()
{
	return m_simu.getNbAcquiredFrames();
}

