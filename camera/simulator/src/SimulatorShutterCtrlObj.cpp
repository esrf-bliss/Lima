//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2012
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
#include "SimulatorShutterCtrlObj.h"

using namespace lima;
using namespace lima::Simulator;
using namespace std;

//-----------------------------------------------------
// @brief Ctor
//-----------------------------------------------------
ShutterCtrlObj::ShutterCtrlObj(Camera& cam)
: m_cam(cam),m_simu_shutter_mode(Camera::FRAME)
{
    DEB_CONSTRUCTOR();
}

//-----------------------------------------------------
// @brief Dtor
//-----------------------------------------------------
ShutterCtrlObj::~ShutterCtrlObj()
{
    DEB_DESTRUCTOR();
}

//-----------------------------------------------------
// @brief return true if the mode is valid
//-----------------------------------------------------
bool ShutterCtrlObj::checkMode(ShutterMode shut_mode) const
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(shut_mode);

    bool valid_mode;
    switch (shut_mode) 
    {	
    case ShutterAutoFrame:
    case ShutterManual:
        valid_mode = true;
        break;
    default:
        // No AutoSequence mode for Simulator !
        valid_mode = false;
    }

    DEB_RETURN() << DEB_VAR1(valid_mode);
    return valid_mode;
}

//-----------------------------------------------------
// @brief return the shutter valid mode list
//-----------------------------------------------------
void ShutterCtrlObj::getModeList(ShutterModeList& mode_list) const
{
    DEB_MEMBER_FUNCT();
    mode_list.push_back(ShutterAutoFrame);
    mode_list.push_back(ShutterManual);
}

//-----------------------------------------------------
// @brief set the shutter mode
//-----------------------------------------------------
void ShutterCtrlObj::setMode(ShutterMode shut_mode)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(shut_mode);

    if (!checkMode(shut_mode))
        throw LIMA_HW_EXC(InvalidValue, "Invalid Shutter mode");

    m_simu_shutter_mode = (shut_mode == ShutterAutoFrame) ? Camera::FRAME : Camera::MANUAL;
}

//-----------------------------------------------------
// @brief return the shutter mode
//-----------------------------------------------------
void ShutterCtrlObj::getMode(ShutterMode& shut_mode) const
{
    DEB_MEMBER_FUNCT();

    shut_mode = (m_simu_shutter_mode == Camera::FRAME) ? ShutterAutoFrame : ShutterManual;
    DEB_RETURN() << DEB_VAR1(shut_mode);
}

//-----------------------------------------------------
// @brief open or close manually the shutter
//-----------------------------------------------------
void ShutterCtrlObj::setState(bool open)
{
    DEB_MEMBER_FUNCT();
    m_manual_state = open;
}

//-----------------------------------------------------
// @brief return the shutter state, valid if the shutter
// is in manual mode
//-----------------------------------------------------
void ShutterCtrlObj::getState(bool& open) const
{
    DEB_MEMBER_FUNCT();
    open = m_manual_state;
}

//-----------------------------------------------------
// @brief set the shutter opening time
//-----------------------------------------------------
void ShutterCtrlObj::setOpenTime(double shut_open_time)
{
    DEB_MEMBER_FUNCT();			
    m_open_time = shut_open_time;
}

//-----------------------------------------------------
// @brief return the shutter opening time
//-----------------------------------------------------
void ShutterCtrlObj::getOpenTime(double& shut_open_time) const
{
    DEB_MEMBER_FUNCT();
    shut_open_time = m_open_time;

    DEB_RETURN() << DEB_VAR1(shut_open_time);
}

//-----------------------------------------------------
// @brief set the shutter closing time
//-----------------------------------------------------
void ShutterCtrlObj::setCloseTime(double shut_close_time)
{
    DEB_MEMBER_FUNCT();
    m_close_time = shut_close_time;
}

//-----------------------------------------------------
// @brief return the shutter closing time
//-----------------------------------------------------
void ShutterCtrlObj::getCloseTime(double& shut_close_time) const
{
    DEB_MEMBER_FUNCT();
    shut_close_time = m_close_time;
}
