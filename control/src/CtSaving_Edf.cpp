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
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
static const long int WRITE_BUFFER_SIZE = 64*1024;
#else
#include <time_compat.h>
#endif

#include "CtSaving_Edf.h"

using namespace lima;

#ifdef WITH_EDFGZ_SAVING
#include <zlib.h>
#include "processlib/SinkTask.h"

#define TEST_AVAIL_OUT	if(!m_compression_struct.avail_out) \
    {							    \
      _BufferHelper *newBuffer = new _BufferHelper();			\
      m_compression_struct.next_out = (Bytef*)newBuffer->buffer;	\
      m_compression_struct.avail_out = newBuffer->BUFFER_HELPER_SIZE;	\
      return_buffers->push_back(newBuffer);				\
    }

class SaveContainerEdf::Compression : public SinkTaskBase
{
  DEB_CLASS_NAMESPC(DebModControl,"Compression Task","Control");

  SaveContainerEdf& 	m_container;
  int 			m_frame_per_file;
  CtSaving::HeaderMap 	m_header;

  z_stream_s		m_compression_struct;
public:
  Compression(SaveContainerEdf &save_cnt,
	      int framesPerFile,const CtSaving::HeaderMap &header) :
    m_container(save_cnt),m_frame_per_file(framesPerFile),m_header(header)
  {
    DEB_CONSTRUCTOR();

    m_compression_struct.next_in = NULL;
    m_compression_struct.avail_in = 0;
    m_compression_struct.total_in = 0;

    m_compression_struct.next_out = NULL;
    m_compression_struct.avail_out = 0;
    m_compression_struct.total_out = 0;

    m_compression_struct.zalloc = NULL;
    m_compression_struct.zfree = NULL;

    if(deflateInit2(&m_compression_struct,Z_DEFAULT_COMPRESSION,
		    Z_DEFLATED,
		    31,
		    8,
		    Z_DEFAULT_STRATEGY) != Z_OK)
      THROW_CTL_ERROR(Error) << "Can't init compression struct";
  };
  ~Compression()
  {
    deflateEnd(&m_compression_struct);
  }

  virtual void process(Data &aData)
  {
    std::ostringstream buffer;
    SaveContainerEdf::_writeEdfHeader(aData,m_header,
 				      m_frame_per_file,
 				      buffer);
    ZBufferType *aBufferListPt = new ZBufferType();
    const std::string& tmpBuffer = buffer.str();
    try
      {
	_compression(tmpBuffer.c_str(),tmpBuffer.size(),aBufferListPt);
	_compression((char*)aData.data(),aData.size(),aBufferListPt);
	_end_compression(aBufferListPt);
      }
    catch(Exception&)
      {
	for(ZBufferType::iterator i = aBufferListPt->begin();
	    i != aBufferListPt->end();++i)
	  delete *i;
	delete aBufferListPt;
	throw;
      }
    m_container._setBuffer(aData.frameNumber,aBufferListPt);
  }

  void _compression(const char *buffer,int size,ZBufferType* return_buffers)
  {
    DEB_MEMBER_FUNCT();

    m_compression_struct.next_in = (Bytef*)buffer;
    m_compression_struct.avail_in = size;
    
    while(m_compression_struct.avail_in)
      {
	TEST_AVAIL_OUT;
	if(deflate(&m_compression_struct,Z_NO_FLUSH) != Z_OK)
	  THROW_CTL_ERROR(Error) << "deflate error";

	return_buffers->back()->used_size = _BufferHelper::BUFFER_HELPER_SIZE -
	  m_compression_struct.avail_out;
      }
  }
  void _end_compression(ZBufferType* return_buffers)
  {
    DEB_MEMBER_FUNCT();

    int deflate_res = Z_OK;
    while(deflate_res == Z_OK)
      {
	TEST_AVAIL_OUT;
	deflate_res = deflate(&m_compression_struct,Z_FINISH);
	return_buffers->back()->used_size = _BufferHelper::BUFFER_HELPER_SIZE - 
	  m_compression_struct.avail_out;
      }
    if(deflate_res != Z_STREAM_END)
      THROW_CTL_ERROR(Error) << "deflate error";
  }
};
#endif



