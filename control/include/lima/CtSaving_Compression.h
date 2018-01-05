//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2017
// European Synchrotron Radiation Facility
// CS40220 38043 Grenoble Cedex 9 
// FRANCE
//
// Contact: lima@esrf.fr
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

#ifndef CTSAVING_COMPRESSION_H
#define CTSAVING_COMPRESSION_H

#include "lima/CtSaving.h"
#include "processlib/SinkTask.h"

namespace lima {

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
typedef std::vector<_BufferHelper*> ZBufferType;
typedef std::map<int,ZBufferType*> dataId2ZBufferType;

#ifdef WITH_Z_COMPRESSION 
#include <zlib.h>

#define TEST_AVAIL_OUT	if(!m_compression_struct.avail_out) \
    {									\
     _BufferHelper *newBuffer = new _BufferHelper(); \
      m_compression_struct.next_out = (Bytef*)newBuffer->buffer;	\
      m_compression_struct.avail_out = newBuffer->BUFFER_HELPER_SIZE;	\
      return_buffers->push_back(newBuffer);				\
    }

   class FileZCompression: public SinkTaskBase
  {
    DEB_CLASS_NAMESPC(DebModControl,"File Z Compression Task","Control");

    CtSaving::SaveContainer& 	m_container;
    int 			m_frame_per_file;
    CtSaving::HeaderMap 	m_header;
    
    z_stream_s		m_compression_struct;
   public:
    FileZCompression(CtSaving::SaveContainer &save_cnt,
		 int framesPerFile,const CtSaving::HeaderMap &header);
     ~FileZCompression();
    virtual void process(Data &aData);
    void _compression(const char *buffer,int size,ZBufferType* return_buffers);
    void _end_compression(ZBufferType* return_buffers);
  };
#endif // WITH_Z_COMPRESSION

#ifdef WITH_LZ4_COMPRESSION
#include <lz4frame.h>

static const int LZ4_HEADER_SIZE = 19;
static const int LZ4_FOOTER_SIZE = 4;

static const LZ4F_preferences_t lz4_preferences = {
  { LZ4F_max256KB, LZ4F_blockLinked, LZ4F_noContentChecksum, LZ4F_frame, 0, { 0, 0 } },
  0,   /* compression level */
  1,   /* autoflush */
  { 0, 0, 0, 0 },  /* reserved, must be set to 0 */
};

 class FileLz4Compression: public SinkTaskBase
 {
   DEB_CLASS_NAMESPC(DebModControl,"File Lz4 Compression Task","Control");
   
   CtSaving::SaveContainer&	m_container;
   int				m_frame_per_file;
   CtSaving::HeaderMap		m_header;
   LZ4F_compressionContext_t	m_ctx;
 public:
   FileLz4Compression(CtSaving::SaveContainer &save_cnt,
		  int framesPerFile,const CtSaving::HeaderMap &header);
   ~FileLz4Compression();
   virtual void process(Data &aData);
   void _compression(const char *src,int size,ZBufferType* return_buffers);   
 };
#endif // WITH_LZ4_COMPRESSION

#ifdef WITH_BS_COMPRESSION

class ImageBsCompression: public SinkTaskBase
{
  DEB_CLASS_NAMESPC(DebModControl, "Image BS/LZ4 Compression Task", "Control");

  CtSaving::SaveContainer&	m_container;

 public:
  ImageBsCompression(CtSaving::SaveContainer &save_cnt);
  ~ImageBsCompression();
  virtual void process(Data &aData);
  void _compression(const char *buffer,int size,int depth,ZBufferType* return_buffers);
};
#endif // WITH_BS_COMPRESSION 

#ifdef WITH_Z_COMPRESSION 
#include <zlib.h>

class ImageZCompression: public SinkTaskBase
{
  DEB_CLASS_NAMESPC(DebModControl,"Image Z Compression Task","Control");
  
  CtSaving::SaveContainer& 	m_container;
  int                              m_compression_level;
 public:
  ImageZCompression(CtSaving::SaveContainer &save_cnt,
	       int level);
  
  ~ImageZCompression();
  virtual void process(Data &aData);
  void _compression(const char *buffer,int size,ZBufferType* return_buffers);
};
#endif // WITH_Z_COMPRESSION

};


#endif // CTSAVING_COMPRESSION_H
