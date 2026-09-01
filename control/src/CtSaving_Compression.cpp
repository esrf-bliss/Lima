
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
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(aData.frameNumber);
  ZBufferList aBufferListPt = _compress_header(aData, false);
  _compression((char*)aData.data(),aData.size(),aBufferListPt);
  _end_compression(aBufferListPt);

  m_container._setBuffers(aData,std::move(aBufferListPt));
}

ZBufferList FileZCompression::compress_header(Data &aData)
{
  return _compress_header(aData, true);
}

ZBufferList FileZCompression::_compress_header(Data &aData, bool end_stream)
{
  ZBufferList aBufferListPt;
  std::ostringstream buffer;
  m_container._writeEdfHeader(aData,m_header, buffer);
  const std::string& tmpBuffer = buffer.str();
  _compression(tmpBuffer.data(), tmpBuffer.size(), aBufferListPt);
  if (end_stream)
    _end_compression(aBufferListPt);
  return aBufferListPt;
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
  DEB_PARAM() << DEB_VAR1(aData.frameNumber);
  ZBufferList aBufferListPt = compress_header(aData);
  _compression((char*)aData.data(),aData.size(),aBufferListPt);
  m_container._setBuffers(aData,std::move(aBufferListPt));
}

ZBufferList FileLz4Compression::compress_header(Data &aData)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(aData);
  
  std::ostringstream buffer;
  m_container._writeEdfHeader(aData,m_header,buffer);
  ZBufferList aBufferListPt;
  const std::string& tmpBuffer = buffer.str();
  _compression(tmpBuffer.c_str(),tmpBuffer.size(),aBufferListPt);
  return aBufferListPt;
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

int ImageBsCompression::calcBufferSize(int data_size, int data_depth)
{
  unsigned int bs_block_size= 0;
  unsigned int bs_in_size= (unsigned int)(data_size/data_depth);

  size_t header_size = 8 + 4;
  size_t bs_out_bound = bshuf_compress_lz4_bound(bs_in_size, data_depth, bs_block_size);

  return int(header_size + bs_out_bound);
}

void ImageBsCompression::process(Data &aData)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(aData.frameNumber);
  ZBufferList aBufferListPt;
  _compression((char*)aData.data(), aData.size(), aData.depth(), aBufferListPt);
  m_container._setBuffers(aData,std::move(aBufferListPt));
}

