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

#include "lima/CtSaving_Compression.h"
#include "CtSaving_Edf.h"

using namespace lima;

#ifdef WITH_Z_COMPRESSION
const int FileZCompression::BUFFER_HELPER_SIZE = 64 * 1024;

FileZCompression::FileZCompression(SaveContainerEdf &save_cnt,
				   const CtSaving::HeaderMap &header) :
  m_container(save_cnt),m_header(header)
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
  
  if(deflateInit2(&m_compression_struct, 8,
		  Z_DEFLATED,
		  31,
		  8,
		  Z_DEFAULT_STRATEGY) != Z_OK)
    THROW_CTL_ERROR(Error) << "Can't init compression struct";
};

FileZCompression::~FileZCompression()
{
  deflateEnd(&m_compression_struct);
}

void FileZCompression::process(Data &aData)
{
  ZBufferList aBufferListPt;

  std::ostringstream buffer;
  m_container._writeEdfHeader(aData,m_header, buffer);
  const std::string& tmpBuffer = buffer.str();
  _compression(tmpBuffer.c_str(),tmpBuffer.size(),aBufferListPt);
  _compression((char*)aData.data(),aData.size(),aBufferListPt);
  _end_compression(aBufferListPt);

  m_container._setBuffer(aData.frameNumber,std::move(aBufferListPt));
}

inline void FileZCompression::_test_avail_out(ZBufferList& return_buffers)
{
  if(!m_compression_struct.avail_out)
    {
      return_buffers.emplace_back(BUFFER_HELPER_SIZE);
      ZBuffer& newBuffer = return_buffers.back();
      m_compression_struct.next_out = (Bytef*)newBuffer.ptr();
      m_compression_struct.avail_out = BUFFER_HELPER_SIZE;
    }
}

inline void FileZCompression::_update_used_size(ZBufferList& return_buffers)
{
  return_buffers.back().used_size = (BUFFER_HELPER_SIZE -
				     m_compression_struct.avail_out);
}

void FileZCompression::_compression(const char *buffer,int size,ZBufferList& return_buffers)
{
  DEB_MEMBER_FUNCT();
  
  m_compression_struct.next_in = (Bytef*)buffer;
  m_compression_struct.avail_in = size;
  
  while(m_compression_struct.avail_in)
    {
      _test_avail_out(return_buffers);
      if(deflate(&m_compression_struct,Z_NO_FLUSH) != Z_OK)
	THROW_CTL_ERROR(Error) << "deflate error";
      _update_used_size(return_buffers);
    }
}
void FileZCompression::_end_compression(ZBufferList& return_buffers)
{
  DEB_MEMBER_FUNCT();
  
  int deflate_res = Z_OK;
  while(deflate_res == Z_OK)
    {
      _test_avail_out(return_buffers);
      deflate_res = deflate(&m_compression_struct,Z_FINISH);
      _update_used_size(return_buffers);
    }
  if(deflate_res != Z_STREAM_END)
    THROW_CTL_ERROR(Error) << "deflate error";
}
#endif // WITH_Z_COMPRESSION


#ifdef WITH_LZ4_COMPRESSION
FileLz4Compression::FileLz4Compression(SaveContainerEdf &save_cnt,
				       const CtSaving::HeaderMap &header) :
  m_container(save_cnt),m_header(header)
{
  DEB_CONSTRUCTOR();
  
  LZ4F_errorCode_t result = LZ4F_createCompressionContext(&m_ctx, LZ4F_VERSION);
  if(LZ4F_isError(result))
    THROW_CTL_ERROR(Error) << "LZ4 context init failed: " << DEB_VAR1(result);
}

FileLz4Compression::~FileLz4Compression()
{
  LZ4F_freeCompressionContext(m_ctx);
}

void FileLz4Compression::process(Data &aData)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(aData);
  
  std::ostringstream buffer;
  m_container._writeEdfHeader(aData,m_header,buffer);
  ZBufferList aBufferListPt;
  const std::string& tmpBuffer = buffer.str();
  _compression(tmpBuffer.c_str(),tmpBuffer.size(),aBufferListPt);
  _compression((char*)aData.data(),aData.size(),aBufferListPt);
  m_container._setBuffer(aData.frameNumber,std::move(aBufferListPt));
}

