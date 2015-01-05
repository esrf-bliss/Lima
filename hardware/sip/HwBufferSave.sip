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
class HwBufferSave 
{
%TypeHeaderCode
#include "lima/HwBufferSave.h"
using namespace lima;
%End
  public :
	enum FileFormat {
		Raw, EDF,
	};

	HwBufferSave( FileFormat format = Raw, 
		    const std::string& prefix = "img", 
		    int idx = 0, const std::string& suffix = "", 
		    bool overwrite = false , int tot_file_frames = 1);
	~HwBufferSave( );

	void writeFrame( const HwFrameInfoType& finfo );

	void setPrefix(const std::string& prefix);
	void getPrefix(std::string& prefix /Out/) const;

	void setFormat(FileFormat  format);
	void getFormat(FileFormat& format /Out/) const;

	void setIndex(int  idx);
	void getIndex(int& idx /Out/) const;

	void setTotFileFrames(int  tot_file_frames);
	void getTotFileFrames(int& tot_file_frames /Out/) const;

	void getOpenFileName(std::string& file_name /Out/) const;
	bool isFileOpen() const;
};
