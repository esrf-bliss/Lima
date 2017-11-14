//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2014
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
#include <cmath>
#include "CtSaving_Hdf5.h"
#include "H5Cpp.h"
#include "lima/CtControl.h"
#include "lima/CtImage.h"
#include "lima/CtAcquisition.h"
#include "lima/HwInterface.h"
#include "lima/HwCap.h"

using namespace lima;
using namespace H5;
using namespace std;

const int RANK_ONE = 1;
const int RANK_TWO = 2;
const int RANK_THREE = 3;
/* file class */
struct SaveContainerHdf5::_File
{
  _File() : 
	m_format_written(false),
    m_in_append(false),
    m_dataset_extended(false),
    m_image_dataspace(NULL),
    m_image_dataset(NULL),
    m_file(NULL),
    m_entry(NULL),
    m_measurement_detector(NULL),
    m_instrument_detector(NULL),
    m_measurement_detector_info(NULL),
    m_measurement_detector_parameters(NULL),
    m_entry_index(0)
  {}
  
  ~_File()
  {
    delete m_image_dataspace;
    delete m_image_dataset;
    delete m_measurement_detector;
    delete m_instrument_detector;
    delete m_measurement_detector_parameters;
    delete m_entry;
    delete m_file;
  }
  
  _File(_File& other)
  { 
    m_format_written = other.m_format_written;
    m_in_append = other.m_in_append;
    m_dataset_extended = other.m_dataset_extended;
    m_prev_images_written = other.m_prev_images_written;
    m_image_dataspace = other.m_image_dataspace;
    m_image_dataset = other.m_image_dataset;
    m_file = other.m_file;
    m_entry = other.m_entry;
    m_measurement_detector = other.m_measurement_detector;
    m_instrument_detector = other.m_instrument_detector;
    m_measurement_detector_info = other.m_measurement_detector_info;
    m_measurement_detector_parameters = other.m_measurement_detector_parameters;
    m_entry_index = other.m_entry_index;
    m_entry_name = other.m_entry_name;

    //transfer pointer to the new structure
    other.m_image_dataspace = NULL;
    other.m_image_dataset = NULL;
    other.m_file = NULL;
    other.m_entry = NULL;
    other.m_measurement_detector = NULL;
    other.m_instrument_detector = NULL;
    other.m_measurement_detector_info = NULL;
    other.m_measurement_detector_parameters = NULL;
  }

  bool m_format_written;
  bool m_in_append;
  bool m_dataset_extended;
  int m_prev_images_written;
  DataSpace *m_image_dataspace;
  DataSet *m_image_dataset;
  H5File *m_file;
  Group *m_entry;
  Group *m_measurement_detector;
  Group *m_instrument_detector;
  Group *m_measurement_detector_info;
  Group *m_measurement_detector_parameters;
  int m_entry_index;
  string m_entry_name;
};
/* Static function helper*/
DataType get_h5_type(unsigned char)		{return PredType(PredType::NATIVE_UINT8);}
DataType get_h5_type(char)			{return PredType(PredType::NATIVE_INT8);}
DataType get_h5_type(unsigned short)		{return PredType(PredType::NATIVE_UINT16);}
DataType get_h5_type(short)			{return PredType(PredType::NATIVE_INT16);}
DataType get_h5_type(unsigned int)		{return PredType(PredType::NATIVE_UINT32);}
DataType get_h5_type(int)			{return PredType(PredType::NATIVE_INT32);}
DataType get_h5_type(unsigned long long)	{return PredType(PredType::NATIVE_UINT64);}
DataType get_h5_type(long long)			{return PredType(PredType::NATIVE_INT64);}
DataType get_h5_type(float)			{return PredType(PredType::NATIVE_FLOAT);}
DataType get_h5_type(double)			{return PredType(PredType::NATIVE_DOUBLE);}
DataType get_h5_type(std::string& s)            {return StrType(H5T_C_S1, s.size()? s.size():1);}
DataType get_h5_type(bool)			{return PredType(PredType::NATIVE_UINT8);}

template <class T>
void write_h5_dataset(Group group,const char* entry_name,T& val)
{
       DataSpace dataspace(H5S_SCALAR);
       DataType datatype = get_h5_type(val);
       DataSet dataset(group.createDataSet(entry_name,datatype, dataspace));
       dataset.write(&val, datatype);

}

