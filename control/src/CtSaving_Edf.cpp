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

#ifdef __unix
SaveContainerEdf::MmapInfo::~MmapInfo()
{
  if(mmap_addr)
    munmap(mmap_addr, header_size);
}

inline void SaveContainerEdf::MmapInfo::map(const std::string& fname,
					    long long header_position)
{
  header_position -= header_size;
  long sz = sysconf(_SC_PAGESIZE);
  long long mapping_offset = header_position / sz * sz;
  header_size += header_position - mapping_offset;
  height_offset -= mapping_offset;
  size_offset -= mapping_offset;
  int fd = ::open(fname.c_str(),O_RDWR);
  if(fd < 0)
    throw LIMA_CTL_EXC(Error, "Error opening ") << fname << ": " << strerror(errno);
  mmap_addr = mmap(NULL,header_size,
		   PROT_WRITE,MAP_SHARED,fd,mapping_offset);
  int mmap_err = errno;
  ::close(fd);
  if(!mmap_addr)
    throw LIMA_CTL_EXC(Error, "Error mapping ") << fname << ": " << strerror(mmap_err);
}
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

/** @brief saving container handle
 *
 *  This class manage the low-level handle
 */
SaveContainerEdf::File::File(SaveContainerEdf& cont,
			     const std::string& filename,
			     std::ios_base::openmode openFlags)
  : m_cont(cont), m_filename(filename)
#ifdef __unix
    , m_height(0), m_size(0)
#endif
{
  m_fout.exceptions(std::ios_base::failbit | std::ios_base::badbit);
  m_fout.open(filename.c_str(),openFlags);
#ifdef __unix
  m_buffer = m_cont.getNewBuffer();
  m_fout.rdbuf()->pubsetbuf((char*)m_buffer,WRITE_BUFFER_SIZE);
#endif
}

SaveContainerEdf::File::~File()
{
#ifdef __unix
  m_cont.releaseBuffer(m_buffer);
#endif
}

/** @brief saving container
 *
 *  This class manage file saving
 */
SaveContainerEdf::SaveContainerEdf(CtSaving::Stream& stream,
				   CtSaving::FileFormat format) :
  CtSaving::SaveContainer(stream),
#ifdef __unix
  m_nb_buffers(0),
#endif
  m_format(format)
{
  DEB_CONSTRUCTOR();
}

SaveContainerEdf::~SaveContainerEdf()
{
  DEB_DESTRUCTOR();
#ifdef __unix
  if(m_free_buffers.size() != m_nb_buffers)
    DEB_WARNING() << "Missing free buffers: "
		  << "got " << m_free_buffers.size() << ", "
		  << "expected " << m_nb_buffers;
  while(!m_free_buffers.empty())
    free(getNewBuffer());
#endif
}

#ifdef __unix
void *SaveContainerEdf::getNewBuffer()
{
  DEB_MEMBER_FUNCT();

  void *buffer;
  if(!m_free_buffers.empty())
    {
      buffer = m_free_buffers.top();
      m_free_buffers.pop();
    }
  else
    {
      if(posix_memalign(&buffer,4*1024,WRITE_BUFFER_SIZE))
	THROW_CTL_ERROR(Error) << "Can't allocate write buffer";
      ++m_nb_buffers;
    }
  return buffer;
}

void SaveContainerEdf::releaseBuffer(void *buffer)
{
  DEB_MEMBER_FUNCT();
  m_free_buffers.push(buffer);
}
#endif

void* SaveContainerEdf::_open(const std::string &filename,
			      std::ios_base::openmode openFlags)
{
  DEB_MEMBER_FUNCT();
  return new File(*this, filename, openFlags);
}

void SaveContainerEdf::_close(void* f)
{
  DEB_MEMBER_FUNCT();
  File* file = (File*) f;
  delete file;
}

long SaveContainerEdf::_writeFile(void* f,Data &aData,
				  CtSaving::HeaderMap &aHeader,
				  CtSaving::FileFormat aFormat)
{
  DEB_MEMBER_FUNCT();
  long write_size = 0;
  File* file = (File*) f;
  File::Stream* fout = &file->m_fout;

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
      file->m_height += aData.dimensions[1];
      file->m_size += aData.size();
      MmapInfo& mmap_info = file->m_mmap_info;
      if(!mmap_info)	// Create header and mmap
	{
	  const CtSaving::Parameters& pars = m_stream.getParameters(CtSaving::Acq);
	  mmap_info = _writeEdfHeader(aData,aHeader,pars.framesPerFile,*fout,8);
	  write_size += mmap_info.header_size;
	  fout->flush();
	  long long header_position = fout->tellp();
	  mmap_info.map(file->m_filename, header_position);
	}
      else
	{
	  char* start_size_string = mmap_info.sizeLocation();
	  int nbchar = sprintf(start_size_string,"%lld",file->m_size);
	  start_size_string[nbchar] = ' ';

	  char* start_height_string = mmap_info.heightLocation();
	  nbchar = sprintf(start_height_string,"%lld",file->m_height);
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

