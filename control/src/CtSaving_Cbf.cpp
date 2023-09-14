//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2019
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
#include <ctype.h>
//#include <openssl/md5.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

#include "CtSaving_Cbf.h"

#include "processlib/SinkTask.h"

using namespace lima;

static const char *DEFAULT_CATEGORY = "Misc";
static const char LIMA_HEADER_KEY_SEPARATOR = '/';
static const long int WRITE_BUFFER_SIZE = 64*1024;

struct SaveContainerCbf::_File
{
  DEB_CLASS_NAMESPC(DebModControl,"Saving CBF Container","Control");

public:
  _File(const std::string& filename,char* openFlags) :
    m_fout(NULL),
    m_current_cbf(NULL)
  {
    DEB_CONSTRUCTOR();

    if(posix_memalign(&m_fout_buffer,4*1024,WRITE_BUFFER_SIZE))
      THROW_CTL_ERROR(Error) << "Can't allocated write buffer";
    m_fout = fopen(filename.c_str(),openFlags);
    if(m_fout)
      setbuffer(m_fout,(char*)m_fout_buffer,WRITE_BUFFER_SIZE);
    else
      THROW_CTL_ERROR(Error) << "Can't open file:" << DEB_VAR1(filename);
  }
  ~_File()
  {
    if(m_current_cbf) cbf_free_handle(m_current_cbf);
    if(m_fout) fclose(m_fout);
    free(m_fout_buffer);
  }
  FILE*		m_fout;
  void*		m_fout_buffer;
  cbf_handle	m_current_cbf;
};

class SaveContainerCbf::Compression : public SinkTaskBase
{
  DEB_CLASS_NAMESPC(DebModControl,"Compression Task","Control");

  SaveContainerCbf& 	m_container;
  CtSaving::HeaderMap 	m_header;

  int _fillHeader(Data &aData,CtSaving::HeaderMap &aHeader,cbf_handle cbf)
  {
    DEB_MEMBER_FUNCT();

    CtSaving::HeaderOrderedMap anOrderedHeader;
    
    cbf_failnez(cbf_new_datablock(cbf, "image_0"));

    {
      typedef Data::HeaderContainer::LockedRef LockedRef;
      LockedRef aLockedRef(aData.header);
      LockedRef::Header &aDataHeader = aLockedRef.get();
      aHeader.insert(aDataHeader.begin(),aDataHeader.end());
    }

    // Here prepare a Ordered HeaderMap, so sort the header values and insert them
    // according to some conventions like Dectris Mini CBF:
    // Dectris Mini CBF, look for categories
    // 'array_data/header_convention' and 'array_data/header_contents',
    // they must be inserted in the header in special order,
    // 'convention' BEFORE 'contents'
    CtSaving::HeaderMap::iterator it1, it2;
    it1  = aHeader.find("array_data/header_convention");
    if (it1 != aHeader.end())
      {
	it2  = aHeader.find("array_data/header_contents");
	if (it2 != aHeader.end())
	  {
	    anOrderedHeader.insert(*it1);
	    anOrderedHeader.insert(*it2);
	    aHeader.erase(it1);
	    aHeader.erase(it2);
	  }		
      }
    _fillHeaderFromMap<CtSaving::HeaderMap>(aHeader, cbf);
    // Now append ordered header if any
    _fillHeaderFromMap<CtSaving::HeaderOrderedMap>(anOrderedHeader, cbf);
    return 0;
  }
  
