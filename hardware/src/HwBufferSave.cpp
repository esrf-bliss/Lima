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
 * @file HwBufferSave.cpp
 * @brief This file contains the HwBufferSave class implementation
 *
 * @author A.Kirov, A.Homs
 * @date 03/06/2009
 *******************************************************************/

#include "lima/HwBufferSave.h"

#include <ctime>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#ifdef __unix
#include <unistd.h>
#include <sys/time.h>
#else
#include <processlib/win/unistd.h>
#include <processlib/win/time_compat.h>
#endif
using namespace lima;
using namespace std;

#define EDF_HEADER_LEN		1024
#define EDF_HEADER_BUFFER_LEN	(10 * EDF_HEADER_LEN)


/***************************************************************//**
 * @brief HwBufferSave class constructor setting member variables
 *
 * @param[in] format           EDF or Raw file format
 * @param[in] prefix           File name prefix
 * @param[in] idx              The starting file index
 * @param[in] suffix           File name suffix
 * @param[in] overwrite        Whether to overwrite existing files
 * @param[in] tot_file_frames  Frames per file
 *******************************************************************/
HwBufferSave::HwBufferSave( FileFormat format, const string& prefix, int idx,
			const string& suffix, bool overwrite, 
			int tot_file_frames ) 
	: m_format(format), m_prefix(prefix), m_idx(idx), m_suffix(suffix),
	  m_overwrite(overwrite), m_tot_file_frames(tot_file_frames)
{
	m_written_frames = 0;
	m_fout = NULL;
	if (m_suffix == "")
		m_suffix = getDefSuffix();
}


HwBufferSave::~HwBufferSave( ) 
{
	closeFile();
}

string HwBufferSave::getDefSuffix() const
{
	return (m_format == EDF) ? ".edf" : ".raw";
}

void HwBufferSave::openFile()
{
	if (isFileOpen())
		return;

	ostringstream idx;
	idx.width(4);
	idx.fill('0');
	idx << m_idx;
	
	m_file_name = m_prefix + idx.str() + m_suffix;
	m_fout = new ofstream(m_file_name.c_str(), 
			      ios_base::out | ios_base::binary);

}

void HwBufferSave::closeFile()
{
	if (!isFileOpen())
		return;

	delete m_fout;
	m_fout = NULL;
	m_file_name = "";
	m_written_frames = 0;
	m_idx++;
}

void HwBufferSave::getOpenFileName(std::string& file_name) const
{
	file_name = m_file_name;
}

void HwBufferSave::writeEdfHeader( const HwFrameInfoType& finfo )
{
	time_t ctime_now;
	time(&ctime_now);

	struct timeval tod_now;
	gettimeofday(&tod_now, NULL);

	char time_str[64];
	ctime_r(&ctime_now, time_str);
	time_str[strlen(time_str) - 1] = '\0';

	const FrameDim *fdim = &finfo.frame_dim;
	const Size& frame_size = fdim->getSize();
	int depth = fdim->getDepth();
	int image_nb = m_written_frames + 1;

	char buffer[EDF_HEADER_BUFFER_LEN];
	char *p = buffer;
	p += snprintf(p, sizeof(buffer) - (p - buffer), "{\n");
	p += snprintf(p, sizeof(buffer) - (p - buffer), "HeaderID = EH:%06u:000000:000000 ;\n", image_nb);
	p += snprintf(p, sizeof(buffer) - (p - buffer), "Image = %u ;\n", image_nb);
	p += snprintf(p, sizeof(buffer) - (p - buffer), "ByteOrder = LowByteFirst ;\n");
	p += snprintf(p, sizeof(buffer) - (p - buffer), "DataType = %s ;\n", 
		      (depth == 1) ? "UnsignedByte" :
		     ((depth == 2) ? "UnsignedShort" : "UnsignedLong"));
	p += snprintf(p, sizeof(buffer) - (p - buffer), "Size = %d ;\n", fdim->getMemSize());
	p += snprintf(p, sizeof(buffer) - (p - buffer), "Dim_1 = %d ;\n", frame_size.getWidth());
	p += snprintf(p, sizeof(buffer) - (p - buffer), "Dim_2 = %d ;\n", frame_size.getHeight());

	p += snprintf(p, sizeof(buffer) - (p - buffer), "acq_frame_nb = %d ;\n", finfo.acq_frame_nb);
	p += snprintf(p, sizeof(buffer) - (p - buffer), "time = %s ;\n", time_str);
	p += snprintf(p, sizeof(buffer) - (p - buffer), "time_of_day = %ld.%06ld ;\n", 
		     tod_now.tv_sec, tod_now.tv_usec);
        if (finfo.frame_timestamp.isSet())
                p += snprintf(p, sizeof(buffer) - (p - buffer), "time_of_frame = %.6f ;\n",
                             double(finfo.frame_timestamp));
	p += snprintf(p, sizeof(buffer) - (p - buffer), "valid_pixels = %d ;\n", finfo.valid_pixels);

	long l = long(p - buffer);
	long len = l;
	long rem = len % EDF_HEADER_LEN;
	if (rem > 0)
		len += EDF_HEADER_LEN - rem;
	p += snprintf(p, sizeof(buffer) - (p - buffer), "%*s}\n", int(len - (l + 2)), "");
	len = long(p - buffer);

	m_fout->write(buffer, len);
}

/***************************************************************//**
 * @brief     This method writes a frame into a file
 *
 * @param[in] finfo  HwFrameInfoType structure reference
 *******************************************************************/
void HwBufferSave::writeFrame( const HwFrameInfoType& finfo )
{
	const FrameDim *fdim = &finfo.frame_dim;
	if (!fdim)
		throw LIMA_HW_EXC(InvalidValue, "Null finfo.fdim");

	openFile();

	if ( m_format == EDF )
		writeEdfHeader(finfo);

	m_fout->write((char *)finfo.frame_ptr, fdim->getMemSize());

	m_written_frames++;
	if (m_written_frames == m_tot_file_frames)
		closeFile();
}

bool HwBufferSave::isFileOpen() const
{
	return !!m_fout;
}

void HwBufferSave::setPrefix(const string& prefix)
{
	if (prefix == m_prefix)
		return;

	if (isFileOpen())
		throw LIMA_HW_EXC(InvalidValue, "Set prefix with open file");

	m_prefix = prefix;
}

void HwBufferSave::getPrefix(string& prefix) const
{
	prefix = m_prefix;
}

void HwBufferSave::setFormat(FileFormat format)
{
	if (format == m_format)
		return;

	if (isFileOpen())
		throw LIMA_HW_EXC(InvalidValue, "Set format with open file");

	m_format = format;
	m_suffix = getDefSuffix();

}

void HwBufferSave::getFormat(FileFormat& format) const
{
	format = m_format;
}

void HwBufferSave::setIndex(int idx)
{
	if (idx == m_idx)
		return;

	if (isFileOpen())
		throw LIMA_HW_EXC(InvalidValue, "Set idx with open file");

	m_idx = idx;
}

void HwBufferSave::getIndex(int& idx) const
{
	idx = m_idx;
}

void HwBufferSave::setTotFileFrames(int tot_file_frames)
{
	if (tot_file_frames == m_tot_file_frames)
		return;

	if (isFileOpen())
		throw LIMA_HW_EXC(InvalidValue, 
				  "Set tot_file_frames with open file");

	m_tot_file_frames = tot_file_frames;
}

void HwBufferSave::getTotFileFrames(int& tot_file_frames) const
{
	tot_file_frames = m_tot_file_frames;
}

