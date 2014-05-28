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

#include "CtSaving_Hdf5.h"
#include "H5Cpp.h"

using namespace lima;
using namespace H5;
using namespace std;

const int RANK_ONE = 1;
const int RANK_TWO = 2;
const int RANK_THREE = 3;

/** @brief saving container
 *
 *  This class manage file saving
 */
SaveContainerHdf5::SaveContainerHdf5(CtSaving::Stream& stream, CtSaving::FileFormat format) :
		CtSaving::SaveContainer(stream), m_format(format) {
	DEB_CONSTRUCTOR();
	m_already_opened = false;
	m_format_written = false;
}

SaveContainerHdf5::~SaveContainerHdf5() {
	DEB_DESTRUCTOR();
}

bool SaveContainerHdf5::_open(const std::string &filename, std::ios_base::openmode openFlags) {
	DEB_MEMBER_FUNCT();

	try
	{
		// Turn off the auto-printing when failure occurs so that we can
		// handle the errors appropriately
		H5::Exception::dontPrint();

		if (!m_already_opened) {
			if (openFlags & std::ios_base::trunc) {
				m_file = new H5File(filename, H5F_ACC_TRUNC);
			} else {
				m_file = new H5File(filename, H5F_ACC_EXCL);
			}
			m_already_opened = true;
		}
	} catch (FileIException &error) {
		THROW_CTL_ERROR(Error) << "File " << filename << " not opened successfully";
	}
	return true;
}

void SaveContainerHdf5::_close() {
	DEB_MEMBER_FUNCT();

	Group data = Group(m_entry.createGroup("Data"));
	// Create hard link to the Data group.
	data.link(H5L_TYPE_HARD, "/Entry/Instrument/Detector/image", "image");

	// ISO 8601 Time format
	time_t now;
	time(&now);
	char buf[sizeof("2011-10-08T07:07:09Z")];
	strftime(buf, sizeof(buf), "%FT%TZ", gmtime(&now));
	string etime = string(buf);
	hsize_t strdim[] = { 1 }; /* Dataspace dimensions */
	DataSpace dataspace(RANK_ONE, strdim);
	StrType datatype(H5T_C_S1, etime.size());
	DataSet dataset = DataSet(m_entry.createDataSet("end_time", datatype, dataspace));
	dataset.write(etime, datatype);

	m_file->close();
	m_already_opened = false;
	m_format_written = false;
	delete m_file;
	DEB_TRACE() << "Close current file";
	return;
}

