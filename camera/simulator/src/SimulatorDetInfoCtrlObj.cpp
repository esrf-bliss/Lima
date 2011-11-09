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
#include "SimulatorDetInfoCtrlObj.h"

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
