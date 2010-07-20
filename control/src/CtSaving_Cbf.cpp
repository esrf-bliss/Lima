#include <ctype.h>

#include "CtSaving_Cbf.h"
using namespace lima;

static const char *DEFAULT_CATEGORY = "Misc";
static const char LIMA_HEADER_KEY_SEPARATOR = '/';

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
  else
    openFlags[0] = 'w';
  
  if(stdOpenflags & std::ios_base::binary)
    openFlags[1] = 'b',openFlags[2] = '+',openFlags[3] = '\0';
  else
    openFlags[1] = '+',openFlags[2] = '\0';


  DEB_TRACE() << "Open file name: " << filename << " with open flags: " << openFlags;
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

  char imageBuffer[64];
  snprintf(imageBuffer,sizeof(imageBuffer),"image_%d",m_written_frames);
  
  cbf_failnez(cbf_new_datablock (m_cbf, imageBuffer));

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
					 aData.size()/aData.depth(),
					 "little_endian",
					 aData.width,
					 aData.height,
					 0,
					 0));

  cbf_failnez(cbf_write_file(m_cbf,m_fout,0,CBF,MSG_DIGEST|MIME_HEADERS,0));
  return 0;
}