void ImageBsCompression::_compression(const char *src,int data_size,int data_depth,
				      ZBufferList& return_buffers)
{
  DEB_MEMBER_FUNCT();

  unsigned int bs_block_size= 0;
  unsigned int bs_in_size= (unsigned int)(data_size/data_depth);

  size_t header_size = 8 + 4;
  size_t bs_out_bound = bshuf_compress_lz4_bound(bs_in_size, data_depth, bs_block_size);
  int buffer_size = header_size + bs_out_bound;

  BufferHelper& buffer_helper = m_container.getZBufferHelper();
  std::shared_ptr<void> p = buffer_helper.getBuffer(buffer_size);
  if (!p)
    THROW_CTL_ERROR(Error) << "BS Compression failed: helper has no buffer";
  return_buffers.emplace_back(p, buffer_size);

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

#ifdef WITH_JP2K_COMPRESSION
#include "CtSaving_Hdf5Jp2k.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <vector>

#ifdef WITH_OPENJPH_JP2K
#include "openjph/ojph_arch.h"
#include "openjph/ojph_codestream.h"
#include "openjph/ojph_file.h"
#include "openjph/ojph_mem.h"
#include "openjph/ojph_params.h"
#endif

#ifdef WITH_KAKADU_JP2K
#include <kdu_compressed.h>
#include <kdu_messaging.h>
#include <kdu_params.h>
#include <kdu_stripe_compressor.h>
#endif

DEB_GLOBAL(DebModControl)

inline int jp2k_precision(lima::hdf5_jp2k::DataType type)
{
  switch (type) {
  case lima::hdf5_jp2k::JP2K_UINT8:
  case lima::hdf5_jp2k::JP2K_INT8:
    return 8;
  case lima::hdf5_jp2k::JP2K_UINT16:
  case lima::hdf5_jp2k::JP2K_INT16:
    return 16;
  }
  return 0;
}

inline bool jp2k_is_signed(lima::hdf5_jp2k::DataType type)
{
  return type == lima::hdf5_jp2k::JP2K_INT8 ||
         type == lima::hdf5_jp2k::JP2K_INT16;
}

#ifdef WITH_OPENJPH_JP2K

void encode_openjph(Data& data, double /*compression_ratio*/,
  std::vector<unsigned char>& out)
{
  using namespace lima::hdf5_jp2k;

  DataType jp2k_type = toJp2kType(data.type);

  const ojph::ui32 width = static_cast<ojph::ui32>(data.dimensions[0]);
  const ojph::ui32 height = static_cast<ojph::ui32>(data.dimensions[1]);
  const ojph::ui32 precision = static_cast<ojph::ui32>(jp2k_precision(jp2k_type));
  const bool is_signed = jp2k_is_signed(jp2k_type);
  const void* input_buffer = data.data();

  try {
    ojph::codestream codestream;

    ojph::param_siz siz = codestream.access_siz();
    siz.set_image_extent(ojph::point(width, height));
    siz.set_num_components(1);
    siz.set_component(0, ojph::point(1, 1), precision, is_signed);
    siz.set_image_offset(ojph::point(0, 0));
    siz.set_tile_size(ojph::size(0, 0));
    siz.set_tile_offset(ojph::point(0, 0));

    ojph::param_cod cod = codestream.access_cod();
    cod.set_num_decomposition(5);
    cod.set_block_dims(64, 64);
    cod.set_progression_order("RPCL");
    cod.set_color_transform(false); /* samples are not assumed to be RGB */
    cod.set_reversible(true);       /* lossless 5/3 wavelet (HTJ2K) */

    codestream.set_planar(false);

    ojph::mem_outfile file;
    file.open();
    codestream.write_headers(&file);

    ojph::ui32 next_comp;
    ojph::line_buf* cur_line = codestream.exchange(nullptr, next_comp);

    for (ojph::ui32 y = 0; y < height; y++) {
      ojph::si32* dst = cur_line->i32;
      const std::size_t row_offset = static_cast<std::size_t>(y) * width;

      if (precision <= 8) {
        if (is_signed) {
          const int8_t* src = static_cast<const int8_t*>(input_buffer) + row_offset;
          for (ojph::ui32 x = 0; x < width; x++)
            dst[x] = src[x];
        } else {
          const uint8_t* src = static_cast<const uint8_t*>(input_buffer) + row_offset;
          for (ojph::ui32 x = 0; x < width; x++)
            dst[x] = src[x];
        }
      } else {
        if (is_signed) {
          const int16_t* src = static_cast<const int16_t*>(input_buffer) + row_offset;
          for (ojph::ui32 x = 0; x < width; x++)
            dst[x] = src[x];
        } else {
          const uint16_t* src = static_cast<const uint16_t*>(input_buffer) + row_offset;
          for (ojph::ui32 x = 0; x < width; x++)
            dst[x] = src[x];
        }
      }

      cur_line = codestream.exchange(cur_line, next_comp);
    }

    codestream.flush();
    codestream.close();

    const std::size_t used = file.get_used_size();
    out.resize(used);
    std::memcpy(out.data(), file.get_data(), used);
  } catch (const std::exception& e) {
    throw LIMA_CTL_EXC(Error, "[OpenJPH] encode error") << ": " << e.what();
  } catch (...) {
    throw LIMA_CTL_EXC(Error, "[OpenJPH] encode unknown error");
  }
}

#endif //WITH_OPENJPH_JP2K

#ifdef WITH_KAKADU_JP2K

#include <cmath>

using namespace kdu_core;
using namespace kdu_supp;

class KduMemTarget : public kdu_compressed_target
{
public:
  std::vector<unsigned char>& out;

  KduMemTarget(std::vector<unsigned char>& out) : out(out) {}

  bool write(const kdu_byte* buf, int num_bytes)
  {
    out.insert(out.end(), buf, buf + num_bytes);
    return true;
  }
};

class KduStreamMessage : public kdu_message
{
public:
  std::ostream& stream;

  KduStreamMessage(std::ostream& stream) : stream(stream) {}
  void put_text(const char* text) { stream << text; }
  void flush(bool end_of_message=false) { stream.flush(); }
};

template <class T>
void copy_to_kdu16(Data& data, std::vector<kdu_int16>& out)
{
  const T* src = static_cast<const T*>(data.data());
  std::size_t nb_pixels = static_cast<std::size_t>(data.dimensions[0]) *
        static_cast<std::size_t>(data.dimensions[1]);
  out.resize(nb_pixels);
  for (std::size_t i = 0; i < nb_pixels; ++i)
    out[i] = static_cast<kdu_int16>(src[i]);
}

class KduErrorHandler : public kdu_message {
public:
  explicit KduErrorHandler(bool debug_enabled) : debug(debug_enabled) {}

  void put_text(const char *string) override {
    if (string) {
      buffer.append(string);
    }
  }

  void flush(bool end_of_message) override {
    if (end_of_message) {
      if (debug) {
        fprintf(stderr, "[hdf5plugin/jpeg2000] Kakadu error: %s\n", buffer.c_str());
      }
      buffer.clear();
      throw KDU_ERROR_EXCEPTION;
    }
  }

private:
  bool debug = false;
  std::string buffer;
};

class KduWarningHandler : public kdu_message {
public:
  explicit KduWarningHandler(bool debug_enabled) : debug(debug_enabled) {}

  void put_text(const char *string) override {
    if (string) {
      buffer.append(string);
    }
  }

  void flush(bool end_of_message) override {
    if (end_of_message) {
      if (debug) {
        fprintf(stderr, "[hdf5plugin/jpeg2000] Kakadu warning: %s\n", buffer.c_str());
      }
      buffer.clear();
    }
  }

private:
  bool debug = false;
  std::string buffer;
};

void ensure_kakadu_handlers(bool debug) {
  static bool configured = false;
  if (configured) {
    return;
  }
  configured = true;
  static KduErrorHandler err_handler(debug);
  static KduWarningHandler warn_handler(debug);
  kdu_customize_errors(&err_handler);
  kdu_customize_warnings(&warn_handler);
}

void encode_kakadu(Data& data, double compression_ratio,
       std::vector<unsigned char>& out)
{
  DEB_GLOBAL_FUNCT();

  using namespace kdu_core;
  using namespace kdu_supp;

  lima::hdf5_jp2k::DataType jp2k_type = lima::hdf5_jp2k::toJp2kType(data.type);
  int width = data.dimensions[0];
  int height = data.dimensions[1];
  int prec = jp2k_precision(jp2k_type);
  bool sgnd = jp2k_is_signed(jp2k_type);

  std::vector<kdu_int16> samples;
  switch (jp2k_type) {
  case lima::hdf5_jp2k::JP2K_UINT8:
    copy_to_kdu16<unsigned char>(data, samples);
    break;
  case lima::hdf5_jp2k::JP2K_INT8:
    copy_to_kdu16<char>(data, samples);
    break;
  case lima::hdf5_jp2k::JP2K_UINT16:
    copy_to_kdu16<unsigned short>(data, samples);
    break;
  case lima::hdf5_jp2k::JP2K_INT16:
    copy_to_kdu16<short>(data, samples);
    break;
  }

  KduMemTarget target(out);
  siz_params siz;
  siz.set(Scomponents, 0, 0, 1);
  siz.set(Sdims, 0, 0, height);
  siz.set(Sdims, 0, 1, width);
  siz.set(Sprecision, 0, 0, prec);
  siz.set(Ssigned, 0, 0, sgnd ? 1 : 0);
  siz.parse_string("Scap=P15");
  siz.finalize_all();
  
  kdu_codestream codestream;
  kdu_stripe_compressor compressor;
  try {
    codestream.create(&siz, &target);
    kdu_params* siz = codestream.access_siz();

    siz->parse_string("Creversible=no");
    siz->parse_string("Clevels=5");
    siz->parse_string("Clayers=1");
    double quantize_step = std::max(std::pow(2, -(prec +5)), std::pow(2, -18));
    char qstep[64];
    snprintf(qstep, sizeof(qstep), "Qstep=%.17g", quantize_step);
    siz->parse_string(qstep);

    siz->parse_string("Cmodes=HT");

    siz->finalize_all();
  
    kdu_long layer_size = static_cast<kdu_long>(
      std::max<double>(1.0, lima::hdf5_jp2k::rawSize(width, height, jp2k_type) /
            compression_ratio));

    DEB_TRACE() << DEB_VAR1(layer_size);

    compressor.start(codestream, 1, &layer_size, NULL, 0, /*force_precise=*/true);
    int stripe_heights[1] = {height};
    int precisions[1] = {prec};
    bool is_signed_arr[1] = {sgnd};
    kdu_int16* bufs[1] = {samples.data()};
    compressor.push_stripe(bufs, stripe_heights, NULL, NULL,
         precisions, is_signed_arr);
    if (!compressor.finish())
      throw LIMA_CTL_EXC(Error, "Kakadu encoding did not finish");
    codestream.destroy();
    target.close();
  } catch (...) {
    compressor.reset();
    codestream.destroy();
    throw;
  }
}
#endif // WITH_KAKADU_JP2K

size_t hdf5_jp2k_filter(unsigned int flags, size_t cd_nelmts,
      const unsigned int cd_values[], size_t nbytes,
      size_t* buf_size, void** buf)
{
  return 0;
}

const H5Z_class2_t jp2k_filter_class = {
  H5Z_CLASS_T_VERS,
  LIMA_HDF5_JP2K_FILTER,
  1,
  1,
  "lima_htj2k",
  NULL,
  NULL,
  hdf5_jp2k_filter
};

bool lima::hdf5_jp2k::isSupported(Data::TYPE type)
{
  return type == Data::UINT8 || type == Data::INT8 ||
         type == Data::UINT16 || type == Data::INT16;
}

lima::hdf5_jp2k::DataType lima::hdf5_jp2k::toJp2kType(Data::TYPE type)
{
  switch (type) {
  case Data::UINT8:
    return JP2K_UINT8;
  case Data::INT8:
    return JP2K_INT8;
  case Data::UINT16:
    return JP2K_UINT16;
  case Data::INT16:
    return JP2K_INT16;
  default:
    throw LIMA_CTL_EXC(Error, "Unsupported HDF5 JP2K image type");
  }
}

std::size_t lima::hdf5_jp2k::rawSize(std::size_t width, std::size_t height,
             DataType type)
{
  return width * height * static_cast<std::size_t>(jp2k_precision(type) / 8);
}

std::size_t lima::hdf5_jp2k::compressedBufferBound(std::size_t raw_size)
{
  return raw_size * 2 + 4096;
}

void lima::hdf5_jp2k::encode(Data& data, std::vector<unsigned char>& out)
{
  encode(data, 10.0, out);
}

void lima::hdf5_jp2k::encode(Data& data, double compression_ratio,
           std::vector<unsigned char>& out)
{
  encode(data, compression_ratio, OpenJPH, out);
}

void lima::hdf5_jp2k::encode(Data& data, double compression_ratio, Codec codec,
           std::vector<unsigned char>& out)
{
  out.clear();

  switch (codec)
  {
    case Kakadu:
#if defined(WITH_KAKADU_JP2K)
      encode_kakadu(data, compression_ratio, out);
      break;
#else
    throw LIMA_CTL_EXC(Error, "Lima is not compiled with Kakadu JP2K support");
#endif
    case OpenJPH:
#if defined(WITH_OPENJPH_JP2K)
      encode_openjph(data, compression_ratio, out);
      break;
#else
    throw LIMA_CTL_EXC(Error, "Lima is not compiled with OpenJPH JP2K support");
#endif
    default:
      throw LIMA_CTL_EXC(Error, "Unknown JP2K codec");
  }
}

int lima::hdf5_jp2k::register_filter()
{
  htri_t available = H5Zfilter_avail(filter_id());
  if (available > 0)
    return 0;
  return H5Zregister(&jp2k_filter_class);
}

H5Z_filter_t lima::hdf5_jp2k::filter_id()
{
  return static_cast<H5Z_filter_t>(LIMA_HDF5_JP2K_FILTER);
}

const H5Z_class2_t* lima::hdf5_jp2k::filter_class()
{
  return &jp2k_filter_class;
}

ImageJp2kCompression::ImageJp2kCompression(CtSaving::SaveContainer &save_cnt,
					   double compression_ratio,
					   CtSaving::Jp2kCompressionCodec codec):
  m_container(save_cnt), m_compression_ratio(compression_ratio), m_codec(codec)
{
  DEB_CONSTRUCTOR();

#if defined(WITH_KAKADU_JP2K)
  ensure_kakadu_handlers(true);
#endif
};

ImageJp2kCompression::~ImageJp2kCompression()
{
}

int ImageJp2kCompression::calcBufferSize(int data_size, int /*data_depth*/)
{
  return int(hdf5_jp2k::compressedBufferBound(data_size));
}

void ImageJp2kCompression::process(Data &aData)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(aData.frameNumber);

  if (!hdf5_jp2k::isSupported(aData.type))
    THROW_CTL_ERROR(Error) << "HDF5 JP2K supports only 8-bit and 16-bit integer image data";

  std::vector<unsigned char> compressed;
  hdf5_jp2k::encode(aData, m_compression_ratio,
		    (m_codec == CtSaving::JP2KKakadu) ?
		    hdf5_jp2k::Kakadu : hdf5_jp2k::OpenJPH,
		    compressed);

  ZBufferList aBufferList;
  BufferHelper& buffer_helper = m_container.getZBufferHelper();
  std::shared_ptr<void> p = buffer_helper.getBuffer(compressed.size());
  if (!p)
    p = std::shared_ptr<void>(std::malloc(compressed.size()), std::free);
  if (!p)
    THROW_CTL_ERROR(Error) << "JP2K Compression failed: no output buffer";

  std::memcpy(p.get(), &compressed[0], compressed.size());
  aBufferList.emplace_back(p, compressed.size());
  aBufferList.back().used_size = compressed.size();
  m_container._setBuffers(aData, std::move(aBufferList));
}