#ifdef WIN32
/** @brief this is a small wrapper class for ofstream class.
 *  All this is for overcome performance issue with window std::ofstream
 */
SaveContainerEdf::_OfStream::_OfStream() : 
    m_fout(NULL),
    m_exc_flag(0)
  {
  }
SaveContainerEdf::_OfStream::~_OfStream()
  {
    if(is_open())
      close();
  }

void SaveContainerEdf::_OfStream::clear() {}
void SaveContainerEdf::_OfStream::exceptions(int exc) {m_exc_flag = exc;}
void SaveContainerEdf::_OfStream::open(const char* filename,
				       std::ios_base::openmode openFlags)
{
  const char *mode = openFlags & std::ios_base::app ? "ab" : "wb";
  fopen_s(&m_fout,filename,mode);
  if(!m_fout && (m_exc_flag & std::ios_base::failbit))
    {
      std::string err = "Failed to open : ";
      err += filename;
      throw std::ios_base::failure(err);
    }
}
bool SaveContainerEdf::_OfStream::is_open() const {return !!m_fout;}
void SaveContainerEdf::_OfStream::close()
{
  fclose(m_fout);
}
__int64 SaveContainerEdf::_OfStream::tellp() const
{
  return _ftelli64(m_fout);
}

SaveContainerEdf::_OfStream& 
SaveContainerEdf::_OfStream::write(const char* data,int size)
{
  int nbItemsWritten = fwrite(data,size,1,m_fout);
  if(nbItemsWritten <= 0 &&
     (m_exc_flag & std::ios_base::badbit))
    throw std::ios_base::failure("Failed to write");
  return *this;
}

SaveContainerEdf::_OfStream& 
SaveContainerEdf::_OfStream::operator<< (const char *data)
{
  return write(data,strlen(data));
}

SaveContainerEdf::_OfStream& 
SaveContainerEdf::_OfStream::operator<< (const std::string& data)
{
  return write(data.data(),data.size());
}

SaveContainerEdf::_OfStream& 
SaveContainerEdf::_OfStream::operator<< (const int data)
{
  char aBuffer[32];
  snprintf(aBuffer,sizeof(aBuffer),"%d",data);
  return write(aBuffer,strlen(aBuffer));
}

SaveContainerEdf::_OfStream& 
SaveContainerEdf::_OfStream::operator<< (const long data)
{
  char aBuffer[32];
  snprintf(aBuffer,sizeof(aBuffer),"%ld",data);
  return write(aBuffer,strlen(aBuffer));
}
#endif

/** @brief saving container
 *
 *  This class manage file saving
 */
SaveContainerEdf::SaveContainerEdf(CtSaving::Stream& stream,
				   CtSaving::FileFormat format) :
  CtSaving::SaveContainer(stream),
  m_format(format)
{
  DEB_CONSTRUCTOR();
#ifdef __unix
  if(posix_memalign(&m_fout_buffer,4*1024,WRITE_BUFFER_SIZE))
    THROW_CTL_ERROR(Error) << "Can't allocated write buffer";
#endif
}

SaveContainerEdf::~SaveContainerEdf()
{
  DEB_DESTRUCTOR();
#ifdef __unix
  free(m_fout_buffer);
#endif
}

bool SaveContainerEdf::_open(const std::string &filename,
			     std::ios_base::openmode openFlags)
{
  DEB_MEMBER_FUNCT();
  m_fout.clear();
  m_fout.exceptions(std::ios_base::failbit | std::ios_base::badbit);
  m_fout.open(filename.c_str(),openFlags);
#ifdef __unix
  m_fout.rdbuf()->pubsetbuf((char*)m_fout_buffer,WRITE_BUFFER_SIZE);
#endif
  m_current_filename = filename;
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
#ifdef __unix
  if(m_mmap_info.mmap_addr)
    {
      munmap(m_mmap_info.mmap_addr,m_mmap_info.header_size);
      m_mmap_info.mmap_addr = NULL;
    }
#endif
}

