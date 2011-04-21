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

#include "CtSaving_Edf.h"

using namespace lima;

/** @brief saving container
 *
 *  This class manage file saving
 */
SaveContainerEdf::SaveContainerEdf(CtSaving &aCtSaving) :
  CtSaving::SaveContainer(aCtSaving)
{
  DEB_CONSTRUCTOR();
}

SaveContainerEdf::~SaveContainerEdf()
{
  DEB_DESTRUCTOR();
}

bool SaveContainerEdf::_open(const std::string &filename,
			     std::ios_base::openmode openFlags)
{
  DEB_MEMBER_FUNCT();
  m_fout.clear();
  m_fout.exceptions(std::ios_base::failbit | std::ios_base::badbit);
  m_fout.open(filename.c_str(),openFlags);
  return true;
}

void SaveContainerEdf::_close()
{
  DEB_MEMBER_FUNCT();
  
  if (!m_fout.is_open()) {
    DEB_TRACE() << "Nothing to do";
    return;
  }

  DEB_TRACE() << "Close current file";

  m_fout.close();
}

void SaveContainerEdf::_writeFile(Data &aData,
				  CtSaving::HeaderMap &aHeader,
				  CtSaving::FileFormat aFormat)
{
  if(aFormat == CtSaving::EDF)
    _writeEdfHeader(aData,aHeader);
  
  m_fout.write((char*)aData.data(),aData.size());
}

void SaveContainerEdf::_writeEdfHeader(Data &aData,CtSaving::HeaderMap &aHeader)
{
  DEB_MEMBER_FUNCT();

  time_t ctime_now;
  time(&ctime_now);

  struct timeval tod_now;
  gettimeofday(&tod_now, NULL);

  char time_str[64];
  ctime_r(&ctime_now, time_str);
  time_str[strlen(time_str) - 1] = '\0';
	
  int image_nb = m_written_frames + 1;

  char aBuffer[2048];
  long aStartPosition = m_fout.tellp();
  m_fout << "{\n";

  snprintf(aBuffer,sizeof(aBuffer),"HeaderID = EH:%06u:000000:000000 ;\n", image_nb);
  m_fout << aBuffer;

  m_fout << "ByteOrder = LowByteFirst ;\n";
  const char *aStringType = NULL;
  switch(aData.type)
    {
    case Data::UINT8:	aStringType = "UnsignedByte";break;
    case Data::INT8:	aStringType = "SignedByte";break;
    case Data::UINT16:	aStringType = "UnsignedShort";break;
    case Data::INT16:	aStringType = "SignedShort";break;
    case Data::UINT32:	aStringType = "UnsignedInteger";break;
    case Data::INT32:	aStringType = "SignedInteger";break;
    case Data::UINT64:	aStringType = "Unsigned64";break;
    case Data::INT64:	aStringType = "Signed64";break;
    case Data::FLOAT:	aStringType = "FloatValue";break;
    case Data::DOUBLE:	aStringType = "DoubleValue";break;
    default:
      break;		// @todo ERROR has to be manage
    }
  m_fout << "DataType = " << aStringType << " ;\n";

  m_fout << "Size = " << aData.size() << " ;\n";
  m_fout << "Dim_1 = " << aData.dimensions[0] << " ;\n";
  m_fout << "Dim_2 = " << aData.dimensions[1] << " ;\n";
  m_fout << "Image = " << m_written_frames << " ;\n";

  m_fout << "acq_frame_nb = " << aData.frameNumber << " ;\n";
  m_fout << "time = " << time_str << " ;\n";

  snprintf(aBuffer,sizeof(aBuffer),"time_of_day = %ld.%06ld ;\n",tod_now.tv_sec, tod_now.tv_usec);
  m_fout << aBuffer;

  snprintf(aBuffer,sizeof(aBuffer),"time_of_frame = %.6f ;\n",aData.timestamp);
  m_fout << aBuffer;

  //@todo m_fout << "valid_pixels = " << aData.validPixels << " ;\n";
  
  
  aData.header.lock();
  Data::HeaderContainer::Header &aDataHeader = aData.header.header();
  for(Data::HeaderContainer::Header::iterator i = aDataHeader.begin();i != aDataHeader.end();++i)
    m_fout << i->first << " = " << i->second << " ;\n";
  aData.header.unlock();

  for(CtSaving::HeaderMap::iterator i = aHeader.begin(); i != aHeader.end();++i)
    m_fout << i->first << " = " << i->second << " ;\n";
  
  long aEndPosition = m_fout.tellp();
  
  long lenght = aEndPosition - aStartPosition + 2;
  long finalHeaderLenght = (lenght + 1023) & ~1023; // 1024 alignment
  snprintf(aBuffer,sizeof(aBuffer),"%*s}\n",int(finalHeaderLenght - lenght),"");
  m_fout << aBuffer;
}
