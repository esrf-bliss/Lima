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

#ifdef __unix
#include <sys/time.h>
#else
#include <time_compat.h>
#endif

#include <iostream>
#include <memory>
#include <numeric>
#include <functional>
#include <sstream>


#include "CtSaving_Tiff.h"

#include <iostream>

using namespace std;

using namespace lima;

/** @brief saving container
 *
 *  This class manage file saving
 */
SaveContainerTiff::SaveContainerTiff(CtSaving::Stream& stream) :
  CtSaving::SaveContainer(stream)
{
  DEB_CONSTRUCTOR();
}

SaveContainerTiff::~SaveContainerTiff()
{
  DEB_DESTRUCTOR();
}

void* SaveContainerTiff::_open(const std::string &filename,
		std::ios_base::openmode flags)
{
  DEB_MEMBER_FUNCT();
  return new std::string(filename);
}

void SaveContainerTiff::_close(void* f)
{
  DEB_MEMBER_FUNCT();
  std::string* filename = (std::string*)f;
  delete filename;
}

void SaveContainerTiff::_writeFile(void* f,Data &aData,
				  CtSaving::HeaderMap &aHeader,
				  CtSaving::FileFormat aFormat)
{
    DEB_MEMBER_FUNCT();

    TIFF *image;
    std::string* filename = (std::string*)f;
    if((image = TIFFOpen(filename->c_str(), "w")) == NULL)
      {
	DEB_TRACE()<<"SaveContainerTiff::_writeFile() - not able to open tiff file";
	throw LIMA_CTL_EXC(Error,"SaveContainerTiff::_writeFile() - not able to open tiff file");
      }
    
    /* If additional info wants to be written */
  
    char add_info[1000] = "\0";
    sprintf(add_info,"Frame number %d", aData.frameNumber);
    
    TIFFSetField(image, TIFFTAG_IMAGEWIDTH, aData.dimensions[0]);
    TIFFSetField(image, TIFFTAG_IMAGELENGTH, aData.dimensions[1]);

    int sampleformat;
    int bytespersample;
 
    switch(aData.type)
      {
      case Data::UINT8:
	bytespersample = sizeof(uint8);
	sampleformat = SAMPLEFORMAT_UINT;
	break;
      case Data::INT8: 
	bytespersample = sizeof(int8);
	sampleformat = SAMPLEFORMAT_INT;
	break;
      case Data::UINT16:
	bytespersample = sizeof(uint16);
	sampleformat = SAMPLEFORMAT_UINT;
	break;
      case Data::INT16:	
	bytespersample = sizeof(int16);
	sampleformat = SAMPLEFORMAT_INT;
	break;
      case Data::UINT32:
	bytespersample = sizeof(uint32);
	sampleformat = SAMPLEFORMAT_UINT;
	break;
      case Data::INT32:	
	bytespersample = sizeof(int32);
	sampleformat = SAMPLEFORMAT_INT;
	break;
      case Data::UINT64:
	bytespersample = 8;
	sampleformat = SAMPLEFORMAT_UINT;
	break;
      case Data::INT64:	
	bytespersample = 8;
	sampleformat = SAMPLEFORMAT_INT;
	break;
      case Data::FLOAT:	
	bytespersample = sizeof(float);
	sampleformat = SAMPLEFORMAT_IEEEFP;
	break;
      case Data::DOUBLE: 
	bytespersample = sizeof(double);
	sampleformat = SAMPLEFORMAT_IEEEFP;
	break;
      default:
	break;		// @todo ERROR has to be manage
      }


 
    TIFFSetField(image, TIFFTAG_BITSPERSAMPLE, bytespersample * 8);
    TIFFSetField(image, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(image, TIFFTAG_SAMPLEFORMAT,  sampleformat);
    TIFFSetField(image, TIFFTAG_ROWSPERSTRIP, aData.dimensions[1]);
    TIFFSetField(image, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
    TIFFSetField(image, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(image, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);    // set the origin of the image.
    TIFFSetField(image, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(image, TIFFTAG_SOFTWARE, "Lima image");  
    TIFFSetField(image, TIFFTAG_IMAGEDESCRIPTION, add_info);


    // Write the information to the file

    long w_size = TIFFWriteEncodedStrip(image, 0, (unsigned char*)aData.data(), (aData.dimensions[0]* aData.dimensions[1] * bytespersample));
 
    DEB_TRACE() <<  "Bytes written to tif file : " << w_size;

    // Close the file
    TIFFClose(image);

}
















