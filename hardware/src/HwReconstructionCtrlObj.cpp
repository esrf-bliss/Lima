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
#include "lima/HwReconstructionCtrlObj.h"
#include "lima/Exceptions.h"
#include "processlib/LinkTask.h"

using namespace lima;

HwReconstructionCtrlObj::HwReconstructionCtrlObj() : m_cbk(NULL)
{
  DEB_CONSTRUCTOR();
  DEB_TRACE() << DEB_VAR1(m_cbk);
}

HwReconstructionCtrlObj::~HwReconstructionCtrlObj()
{
  DEB_DESTRUCTOR();
  if(m_cbk)
    unregisterReconstructionChangeCallback(*m_cbk);
}

void HwReconstructionCtrlObj::reconstructionChange(LinkTask* aNewRecTaskPt)
{
  DEB_MEMBER_FUNCT();

  if(m_cbk)
    m_cbk->change(aNewRecTaskPt);
}

void HwReconstructionCtrlObj::registerReconstructionChangeCallback(HwReconstructionCtrlObj::Callback& cbk)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(&cbk,m_cbk);

  if(m_cbk)
    THROW_HW_ERROR(InvalidValue) << "Callback already registered";

  m_cbk = &cbk;
}

void HwReconstructionCtrlObj::unregisterReconstructionChangeCallback(HwReconstructionCtrlObj::Callback& cbk)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(&cbk,m_cbk);

  if(m_cbk != &cbk)
    THROW_HW_ERROR(InvalidValue) << "Specified Callback is not registered";
  
  m_cbk = NULL;
}

