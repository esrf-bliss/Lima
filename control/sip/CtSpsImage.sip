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
%If (WITH_SPS_IMAGE)
%If (POSIX_PLATFORM)
class CtSpsImage 
{
%TypeHeaderCode
#include "lima/CtSpsImage.h"
using namespace lima;
%End  
 public:
	CtSpsImage();
	~CtSpsImage();
	
	void setNames(const std::string& spec_name, 
		      const std::string& array_name);
        void getNames(std::string& spec_name /Out/,
		      std::string& array_name /Out/);
		
	void prepare(const FrameDim &frame_dim);
	void frameReady(Data&);
	void reset();
	void setActive(bool aFlag);
	bool isActive() const;
};
%End
%End