template <>
void write_h5_dataset(Group group,const char* entry_name,std::string& val)
{
       DataSpace dataspace(H5S_SCALAR);
       DataType datatype = get_h5_type(val);
       DataSet dataset(group.createDataSet(entry_name,datatype, dataspace));
       dataset.write(val.c_str(), datatype);


}
template <class L,class T>
void write_h5_attribute(L location,const char* entry_name,T& val)
{
       DataSpace dataspace(H5S_SCALAR);
       DataType datatype = get_h5_type(val);
       Attribute attr(location.createAttribute(entry_name,datatype, dataspace));
       attr.write(datatype, &val);
}

template <class L>
void write_h5_attribute(L location,const char* entry_name,std::string& val)
{
       DataSpace dataspace(H5S_SCALAR);
       DataType datatype = get_h5_type(val);
       Attribute attr(location.createAttribute(entry_name,datatype, dataspace));
       attr.write(datatype, val.c_str());
    
}
/** @brief helper to calculate an optimized chuncking of the image data set
 * 
 *
 */
static void calculate_chunck(hsize_t* data_size, hsize_t* chunck, int  depth)
{
       const double request_chunck_size = 256.;
       const double request_chunck_memory_size = 1024.*1024.;
       double request_chunck_pixel_nb = request_chunck_memory_size / depth;
       long x_chunk = (long) ceil(double(data_size[2]) / request_chunck_size);
       long y_chunk = (long) ceil(double(data_size[1]) / request_chunck_size);
       long z_chunk = (long) ceil(request_chunck_pixel_nb / ((data_size[2] / x_chunk) * (data_size[1] / y_chunk)));

       hsize_t nb_image = data_size[0];
       while(hsize_t(z_chunk) > nb_image) {
              --x_chunk,--y_chunk;
	      if(!x_chunk  || !y_chunk) break;
	      z_chunk = (long) ceil(request_chunck_pixel_nb / ((data_size[2] / x_chunk) * (data_size[1] / y_chunk)));
       }
       if(!x_chunk) x_chunk = 1;
       else if(x_chunk > 8) x_chunk = 8;
       
       if(!y_chunk) y_chunk = 1;
       else if(y_chunk > 8) y_chunk = 8;
       
       z_chunk = (long) ceil(request_chunck_pixel_nb / ((data_size[2] / x_chunk) * (data_size[1] / y_chunk)));
              
       if(hsize_t(z_chunk) > nb_image) z_chunk = nb_image;
       else if(!z_chunk) z_chunk = 1;

       chunck[0] = z_chunk;
       chunck[1] = (hsize_t) ceil(double(data_size[1]) / y_chunk);
       chunck[2] = (hsize_t) ceil(double(data_size[2]) / x_chunk);
}

/** @brief saving container
 *
 *  This class manage file saving
 */
SaveContainerHdf5::SaveContainerHdf5(CtSaving::Stream& stream, CtSaving::FileFormat format) :
  CtSaving::SaveContainer(stream), m_format(format) {
	DEB_CONSTRUCTOR();
}

SaveContainerHdf5::~SaveContainerHdf5() {
	DEB_DESTRUCTOR();
}

