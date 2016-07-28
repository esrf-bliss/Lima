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
#include "lima/CtSaving.h"
#include "lima/CtAcquisition.h"
#include <string>

using namespace H5;
using namespace std;

namespace lima {

class CtControl;
class CtImage;
class CtAcquisition;
class HwInterface;

class SaveContainerHdf5: public CtSaving::SaveContainer {
DEB_CLASS_NAMESPC(DebModControl,"Saving HDF5 Container","Control");

public:

	SaveContainerHdf5(CtSaving::Stream& stream, CtSaving::FileFormat format);
	virtual ~SaveContainerHdf5();

protected:
	virtual void _prepare(CtControl &control);
	virtual void* _open(const std::string &filename, std::ios_base::openmode flags);
	virtual void _close(void*);
	virtual void _writeFile(void*,Data &data, CtSaving::HeaderMap &aHeader, CtSaving::FileFormat);
	virtual void _clear();

private:
	struct _File;
	int findLastEntry(const _File&);

	struct Parameters{
	  string det_name;
	  string instrument_name;
	  string det_model;
	  string det_type;
	  double pixel_size[2];
	  Size max_image_size;
	  ImageType curr_image_type;
	  AcqMode acq_mode;
	  double acq_expo_time;
	  double acq_latency_time;
	  int acq_nbframes;
	  TrigMode acq_trigger_mode;
	  CtAcquisition::AccTimeMode acc_time_mode;
	  double acc_max_expotime;
	  double acc_expotime;
	  double acc_livetime;
	  double acc_deadtime;
	  int concat_nbframes;
	  Bin image_bin;
	  Roi image_roi;
	  Flip image_flip;
	  RotationMode image_rotation;
	  FrameDim image_dim;
	};

	CtSaving::FileFormat m_format;
	Parameters m_ct_parameters;
	CtImage *m_ct_image;
	CtAcquisition *m_ct_acq;
	HwInterface *m_hw_int;
	bool m_is_multiset;
	int m_nbframes;
};

}
#endif // CTSAVING_HDF5_H
