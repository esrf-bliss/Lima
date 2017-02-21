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

namespace lima {

  class SaveContainerEdf : public CtSaving::SaveContainer
  {
    DEB_CLASS_NAMESPC(DebModControl,"Saving EDF Container","Control");
    class Compression;
    friend class Compression;
    class Lz4Compression;
    friend class Lz4Compression;
  public:
    struct _BufferHelper
    {
      DEB_CLASS_NAMESPC(DebModControl,"_BufferHelper","Control");

      void _init(int buffer_size);
    public:
      static const int BUFFER_HELPER_SIZE;
      _BufferHelper();
      _BufferHelper(int buffer_size);
      ~_BufferHelper();
      int used_size;
      void *buffer;
    };

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
    virtual void _clear();
  private:
    typedef std::vector<_BufferHelper*> ZBufferType;
    typedef std::map<int,ZBufferType*> dataId2ZBufferType;
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
    void _setBuffer(int frameNumber,ZBufferType*);
    ZBufferType* _takeBuffer(int dataId);
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
    dataId2ZBufferType		 m_buffers;
    Mutex			 m_lock;
    MmapInfo			 m_mmap_info;
    std::string			 m_current_filename;
  };

}
#endif // CTSAVING_EDF_H