void SaveContainerHdf5::_prepare(CtControl& control) {
	DEB_MEMBER_FUNCT();
	
	m_ct_image = control.image();
	m_ct_acq = control.acquisition();
	m_hw_int = control.hwInterface();
	

	// Get detector info 
	HwDetInfoCtrlObj *det_info;
	m_hw_int->getHwCtrlObj(det_info);
	det_info->getUserDetectorName(m_ct_parameters.det_name);
	det_info->getInstrumentName(m_ct_parameters.instrument_name);
	det_info->getDetectorModel(m_ct_parameters.det_model);
	det_info->getDetectorType(m_ct_parameters.det_type);
	det_info->getPixelSize(m_ct_parameters.pixel_size[0], m_ct_parameters.pixel_size[1]);
	det_info->getMaxImageSize(m_ct_parameters.max_image_size);
	det_info->getCurrImageType(m_ct_parameters.curr_image_type);

	// Get acquisition parameters
	m_ct_acq->getAcqMode(m_ct_parameters.acq_mode);
	m_ct_acq->getAcqExpoTime(m_ct_parameters.acq_expo_time);
	m_ct_acq->getAcqNbFrames(m_ct_parameters.acq_nbframes);
	m_ct_acq->getLatencyTime(m_ct_parameters.acq_latency_time);
	m_ct_acq->getTriggerMode(m_ct_parameters.acq_trigger_mode);

	if (m_ct_parameters.acq_mode == Accumulation) {
	       m_ct_acq->getAccTimeMode(m_ct_parameters.acc_time_mode);
	       m_ct_acq->getAccMaxExpoTime(m_ct_parameters.acc_max_expotime);
	       m_ct_acq->getAccExpoTime(m_ct_parameters.acc_expotime);
	       m_ct_acq->getAccLiveTime(m_ct_parameters.acc_livetime);
	       m_ct_acq->getAccDeadTime(m_ct_parameters.acc_deadtime);			
	} else if (m_ct_parameters.acq_mode == Concatenation){
	  m_ct_acq->getConcatNbFrames(m_ct_parameters.concat_nbframes);
	}	
	// Get image operations
	m_ct_image->getBin(m_ct_parameters.image_bin);
	m_ct_image->getRoi(m_ct_parameters.image_roi);
	m_ct_image->getFlip(m_ct_parameters.image_flip);
	m_ct_image->getRotation(m_ct_parameters.image_rotation);
	m_ct_image->getImageDim(m_ct_parameters.image_dim);

	// Check if the overwrite policy if "MultiSet" is activated	
	CtSaving::OverwritePolicy overwrite_policy;
	control.saving()->getOverwritePolicy(overwrite_policy);
	m_is_multiset = (overwrite_policy == CtSaving::MultiSet);
	CtSaving::Parameters pars;
	control.saving()->getParameters(pars);
	if (m_is_multiset) 
	  m_nbframes = m_ct_parameters.acq_nbframes;
	else 
	  m_nbframes = pars.framesPerFile;

}
void* SaveContainerHdf5::_open(const std::string &filename, std::ios_base::openmode openFlags) {
	DEB_MEMBER_FUNCT();

	_File new_file;

	try
	{
		// Turn off the auto-printing when failure occurs so that we can
		// handle the errors appropriately
		H5::Exception::dontPrint();

		bool is_hdf5 = false;
		bool file_exists = true;

		if (openFlags & std::ios_base::trunc) {
		  // overwrite existing file
		  new_file.m_file = new H5File(filename, H5F_ACC_TRUNC);
		  new_file.m_entry_index = 0;
		} else if (openFlags & std::ios_base::app) {
		  // Append if the file exists and it is a HDF5 file			  
		  try{
		    new_file.m_file = new H5File();
		    is_hdf5 = new_file.m_file->isHdf5(filename);
		  } catch (FileIException &error){
		    //error.printError();
		    file_exists = false;
		    
		  }
		  if (file_exists && is_hdf5){
		    new_file.m_file->openFile(filename, H5F_ACC_RDWR);
		    new_file.m_in_append = true;
		    new_file.m_entry_index = findLastEntry(new_file);
		    if (m_is_multiset) {
		      new_file.m_entry_index++;
		    } else {
		      new_file.m_format_written = true;
		    }
		  }  else if (!file_exists){
		    DEB_TRACE() << "append mode but file does not exist, " << filename;
		    delete new_file.m_file;
		    new_file.m_file = new H5File(filename, H5F_ACC_EXCL);
		    new_file.m_entry_index = 0;
		  } else {
		    THROW_CTL_ERROR(Error) << "File " << filename 
					   <<  "is not an HDF5 file, bad or corrupted format !" ;
		  }
		} else {
		  // fail if file already exists
		  new_file.m_file = new H5File(filename, H5F_ACC_EXCL);
		  new_file.m_entry_index = 0;
		}
		
		// create the next entry name and 
		//fails if it already exists in the file
		char strname[256];
		sprintf(strname,"/entry_%04d", new_file.m_entry_index);			
		if (!new_file.m_format_written) {
		  Group *group = NULL;
		  if (new_file.m_in_append) {
		    try {
		      group = new Group(new_file.m_file->openGroup(strname));
		      DEB_TRACE() << "Hoops the new entry " << strname << " already exists in file " << filename;
		      delete group;
		      THROW_CTL_ERROR(Error) << "In file " << filename << " the entry  " << strname 
					     << " already exists";	
		    } catch (...) {
		      DEB_TRACE() << "Ok the new entry " << strname << " is free in file " << filename;
		    }
		  }
		  // fine, this entry does not exist
		  new_file.m_entry_name = strname;
		  // Create the new entry  for the dataset
		  new_file.m_entry = new Group(new_file.m_file->createGroup(new_file.m_entry_name));	
		  string nxentry = "NXentry";
		  write_h5_attribute(*new_file.m_entry, "NX_class", nxentry);
		  string title = "Lima 2D detector acquisition";
		  write_h5_dataset(*new_file.m_entry, "title", title);
		  
		  // could be the beamline/instrument name instead
		  Group instrument = Group(new_file.m_entry->createGroup(m_ct_parameters.instrument_name));
		  string nxinstrument = "NXinstrument";
		  write_h5_attribute(instrument, "NX_class", nxinstrument);
		  
		  new_file.m_instrument_detector = new Group(instrument.createGroup(m_ct_parameters.det_name));
		  string nxdetector = "NXdetector";
		  write_h5_attribute(*new_file.m_instrument_detector, "NX_class", nxdetector);
		  
		  Group measurement = Group(new_file.m_entry->createGroup("measurement"));
		  string nxcollection = "NXcollection";
		  write_h5_attribute(measurement, "NX_class", nxcollection);
		  
		  new_file.m_measurement_detector = new Group(measurement.createGroup(m_ct_parameters.det_name));
		  write_h5_attribute(*new_file.m_measurement_detector, "NX_class", nxdetector);
		  
		  new_file.m_measurement_detector_parameters = 
		    new Group(new_file.m_measurement_detector->createGroup("parameters"));
		  
		  // write the control parameters (detinfo, acq and image)
		  
		  //Det Info
		  Group det_info = Group(new_file.m_instrument_detector->createGroup("detector_information"));
		  write_h5_dataset(det_info,"name",m_ct_parameters.det_name);
		  write_h5_dataset(det_info,"type",m_ct_parameters.det_type);
		  write_h5_dataset(det_info,"model",m_ct_parameters.det_model);
		  Group pixelsize = Group(det_info.createGroup("pixel_size"));
		  write_h5_dataset(pixelsize,"xsize",m_ct_parameters.pixel_size[0]);
		  write_h5_dataset(pixelsize,"ysize",m_ct_parameters.pixel_size[1]);
		  Group imagesize = Group(det_info.createGroup("max_image_size"));
		  int width = m_ct_parameters.max_image_size.getWidth();
		  int height = m_ct_parameters.max_image_size.getHeight();
		  write_h5_dataset(imagesize,"xsize",width);
		  write_h5_dataset(imagesize,"ysize",height);
		  string im_type=lima::convert_2_string(m_ct_parameters.curr_image_type);
		  write_h5_dataset(det_info,"image_lima_type",im_type);
		  // Acquisition
		  Group acq = Group(new_file.m_instrument_detector->createGroup("acquisition"));
		  string acq_mode = lima::convert_2_string(m_ct_parameters.acq_mode);
		  write_h5_dataset(acq,"mode",acq_mode);
		  write_h5_dataset(acq,"exposure_time",m_ct_parameters.acq_expo_time);
		  write_h5_dataset(acq,"latency_time",m_ct_parameters.acq_latency_time);
		  write_h5_dataset(acq,"nb_frames",m_ct_parameters.acq_nbframes);
		  string trig_mode = lima::convert_2_string(m_ct_parameters.acq_trigger_mode);
		  write_h5_dataset(acq,"trigger_mode",trig_mode);
		  if (m_ct_parameters.acq_mode == Accumulation) {
		    Group acc = Group(acq.createGroup("accumulation"));
		    string time_mode = lima::convert_2_string(m_ct_parameters.acc_time_mode);
		    write_h5_dataset(acc,"time_mode",time_mode);
		    write_h5_dataset(acc,"max_exposure_time",m_ct_parameters.acc_max_expotime);
		    write_h5_dataset(acc,"exposure_time",m_ct_parameters.acc_expotime);
		    write_h5_dataset(acc,"live_time",m_ct_parameters.acc_livetime);
		    write_h5_dataset(acc,"dead_time",m_ct_parameters.acc_deadtime);
		  }
		  if (m_ct_parameters.acq_mode == Concatenation) {
		    Group concat = Group(acq.createGroup("concatenation"));
		    write_h5_dataset(concat,"nb_frames",m_ct_parameters.concat_nbframes);
		  }
		  // Image
		  Group image = Group(new_file.m_instrument_detector->createGroup("image_operation"));
		  Group dim = Group(image.createGroup("dimension"));
		  int xsize = m_ct_parameters.image_dim.getSize().getWidth();
		  int ysize = m_ct_parameters.image_dim.getSize().getHeight();
		  write_h5_dataset(dim,"xsize",xsize);				  
		  write_h5_dataset(dim,"ysize",ysize);				  
		  Group bin = Group(image.createGroup("binning"));
		  int xbin = m_ct_parameters.image_bin.getX();
		  int ybin = m_ct_parameters.image_bin.getY();
		  write_h5_dataset(bin,"x",xbin);				  
		  write_h5_dataset(bin,"y",ybin);				  				
		  Group roi = Group(image.createGroup("region_of_interest"));
		  Point roip = m_ct_parameters.image_roi.getTopLeft();
		  Size roiz = m_ct_parameters.image_roi.getSize();
		  xsize = roiz.getWidth();
		  ysize = roiz.getHeight();
		  write_h5_dataset(roi,"xstart",roip.x);
		  write_h5_dataset(roi,"ystart",roip.y);
		  write_h5_dataset(roi,"xsize",xsize);
		  write_h5_dataset(roi,"ysize",ysize);
		  Group flipping = Group(image.createGroup("flipping"));
		  write_h5_dataset(flipping,"x",m_ct_parameters.image_flip.x);
		  write_h5_dataset(flipping,"y",m_ct_parameters.image_flip.y);
		  string rot = lima::convert_2_string(m_ct_parameters.image_rotation);
		  write_h5_dataset(image, "rotation", rot);					
		} else {
		  new_file.m_entry = new Group(new_file.m_file->openGroup(strname));
		  Group instrument = Group(new_file.m_entry->openGroup(m_ct_parameters.instrument_name));
		  Group measurement = Group(new_file.m_entry->openGroup("measurement"));
		  new_file.m_measurement_detector = new Group(measurement.openGroup(m_ct_parameters.det_name));
		  new_file.m_instrument_detector = new Group(instrument.openGroup(m_ct_parameters.det_name));
		}
	} catch (FileIException &error) {
	      THROW_CTL_ERROR(Error) << "File " << filename << " not opened successfully";
	}

	return new _File(new_file);
}


