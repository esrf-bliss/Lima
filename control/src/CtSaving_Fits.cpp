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

#include <CCfits/CCfits>

#include "CtSaving_Fits.h"

#define WORKING 1

using namespace lima;

/** @brief saving container
 *
 *  This class manage file saving
 */
SaveContainerFits::SaveContainerFits(CtSaving::Stream& stream) :
  CtSaving::SaveContainer(stream)
{
  DEB_CONSTRUCTOR();
}

SaveContainerFits::~SaveContainerFits()
{
  DEB_DESTRUCTOR();
}

bool SaveContainerFits::_open(const std::string &filename,
			     std::ios_base::openmode openFlags)
{
  DEB_MEMBER_FUNCT();
  _filename = "!" + filename + ".fits";
  return true;
}

void SaveContainerFits::_close()
{
  DEB_MEMBER_FUNCT();
}

void SaveContainerFits::_writeFile(Data &aData,
				  CtSaving::HeaderMap &aHeader,
				  CtSaving::FileFormat aFormat)
{
    DEB_MEMBER_FUNCT();


    // init file
    long naxis = aData.dimensions.size()+1; // +1 = frames per files
    long *naxes = new long[naxis];

    for(int i(0); i < naxis; ++i)
        naxes[i] = aData.dimensions[i];


    lima::CtSaving::Parameters param;
    getParameters(param);

    naxes[naxis-1] = param.framesPerFile;


    std::auto_ptr<CCfits::FITS> pFits(NULL);

    try
    {
        // determine bits per pixel
        short bitpix;
        std::string bitpixName;
        switch(aData.type)
        {
        case Data::UINT8:
            bitpix = SBYTE_IMG;
            bitpixName = "SBYTE_IMG";
            break;
        case Data::INT8:
            bitpix = BYTE_IMG;
            bitpixName = "BYTE_IMG";
            break;
        case Data::UINT16:
            bitpix = USHORT_IMG;
            bitpixName = "USHORT_IMG";
            break;
        case Data::INT16:
            bitpix = SHORT_IMG;
            bitpixName = "SHORT_IMG";
            break;
        case Data::UINT32:
            bitpix = ULONG_IMG;
            bitpixName = "ULONG_IMG";
            break;
        case Data::INT32:
            bitpix = LONG_IMG;
            bitpixName = "LONG_IMG";
            break;
        case Data::UINT64:
        case Data::INT64:
            bitpix = LONGLONG_IMG;
            bitpixName = "LONGLONG_IMG";
            break;
        case Data::FLOAT:
            bitpix = FLOAT_IMG;
            bitpixName = "FLOAT_IMG";
            break;
        case Data::DOUBLE:
            bitpix = DOUBLE_IMG;
            bitpixName = "DOUBLE_IMG";
            break;
        default:
          THROW_CTL_ERROR(Error) <<  "Can't determine bitpix";
        }

        DEB_TRACE() << "SaveContainerFits::_writeFile(): used bitpix: " << bitpixName << "(" << bitpix << ")";


        // create fits file
        pFits.reset(new CCfits::FITS(_filename,bitpix,naxis,naxes));
        pFits->setVerboseMode(true);

        // write
        writeHeader(pFits, aHeader);
        writeData(pFits, aData, bitpix);

        DEB_TRACE() << "FITS PHDU: " << pFits->pHDU();

    }catch(CCfits::FITS::CantCreate)
    {
        THROW_CTL_ERROR(Error) <<  "Can't create FITS file";
    }

    delete naxes;
}

void SaveContainerFits::writeHeader(std::auto_ptr<CCfits::FITS> &fitsFile, CtSaving::HeaderMap &header)
{
    DEB_MEMBER_FUNCT();


    for(CtSaving::HeaderMap::iterator it = header.begin();it != header.end();++it)
    {
        std::string keyword = it->first;
        std::transform(keyword.begin(), keyword.end(),
        keyword.begin(), ::toupper);

        fitsFile->pHDU().addKey(keyword,it->second, "Dynamic header information");
    }
}

void SaveContainerFits::writeData(std::auto_ptr<CCfits::FITS> &fitsFile, Data &data, short dataType)
{
    DEB_MEMBER_FUNCT();

    lima::CtSaving::Parameters param;
    getParameters(param);


    // number of pixels: x*y
    unsigned long nPixels = data.dimensions[0] * data.dimensions[1] * param.framesPerFile;

    switch(param.imageType)
    {
    case lima::Bpp8:
        {
            unsigned char *castedData = (unsigned char *)data.data();
            std::valarray<unsigned char> dataArray(nPixels);

            for(int i(0); i < nPixels; ++i)
                dataArray[i] = castedData[i];

            fitsFile->pHDU().write(1,nPixels,dataArray);
        }
        break;
    case lima::Bpp32:
        {
            unsigned int *castedData = (unsigned int *)data.data();
            std::valarray<unsigned int> dataArray(nPixels);

            for(int i(0); i < nPixels; ++i)
                dataArray[i] = castedData[i];

            fitsFile->pHDU().write(1,nPixels,dataArray);
        }
        break;
    default:
        {
            unsigned short *castedData = (unsigned short *)data.data();
            std::valarray<unsigned short> dataArray(nPixels);

            for(int i(0); i < nPixels; ++i)
                dataArray[i] = castedData[i];

            fitsFile->pHDU().write(1,nPixels,dataArray);
        }
    }
}



















