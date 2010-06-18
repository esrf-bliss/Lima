#include <ctype.h>

#include "CtSaving_Cbf.h"
using namespace lima;

static const char *DEFAULT_CATEGORY = "Misc";
static const char LIMA_HEADER_KEY_SEPARATOR = '/';

SaveContainerCbf::SaveContainerCbf(CtSaving &aCtSaving) :
  CtSaving::SaveContainer(aCtSaving),
  m_fout(NULL)
{
  DEB_CONSTRUCTOR();
}

SaveContainerCbf::~SaveContainerCbf()
{
  DEB_DESTRUCTOR();
  _close();
}

bool SaveContainerCbf::_open(const std::string &filename,
			     std::_Ios_Openmode stdOpenflags)
{
  DEB_MEMBER_FUNCT();
  char openFlags[8];
  if(stdOpenflags & std::ios_base::app)
    openFlags[0] = 'a';
  else if(stdOpenflags & std::ios_base::trunc)
    openFlags[0] = 'w';
  
  if(stdOpenflags & std::ios_base::binary)
    openFlags[1] = 'b',openFlags[2] = '+',openFlags[3] = '\0';
  else
    openFlags[1] = '+',openFlags[2] = '\0';


  m_fout = fopen(filename.c_str(),openFlags);
  if(m_fout)
    cbf_make_handle(&m_cbf);
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

  cbf_free_handle(m_cbf);
  fclose(m_fout);
  m_fout = NULL;
}

void SaveContainerCbf::_writeFile(Data &aData,
				  CtSaving::HeaderMap &aHeader,
				  CtSaving::FileFormat)
{
  if(_writeCbfHeader(aData,aHeader))
    throw LIMA_CTL_EXC(Error,"Something went wrong during CBF header writing");

  if(_writeCbfData(aData))
    throw LIMA_CTL_EXC(Error,"Something went wrong during CBF data writing");
}

int SaveContainerCbf::_writeCbfHeader(Data &aData,
				      CtSaving::HeaderMap &aHeader)
{
  DEB_MEMBER_FUNCT();

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

      if(previousCategory != currentCategory)
	{
	  previousCategory = currentCategory;
	  cbf_failnez(cbf_new_category(m_cbf,currentCategory.c_str()));
	}
      cbf_failnez(cbf_new_column(m_cbf,key.c_str()));
      cbf_failnez(cbf_set_value(m_cbf,i->second.c_str()));
    }
  return 0;
}

int SaveContainerCbf::_writeCbfData(Data &aData)
{
  DEB_MEMBER_FUNCT();

  char imageBuffer[64];
  snprintf(imageBuffer,sizeof(imageBuffer),"image_%d",m_written_frames);

  cbf_failnez(cbf_new_category		(m_cbf, "array_data"));
  cbf_failnez(cbf_new_column		(m_cbf, "array_id"));
  cbf_failnez(cbf_set_value		(m_cbf, imageBuffer));
  cbf_failnez(cbf_new_column		(m_cbf, "binary_id"));
  cbf_failnez(cbf_set_integervalue	(m_cbf, m_written_frames));
  cbf_failnez(cbf_new_column		(m_cbf, "data"));

  cbf_failnez(cbf_set_integerarray_wdims(m_cbf,
					 CBF_BYTE_OFFSET,
					 m_written_frames,
					 aData.data(),
					 aData.depth(),
					 aData.is_signed(),
					 aData.size(),
					 "little_endian",
					 aData.height,
					 aData.width,
					 0,
					 0));
  cbf_failnez(cbf_write_file(m_cbf,m_fout,0,CBF,MSG_DIGEST|MIME_HEADERS,0));
  return 0;
}
