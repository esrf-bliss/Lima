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

bool SaveContainerTiff::_open(const std::string &filename,
		std::ios_base::openmode flags)
{
  DEB_MEMBER_FUNCT();
  _filename = filename;
  return true;
}

void SaveContainerTiff::_close()
{
  DEB_MEMBER_FUNCT();
}

void SaveContainerTiff::_writeFile(Data &aData,
				  CtSaving::HeaderMap &aHeader,
				  CtSaving::FileFormat aFormat)
{
    DEB_MEMBER_FUNCT();

    TIFF *image;

    if((image = TIFFOpen((const char*)_filename.c_str(), "w")) == NULL)
      {
	DEB_TRACE()<<"SaveContainerTiff::_writeFile() - not able to open tiff file";
	throw LIMA_CTL_EXC(Error,"SaveContainerTiff::_writeFile() - not able to open tiff file");
      }
    
    /* If additional info wants to be written */
  
    char add_info[1000] = "\0";
    sprintf(add_info,"Frame number %d", aData.frameNumber);
    
    TIFFSetField(image, TIFFTAG_IMAGEWIDTH, aData.dimensions[0]);
    TIFFSetField(image, TIFFTAG_IMAGELENGTH, aData.dimensions[1]);

    printf("Teresa: _writeFile (tiff) \n");
    printf("Teresa: dim[0] %d \n", aData.dimensions[0]);
    printf("Teresa: dim[1] %d \n", aData.dimensions[1]);
    switch(aData.type)
      {
      case Data::UINT8:
	printf("Teresa: data type uint8\n");
	break;
      case Data::INT8: 
	printf("Teresa: data type int8\n");
	break;
      case Data::UINT16:
	printf("Teresa: data type uint16\n");
	break;
      case Data::INT16:	
	printf("Teresa: data type int16\n");
	break;
      case Data::UINT32:
	printf("Teresa: data type uint32\n");
	break;
      case Data::INT32:	
	printf("Teresa: data type int32\n");
	break;
      case Data::UINT64:
	printf("Teresa: data type uint64\n");
	break;
      case Data::INT64:	
	printf("Teresa: data type int64\n");
	break;
      case Data::FLOAT:	
	printf("Teresa: data type float\n");
	break;
      case Data::DOUBLE: 
	printf("Teresa: data type double\n");
	break;
      default:
	break;		// @todo ERROR has to be manage
      }

    // Tengo que revisar que hacer con el datatype. Afecta a:
    // -> TIFFSetField(image, TIFFTAG_BITSPERSAMPLE, 8);
    // -> TIFFWriteEncodedStrip(image, 0, (unsigned char*)aData.data(), (aData.dimensions[0]* aData.dimensions[1] * 2)); (el factor 2 y el cast a (unsigned char*)

    //     TIFFSetField(image, TIFFTAG_BITSPERSAMPLE, 16);
    //   TIFFSetField(image, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(image, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(image, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(image, TIFFTAG_SAMPLEFORMAT, 3);
    TIFFSetField(image, TIFFTAG_ROWSPERSTRIP, aData.dimensions[1]);
    TIFFSetField(image, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
    TIFFSetField(image, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(image, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);    // set the origin of the image.
    //TIFFSetField(image, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
    TIFFSetField(image, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    //TIFFSetField(image, TIFFTAG_XRESOLUTION, 150.0);
    //TIFFSetField(image, TIFFTAG_YRESOLUTION, 150.0);
    //TIFFSetField(image, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
    TIFFSetField(image, TIFFTAG_SOFTWARE, "Lima image");  
    TIFFSetField(image, TIFFTAG_IMAGEDESCRIPTION, add_info);
    _writeTIFF(image, aData.data(), aData.dimensions[0], aData.dimensions[1], 1, aData.type);


    for (int i = 0; i < 40000; i++){
      if (((uint32*)aData.data())[i] != 0.){
	printf("Teresa:  buf %d %f \n",i, ((uint32*)aData.data())[5000]);
      }
    }

    // Write the information to the file

    //    long w_size = TIFFWriteEncodedStrip(image, 0, (unsigned char*)aData.data(), (aData.dimensions[0]* aData.dimensions[1] * 4));
    
//    long w_size = TIFFWriteEncodedStrip(image, 0, (unsigned char*)aData.data(), (aData.dimensions[0]* aData.dimensions[1] * 4));



//    DEB_TRACE() <<  "Bytes written to tif file : " << w_size;

//    printf("Teresa: Bytes written to tif file %d \n:", w_size);
    // Close the file
    TIFFClose(image);

}


int SaveContainerTiff::_writeTIFF(TIFF * tif, void *data, size_t w, size_t h,
				  size_t c, Data::TYPE datatype)
{
     uint32 rowsperstrip;
     int ok;
     size_t k, i;
     uint32 *line;
 
     TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, (uint32) w);
     TIFFSetField(tif, TIFFTAG_IMAGELENGTH, (uint32) h);
     TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_SEPARATE);
     TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, (uint16) c);
     TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, (uint16) sizeof(uint32) * 8);
     TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_IEEEFP);
     rowsperstrip = TIFFDefaultStripSize(tif, (uint32) h);
     TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
     TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
     TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
 
     ok = 1;
     for (k = 0; ok && k < c; k++)
         for (i = 0; ok && i < h; i++) {
	   line = (uint32 *) ((uint32*)data + (i + k * h) * w);
             if (TIFFWriteScanline(tif, line, (uint32) i, (tsample_t) k) < 0) {
                 fprintf(stderr, "writeTIFF: error writing row %i\n", (int) i);
                 ok = 0;
             }
         }
     return ok;
}














