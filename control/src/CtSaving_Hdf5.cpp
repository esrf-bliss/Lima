//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2020
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
#include "lima/CtControl.h"
#include "lima/CtImage.h"
#include "lima/CtAcquisition.h"
#include "lima/HwInterface.h"
#include "lima/HwCap.h"
#include "lima/SizeUtils.h"

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
    m_instrument_detector(NULL),
    m_instrument_detector_header(NULL),
    m_instrument_detector_plot(NULL),
    m_entry_index(0)
  {}
  
  ~_File()
  {
    delete m_image_dataspace;
    delete m_image_dataset;
    delete m_instrument_detector;
    delete m_instrument_detector_header;
    delete m_instrument_detector_plot;
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
    m_instrument_detector = other.m_instrument_detector;
    m_instrument_detector_header = other.m_instrument_detector_header;
    m_instrument_detector_plot = other.m_instrument_detector_plot;
    m_entry_index = other.m_entry_index;
    m_entry_name = other.m_entry_name;
    m_data_name = other.m_data_name;
    m_path_to_data = other.m_path_to_data;

    //transfer pointer to the new structure
    other.m_image_dataspace = NULL;
    other.m_image_dataset = NULL;
    other.m_file = NULL;
    other.m_entry = NULL;
    other.m_instrument_detector = NULL;
    other.m_instrument_detector_data = NULL;
    other.m_instrument_detector_header = NULL;
    other.m_instrument_detector_plot = NULL;
  }

  bool m_format_written;
  bool m_in_append;
  bool m_dataset_extended;
  int m_prev_images_written;
  DataSpace *m_image_dataspace;
  DataSet *m_image_dataset;
  H5File *m_file;
  Group *m_entry;
  Group *m_instrument_detector;
  Group *m_instrument_detector_data;
  Group *m_instrument_detector_header;
  Group *m_instrument_detector_plot;
  int m_entry_index;
  string m_entry_name;
  string m_data_name;
  string m_path_to_data;

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
DataType get_h5_type(std::string& /*s*/)	{StrType type = StrType(H5T_C_S1, H5T_VARIABLE); type.setCset(H5T_CSET_UTF8); return type;}
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
       dataset.write(val, datatype);
}

template <class L,class T>
void write_h5_attribute(L location,const char* entry_name,T& val)
{
       DataType datatype = get_h5_type(val);
       Attribute attr;
       if (!location.attrExists(entry_name)) {
           DataSpace dataspace(H5S_SCALAR);
           attr = Attribute(location.createAttribute(entry_name,datatype, dataspace));
       }
       else {
           attr = location.openAttribute(entry_name);
       }
       attr.write(datatype, &val);
}

template <class L>
void write_h5_attribute(L location,const char* entry_name,std::string& val)
{
       DataType datatype = get_h5_type(val);
       Attribute attr;
       if (!location.attrExists(entry_name)) {
           DataSpace dataspace(H5S_SCALAR);
           attr = Attribute(location.createAttribute(entry_name,datatype, dataspace));
       }
       else {
           attr = location.openAttribute(entry_name);
       }
       attr.write(datatype, val);
}

/** @brief helper to calculate an optimized chuncking of the image data set
 * 
 *
 */
LIMA_MAYBE_UNUSED
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
#if defined(WITH_BS_COMPRESSION)
    if (format == CtSaving::HDF5BS) {
        int ret= bshuf_register_h5filter();
        if (ret < 0) {
          THROW_CTL_ERROR(Error) << "Cannot register H5BSHUF filter";
        }
    }
#endif
}

SaveContainerHdf5::~SaveContainerHdf5() {
	DEB_DESTRUCTOR();
}

