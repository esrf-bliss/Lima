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

#ifdef WITH_Z_COMPRESSION 
#include <zlib.h>

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
  private:
    static const int BUFFER_HELPER_SIZE;

    void _test_avail_out(ZBufferList& return_buffers);
    void _update_used_size(ZBufferList& return_buffers);
    void _compression(const char *buffer,int size,ZBufferList& return_buffers);
    void _end_compression(ZBufferList& return_buffers);
  };
#endif // WITH_Z_COMPRESSION

#ifdef WITH_LZ4_COMPRESSION
#include <lz4frame.h>

static const int LZ4_HEADER_SIZE = 19;
static const int LZ4_FOOTER_SIZE = 4;

static const LZ4F_preferences_t lz4_preferences = {
  {
    LZ4F_max256KB,            /* blockSizeID */
    LZ4F_blockLinked,         /* blockMode */
    LZ4F_noContentChecksum,   /* contentChecksumFlag */
    LZ4F_frame,               /* frameType */
    0,                        /* contentSize */
    0,                        /* dictID */
    LZ4F_noBlockChecksum      /* blockChecksumFlag */
  },
  0,   /* compressionLevel */
  1,   /* autoFlush */
  0,   /* favorDecSpeed */
  { 0, 0, 0 },  /* reserved, must be set to 0 */
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
   void _compression(const char *src,int size,ZBufferList& return_buffers);   
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
  void _compression(const char *buffer,int size,int depth,ZBufferList& return_buffers);
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
  void _compression(const char *buffer,int size,ZBufferList& return_buffers);
};
#endif // WITH_Z_COMPRESSION

};


#endif // CTSAVING_COMPRESSION_H
