#include <sstream>

#include "CtSaving.h"

#include "TaskMgr.h"
#include "SinkTask.h"

using namespace lima;

static const char DIR_SEPARATOR = '/';

class CtSaving::_SaveContainer
{
    DEB_CLASS_NAMESPC(DebModControl,"Saving Container","Control");
public:
  _SaveContainer(CtSaving&);
  ~_SaveContainer();

  void writeFile(Data&,CtSaving::HeaderMap &);
  void setStatisticSize(int aSize);
  void getStatistic(std::list<double>&) const;
  void clear();

private:
  CtSaving		&m_saving;
  int			m_written_frames;
  std::ofstream		m_fout;
  std::list<double>	m_statistic_list;
  int			m_statistic_size;
  mutable Cond		m_cond;

  void		_open();
  void		_close();
  void		_writeEdfHeader(Data&,CtSaving::HeaderMap&);
};
/** @brief save task class
 */
class CtSaving::_SaveTask : public SinkTaskBase
{
    DEB_CLASS_NAMESPC(DebModControl,"Saving Task","Control");
public:
  _SaveTask(CtSaving::_SaveContainer &save_cnt) : SinkTaskBase(),m_container(save_cnt) {}
  virtual void process(Data &aData)
  {
    m_container.writeFile(aData,m_header);
  }

  CtSaving::HeaderMap	 m_header;
private:
  _SaveContainer &m_container;
};
/** @brief save callback
 */
class CtSaving::_SaveCBK : public TaskEventCallback
{
public:
  _SaveCBK(CtSaving &aCtSaving) : m_saving(aCtSaving) {}
  virtual void finished(Data &aData)
  {
    m_saving._save_finished(aData);
  }
private:
  CtSaving &m_saving;
};

//@brief constructor
CtSaving::CtSaving(CtControl &aCtrl) :
  m_ctrl(aCtrl),
  m_ready_flag(true),
  m_end_cbk(NULL)
{
  DEB_CONSTRUCTOR();

  m_save_cnt = new _SaveContainer(*this);
  m_saving_cbk = new _SaveCBK(*this);
  resetLastFrameNb();
}

//@brief destructor
CtSaving::~CtSaving()
{
  DEB_DESTRUCTOR();

  delete m_save_cnt;
  m_saving_cbk->unref();
  setEndCallback(NULL);
}

void CtSaving::setParameters(const CtSaving::Parameters &pars)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(pars);

  AutoMutex aLock(m_cond.mutex());
  m_pars = pars;
}

void CtSaving::getParameters(CtSaving::Parameters &pars) const
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  pars = m_pars;

  DEB_RETURN() << DEB_VAR1(pars);
}

void CtSaving::setDirectory(const std::string &directory)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(directory);

  AutoMutex aLock(m_cond.mutex());
  m_pars.directory = directory;
}

void CtSaving::getDirectory(std::string& directory) const
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  directory = m_pars.directory;

  DEB_RETURN() << DEB_VAR1(directory);
}

void CtSaving::setPrefix(const std::string &prefix)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(prefix);

  AutoMutex aLock(m_cond.mutex());
  m_pars.prefix = prefix;
}
void CtSaving::getPrefix(std::string& prefix) const
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  prefix = m_pars.prefix;

  DEB_RETURN() << DEB_VAR1(prefix);
}

void CtSaving::setSuffix(const std::string &suffix)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(suffix);

  AutoMutex aLock(m_cond.mutex());
  m_pars.suffix = suffix;
}
void CtSaving::getSuffix(std::string& suffix) const
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  suffix = m_pars.suffix;

  DEB_RETURN() << DEB_VAR1(suffix);
}

void CtSaving::setNextNumber(long number)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(number);

  AutoMutex aLock(m_cond.mutex());
  m_pars.nextNumber = number;
}
void CtSaving::getNextNumber(long& number) const
{
  DEB_MEMBER_FUNCT();
  
  AutoMutex aLock(m_cond.mutex());
  number = m_pars.nextNumber;

  DEB_RETURN() << DEB_VAR1(number);
}

void CtSaving::setFormat(const FileFormat &format)
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  m_pars.fileFormat = format;

  DEB_RETURN() << DEB_VAR1(format);
}
void CtSaving::getFormat(FileFormat& format) const
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  format = m_pars.fileFormat;

  DEB_RETURN() << DEB_VAR1(format);
}