  template <class T>
  int _fillHeaderFromMap(T& aHeader, cbf_handle cbf)
  {
    
    std::string previousCategory;
    
    for(typename T::iterator i = aHeader.begin();
	i != aHeader.end();++i)
      {
	size_t found = i->first.find_last_of(LIMA_HEADER_KEY_SEPARATOR);
	std::string currentCategory = i->first.substr(0,found);
	std::string key;
	if(found != std::string::npos)
	  key = i->first.substr(found);
	// no category was find so set it to default
	if(key.empty())		
	  {
	    key = currentCategory;
	    currentCategory = DEFAULT_CATEGORY;
	  }
	else
	  key = key.substr(1);

	if(previousCategory != currentCategory)
	  {
	    previousCategory = currentCategory;
	    cbf_failnez(cbf_new_category(cbf,currentCategory.c_str()));
	  }
	cbf_failnez(cbf_new_column(cbf,key.c_str()));
	if(!i->second.find_first_of('#')) // Goreterie MOSFLM
	  {
	    std::string tmpString = "\n";
	    tmpString += i->second;
	    cbf_failnez(cbf_set_value(cbf,tmpString.c_str()));
	  }
	else
	  cbf_failnez(cbf_set_value(cbf,i->second.c_str()));
      }
    return 0;
  }
    
  int _fillData(Data &aData,cbf_handle cbf)
  {
    DEB_MEMBER_FUNCT();

    cbf_failnez(cbf_new_category(cbf, "array_data"));
    cbf_failnez(cbf_new_column(cbf, "data"));

    cbf_failnez(cbf_set_integerarray_wdims(cbf,
					   CBF_BYTE_OFFSET,
					   0,
					   aData.data(),
					   aData.depth(),
					   aData.is_signed(),
					   aData.size()/aData.depth(),
					   "little_endian",
					   aData.dimensions[0],
					   aData.dimensions[1],
					   0,
					   128));

    return 0;
  }

public:
  Compression(SaveContainerCbf &save_cnt,const CtSaving::HeaderMap &header) :
    SinkTaskBase(),m_container(save_cnt),m_header(header) {}
  virtual ~Compression() {};

  virtual void process(Data &aData)
  {
    DEB_MEMBER_FUNCT();
    cbf_handle cbf;
    cbf_make_handle(&cbf);
    if(_fillHeader(aData,m_header,cbf))
      {
	cbf_free_handle(cbf);
	THROW_CTL_ERROR(Error) << "Something went wrong during CBF header filling";
      }

    if(_fillData(aData,cbf))
      {
	cbf_free_handle(cbf);
	THROW_CTL_ERROR(Error) << "Something went wrong during CBF data filling";
      }
    Handle h;
    h.format = CtSaving::CBFFormat,h.handle = cbf;
    m_container._setHandle(aData.frameNumber,h);
  }
};

class SaveContainerCbf::MHCompression : public SinkTaskBase
{
  DEB_CLASS_NAMESPC(DebModControl,"Compression MH Task","Control");
  