void FileLz4Compression::_compression(const char *src, size_t size,
				      ZBufferList& return_buffers)
{
  DEB_MEMBER_FUNCT();
  
  size_t buffer_size = LZ4F_compressFrameBound(size,&lz4_preferences);
  buffer_size += LZ4_HEADER_SIZE + LZ4_FOOTER_SIZE;
  
  return_buffers.emplace_back(buffer_size);
  ZBuffer& newBuffer = return_buffers.back();
  char* buffer = (char*)newBuffer.ptr();
  
  size_t offset = LZ4F_compressBegin(m_ctx,buffer,
				  buffer_size,&lz4_preferences);
  if(LZ4F_isError(offset))
    THROW_CTL_ERROR(Error) << "Failed to start compression: " << DEB_VAR1(offset);

  size_t error_code = LZ4F_compressUpdate(m_ctx,buffer + offset,buffer_size - offset,
				       src,size,NULL);
  if(LZ4F_isError(error_code))
    THROW_CTL_ERROR(Error) << "Compression Failed: " 
			   << DEB_VAR2(error_code,LZ4F_getErrorName(error_code));
  offset += error_code;
  
  error_code = LZ4F_compressEnd(m_ctx, buffer + offset, size - offset, NULL);
  if(LZ4F_isError(error_code))
    THROW_CTL_ERROR(Error) << "Failed to end compression: " << DEB_VAR1(error_code);
  offset += error_code;
  newBuffer.used_size = offset;
}
#endif // WITH_LZ4_COMPRESSION

#ifdef WITH_BS_COMPRESSION

#include "bitshuffle.h"

extern "C" {
void bshuf_write_uint64_BE(void* buf, uint64_t num);
extern void bshuf_write_uint32_BE(void* buf, uint32_t num);
}


ImageBsCompression::ImageBsCompression(CtSaving::SaveContainer &save_cnt):
  m_container(save_cnt)
{
  DEB_CONSTRUCTOR();
  DEB_TRACE() << "BitShuffle using SSE2=" << bshuf_using_SSE2() << " AVX2=" << bshuf_using_AVX2();
};

ImageBsCompression::~ImageBsCompression()
{
}

void ImageBsCompression::process(Data &aData)
{
  ZBufferList aBufferListPt;
  _compression((char*)aData.data(), aData.size(), aData.depth(), aBufferListPt);
  m_container._setBuffer(aData.frameNumber,std::move(aBufferListPt));
}

void ImageBsCompression::_compression(const char *src,int data_size,int data_depth,
				      ZBufferList& return_buffers)
{
  DEB_MEMBER_FUNCT();

  unsigned int bs_block_size= 0;
  unsigned int bs_in_size= (unsigned int)(data_size/data_depth);

  size_t header_size = 8 + 4;
  size_t bs_out_bound = bshuf_compress_lz4_bound(bs_in_size, data_depth, bs_block_size);

  return_buffers.emplace_back(header_size + bs_out_bound);
  ZBuffer& newBuffer = return_buffers.back();
  char* bs_buffer = (char*)newBuffer.ptr();

  bshuf_write_uint64_BE(bs_buffer, data_size);
  bs_buffer += 8;
  bshuf_write_uint32_BE(bs_buffer, bs_block_size);
  bs_buffer += 4;
  int64_t bs_out_size = bshuf_compress_lz4(src, bs_buffer, bs_in_size, data_depth, bs_block_size);
  if (bs_out_size < 0)
    THROW_CTL_ERROR(Error) << "BS Compression failed: error code [" << bs_out_size << "]";

  DEB_TRACE() << "BitShuffle Compression IN[" << data_size << "] OUT[" << bs_out_size << "]";
  newBuffer.used_size = header_size + bs_out_size;
}

#endif // WITH_BS_COMPRESSION

#ifdef WITH_Z_COMPRESSION
ImageZCompression::ImageZCompression(CtSaving::SaveContainer &save_cnt,  int  level):
  m_container(save_cnt), m_compression_level(level)
{
  DEB_CONSTRUCTOR();
};

ImageZCompression::~ImageZCompression()
{
}

void ImageZCompression::process(Data &aData)
{
  ZBufferList aBufferListPt;
  _compression((char*)aData.data(),aData.size(),aBufferListPt);
  m_container._setBuffer(aData.frameNumber,std::move(aBufferListPt));
}

void ImageZCompression::_compression(const char *src,int size,
				     ZBufferList& return_buffers)
{
  DEB_MEMBER_FUNCT();
  uLong buffer_size;
  int status;
  // cannot know compression ratio in advance so allocate a buffer for full image size
  buffer_size = compressBound(size);
  return_buffers.emplace_back(buffer_size);
  ZBuffer& newBuffer = return_buffers.back();
  char* buffer = (char*)newBuffer.ptr();
  
  if ((status=compress2((Bytef*)buffer, &buffer_size, (Bytef*)src, size, m_compression_level)) < 0)
    THROW_CTL_ERROR(Error) << "Compression failed: error code " << status;
        
  newBuffer.used_size = buffer_size;
}
#endif // WITH_Z_COMPRESSION

