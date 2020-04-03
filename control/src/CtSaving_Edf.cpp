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
SaveContainerEdf::_OfStream::write(const char* data,size_t size)
{
  size_t nbItemsWritten = fwrite(data,size,1,m_fout);
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

void* SaveContainerEdf::_open(const std::string &filename,
			      std::ios_base::openmode openFlags)
{
  DEB_MEMBER_FUNCT();
#ifdef WIN32
  _OfStream* fout = new _OfStream();
#else
  std::ofstream* fout = new std::ofstream();
#endif
  try
    {
      fout->exceptions(std::ios_base::failbit | std::ios_base::badbit);
      fout->open(filename.c_str(),openFlags);
#ifdef __unix
      fout->rdbuf()->pubsetbuf((char*)m_fout_buffer,WRITE_BUFFER_SIZE);
#endif
    }
  catch(...)
    {
      delete fout;
      throw;
    }

  m_current_filename = filename;
	
  return fout;
}

void SaveContainerEdf::_close(void* f)
{
  DEB_MEMBER_FUNCT();
#ifdef WIN32
  _OfStream* fout = (_OfStream*)f;
#else
  std::ofstream* fout = (std::ofstream*)f;
#endif

  delete fout;			// close file

#ifdef __unix
  if(m_mmap_info.mmap_addr)
    {
      munmap(m_mmap_info.mmap_addr,m_mmap_info.header_size);
      m_mmap_info.mmap_addr = NULL;
    }
#endif
}

long SaveContainerEdf::_writeFile(void* f,Data &aData,
				  CtSaving::HeaderMap &aHeader,
				  CtSaving::FileFormat aFormat)
{
  DEB_MEMBER_FUNCT();
  long write_size = 0;
#ifdef WIN32
  _OfStream* fout = (_OfStream*)f;
#else
  std::ofstream* fout = (std::ofstream*)f;
#endif

#if defined(WITH_Z_COMPRESSION) || defined(WITH_LZ4_COMPRESSION)
  if(aFormat == CtSaving::EDFGZ || aFormat == CtSaving::EDFLZ4)
    {
      ZBufferList buffers = _takeBuffers(aData.frameNumber);
      for(ZBufferList::iterator i = buffers.begin(); i != buffers.end();++i)
	{
	  ZBuffer& b = *i;
	  fout->write((char*)b.ptr(),b.used_size);
	  write_size += b.used_size;
	}
    }
  else
    {
#endif

  if(aFormat == CtSaving::EDF)
    {
      const CtSaving::Parameters& pars = m_stream.getParameters(CtSaving::Acq);
      MmapInfo info = _writeEdfHeader(aData,aHeader,
				      pars.framesPerFile,*fout);
      write_size += info.header_size;
    }
#ifdef __unix
  else if(aFormat == CtSaving::EDFConcat)
    {
      m_mmap_info.height += aData.dimensions[1];
      m_mmap_info.size += aData.size();
      if(!m_mmap_info.mmap_addr)	// Create header and mmap
	{
	  const CtSaving::Parameters& pars = m_stream.getParameters(CtSaving::Acq);
	  m_mmap_info = _writeEdfHeader(aData,aHeader,pars.framesPerFile,*fout,8);
	  write_size += m_mmap_info.header_size;
	  fout->flush();
	  long long header_position = fout->tellp();
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
  fout->write((char*)aData.data(),aData.size());
  write_size += aData.size();

#if defined(WITH_Z_COMPRESSION) || defined(WITH_LZ4_COMPRESSION)
    } // else
#endif
  return write_size;
}


SinkTaskBase* SaveContainerEdf::getCompressionTask(const CtSaving::HeaderMap& header)
{
#if defined(WITH_Z_COMPRESSION) || defined(WITH_LZ4_COMPRESSION)
  const CtSaving::Parameters& pars = m_stream.getParameters(CtSaving::Acq);
#endif
#ifdef WITH_Z_COMPRESSION
  if(m_format == CtSaving::EDFGZ)
    return new FileZCompression(*this,pars.framesPerFile,header);
  else
#endif
    
#ifdef WITH_LZ4_COMPRESSION
  if(m_format == CtSaving::EDFLZ4)
    return new FileLz4Compression(*this,pars.framesPerFile,header);
  else
#endif
  return NULL;
}

