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
#ifndef CTSAVING_HDF5_H
#define CTSAVING_HDF5_H

#include "H5Cpp.h"
#include "CtSaving.h"

using namespace H5;

namespace lima {

class SaveContainerHdf5: public CtSaving::SaveContainer {
DEB_CLASS_NAMESPC(DebModControl,"Saving HDF5 Container","Control");

public:

	SaveContainerHdf5(CtSaving::Stream& stream, CtSaving::FileFormat format);
	virtual ~SaveContainerHdf5();

protected:
	virtual bool _open(const std::string &filename, std::ios_base::openmode flags);
	virtual void _close();
	virtual void _writeFile(Data &data, CtSaving::HeaderMap &aHeader, CtSaving::FileFormat);
	virtual void _clear();

private:
	CtSaving::FileFormat m_format;
	Mutex m_lock;
	bool m_already_opened;
	bool m_format_written;
	DataSpace m_image_dataspace;
	DataSet m_image_dataset;
	H5File *m_file;
	Group m_entry;
};

}
#endif // CTSAVING_HDF5_H
