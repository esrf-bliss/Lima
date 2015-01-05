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
/***************************************************************//**
 * @file HwBufferSave.h
 * @brief This file contains the HwBufferSave class used to save frames
 *
 * @author A.Kirov, A.Homs
 * @date 03/06/2009
 *******************************************************************/

#ifndef BUFFERSAVE_H
#define BUFFERSAVE_H

#include <stdio.h>
#include <string>
#include <fstream>

#include "LimaCompatibility.h"
#include "HwFrameInfo.h"

namespace lima {


/***************************************************************//**
 * @class HwBufferSave
 *
 * The main method is writeFrame(const HwFrameInfoType& finfo).
 * The other methods configure the saving parameters.
 *******************************************************************/
class LIMACORE_API HwBufferSave {
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
	void getPrefix(std::string& prefix) const;

	void setFormat(FileFormat  format);
	void getFormat(FileFormat& format) const;

	void setIndex(int  idx);
	void getIndex(int& idx) const;

	void setTotFileFrames(int  tot_file_frames);
	void getTotFileFrames(int& tot_file_frames) const;

	void getOpenFileName(std::string& file_name) const;
	bool isFileOpen() const;

  private:
	std::string getDefSuffix() const;
	void openFile();
	void closeFile();

	void writeEdfHeader( const HwFrameInfoType& finfo );

	FileFormat m_format;
	std::string m_prefix;
	int m_idx;
	std::string m_suffix;
	std::string m_file_name;
	bool m_overwrite;
	int m_written_frames;
	int m_tot_file_frames;
	HwFrameInfoType m_last_frame;
	std::ofstream *m_fout;
};


}

#endif /* BUFFERSAVE_H */