void SaveContainerHdf5::_close(void* f) {
	DEB_MEMBER_FUNCT();

	_File *file = (_File*)f;
	if (!file->m_in_append || m_is_multiset) {
		// Create hard link to the Data group.
		string img_path = file->m_entry_name;
		img_path += "/measurement/" + m_ct_parameters.det_name+"/data";
		file->m_instrument_detector->link(H5L_TYPE_HARD, img_path, "data");

		// ISO 8601 Time format
		time_t now;
		time(&now);
		char buf[sizeof("2011-10-08T07:07:09Z")];
		strftime(buf, sizeof(buf), "%FT%TZ", gmtime(&now));
		string etime = string(buf);
		write_h5_dataset(*file->m_entry,"end_time",etime);
	}
	delete file;
	DEB_TRACE() << "Close current file";
}

long SaveContainerHdf5::_writeFile(void* f,Data &aData,
				   CtSaving::HeaderMap &aHeader,
				   CtSaving::FileFormat aFormat) {
	DEB_MEMBER_FUNCT();
	_File* file = (_File*)f;
	if (aFormat == CtSaving::HDF5) {

		// get the proper data type
		PredType data_type(PredType::NATIVE_UINT8);
		switch (aData.type) {
		case Data::UINT8:
		       break;
		case Data::INT8:
		       data_type = PredType::NATIVE_INT8;
		       break;
		case Data::UINT16:
		       data_type = PredType::NATIVE_UINT16;
		       break;
		case Data::INT16:
		       data_type = PredType::NATIVE_INT16;
		       break;
		case Data::UINT32:
		       data_type = PredType::NATIVE_UINT32;
		       break;
		case Data::INT32:
		       data_type = PredType::NATIVE_INT32;
		       break;
		case Data::UINT64:
		       data_type = PredType::NATIVE_UINT64;
		       break;
		case Data::INT64:
		       data_type = PredType::NATIVE_INT64;
		       break;
		case Data::FLOAT:
		       data_type = PredType::NATIVE_FLOAT;
		       break;
		case Data::DOUBLE:
		       data_type = PredType::NATIVE_DOUBLE;
		       break;
		case Data::UNDEF:
		default:
		  THROW_CTL_ERROR(Error) << "Invalid image type";
		}

		try {
			if (!file->m_format_written) {
			       
			        // ISO 8601 Time format
			        time_t now;
				time(&now);
				char buf[sizeof("2011-10-08T07:07:09Z")];
				strftime(buf, sizeof(buf), "%FT%TZ", gmtime(&now));
				string stime = string(buf);
				write_h5_dataset(*file->m_entry,"start_time",stime);
				// write header only once into "parameters" group 
				// but we should write some keys into measurement, like motor_pos counter_pos (spec)???
				if (!aHeader.empty()) {
					for (map<string, string>::const_iterator it = aHeader.begin(); it != aHeader.end(); it++) {

						string key = it->first;
						string value = it->second;
						write_h5_dataset(*file->m_measurement_detector_parameters,
								 key.c_str(),value);
					}
				}
				delete file->m_measurement_detector_parameters;
				file->m_measurement_detector_parameters = NULL;
					
				// create the image data structure in the file
				hsize_t data_dims[3], max_dims[3];
				data_dims[1] = aData.dimensions[1];
				data_dims[2] = aData.dimensions[0];
				data_dims[0] = m_nbframes;
				max_dims[1] = aData.dimensions[1];
				max_dims[2] = aData.dimensions[0];
				max_dims[0] = H5S_UNLIMITED;
				// Create property list for the dataset and setup chunk size
				DSetCreatPropList plist;
				hsize_t chunk_dims[3];
				// calculate a optimized chunking
				calculate_chunck(data_dims, chunk_dims, aData.depth());
				plist.setChunk(RANK_THREE, chunk_dims);

				// create new dspace
				file->m_image_dataspace = new DataSpace(RANK_THREE, data_dims, max_dims);
				file->m_image_dataset = 
				  new DataSet(file->m_measurement_detector->createDataSet("data",
											  data_type,
											  *file->m_image_dataspace,
											  plist));
				string nxdata = "NXdata";
				write_h5_attribute(*file->m_image_dataset, "NX_class", nxdata);
				string image = "image"; 
				write_h5_attribute(*file->m_image_dataset, "interpretation", image);
				file->m_prev_images_written = 0;
				file->m_format_written = true;
			} else if (file->m_in_append && !m_is_multiset && !file->m_dataset_extended) {
				hsize_t allocated_dims[3];
				file->m_image_dataset = new DataSet(file->m_measurement_detector->
								    openDataSet("data"));
				file->m_image_dataspace = new DataSpace(file->m_image_dataset->getSpace());
				file->m_image_dataspace->getSimpleExtentDims(allocated_dims);

				hsize_t data_dims[3];
				data_dims[1] = aData.dimensions[1];
				data_dims[2] = aData.dimensions[0];
				data_dims[0] = allocated_dims[0] + m_nbframes;

				if (data_dims[1] != allocated_dims[1] && data_dims[2] != allocated_dims[2]) {
					THROW_CTL_ERROR(Error) << "You are trying to extend the dataset with mismatching image dimensions";
				}

				file->m_image_dataset->extend(data_dims);
				file->m_image_dataspace->close();
				delete file->m_image_dataset;
				file->m_image_dataspace = new DataSpace(file->m_image_dataset->getSpace());
				file->m_prev_images_written = allocated_dims[0];
				file->m_dataset_extended = true;
			}
			// write the image data
			hsize_t slab_dim[3];
			slab_dim[2] = aData.dimensions[0];
			slab_dim[1] = aData.dimensions[1];
			slab_dim[0] = 1;
			DataSpace slabspace = DataSpace(RANK_THREE, slab_dim);
			int image_nb = aData.frameNumber % m_nbframes;
			hsize_t start[] = { file->m_prev_images_written + image_nb, 0, 0 };
			hsize_t count[] = { 1, aData.dimensions[1], aData.dimensions[0] };
			file->m_image_dataspace->selectHyperslab(H5S_SELECT_SET, count, start);
			file->m_image_dataset->write((u_int8_t*) aData.data(), data_type,
						     slabspace, *file->m_image_dataspace);

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
	return aData.size();	// fix me ;-o
}


void SaveContainerHdf5::_clear()
{
	// dont know what to do yet!
	// Inheritance requires me.
}

int SaveContainerHdf5::findLastEntry(const _File &file) {
	char entryName[32];
	int index = -1; 
	try { // determine the next group Entry index
		do {
			sprintf(entryName, "/entry_%04d", index+1);
			file.m_file->openGroup(entryName);
			index++;
		} while (1);
	} catch (H5::Exception &no_group) {
		// ignore this indicates the last group
	}
	return index;
}