void CtSaving::setSavingMode(SavingMode mode)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(mode);

  AutoMutex aLock(m_cond.mutex());
  m_pars.savingMode = mode;
}
void CtSaving::getSavingMode(SavingMode& mode) const
{ 
  DEB_MEMBER_FUNCT();
  
  AutoMutex aLock(m_cond.mutex());
  mode = m_pars.savingMode;
  
  DEB_RETURN() << DEB_VAR1(mode);
}

void CtSaving::setOverwritePolicy(OverwritePolicy policy)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(policy);

  AutoMutex aLock(m_cond.mutex());
  m_pars.overwritePolicy = policy;
}

void CtSaving::getOverwritePolicy(OverwritePolicy& policy) const
{
  DEB_MEMBER_FUNCT();
  
  AutoMutex aLock(m_cond.mutex());
  policy = m_pars.overwritePolicy;

  DEB_RETURN() << DEB_VAR1(policy);
}

void CtSaving::setFramesPerFile(unsigned long frames_per_file)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(frames_per_file);

  AutoMutex aLock(m_cond.mutex());
  m_pars.framesPerFile = frames_per_file;
}

void CtSaving::getFramePerFile(unsigned long& frames_per_file) const
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  frames_per_file = m_pars.framesPerFile;

  DEB_RETURN() << DEB_VAR1(frames_per_file);
}

void CtSaving::resetCommonHeader()
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  m_common_header.clear();
}

void CtSaving::setCommonHeader(const HeaderMap &header)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(header);

  AutoMutex aLock(m_cond.mutex());
  m_common_header = header;
}

void CtSaving::updateCommonHeader(const HeaderMap &header)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(header);

  AutoMutex aLock(m_cond.mutex());
  m_common_header.insert(header.begin(),header.end());
}
void CtSaving::getCommonHeader(HeaderMap& header) const
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  header = HeaderMap(m_common_header);

  DEB_RETURN() << DEB_VAR1(header);
}

void CtSaving::addToCommonHeader(const HeaderValue &value)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(value);

  AutoMutex aLock(m_cond.mutex());
  m_common_header.insert(value);
}

void CtSaving::addToFrameHeader(long frame_nr,const HeaderValue &value)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(frame_nr,value);

  AutoMutex aLock(m_cond.mutex());
  m_frame_headers[frame_nr].insert(value);
}

void CtSaving::updateFrameHeader(long frame_nr,const HeaderMap &header)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(frame_nr,header);

  AutoMutex aLock(m_cond.mutex());
  HeaderMap &frameHeader = m_frame_headers[frame_nr];
  frameHeader.insert(header.begin(),header.end());
}

void CtSaving::validateFrameHeader(long frame_nr)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(frame_nr);

  AutoMutex aLock(m_cond.mutex());
  switch(m_pars.savingMode)
    {
    case CtSaving::AutoHeader:
      {
	FrameMap::iterator frame_iter = m_frame_datas.find(frame_nr);
	if(frame_iter != m_frame_datas.end() &&
	   m_ready_flag && m_last_frameid_saved == frame_nr - 1)
	  {
	    
	    _SaveTask *aSaveTaskPt = new _SaveTask(*m_save_cnt);
	    _get_common_header(aSaveTaskPt->m_header);
	    std::map<long,HeaderMap>::iterator headerIter =
	      m_frame_headers.find(frame_nr);

	    if(headerIter != m_frame_headers.end())
	      {
		aSaveTaskPt->m_header.insert(headerIter->second.begin(),
					     headerIter->second.end());
		m_frame_headers.erase(headerIter);
	      }
	    Data aData = frame_iter->second;
	    m_frame_datas.erase(frame_iter);
	    m_ready_flag = false,m_last_frameid_saved = frame_nr;
	    aLock.unlock();
	    _post_save_task(aData,aSaveTaskPt);
	    break;
	  }
      }
    default:
      break;
    }
}

	
void CtSaving::getFrameHeader(long frame_nr, HeaderMap& header) const
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(frame_nr);

  AutoMutex aLock(m_cond.mutex());
  std::map<long,HeaderMap>::const_iterator i = m_frame_headers.find(frame_nr);
  if(i != m_frame_headers.end())
    header.insert(i->second.begin(),i->second.end());

  DEB_RETURN() << DEB_VAR1(header);
}