void SaveContainerEdf::_writeFile(Data &aData,
				  CtSaving::HeaderMap &aHeader,
				  CtSaving::FileFormat aFormat)
{
#ifdef WITH_EDFGZ_SAVING
  if(aFormat == CtSaving::EDFGZ)
    {
      ZBufferType* buffers = _takeBuffer(aData.frameNumber);
      for(ZBufferType::iterator i = buffers->begin();
	  i != buffers->end();++i)
	{
	  m_fout.write((*i)->buffer,(*i)->used_size);
	  delete *i;
	}
      delete buffers;
    }
  else
    {
#endif

  if(aFormat == CtSaving::EDF)
    {
      const CtSaving::Parameters& pars = m_stream.getParameters(CtSaving::Acq);
      _writeEdfHeader(aData,aHeader,
		      pars.framesPerFile,m_fout);
    }
#ifdef __unix
  else if(aFormat == CtSaving::EDFConcat)
    {
      m_mmap_info.height += aData.dimensions[1];
      m_mmap_info.size += aData.size();
      if(!m_mmap_info.mmap_addr)	// Create header and mmap
	{
	  const CtSaving::Parameters& pars = m_stream.getParameters(CtSaving::Acq);
	  m_mmap_info = _writeEdfHeader(aData,aHeader,pars.framesPerFile,m_fout,8);
	  m_fout.flush();
	  long long header_position = m_fout.tellp();
	  header_position -= m_mmap_info.header_size;
          long sz = sysconf(_SC_PAGESIZE);
	  long long mapping_offset = header_position / sz * sz;
	  m_mmap_info.header_size += header_position - mapping_offset;
	  m_mmap_info.height_offset -= mapping_offset;
	  m_mmap_info.size_offset -= mapping_offset;
	  int fd = ::open(m_current_filename.c_str(),O_RDWR);
	  if(fd > -1)
	    {
	      m_mmap_info.mmap_addr = mmap(NULL,m_mmap_info.header_size,
					   PROT_WRITE,MAP_SHARED,fd,mapping_offset);
	      ::close(fd);
	    }
	  m_mmap_info.height = aData.dimensions[1];
	  m_mmap_info.size = aData.size();
	}
      else
	{
	  char* start_size_string = (char*)m_mmap_info.mmap_addr + m_mmap_info.size_offset;
	  int nbchar = sprintf(start_size_string,"%lld",m_mmap_info.size);
	  start_size_string[nbchar] = ' ';

	  char* start_height_string = (char*)m_mmap_info.mmap_addr + m_mmap_info.height_offset;
	  nbchar = sprintf(start_height_string,"%lld",m_mmap_info.height);
	  start_height_string[nbchar] = ' ';
	}
    }
#endif
  m_fout.write((char*)aData.data(),aData.size());


#ifdef WITH_EDFGZ_SAVING
    } // else
#endif
}

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

SinkTaskBase* SaveContainerEdf::getCompressionTask(const CtSaving::HeaderMap& header)
{
#ifdef WITH_EDFGZ_SAVING
  const CtSaving::Parameters& pars = m_stream.getParameters(CtSaving::Acq);
  return new Compression(*this,pars.framesPerFile,header);
#else
  return NULL;
#endif
}

void SaveContainerEdf::_setBuffer(int frameNumber,
				  ZBufferType* buffers)
{
  AutoMutex aLock(m_lock);
  std::pair<dataId2ZBufferType::iterator,bool> result = 
    m_buffers.insert(std::pair<int,ZBufferType*>(frameNumber,buffers));
  if(!result.second)
    {
      for(ZBufferType::iterator i = result.first->second->begin();
	  i != result.first->second->end();++i)
	delete *i;
      delete result.first->second;
      result.first->second = buffers;
    }
}

SaveContainerEdf::ZBufferType* SaveContainerEdf::_takeBuffer(int dataId)
{
  AutoMutex aLock(m_lock);
  dataId2ZBufferType::iterator i = m_buffers.find(dataId);
  ZBufferType* aReturnBufferPt = i->second;
  m_buffers.erase(i);
  return aReturnBufferPt;
}

void SaveContainerEdf::_clear()
{
  AutoMutex aLock(m_lock);
  for(dataId2ZBufferType::iterator i = m_buffers.begin();
      i != m_buffers.end();++i)
    {
      for(ZBufferType::iterator k = i->second->begin();
	  k != i->second->end();++k)
	delete *k;
      delete i->second;
    }
  m_buffers.clear();
}