  SaveContainerCbf&	m_container;
  CtSaving::HeaderMap	m_header;
  void*			m_buffer;
  int			m_buffer_size;
  void*			m_header_str;
  int			m_header_size;
  int			m_header_memory_size;
public:
  MHCompression(SaveContainerCbf &save_cnt,const CtSaving::HeaderMap &header) :
    SinkTaskBase(),m_container(save_cnt),m_header(header),
    m_buffer(NULL),m_header_str(NULL),m_header_size(0),m_header_memory_size(0) {}
  virtual ~MHCompression() 
  {
    if(m_buffer) free(m_buffer);
    if(m_header_str) free(m_header_str);
  }
  virtual void process(Data &aData)
  {
    DEB_MEMBER_FUNCT();

    if(aData.type != Data::INT32)
      THROW_CTL_ERROR(Error) << "cbf mini header only manage signed int data type";

    long width = aData.dimensions[0];
    long height = aData.dimensions[1];
    long nb_pixel = width * height;
    long nb_pixel_4 = nb_pixel / 4;
    if(posix_memalign(&m_buffer,4*1024,nb_pixel * sizeof(int) * 2))
      THROW_CTL_ERROR(Error) << "Can't allocate compressed buffer";
    union { char *cp; short *sp; int *ip;} dst;
    dst.cp = (char*)m_buffer;
    int *src = (int*)aData.data();
    int prev_val = 0;
    int nb_pixel_uncompressed = 0;
    for(int i = 0; i < nb_pixel; ++i,++src)
      {
	int val = *src;
	int diff = val - prev_val;
	if(abs(diff) <= 127)
	  *dst.cp++ = diff;
	else
	  {
	    *dst.cp++ = 0x80;
	    if(abs(diff) <= 32767)
	      *dst.sp++ = diff;
	    else
	      {
		*dst.sp++ = 0x8000;
		*dst.ip++ = diff;
		if(++nb_pixel_uncompressed > nb_pixel_4)
		  goto end;
	      }
	  }
	prev_val = val;
      }
    m_buffer_size = dst.cp - (char*)m_buffer;
  end:
    const char* cbf_convertion;
    if(!m_buffer_size)		// compression went wrong
      {
	memcpy(m_buffer,aData.data(),aData.size());
	m_buffer_size = aData.size();
	cbf_convertion = "x-CBF_NONE";
      }
    else
      cbf_convertion = "x-CBF_BYTE_OFFSET";

    //MD5
    MD5_CTX context;
    MD5Init(&context);
    MD5Update(&context,(unsigned char*)m_buffer,m_buffer_size);
    unsigned char digest_str[16];
    MD5Final(digest_str,&context);
    //MD5 in base64
    BIO *base64_filter = BIO_new(BIO_f_base64());
    BIO_set_flags(base64_filter, BIO_FLAGS_BASE64_NO_NL);
    BIO *bio = BIO_new(BIO_s_mem());
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_push(base64_filter, bio);
    BIO_write(bio, digest_str, sizeof(digest_str));
    char* dige_str;
    long digest_lenght = BIO_get_mem_data(bio, &dige_str);
    std::string digest(dige_str,digest_lenght);
    BIO_free_all(bio);

    //build header
    _append_header("###CBF: VERSION 1.5\r\n");
    _append_header("# CBF file written by LImA\r\n\r\n");
    _append_header("data_image_%d\r\n\r\n",aData.frameNumber);
    //@todo probably one day need to fill image header
    //_append_header("_array_data.header_contents\r\n;\r\n");
    _append_header("\
_array_data.data\r\n;\r\n\
--CIF-BINARY-FORMAT-SECTION--\r\n\
Content-Type: application/octet-stream;\r\n\
     conversions=\"%s\"\r\n\
Content-Transfer-Encoding: BINARY\r\n\
X-Binary-Size: %d\r\n\
X-Binary-ID: 1\r\n\
X-Binary-Element-Type: \"signed 32-bit integer\"\r\n\
X-Binary-Element-Byte-Order: LITTLE_ENDIAN\r\n\
Content-MD5: %24s\r\n\
X-Binary-Number-of-Elements: %d\r\n\
X-Binary-Size-Fastest-Dimension: %d\r\n\
X-Binary-Size-Second-Dimension: %d\r\n\
X-Binary-Size-Padding: 1\r\n\
\r\n\
\f\032\004\325\
", cbf_convertion,m_buffer_size, digest.c_str(),nb_pixel, 
		   width, height);

    Handle h;
    h.format = CtSaving::CBFMiniHeader;
    h.data_buffer = m_buffer,h.data_buffer_size = m_buffer_size;
    // transfer ownership to Handler
    m_buffer = NULL,m_buffer_size = 0;

    h.header_data = m_header_str,h.header_data_size = m_header_size;
    // transfer ownership to Handler
    m_header_str = NULL,m_header_size = m_header_memory_size = 0;
    
    m_container._setHandle(aData.frameNumber,h);
  }
  inline void _append_header(const char* format,...)
  {
    DEB_MEMBER_FUNCT();

    va_list args;
    va_start(args,format);

    int left_char =  m_header_memory_size - m_header_size;
    int nb_char = vsnprintf((char*)m_header_str + m_header_size,left_char,
				format,args);
    if(nb_char >= left_char)
      {
	m_header_memory_size = (m_header_memory_size + nb_char + 4096) & ~4095; // 4096 include \0
	m_header_str = realloc(m_header_str,m_header_memory_size);
	left_char =  m_header_memory_size - m_header_size;
	nb_char = vsnprintf((char*)m_header_str + m_header_size,left_char,
				format,args);
      }
    else if(nb_char < 0)
      THROW_CTL_ERROR(Error) << "header construction error";
    m_header_size += nb_char;
  }
};