void CtSaving::takeFrameHeader(long frame_nr, HeaderMap& header)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(frame_nr);

  AutoMutex aLock(m_cond.mutex());
  std::map<long,HeaderMap>::iterator i = m_frame_headers.find(frame_nr);
  if(i != m_frame_headers.end())
    {
      header = i->second;
      m_frame_headers.erase(i);
    }
  
  DEB_RETURN() << DEB_VAR1(header);
}

void CtSaving::removeFrameHeader(long frame_nr)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(frame_nr);

  AutoMutex aLock(m_cond.mutex());
  m_frame_headers.erase(frame_nr);
}

void CtSaving::removeAllFrameHeaders()
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  m_frame_headers.clear();
}

// Private methodes

void CtSaving::_get_common_header(HeaderMap &header)
{
  header.insert(m_common_header.begin(),m_common_header.end());
}
void CtSaving::_takeHeader(std::map<long,HeaderMap>::iterator &headerIter, HeaderMap& header)
{
   _get_common_header(header);
  HeaderMap &aFrameHeaderMap = headerIter->second;
  header.insert(aFrameHeaderMap.begin(),aFrameHeaderMap.end());
  m_frame_headers.erase(headerIter);
}

void CtSaving::resetLastFrameNb()
{
  DEB_MEMBER_FUNCT();

  m_last_frameid_saved = -1;
}

void CtSaving::setEndCallback(TaskEventCallback *aCbkPt)
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  if(m_end_cbk)
    m_end_cbk->unref();
  m_end_cbk = aCbkPt;
  if(m_end_cbk)
    m_end_cbk->ref();
}

void CtSaving::frameReady(Data &aData)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(aData);

  AutoMutex aLock(m_cond.mutex());
  switch(m_pars.savingMode)
    {
    case CtSaving::AutoFrame:
      {
	if(m_ready_flag && m_last_frameid_saved == aData.frameNumber - 1)
	  {
	    _SaveTask *aSaveTaskPt = new _SaveTask(*m_save_cnt);
	    _get_common_header(aSaveTaskPt->m_header);
	    m_ready_flag = false,m_last_frameid_saved = aData.frameNumber;
	    aLock.unlock();
	    _post_save_task(aData,aSaveTaskPt);
	  }
	else
	  m_frame_datas.insert(std::pair<long,Data>(aData.frameNumber,aData));
      }
      break;
    case CtSaving::AutoHeader:
      {
	std::map<long,HeaderMap>::iterator aHeaderIter = m_frame_headers.find(aData.frameNumber);
	if(aHeaderIter != m_frame_headers.end() &&
	   m_ready_flag && m_last_frameid_saved == aData.frameNumber - 1)
	  {
	    _SaveTask *aSaveTaskPt = new _SaveTask(*m_save_cnt);
	    _takeHeader(aHeaderIter,aSaveTaskPt->m_header);
	    m_ready_flag = false,m_last_frameid_saved = aData.frameNumber;
	    aLock.unlock();
	    _post_save_task(aData,aSaveTaskPt);
	  }
	else
	  m_frame_datas.insert(std::pair<long,Data>(aData.frameNumber,aData));
      }
      break;
    default:
      m_frame_datas.insert(std::pair<long,Data>(aData.frameNumber,aData));
      break;
    }
}

void CtSaving::getWriteTimeStatistic(std::list<double> &aReturnList) const
{
  DEB_MEMBER_FUNCT();

  m_save_cnt->getStatistic(aReturnList);
}

void CtSaving::setStatisticHistorySize(int aSize)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(aSize);

  m_save_cnt->setStatisticSize(aSize);
}

void CtSaving::clear()
{
  DEB_MEMBER_FUNCT();

  m_save_cnt->clear();
  AutoMutex aLock(m_cond.mutex());
  m_frame_headers.clear();
  m_common_header.clear();	// @fix Should we clear common header???
  m_frame_datas.clear();
  while(!m_ready_flag)
    m_cond.wait();
  resetLastFrameNb();
}

void CtSaving::_post_save_task(Data &aData,_SaveTask *aSaveTaskPt)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(aData);

  aSaveTaskPt->setEventCallback(m_saving_cbk);

  TaskMgr *aSavingMgrPt = new TaskMgr();
  aSavingMgrPt->addSinkTask(0,aSaveTaskPt);
  aSavingMgrPt->setInputData(aData);

  PoolThreadMgr::get().addProcess(aSavingMgrPt);
}