void SaveContainerHdf5::_writeFile(Data &aData, CtSaving::HeaderMap &aHeader, CtSaving::FileFormat aFormat) {
	DEB_MEMBER_FUNCT();
	if (aFormat == CtSaving::HDF5) {
		const CtSaving::Parameters& pars = m_stream.getParameters(CtSaving::Acq);
		try {
			if (!m_format_written) {
				// Create an entry point for the dataset
				m_entry = Group(m_file->createGroup("/Entry"));
				{
					// ISO 8601 Time format
					time_t now;
					time(&now);
					char buf[sizeof("2011-10-08T07:07:09Z")];
					strftime(buf, sizeof(buf), "%FT%TZ", gmtime(&now));
					string stime = string(buf);
					hsize_t strdim[] = { 1 }; /* Dataspace dimensions */
					DataSpace dataspace(RANK_ONE, strdim);
					StrType datatype(H5T_C_S1, stime.size());
					DataSet dataset = DataSet(m_entry.createDataSet("start_time", datatype, dataspace));
					dataset.write(stime, datatype);
				}
				if (!aHeader.empty()) {
					Group header = Group(m_entry.createGroup("Header"));
					for (map<string, string>::const_iterator it = aHeader.begin(); it != aHeader.end(); it++) {

						string key = it->first;
						string value = it->second;
						hsize_t strdim[] = { 1 }; /* Dataspace dimensions */
						DataSpace dataspace(RANK_ONE, strdim);
						StrType datatype(H5T_C_S1, value.size());
						DataSet dataset = DataSet(header.createDataSet(key, datatype, dataspace));
						dataset.write(value, datatype);
					}
				}
				Group instrument = Group(m_entry.createGroup("Instrument"));
				Group detector = Group(instrument.createGroup("Detector"));

				// create the image data structure in the file
				hsize_t data_dims[3];
				data_dims[0] = aData.dimensions[1];
				data_dims[1] = aData.dimensions[0];
				data_dims[2] = pars.framesPerFile;
				m_image_dataspace = DataSpace(RANK_THREE, data_dims); // create new dspace
				switch (aData.type) {
				case Data::UINT8:
					m_image_dataset = DataSet(detector.createDataSet("image", PredType::NATIVE_UINT8, m_image_dataspace));
					break;
				case Data::INT8:
					m_image_dataset = DataSet(detector.createDataSet("image", PredType::NATIVE_INT8, m_image_dataspace));
					break;
				case Data::UINT16:
					m_image_dataset = DataSet(detector.createDataSet("image", PredType::NATIVE_UINT16, m_image_dataspace));
					break;
				case Data::INT16:
					m_image_dataset = DataSet(detector.createDataSet("image", PredType::NATIVE_INT16, m_image_dataspace));
					break;
				case Data::UINT32:
					m_image_dataset = DataSet(detector.createDataSet("image", PredType::NATIVE_UINT32, m_image_dataspace));
					break;
				case Data::INT32:
					m_image_dataset = DataSet(detector.createDataSet("image", PredType::NATIVE_INT32, m_image_dataspace));
					break;
				case Data::UINT64:
					m_image_dataset = DataSet(detector.createDataSet("image", PredType::NATIVE_UINT64, m_image_dataspace));
					break;
				case Data::INT64:
					m_image_dataset = DataSet(detector.createDataSet("image", PredType::NATIVE_INT64, m_image_dataspace));
					break;
				case Data::FLOAT:
					m_image_dataset = DataSet(detector.createDataSet("image", PredType::NATIVE_FLOAT, m_image_dataspace));
					break;
				case Data::DOUBLE:
					m_image_dataset = DataSet(detector.createDataSet("image", PredType::NATIVE_DOUBLE, m_image_dataspace));
					break;
				case Data::UNDEF:
				default:
					THROW_CTL_ERROR(Error) << "Invalid image type";
				}
				m_format_written = true;
			}
			// write the image data
			hsize_t slab_dim[3];
			slab_dim[1] = aData.dimensions[0];
			slab_dim[0] = aData.dimensions[1];
			slab_dim[2] = 1;
			DataSpace slabspace = DataSpace(RANK_THREE, slab_dim);
			int image_nb = aData.frameNumber % pars.framesPerFile;
			hsize_t start[] = { 0, 0, image_nb };
			hsize_t count[] = { aData.dimensions[1], aData.dimensions[0], 1 };
			m_image_dataspace.selectHyperslab(H5S_SELECT_SET, count, start);
			switch (aData.type) {
			case Data::UINT8:
				m_image_dataset.write((u_int8_t*) aData.data(), PredType::NATIVE_UINT8, slabspace, m_image_dataspace);
				break;
			case Data::INT8:
				m_image_dataset.write((int8_t*) aData.data(), PredType::NATIVE_INT8, slabspace, m_image_dataspace);
				break;
			case Data::UINT16:
				m_image_dataset.write((u_int16_t*) aData.data(), PredType::NATIVE_UINT16, slabspace, m_image_dataspace);
				break;
			case Data::INT16:
				m_image_dataset.write((int16_t*) aData.data(), PredType::NATIVE_INT16, slabspace, m_image_dataspace);
				break;
			case Data::UINT32:
				m_image_dataset.write((u_int32_t*) aData.data(), PredType::NATIVE_UINT32, slabspace, m_image_dataspace);
				break;
			case Data::INT32:
				m_image_dataset.write((int32_t*) aData.data(), PredType::NATIVE_INT32, slabspace, m_image_dataspace);
				break;
			case Data::UINT64:
				m_image_dataset.write((u_int64_t*) aData.data(), PredType::NATIVE_UINT64, slabspace, m_image_dataspace);
				break;
			case Data::INT64:
				m_image_dataset.write((int64_t*) aData.data(), PredType::NATIVE_INT64, slabspace, m_image_dataspace);
				break;
			case Data::FLOAT:
				m_image_dataset.write((float*) aData.data(), PredType::NATIVE_FLOAT, slabspace, m_image_dataspace);
				break;
			case Data::DOUBLE:
				m_image_dataset.write((double*) aData.data(), PredType::NATIVE_DOUBLE, slabspace, m_image_dataspace);
				break;
			case Data::UNDEF:
			default:
				THROW_CTL_ERROR(Error) << "Invalid image type";
				break;
			}
			// catch failure caused by the DataSet operations
		} catch (DataSetIException& error) {
			THROW_CTL_ERROR(Error) << "DataSet not created successfully " << error.getCDetailMsg();
			error.printError();
		}
		// catch failure caused by the DataSpace operations
		catch (DataSpaceIException& error) {
			THROW_CTL_ERROR(Error) << "DataSpace not created successfully " << error.getCDetailMsg();
		}
		// catch failure caused by any other HDF5 error
		catch (H5::Exception &e) {
			THROW_CTL_ERROR(Error) << e.getCDetailMsg();
		}
		// catch anything not hdf5 related
		catch (Exception &e) {
			THROW_CTL_ERROR(Error) << e.getErrMsg();
		}
	}
	DEB_RETURN();
}

void SaveContainerHdf5::_clear()
{
	// dont know what to do yet!
	// Inheritance requires me.
}

