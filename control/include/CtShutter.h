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
#ifndef CTSHUTTER_H
#define CTSHUTTER_H

#include "LimaCompatibility.h"
#include "Constants.h"
#include "SizeUtils.h"
#include "HwInterface.h"
#include "HwShutterCtrlObj.h"

#include <vector>

namespace lima {

  class LIMACORE_API CtShutter 
  {
    DEB_CLASS_NAMESPC(DebModControl,"Shutter","Control");

  public:
    
    CtShutter(HwInterface *hw);
    ~CtShutter();
    bool hasCapability() const;

    void getModeList(ShutterModeList& mode_list) const;
    
    bool checkMode(ShutterMode shut_mode) const;
    void setMode(ShutterMode  shut_mode);
    void getMode(ShutterMode& shut_mode) const;

    void setState(bool  shut_open);
    void getState(bool& shut_open) const;

    void setOpenTime (double  shut_open_time);
    void getOpenTime (double& shut_open_time) const;
    void setCloseTime(double  shut_close_time);
    void getCloseTime(double& shut_close_time) const;

  private:
    bool 		m_has_shutter;
    HwShutterCtrlObj* 	m_hw_shutter;
  };
  
} // namespace lima

#endif // CTSHUTTER_H