void CtSaving::_save_finished(Data &aData)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(aData);

  //@todo check if the frame is still available
  AutoMutex aLock(m_cond.mutex());
  if(m_end_cbk)
    m_end_cbk->finished(aData);
  switch(m_pars.savingMode)
    {
    case CtSaving::AutoFrame:
      {
	FrameMap::iterator nextDataIter = m_frame_datas.begin();
	if(nextDataIter != m_frame_datas.end() && m_last_frameid_saved == nextDataIter->first - 1)
	  {
	    _SaveTask *aSaveTaskPt = new _SaveTask(*m_save_cnt);
	    _get_common_header(aSaveTaskPt->m_header);
	    m_last_frameid_saved = nextDataIter->first;
	    Data aData = nextDataIter->second;
	    m_frame_datas.erase(nextDataIter);
	    aLock.unlock();
	    _post_save_task(aData,aSaveTaskPt);
	  }
	else
	  {
	    m_ready_flag = true;
	    m_cond.signal();
	  }
      }
      break;
    case CtSaving::AutoHeader:
      {
	FrameMap::iterator nextDataIter = m_frame_datas.begin();
	if(nextDataIter != m_frame_datas.end() && 
	   m_last_frameid_saved == nextDataIter->first - 1)
	  {
	    std::map<long,HeaderMap>::iterator aHeaderIter = m_frame_headers.find(nextDataIter->first);
	    if(aHeaderIter != m_frame_headers.end())
	      {
		_SaveTask *aSaveTaskPt = new _SaveTask(*m_save_cnt);
		_takeHeader(aHeaderIter,aSaveTaskPt->m_header);
		m_last_frameid_saved = nextDataIter->first;
		Data aData = nextDataIter->second;
		m_frame_datas.erase(nextDataIter);
		aLock.unlock();
		_post_save_task(aData,aSaveTaskPt);
		break;
	      }
	  }
      }
    default:
      m_ready_flag = true;
      m_cond.signal();
    }
}


/** @brief saving container
 *
 *  This class manage file saving
 */
CtSaving::_SaveContainer::_SaveContainer(CtSaving &aCtSaving) :
  m_saving(aCtSaving),m_written_frames(0),m_statistic_size(16)
{
  DEB_CONSTRUCTOR();
}

CtSaving::_SaveContainer::~_SaveContainer()
{
  DEB_DESTRUCTOR();
}

void CtSaving::_SaveContainer::_open()
{
  DEB_MEMBER_FUNCT();

  if(!m_fout.is_open())
    {
      CtSaving::Parameters pars;
      m_saving.getParameters(pars);

      std::ostringstream idx;
      idx.width(4);
      idx.fill('0');
      idx << pars.nextNumber;

      std::string aFileName = pars.directory + DIR_SEPARATOR + pars.prefix + idx.str() + pars.suffix;

      DEB_TRACE() << "Open file: " << aFileName;

      m_fout.open(aFileName.c_str(),
		  std::ios_base::out | std::ios_base::binary);
    }
}

void CtSaving::_SaveContainer::_close()
{
  DEB_MEMBER_FUNCT();
  
  DEB_TRACE() << "Close current file";

  m_fout.close();
  m_written_frames = 0;
  long idx;
  m_saving.getNextNumber(idx);
  m_saving.setNextNumber(idx + 1);
}

void CtSaving::_SaveContainer::writeFile(Data &aData,HeaderMap &aHeader)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(aData,aHeader);

  struct timeval start_write;
  gettimeofday(&start_write, NULL);

  CtSaving::Parameters pars;
  m_saving.getParameters(pars);

  _open();

  if(pars.fileFormat == CtSaving::EDF)
    _writeEdfHeader(aData,aHeader);
  
  m_fout.write((char*)aData.data(),aData.size());
  
  if(++m_written_frames == pars.framesPerFile)
    _close();

  struct timeval end_write;
  gettimeofday(&end_write, NULL);


  double diff = (end_write.tv_sec - start_write.tv_sec) + 
    (end_write.tv_usec - start_write.tv_usec) / 1e6;
  
  DEB_TRACE() << "Write took : " << diff << "s";

  AutoMutex aLock = AutoMutex(m_cond.mutex());
  if(long(m_statistic_list.size()) == m_statistic_size)
    m_statistic_list.pop_front();
  m_statistic_list.push_back(diff);
}
void CtSaving::_SaveContainer::setStatisticSize(int aSize)
{
  AutoMutex aLock = AutoMutex(m_cond.mutex());
  if(long(m_statistic_list.size()) > aSize)
    {
      int aDiffSize = m_statistic_list.size() - aSize;
      for(std::list<double>::iterator i = m_statistic_list.begin();
	  aDiffSize;--aDiffSize)
	i = m_statistic_list.erase(i);
    }
  m_statistic_size = aSize;
}