#endif // WITH_JP2K_COMPRESSION

#ifdef WITH_Z_COMPRESSION
ImageZCompression::ImageZCompression(CtSaving::SaveContainer &save_cnt,  int  level):
  m_container(save_cnt), m_compression_level(level)
{
  DEB_CONSTRUCTOR();
};

ImageZCompression::~ImageZCompression()
{
}

int ImageZCompression::calcBufferSize(int data_size, int data_depth)
{
  return int(compressBound(data_size));
}

void ImageZCompression::process(Data &aData)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(aData.frameNumber);
  ZBufferList aBufferListPt;
  _compression((char*)aData.data(),aData.size(),aBufferListPt);
  m_container._setBuffers(aData,std::move(aBufferListPt));
}

void ImageZCompression::_compression(const char *src,int size,
				     ZBufferList& return_buffers)
{
  DEB_MEMBER_FUNCT();
  // cannot know compression ratio in advance so allocate a buffer large enough
  uLong buffer_size = compressBound(size);
  BufferHelper& buffer_helper = m_container.getZBufferHelper();
  std::shared_ptr<void> p = buffer_helper.getBuffer(buffer_size);
  if (!p)
    THROW_CTL_ERROR(Error) << "[G]Z Compression failed: helper has no buffer";
  return_buffers.emplace_back(p, buffer_size);
  ZBuffer& newBuffer = return_buffers.back();
  char* buffer = (char*)newBuffer.ptr();
  
  int status;
  if ((status=compress2((Bytef*)buffer, &buffer_size, (Bytef*)src, size, m_compression_level)) < 0)
    THROW_CTL_ERROR(Error) << "Compression failed: error code " << status;
        
  newBuffer.used_size = buffer_size;
}
#endif // WITH_Z_COMPRESSION