#ifdef DEBUG
static inline void outerror(int err)
{
  if ((err&CBF_FORMAT)==CBF_FORMAT)
    fprintf(stderr, " cbf: The file format is invalid.\n");
  if ((err&CBF_ALLOC)==CBF_ALLOC)
    fprintf(stderr, " cbf Memory allocation failed.\n");
  if ((err&CBF_ARGUMENT)==CBF_ARGUMENT)
    fprintf(stderr, " cbf: Invalid function argument.\n");
  if ((err&CBF_ASCII)==CBF_ASCII)
    fprintf(stderr, " cbf: The value is ASCII (not binary).\n");
  if ((err&CBF_BINARY)==CBF_BINARY)
    fprintf(stderr, " cbf: The value is binary (not ASCII).\n");
  if ((err&CBF_BITCOUNT)==CBF_BITCOUNT)
    fprintf(stderr, " cbf: The expected number of bits does"
      " not match the actual number written.\n");
  if ((err&CBF_ENDOFDATA)==CBF_ENDOFDATA)
    fprintf(stderr, " cbf: The end of the data was reached"
     " before the end of the array.\n");
  if ((err&CBF_FILECLOSE)==CBF_FILECLOSE)
    fprintf(stderr, " cbf: File close error.\n");
  if ((err&CBF_FILEOPEN)==CBF_FILEOPEN)
    fprintf(stderr, " cbf: File open error.\n");
  if ((err&CBF_FILEREAD)==CBF_FILEREAD)
    fprintf(stderr, " cbf: File read error.\n");
  if ((err&CBF_FILESEEK)==CBF_FILESEEK)
    fprintf(stderr, " cbf: File seek error.\n");
  if ((err&CBF_FILETELL)==CBF_FILETELL)
    fprintf(stderr, " cbf: File tell error.\n");
  if ((err&CBF_FILEWRITE)==CBF_FILEWRITE)
    fprintf(stderr, " cbf: File write error.\n");
  if ((err&CBF_IDENTICAL)==CBF_IDENTICAL)
    fprintf(stderr, " cbf: A data block with the new name already exists.\n");
  if ((err&CBF_NOTFOUND)==CBF_NOTFOUND)
    fprintf(stderr, " cbf: The data block, category, column or"
      " row does not exist.\n");
  if ((err&CBF_OVERFLOW)==CBF_OVERFLOW)
    fprintf(stderr, " cbf: The number read cannot fit into the"
      "destination argument.\n        The destination has been set to the nearest value.\n");
  if ((err& CBF_UNDEFINED)==CBF_UNDEFINED)
    fprintf(stderr, " cbf: The requested number is not defined (e.g. 0/0).\n");
  if ((err&CBF_NOTIMPLEMENTED)==CBF_NOTIMPLEMENTED)
    fprintf(stderr, " cbf: The requested functionality is not yet implemented.\n");
}

#undef cbf_failnez
#define cbf_failnez(x) \
 {int err; \
  err = (x); \
  if (err) { \
    fprintf(stderr," cbf: CBFlib fatal error %d\n",err); \
    outerror(err);   \
    return err;\
  } \
 }
 
#endif

SaveContainerCbf::SaveContainerCbf(CtSaving::Stream& stream,
				   CtSaving::FileFormat format) :
  CtSaving::SaveContainer(stream),
  m_lock(MutexAttr::Normal),
  m_format(format)
{
  DEB_CONSTRUCTOR();
}

SaveContainerCbf::~SaveContainerCbf()
{
  DEB_DESTRUCTOR();
}

SinkTaskBase* SaveContainerCbf::getCompressionTask(const CtSaving::HeaderMap &header)
{
  if(m_format == CtSaving::CBFMiniHeader)
    return new MHCompression(*this,header);
  else
    return new Compression(*this,header);
}

