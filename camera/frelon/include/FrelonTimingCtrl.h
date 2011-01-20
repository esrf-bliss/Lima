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
#ifndef FRELONTIMINGCTRL_H
#define FRELONTIMINGCTRL_H

#include "FrelonModel.h"
#include "FrelonSerialLine.h"

namespace lima
{

namespace Frelon
{

class TimingCtrl
{
	DEB_CLASS_NAMESPC(DebModCamera, "TimingCtrl", "Frelon");

 public:
	TimingCtrl(Model& model, SerialLine& ser_line);
	~TimingCtrl();

 private:
	Model& m_model;
	SerialLine& m_ser_line;
};


} // namespace Frelon

} // namespace lima


#endif // FRELONTIMINGCTRL_H
