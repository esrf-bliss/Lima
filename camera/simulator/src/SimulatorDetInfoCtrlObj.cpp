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
#include "SimulatorFrameBuilder.h"
#include "SimulatorFrameLoader.h"


using namespace lima;
using namespace lima::Simulator;


void DetInfoCtrlObj::getMaxImageSize(Size& max_image_size)
{
	m_simu.getMaxImageSize(max_image_size);
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

void DetInfoCtrlObj::getPixelSize(double& x_size,double& y_size)
{
	x_size = y_size = 1e-6;
}

void DetInfoCtrlObj::getDetectorType(std::string& det_type)
{
	det_type = "Simulator";
}

void DetInfoCtrlObj::getDetectorModel(std::string& det_model)
{
    return m_simu.getDetectorModel(det_model);
}

void DetInfoCtrlObj::registerMaxImageSizeCallback(
						HwMaxImageSizeCallback& cb)
{
    //Register call back for the frame loader
    FrameLoader *loader = m_simu.getFrameLoader();
	if (loader)
        loader->registerMaxImageSizeCallback(cb);
}

void DetInfoCtrlObj::unregisterMaxImageSizeCallback(
						HwMaxImageSizeCallback& cb)
{
    //Register call back for the frame loader
    FrameLoader *loader = m_simu.getFrameLoader();
    if (loader)
        loader->unregisterMaxImageSizeCallback(cb);
}