void* SaveContainerCbf::_open(const std::string &filename,
			      std::ios_base::openmode stdOpenflags,
			      CtSaving::Parameters& /*pars*/)
{
  DEB_MEMBER_FUNCT();
  char openFlags[8];
  if(stdOpenflags & std::ios_base::app)
    openFlags[0] = 'a';
  else
    openFlags[0] = 'w';
  
  if(stdOpenflags & std::ios_base::binary)
    openFlags[1] = 'b',openFlags[2] = '+',openFlags[3] = '\0';
  else
    openFlags[1] = '+',openFlags[2] = '\0';


  DEB_TRACE() << "Open file name: " << filename << " with open flags: " << openFlags;
  return new _File(filename,openFlags);
}

void SaveContainerCbf::_close(void* f)
{
  DEB_MEMBER_FUNCT();
  
  _File* file = (_File*)f;
  delete file;
}

long SaveContainerCbf::_writeFile(void* f,Data &aData,
				  CtSaving::HeaderMap&,
				  CtSaving::FileFormat)
{
  DEB_MEMBER_FUNCT();
  if(_writeCbfData((_File*)f,aData))
    THROW_CTL_ERROR(Error) << "Something went wrong during CBF data writing";
  return ftell(((_File*)f)->m_fout);
}

void SaveContainerCbf::_clear()
{
  AutoMutex aLock(m_lock);
  dataId2cbfHandle::iterator i = m_cbfs.begin();
  while(i != m_cbfs.end())
    {
      if(i->second.format == CtSaving::CBFMiniHeader)
	{
	  free(i->second.data_buffer);
	  free(i->second.header_data);
	}
      else
	cbf_free_handle(i->second.handle);
      dataId2cbfHandle::iterator previous = i++;
      m_cbfs.erase(previous);
    }
}

int SaveContainerCbf::_writeCbfData(_File* file,Data &aData)
{
  DEB_MEMBER_FUNCT();
  Handle handle = _takeHandle(aData.frameNumber);
  if(handle.format == CtSaving::CBFMiniHeader)
    {
      size_t write_size = fwrite(handle.header_data,sizeof(char),handle.header_data_size,
				 file->m_fout);
      if(write_size != size_t(handle.header_data_size))
	{
	  free(handle.header_data),free(handle.data_buffer);
	  DEB_ERROR() << "Can't write header";
	  return -1;		// error
	}
      free(handle.header_data);
      
      write_size = fwrite(handle.data_buffer,sizeof(char),handle.data_buffer_size,
			  file->m_fout);
      bool return_flag = write_size != size_t(handle.data_buffer_size);
      if(return_flag) DEB_ERROR() << "Cannot write image data";
      free(handle.data_buffer);
      return return_flag;
    }
  else
    {
      file->m_current_cbf = handle.handle;
      cbf_failnez(cbf_write_file(file->m_current_cbf,file->m_fout,0,
				 CBF,MSG_DIGEST|MIME_HEADERS,0));
    }
  return 0;
}

SaveContainerCbf::Handle SaveContainerCbf::_takeHandle(int dataId)
{
  AutoMutex aLock(m_lock);
  dataId2cbfHandle::iterator i = m_cbfs.find(dataId);
  Handle aReturnHandle = i->second;
  m_cbfs.erase(i);
  return aReturnHandle;
}

void SaveContainerCbf::_setHandle(int dataId,Handle& handle)
{
  AutoMutex aLock(m_lock);
  std::pair<dataId2cbfHandle::iterator,bool> result = 
    m_cbfs.insert(std::pair<int,Handle>(dataId,handle));
  if(!result.second)		// It can happend if _open failed
    {
      if(result.first->second.format == CtSaving::CBFMiniHeader)
	{
	  free(result.first->second.data_buffer);
	  free(result.first->second.header_data);
	}
      else
	cbf_free_handle(result.first->second.handle);
      result.first->second = handle;
    }
}