void SaveContainerHdf5::_prepare(CtControl& control) {
	DEB_MEMBER_FUNCT();
	
	m_ct_image = control.image();
	m_ct_acq = control.acquisition();
	m_hw_int = control.hwInterface();

  //  lima library version
	m_ct_parameters.lima_version = control.getVersion();

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

	// Check if the overwrite policy  "MultiSet" is activated	
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
		  } catch (FileIException){		    
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
#ifdef WIN32
		sprintf_s(strname,"/entry_%04d", new_file.m_entry_index);			
#else
		sprintf(strname, "/entry_%04d", new_file.m_entry_index);
#endif
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

		  // Add some Nexus attributes to the file: 
			// - default = entry path  to data
			// - NX_class = Nxroot
			// - creator = LIMA-<version>

		  write_h5_attribute(*new_file.m_file, "default", new_file.m_entry_name);
			string nxroot = "NXroot";
			write_h5_attribute(*new_file.m_file, "NX_class", nxroot);
			string nxcreator = "LIMA-"+ m_ct_parameters.lima_version;			
			write_h5_attribute(*new_file.m_file, "creator", nxcreator);

		  // could be the beamline/instrument name instead
		  Group instrument = Group(new_file.m_entry->createGroup(m_ct_parameters.instrument_name));
			string nxinstrument = "NXinstrument";
		  write_h5_attribute(instrument, "NX_class", nxinstrument);
		  Group measurement = Group(new_file.m_entry->createGroup("measurement"));
			string nxmeasurement = "NXcollection";
		  write_h5_attribute(measurement, "NX_class", nxmeasurement);

		  new_file.m_instrument_detector = new Group(instrument.createGroup(m_ct_parameters.det_name));
		  string nxdetector = "NXdetector";
		  write_h5_attribute(*new_file.m_instrument_detector, "NX_class", nxdetector);
		  
		  new_file.m_instrument_detector_header = 
		    new Group(new_file.m_instrument_detector->createGroup("header"));
		  string nxcollection = "NXcollection";
		  write_h5_attribute(*new_file.m_instrument_detector_header, "NX_class", nxcollection);
		  
			new_file.m_instrument_detector_plot = 
		    new Group(new_file.m_instrument_detector->createGroup("plot"));
			string nxdata = "NXdata";
		  write_h5_attribute(*new_file.m_instrument_detector_plot, "NX_class", nxdata);			
		  
			// save dataset name for final data
			new_file.m_data_name = "data";		  		  	  	
			new_file.m_path_to_data = "/" + new_file.m_entry_name+ "/"+ m_ct_parameters.instrument_name + "/" + m_ct_parameters.det_name + "/"+ new_file.m_data_name;
		  
			// set "default" and "signal" attributes "default" for auto plotting
		  string path_to_nxdata = m_ct_parameters.instrument_name + "/"+ m_ct_parameters.det_name + "/plot";
		  write_h5_attribute(*new_file.m_entry, "default", path_to_nxdata);

		  path_to_nxdata = m_ct_parameters.det_name + "/plot";
		  write_h5_attribute(instrument, "default", path_to_nxdata);
			path_to_nxdata = "plot";
			write_h5_attribute(*new_file.m_instrument_detector, "default", path_to_nxdata);		  			
		  write_h5_attribute(*new_file.m_instrument_detector_plot, "signal", new_file.m_data_name);

			// finally create a soft link into measurement to Nxdata
		  measurement.link(H5L_TYPE_SOFT,  new_file.m_path_to_data, "data");

		  // write the control parameters (detinfo, acq and image)
		  
		  //Det Info
		  Group det_info = Group(new_file.m_instrument_detector->createGroup("detector_information"));
			write_h5_attribute(det_info, "NX_class", nxcollection);
		  write_h5_dataset(det_info,"name",m_ct_parameters.det_name);
		  write_h5_dataset(det_info,"type",m_ct_parameters.det_type);
		  write_h5_dataset(det_info,"model",m_ct_parameters.det_model);
		  Group pixelsize = Group(det_info.createGroup("pixel_size"));
			write_h5_attribute(pixelsize, "NX_class", nxcollection);
		  write_h5_dataset(pixelsize,"xsize",m_ct_parameters.pixel_size[0]);
		  write_h5_dataset(pixelsize,"ysize",m_ct_parameters.pixel_size[1]);
		  Group imagesize = Group(det_info.createGroup("max_image_size"));
		  write_h5_attribute(imagesize, "NX_class", nxcollection);
			int width = m_ct_parameters.max_image_size.getWidth();
		  int height = m_ct_parameters.max_image_size.getHeight();
		  write_h5_dataset(imagesize,"xsize",width);
		  write_h5_dataset(imagesize,"ysize",height);
		  string im_type=lima::convert_2_string(m_ct_parameters.curr_image_type);
		  write_h5_dataset(det_info,"image_lima_type",im_type);
		  // Acquisition
		  Group acq = Group(new_file.m_instrument_detector->createGroup("acquisition"));
		  write_h5_attribute(acq, "NX_class", nxcollection);
			string acq_mode = lima::convert_2_string(m_ct_parameters.acq_mode);
		  write_h5_dataset(acq,"mode",acq_mode);
		  write_h5_dataset(acq,"exposure_time",m_ct_parameters.acq_expo_time);
		  write_h5_dataset(acq,"latency_time",m_ct_parameters.acq_latency_time);
		  write_h5_dataset(acq,"nb_frames",m_ct_parameters.acq_nbframes);
		  string trig_mode = lima::convert_2_string(m_ct_parameters.acq_trigger_mode);
		  write_h5_dataset(acq,"trigger_mode",trig_mode);
		  if (m_ct_parameters.acq_mode == Accumulation) {
		    Group acc = Group(acq.createGroup("accumulation"));
				write_h5_attribute(acc, "NX_class", nxcollection);
		    string time_mode = lima::convert_2_string(m_ct_parameters.acc_time_mode);
		    write_h5_dataset(acc,"time_mode",time_mode);
		    write_h5_dataset(acc,"max_exposure_time",m_ct_parameters.acc_max_expotime);
		    write_h5_dataset(acc,"exposure_time",m_ct_parameters.acc_expotime);
		    write_h5_dataset(acc,"live_time",m_ct_parameters.acc_livetime);
		    write_h5_dataset(acc,"dead_time",m_ct_parameters.acc_deadtime);
		  }
		  if (m_ct_parameters.acq_mode == Concatenation) {
		    Group concat = Group(acq.createGroup("concatenation"));
				write_h5_attribute(concat, "NX_class", nxcollection);
		    write_h5_dataset(concat,"nb_frames",m_ct_parameters.concat_nbframes);
		  }
		  // Image
		  Group image = Group(new_file.m_instrument_detector->createGroup("image_operation"));
		  write_h5_attribute(image, "NX_class", nxcollection);
			Group dim = Group(image.createGroup("dimension"));
			write_h5_attribute(dim, "NX_class", nxcollection);
		  int xsize = m_ct_parameters.image_dim.getSize().getWidth();
		  int ysize = m_ct_parameters.image_dim.getSize().getHeight();
		  write_h5_dataset(dim,"xsize",xsize);				  
		  write_h5_dataset(dim,"ysize",ysize);				  
		  Group bin = Group(image.createGroup("binning"));
			write_h5_attribute(bin, "NX_class", nxcollection);
		  int xbin = m_ct_parameters.image_bin.getX();
		  int ybin = m_ct_parameters.image_bin.getY();
		  write_h5_dataset(bin,"x",xbin);				  
		  write_h5_dataset(bin,"y",ybin);				  				
		  Group roi = Group(image.createGroup("region_of_interest"));
		  write_h5_attribute(roi, "NX_class", nxcollection);
			Point roip = m_ct_parameters.image_roi.getTopLeft();
		  Size roiz = m_ct_parameters.image_roi.getSize();
		  xsize = roiz.getWidth();
		  ysize = roiz.getHeight();
		  write_h5_dataset(roi,"xstart",roip.x);
		  write_h5_dataset(roi,"ystart",roip.y);
		  write_h5_dataset(roi,"xsize",xsize);
		  write_h5_dataset(roi,"ysize",ysize);
		  Group flipping = Group(image.createGroup("flipping"));
			write_h5_attribute(flipping, "NX_class", nxcollection);
		  write_h5_dataset(flipping,"x",m_ct_parameters.image_flip.x);
		  write_h5_dataset(flipping,"y",m_ct_parameters.image_flip.y);
		  string rot = lima::convert_2_string(m_ct_parameters.image_rotation);
		  write_h5_dataset(image, "rotation", rot);					
		} else {
		  new_file.m_entry = new Group(new_file.m_file->openGroup(strname));
		  Group instrument = Group(new_file.m_entry->openGroup(m_ct_parameters.instrument_name));
		  new_file.m_instrument_detector = new Group(instrument.openGroup(m_ct_parameters.det_name));
		}
	} catch (FileIException &error) {
		  error.printErrorStack();
	      THROW_CTL_ERROR(Error) << "File " << filename << " not opened successfully";
	}

	return new _File(new_file);
}