void CtSaving::_SaveContainer::getStatistic(std::list<double> &aReturnList) const
{
  AutoMutex aLock = AutoMutex(m_cond.mutex());
  for(std::list<double>::const_iterator i = m_statistic_list.begin();
      i != m_statistic_list.end();++i)
    aReturnList.push_back(*i);
}

void CtSaving::_SaveContainer::clear()
{
  AutoMutex aLock(m_cond.mutex());
  m_statistic_list.clear();
  _close();
}

void CtSaving::_SaveContainer::_writeEdfHeader(Data &aData,HeaderMap &aHeader)
{
  DEB_MEMBER_FUNCT();

  time_t ctime_now;
  time(&ctime_now);

  struct timeval tod_now;
  gettimeofday(&tod_now, NULL);

  char time_str[64];
  ctime_r(&ctime_now, time_str);
  time_str[strlen(time_str) - 1] = '\0';
	
  int image_nb = m_written_frames + 1;

  char aBuffer[512];
  long aStartPosition = m_fout.tellp();
  m_fout << "{\n";

  snprintf(aBuffer,sizeof(aBuffer),"HeaderID = EH:%06u:000000:000000 ;\n", image_nb);
  m_fout << aBuffer;

  m_fout << "ByteOrder = LowByteFirst ;\n";
  const char *aStringType = NULL;
  switch(aData.type)
    {
    case Data::UINT8:	aStringType = "UnsignedByte";break;
    case Data::INT8:	aStringType = "SignedByte";break;
    case Data::UINT16:	aStringType = "UnsignedShort";break;
    case Data::INT16:	aStringType = "SignedShort";break;
    case Data::UINT32:	aStringType = "UnsignedInteger";break;
    case Data::INT32:	aStringType = "SignedInteger";break;
    case Data::UINT64:	aStringType = "Unsigned64";break;
    case Data::INT64:	aStringType = "Signed64";break;
    case Data::FLOAT:	aStringType = "FloatValue";break;
    case Data::DOUBLE:	aStringType = "DoubleValue";break;
    default:
      break;		// @todo ERROR has to be manage
    }
  m_fout << "DataType = " << aStringType << " ;\n";

  m_fout << "Size = " << aData.size() << " ;\n";
  m_fout << "Dim_1 = " << aData.width << " ;\n";
  m_fout << "Dim_2 = " << aData.height << " ;\n";

  m_fout << "acq_frame_nb = " << aData.frameNumber << " ;\n";
  m_fout << "time = " << time_str << " ;\n";

  snprintf(aBuffer,sizeof(aBuffer),"time_of_day = %ld.%06ld ;\n",tod_now.tv_sec, tod_now.tv_usec);
  m_fout << aBuffer;

  snprintf(aBuffer,sizeof(aBuffer),"time_of_frame = %.6f ;\n",aData.timestamp);
  m_fout << aBuffer;

  //@todo m_fout << "valid_pixels = " << aData.validPixels << " ;\n";
  
  
  aData.header.lock();
  Data::HeaderContainer::Header &aDataHeader = aData.header.header();
  for(Data::HeaderContainer::Header::iterator i = aDataHeader.begin();i != aDataHeader.end();++i)
    m_fout << i->first << " = " << i->second << " ;\n";
  aData.header.unlock();

  for(HeaderMap::iterator i = aHeader.begin(); i != aHeader.end();++i)
    m_fout << i->first << " = " << i->second << " ;\n";
  
  long aEndPosition = m_fout.tellp();
  
  long lenght = aEndPosition - aStartPosition;
  long finalHeaderLenght = (lenght + 511) & ~511; // 512 alignment
  snprintf(aBuffer,sizeof(aBuffer),"%*s}\n",int(finalHeaderLenght - lenght - 2),"");
  m_fout << aBuffer;
}
