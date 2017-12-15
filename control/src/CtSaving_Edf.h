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
#ifndef CTSAVING_EDF_H
#define CTSAVING_EDF_H

#include "lima/CtSaving.h"
#include "lima/CtSaving_Compression.h"

namespace lima {

  class SaveContainerEdf : public CtSaving::SaveContainer
  {
    DEB_CLASS_NAMESPC(DebModControl,"Saving EDF Container","Control");
    friend class ZCompression;
    friend class Lz4Compression;
  public:

    SaveContainerEdf(CtSaving::Stream& stream,
		     CtSaving::FileFormat format);
    virtual ~SaveContainerEdf();
    
    virtual bool needParallelCompression() const 
    {return m_format == CtSaving::EDFGZ || m_format == CtSaving::EDFLZ4;}
    virtual SinkTaskBase* getCompressionTask(const CtSaving::HeaderMap&);

  protected:
    virtual void* _open(const std::string &filename,
			std::ios_base::openmode flags);
    virtual void _close(void*);
    virtual long _writeFile(void*,Data &data,
			    CtSaving::HeaderMap &aHeader,
			    CtSaving::FileFormat);
   private:
     struct MmapInfo
    {
      MmapInfo() :
	header_size(0),
	height_offset(0),
	size_offset(0),
	height(0),
	size(0),
	mmap_addr(NULL) {}
      long long header_size;
      long long height_offset;
      long long size_offset;
      long long height;
      long long size;
      void* mmap_addr;
    };
    template<class Stream>
      static MmapInfo _writeEdfHeader(Data&,CtSaving::HeaderMap&,
				      int framesPerFile,Stream&,
				      int nbCharReserved = 0);
#ifdef WIN32
    class _OfStream
    {
    public:
      _OfStream();
      ~_OfStream();

      void clear();
      void exceptions(int exc);
      void open(const char* filename,
		std::ios_base::openmode openFlags = 
		std::ios_base::in | std::ios_base::out);
      bool is_open() const;
      void close();
      _int64 tellp() const;
      inline _OfStream& write(const char* data,size_t size);
      inline _OfStream& operator<< (const char *data);
      inline _OfStream& operator<< (const std::string& data);
      inline _OfStream& operator<< (const int data);
      inline _OfStream& operator<< (const long data);
    private:
      FILE* m_fout;
      int m_exc_flag;
    };
#else
    void*                        m_fout_buffer;
#endif
    CtSaving::FileFormat	 m_format;
    MmapInfo			 m_mmap_info;
    std::string			 m_current_filename;
  };

  template<class Stream>
    SaveContainerEdf::MmapInfo
    SaveContainerEdf::_writeEdfHeader(Data &aData,
				      CtSaving::HeaderMap &aHeader,
				      int framesPerFile,
				      Stream &sout,
				      int nbCharReserved)
    {
      time_t ctime_now;
      time(&ctime_now);
      
      struct timeval tod_now;
      gettimeofday(&tod_now, NULL);
      
      char time_str[64];
      ctime_r(&ctime_now, time_str);
      time_str[strlen(time_str) - 1] = '\0';
      
      int image_nb = aData.frameNumber % framesPerFile;
      
      char aBuffer[2048];
      long long aStartPosition = sout.tellp();
      sout << "{\n";
      
      snprintf(aBuffer,sizeof(aBuffer),"HeaderID = EH:%06u:000000:000000 ;\n", image_nb + 1);
      sout << aBuffer;
      
      sout << "ByteOrder = LowByteFirst ;\n";
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
      sout << "DataType = " << aStringType << " ;\n";
      
      SaveContainerEdf::MmapInfo offset;
      sout << "Size = "; offset.size_offset = sout.tellp();
      snprintf(aBuffer,sizeof(aBuffer),"%*s ;\n",nbCharReserved,"");
      sout << aData.size() << aBuffer;
      
      sout << "Dim_1 = " << aData.dimensions[0] << " ;\n";
      
      sout << "Dim_2 = "; offset.height_offset = sout.tellp();
      snprintf(aBuffer,sizeof(aBuffer),"%*s ;\n",nbCharReserved,"");
      sout << aData.dimensions[1] << aBuffer;
      
      sout << "Image = " << image_nb << " ;\n";
      
      sout << "acq_frame_nb = " << aData.frameNumber << " ;\n";
      sout << "time = " << time_str << " ;\n";
      
      snprintf(aBuffer,sizeof(aBuffer),"time_of_day = %ld.%06ld ;\n",tod_now.tv_sec, tod_now.tv_usec);
      sout << aBuffer;
      
      snprintf(aBuffer,sizeof(aBuffer),"time_of_frame = %.6f ;\n",aData.timestamp);
      sout << aBuffer;
      
      //@todo sout << "valid_pixels = " << aData.validPixels << " ;\n";
      
      
      aData.header.lock();
      Data::HeaderContainer::Header &aDataHeader = aData.header.header();
      for(Data::HeaderContainer::Header::iterator i = aDataHeader.begin();i != aDataHeader.end();++i)
	{
	  if(!i->second.size())
	    sout << i->first << " = " << ";\n";
	  else
	    sout << i->first << " = " << i->second << " ;\n";
	}
      aData.header.unlock();
      
      for(CtSaving::HeaderMap::iterator i = aHeader.begin(); i != aHeader.end();++i)
	{
	  if(!i->second.size())
	    sout << i->first << " = " << ";\n";
	  else
	    sout << i->first << " = " << i->second << " ;\n";
	}
      
      
      long long aEndPosition = sout.tellp();
      
      long long lenght = aEndPosition - aStartPosition + 2;
      long long finalHeaderLenght = (lenght + 1023) & ~1023; // 1024 alignment
      snprintf(aBuffer,sizeof(aBuffer),"%*s}\n",int(finalHeaderLenght - lenght),"");
      sout << aBuffer;
      offset.header_size = finalHeaderLenght;
      return offset;
    }

}
#endif // CTSAVING_EDF_H
