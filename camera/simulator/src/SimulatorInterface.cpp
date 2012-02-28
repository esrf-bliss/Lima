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

	HwBufferCtrlObj *buffer = simu.getBufferCtrlObj();
	m_cap_list.push_back(HwCap(buffer));

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

