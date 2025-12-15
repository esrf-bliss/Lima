//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2026
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
class CtShutter {
%TypeHeaderCode
#include "lima/CtShutter.h"
#include "lima/Constants.h"
#include <sstream>
using namespace lima;
%End
    public:

        CtShutter(HwInterface *hw);
        ~CtShutter();

        bool hasCapability() const;

        void getModeList(ShutterModeList& mode_list /Out/) const;
    
        bool checkMode(ShutterMode shut_mode) const;
        void setMode(ShutterMode  shut_mode);
        void getMode(ShutterMode& shut_mode /Out/) const;

        void setState(bool  shut_open);
        void getState(bool& shut_open /Out/) const;

        void setOpenTime (double  shut_open_time);
        void getOpenTime (double& shut_open_time /Out/) const;
        void setCloseTime(double  shut_close_time);
        void getCloseTime(double& shut_close_time /Out/) const;
        
    private:
        CtShutter(const CtShutter&);
};
