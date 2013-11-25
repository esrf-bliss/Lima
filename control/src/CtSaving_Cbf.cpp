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
#include <ctype.h>

#include "CtSaving_Cbf.h"
#include "SinkTask.h"

using namespace lima;

static const char *DEFAULT_CATEGORY = "Misc";
static const char LIMA_HEADER_KEY_SEPARATOR = '/';

class SaveContainerCbf::Compression : public SinkTaskBase
{
  DEB_CLASS_NAMESPC(DebModControl,"Compression Task","Control");

  SaveContainerCbf& 	m_container;
  CtSaving::HeaderMap 	m_header;

  int _fillHeader(Data &aData,CtSaving::HeaderMap &aHeader,cbf_handle cbf)
  {
    DEB_MEMBER_FUNCT();
  
    cbf_failnez(cbf_new_datablock(cbf, "image_0"));

    aData.header.lock();
    Data::HeaderContainer::Header &aDataHeader = aData.header.header();
    aHeader.insert(aDataHeader.begin(),aDataHeader.end());
    aData.header.unlock();

    std::string previousCategory;

    for(CtSaving::HeaderMap::iterator i = aHeader.begin();
	i != aHeader.end();++i)
      {
	size_t found = i->first.find_last_of(LIMA_HEADER_KEY_SEPARATOR);
	std::string currentCategory = i->first.substr(0,found);
	std::string key = i->first.substr(found);
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
    m_container._setHandle(aData.frameNumber,cbf);
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

SaveContainerCbf::SaveContainerCbf(CtSaving::Stream& stream) :
  CtSaving::SaveContainer(stream),
  m_fout(NULL),
  m_lock(MutexAttr::Normal)
{
  DEB_CONSTRUCTOR();
}

SaveContainerCbf::~SaveContainerCbf()
{
  DEB_DESTRUCTOR();
  _close();
}

SinkTaskBase* SaveContainerCbf::getCompressionTask(const CtSaving::HeaderMap &header)
{
  return new Compression(*this,header);
}

bool SaveContainerCbf::_open(const std::string &filename,
			     std::ios_base::openmode stdOpenflags)
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
  m_fout = fopen(filename.c_str(),openFlags);

  return !!m_fout;
}

void SaveContainerCbf::_close()
{
  DEB_MEMBER_FUNCT();
  
  if (!m_fout)
    {
      DEB_TRACE() << "Nothing to do";
      return;
    }

  DEB_TRACE() << "Close current file";

  cbf_free_handle(m_current_cbf);
  fclose(m_fout);
  m_fout = NULL;
}

void SaveContainerCbf::_writeFile(Data &aData,
				  CtSaving::HeaderMap&,
				  CtSaving::FileFormat)
{
  DEB_MEMBER_FUNCT();
  if(_writeCbfData(aData))
    THROW_CTL_ERROR(Error) << "Something went wrong during CBF data writing";
}

void SaveContainerCbf::_clear()
{
  AutoMutex aLock(m_lock);
  dataId2cbfHandle::iterator i = m_cbfs.begin();
  while(i != m_cbfs.end())
    {
      cbf_free_handle(i->second);
      dataId2cbfHandle::iterator previous = i++;
      m_cbfs.erase(previous);
    }
}

int SaveContainerCbf::_writeCbfData(Data &aData)
{
  DEB_MEMBER_FUNCT();
  m_current_cbf = _takeHandle(aData.frameNumber);
  cbf_failnez(cbf_write_file(m_current_cbf,m_fout,0,CBF,MSG_DIGEST|MIME_HEADERS,0));
  return 0;
}

cbf_handle SaveContainerCbf::_takeHandle(int dataId)
{
  AutoMutex aLock(m_lock);
  dataId2cbfHandle::iterator i = m_cbfs.find(dataId);
  cbf_handle aReturnHandle = i->second;
  m_cbfs.erase(i);
  return aReturnHandle;
}

void SaveContainerCbf::_setHandle(int dataId,cbf_handle cbf)
{
  AutoMutex aLock(m_lock);
  std::pair<dataId2cbfHandle::iterator,bool> result = 
    m_cbfs.insert(std::pair<int,cbf_handle>(dataId,cbf));
  if(!result.second)		// It can happend if _open failed
    {
      cbf_free_handle(result.first->second);
      result.first->second = cbf;
    }
}