void SaveContainerHdf5::_close(void* f) {
	DEB_MEMBER_FUNCT();

	_File *file = (_File*)f;
	if (!file->m_in_append || m_is_multiset) {
		// Finally create in the instrument group a link to the instrument detector data
		file->m_instrument_detector_plot->link(H5L_TYPE_SOFT, file->m_path_to_data , "data");
		
		// ISO 8601 Time format
		time_t now;
		time(&now);
		char buf[sizeof("2011-10-08T07:07:09Z")];
#ifdef WIN32
		struct tm gmtime_now;
		gmtime_s(&gmtime_now, &now);
		strftime(buf, sizeof(buf), "%FT%TZ", &gmtime_now);
#else
		strftime(buf, sizeof(buf), "%FT%TZ", gmtime(&now));
#endif
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
		size_t buf_size = 0;
		
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
#ifdef WIN32
				struct tm gmtime_now;
				gmtime_s(&gmtime_now, &now);
				strftime(buf, sizeof(buf), "%FT%TZ", &gmtime_now);
#else
				strftime(buf, sizeof(buf), "%FT%TZ", gmtime(&now));
#endif
				string stime = string(buf);
				write_h5_dataset(*file->m_entry,"start_time",stime);
				// write header only once into "header" group 				
				if (!aHeader.empty()) {
					for (CtSaving::HeaderMap::const_iterator it = aHeader.begin(); it != aHeader.end(); it++) {

						string key = it->first;
						string value = it->second;
						write_h5_dataset(*file->m_instrument_detector_header,
								 key.c_str(),value);
					}
				}
				delete file->m_instrument_detector_header;
				file->m_instrument_detector_header = NULL;
					
				// create the image data structure in the file
				hsize_t data_dims[3];
				data_dims[1] = aData.dimensions[1];
				data_dims[2] = aData.dimensions[0];
				data_dims[0] = m_nbframes;
				// Create property list for the dataset and setup chunk size
				DSetCreatPropList plist;
				hsize_t chunk_dims[RANK_THREE];
				// test direct chunk write, so chunk dims is 1 image size
				chunk_dims[0] = 1; chunk_dims[1] = data_dims[1]; chunk_dims[2] = data_dims[2];
				
				plist.setChunk(RANK_THREE, chunk_dims);

#if defined(WITH_Z_COMPRESSION)
				if (aFormat == CtSaving::HDF5GZ)
				  plist.setDeflate(m_compression_level);
#endif
#if defined(WITH_BS_COMPRESSION)
				if (aFormat == CtSaving::HDF5BS) {
				  unsigned int opt_vals[2]= {0, BSHUF_H5_COMPRESS_LZ4};
				  plist.setFilter(BSHUF_H5FILTER, H5Z_FLAG_MANDATORY, 2, opt_vals);
				}
#endif
				// create new dspace
				file->m_image_dataspace = new DataSpace(RANK_THREE, data_dims, NULL);
				file->m_image_dataset = 
				  new DataSet(file->m_instrument_detector->createDataSet(file->m_data_name,
											  data_type,
											  *file->m_image_dataspace,
											  plist));
				string image = "image";
				write_h5_attribute(*file->m_image_dataset, "interpretation", image);
				file->m_prev_images_written = 0;
				file->m_format_written = true;
			} else if (file->m_in_append && !m_is_multiset && !file->m_dataset_extended) {
				hsize_t allocated_dims[3];
				file->m_image_dataset = new DataSet(file->m_instrument_detector->
								    openDataSet(file->m_data_name));
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
			hsize_t image_nb = aData.frameNumber % m_nbframes;

			// we test direct chunk write
			hsize_t offset[RANK_THREE] = {image_nb, 0U, 0U};
			uint32_t filter_mask = 0; 
			hid_t dataset = file->m_image_dataset->getId();
			herr_t  status;
			void * buf_data;
			hid_t dxpl;

			dxpl = H5Pcreate(H5P_DATASET_XFER);

			if ((aFormat == CtSaving::HDF5GZ) || (aFormat == CtSaving::HDF5BS))
			  {
			    ZBufferList buffers = _takeBuffers(aData.frameNumber);
			    // with single chunk, only one buffer allocated
			    ZBuffer& b = buffers.front();
			    buf_size = b.used_size;
			    buf_data = b.ptr();
			    //DEB_ALWAYS() << "Image #"<< aData.frameNumber << " buf_size = "<< buf_size;
			    status = H5DOwrite_chunk(dataset, dxpl , filter_mask,  offset, buf_size, buf_data);			
			    if (status<0) {
			      THROW_CTL_ERROR(Error) << "H5DOwrite_chunk() failed";
			    }
			  }
			 else
			   {
			    buf_data = aData.data();
			    buf_size = aData.size();
			    //DEB_ALWAYS() << "Image #"<< aData.frameNumber << " buf_size = "<< buf_size;
			    status = H5DOwrite_chunk(dataset, dxpl , filter_mask,  offset, buf_size, buf_data);			
			    if (status<0) {
			      THROW_CTL_ERROR(Error) << "H5DOwrite_chunk() failed";
			    }

			  } // else
		// catch failure caused by the DataSet operations
		}catch (DataSetIException& error) {
			THROW_CTL_ERROR(Error) << "DataSet not created successfully " << error.getCDetailMsg();
			error.printErrorStack();
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

		DEB_RETURN();
		return buf_size;
}

int SaveContainerHdf5::findLastEntry(const _File &file) {
	char entryName[32];
	int index = -1; 
	try { // determine the next group Entry index
		do {
#ifdef WIN32
			sprintf_s(entryName, "/entry_%04d", index+1);
#else
			sprintf(entryName, "/entry_%04d", index + 1);
#endif
			file.m_file->openGroup(entryName);
			index++;
		} while (1);
	} catch (H5::Exception) {
		// ignore this indicates the last group
	}
	return index;
}

SinkTaskBase* SaveContainerHdf5::getCompressionTask(const CtSaving::HeaderMap& /*header*/)
{
#if defined(WITH_Z_COMPRESSION)
  if(m_format == CtSaving::HDF5GZ)
    {
      m_compression_level = 6;
      return new ImageZCompression(*this, m_compression_level);
    }
#endif
#if defined(WITH_BS_COMPRESSION)
  if(m_format == CtSaving::HDF5BS)
    {
      return new ImageBsCompression(*this);
    }
#endif
    
  return NULL;
}
