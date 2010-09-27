#include <sstream>
#include <unistd.h>

#ifdef __linux__ 
#include <sys/statvfs.h>
#endif

#include "CtSaving.h"
#include "CtSaving_Edf.h"

#ifdef WITH_CBF_SAVING
#include "CtSaving_Cbf.h"
#endif

#include "TaskMgr.h"
#include "SinkTask.h"

using namespace lima;

static const char DIR_SEPARATOR = '/';

/** @brief save task class
 */
class CtSaving::_SaveTask : public SinkTaskBase
{
    DEB_CLASS_NAMESPC(DebModControl,"Saving Task","Control");
public:
  _SaveTask(CtSaving::SaveContainer &save_cnt) : SinkTaskBase(),m_container(save_cnt) {}
  virtual void process(Data &aData)
  {
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(aData);

    m_container.writeFile(aData,m_header);
  }

  CtSaving::HeaderMap	 m_header;
private:
  SaveContainer &m_container;
};
/** @brief save callback
 */
class CtSaving::_SaveCBK : public TaskEventCallback
{
    DEB_CLASS_NAMESPC(DebModControl,"CtSaving::_SaveCBK","Control");
public:
  _SaveCBK(CtSaving &aCtSaving) : m_saving(aCtSaving) {}
  virtual void finished(Data &aData)
  {
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(aData);

    m_saving._save_finished(aData);
  }
private:
  CtSaving &m_saving;
};

/** @brief Parameters default constructor
 */
CtSaving::Parameters::Parameters()
  : nextNumber(0), fileFormat(RAW), savingMode(Manual), 
    overwritePolicy(Abort),indexFormat("%04d"),framesPerFile(1)
{
}

//@brief constructor
CtSaving::CtSaving(CtControl &aCtrl) :
  m_ctrl(aCtrl),
  m_ready_flag(true),
  m_end_cbk(NULL)
{
  DEB_CONSTRUCTOR();

  m_save_cnt = new SaveContainerEdf(*this);
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
  _check_if_multi_frame_per_file_allowed(pars.fileFormat,pars.framesPerFile);
  _create_save_cnt(pars.fileFormat);
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

void CtSaving::setFormat(FileFormat format)
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  _create_save_cnt(format);
  m_pars.fileFormat = format;

  DEB_RETURN() << DEB_VAR1(format);
}
void CtSaving::_create_save_cnt(FileFormat format)
{
  if(format != m_pars.fileFormat)
    {
      // wait until the container is no more used
      while(!m_ready_flag) m_cond.wait();

      switch(format)
	{
	case CBFFormat :
#ifndef WITH_CBF_SAVING
	throw LIMA_CTL_EXC(NotSupported,"Lima is not compiled with the cbf saving option, not managed");  
#endif
	case RAW:
	case EDF:
	  delete m_save_cnt;break;
	default:
	  throw LIMA_CTL_EXC(NotSupported,"File format not yet managed");
	}

      switch(format)
	{
	case RAW:
	case EDF:
	  m_save_cnt = new SaveContainerEdf(*this);break;
#ifdef WITH_CBF_SAVING
	case CBFFormat:
	  m_save_cnt = new SaveContainerCbf(*this);
	  m_pars.framesPerFile = 1;
	  break;
#endif
	default:
	  break;
	}
    }
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
  _check_if_multi_frame_per_file_allowed(m_pars.fileFormat,frames_per_file);
  m_pars.framesPerFile = frames_per_file;
}

void CtSaving::_check_if_multi_frame_per_file_allowed(FileFormat format,int frame_per_file) const
{
  switch(format)
    {
#ifdef WITH_CBF_SAVING
    case CBFFormat :
      if(frame_per_file > 1)
	throw LIMA_CTL_EXC(InvalidValue,"CBF file format does not support multi frame per file");
      break;
#endif
    default:
      break;
    }
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

  AutoMutex aLock(m_cond.mutex());

  while(!m_ready_flag)
    m_cond.wait();

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
  CtControl::Status status;
  m_ctrl.getStatus(status);
  if(status.AcquisitionStatus == AcqFault)
    {
      DEB_WARNING() << "Skip saving data: " << aData;
      return;
    }
  AutoMutex aLock(m_cond.mutex());
  switch(m_pars.savingMode)
    {
    case CtSaving::AutoFrame:
      {
	if(m_ready_flag && m_last_frameid_saved == aData.frameNumber - 1)
	  {
	    _SaveTask *aSaveTaskPt = new _SaveTask(*m_save_cnt);
	    std::map<long,HeaderMap>::iterator aHeaderIter = m_frame_headers.find(aData.frameNumber);
	    if(aHeaderIter != m_frame_headers.end())
	      _takeHeader(aHeaderIter,aSaveTaskPt->m_header);
	    else
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

  resetLastFrameNb();

  m_save_cnt->clear();
  AutoMutex aLock(m_cond.mutex());
  m_frame_headers.clear();
  m_common_header.clear();	// @fix Should we clear common header???
  m_frame_datas.clear();
  
}

void CtSaving::_post_save_task(Data &aData,_SaveTask *aSaveTaskPt)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(aData);

  aSaveTaskPt->setEventCallback(m_saving_cbk);

  TaskMgr *aSavingMgrPt = new TaskMgr();
  aSavingMgrPt->addSinkTask(0,aSaveTaskPt);
  aSaveTaskPt->unref();
  aSavingMgrPt->setInputData(aData);

  PoolThreadMgr::get().addProcess(aSavingMgrPt);
}

void CtSaving::_save_finished(Data &aData)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(aData);

  //@todo check if the frame is still available
  if(m_end_cbk)
    m_end_cbk->finished(aData);

  AutoMutex aLock(m_cond.mutex());

  int next_frame = m_last_frameid_saved + 1;
  switch(m_pars.savingMode)
    {
    case CtSaving::AutoFrame:
      {
	FrameMap::iterator nextDataIter = m_frame_datas.find(next_frame);
	if(nextDataIter != m_frame_datas.end())
	  {
	    _SaveTask *aSaveTaskPt = new _SaveTask(*m_save_cnt);
	    std::map<long,HeaderMap>::iterator aHeaderIter = m_frame_headers.find(nextDataIter->first);
	    if(aHeaderIter != m_frame_headers.end())
	      _takeHeader(aHeaderIter,aSaveTaskPt->m_header);
            else
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
	FrameMap::iterator nextDataIter = m_frame_datas.find(next_frame);
	if(nextDataIter != m_frame_datas.end())
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
/** @brief this methode set the error saving status in CtControl
 */
void CtSaving::_setSavingError(CtControl::ErrorCode anErrorCode)
{
  DEB_MEMBER_FUNCT();
  AutoMutex aLock(m_ctrl.m_cond.mutex());
  if(m_ctrl.m_status.AcquisitionStatus != AcqFault)
    {
      m_ctrl.m_status.AcquisitionStatus = AcqFault;
      m_ctrl.m_status.Error = anErrorCode;
      
      DEB_ERROR() << DEB_VAR1(m_ctrl.m_status);
    }

  aLock.unlock();

  m_ctrl.stopAcq();

  DEB_TRACE() << "Setting ready flag";
  aLock = AutoMutex(m_cond.mutex());
  m_ready_flag = true;
  m_cond.signal();

}

CtSaving::SaveContainer::SaveContainer(CtSaving &aCtSaving) :
  m_written_frames(0),m_saving(aCtSaving),m_statistic_size(16),m_file_opened(false)
{
  DEB_CONSTRUCTOR();
}

CtSaving::SaveContainer::~SaveContainer()
{
  DEB_DESTRUCTOR();
}

void CtSaving::SaveContainer::writeFile(Data &aData,HeaderMap &aHeader)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(aData,aHeader);

  struct timeval start_write;
  gettimeofday(&start_write, NULL);

  CtSaving::Parameters pars;
  m_saving.getParameters(pars);

  open(pars);
  try
    {
      _writeFile(aData,aHeader,pars.fileFormat);
    }
  catch(std::ios_base::failure &error)
    {
      DEB_ERROR() << "Write failed :" << error.what();
#ifdef __linux__ 
      /**       struct statvfs {
		unsigned long  f_bsize;    // file system block size 
		unsigned long  f_frsize;   //fragment size
		fsblkcnt_t     f_blocks;   // size of fs in f_frsize units
		fsblkcnt_t     f_bfree;    // # free blocks 
		fsblkcnt_t     f_bavail;   // # free blocks for non-root
		fsfilcnt_t     f_files;    // # inodes
		fsfilcnt_t     f_ffree;    // # free inodes
		fsfilcnt_t     f_favail;   // # free inodes for non-root
		unsigned long  f_fsid;     // file system ID
		unsigned long  f_flag;     //mount flags
		unsigned long  f_namemax;  // maximum filename length 
      */
      //Check if disk full
      struct statvfs vfs;
      if(!statvfs(pars.directory.c_str(),&vfs))
	{
	  if(vfs.f_favail < 1024 || vfs.f_bavail < 1024)
	    {
	      m_saving._setSavingError(CtControl::SaveDiskFull);
	      DEB_ERROR() << "Disk full!!!";
	      close();
	      return;
	    }
	};

  
#endif
      m_saving._setSavingError(CtControl::SaveUnknownError);
      close();
      return;
    }

  if(++m_written_frames == pars.framesPerFile)
    close();

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

void CtSaving::SaveContainer::setStatisticSize(int aSize)
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

void CtSaving::SaveContainer::getStatistic(std::list<double> &aReturnList) const
{
  AutoMutex aLock = AutoMutex(m_cond.mutex());
  for(std::list<double>::const_iterator i = m_statistic_list.begin();
      i != m_statistic_list.end();++i)
    aReturnList.push_back(*i);
}

void CtSaving::SaveContainer::clear()
{
  AutoMutex aLock(m_cond.mutex());
  m_statistic_list.clear();
  this->close();
}

void CtSaving::SaveContainer::open(const CtSaving::Parameters &pars)
{
  DEB_MEMBER_FUNCT();

  if(!m_file_opened)
    {
      char idx[64];
      snprintf(idx,sizeof(idx),pars.indexFormat.c_str(),pars.nextNumber);

      std::string aFileName = pars.directory + DIR_SEPARATOR + pars.prefix + idx + pars.suffix;
      DEB_TRACE() << DEB_VAR1(aFileName);

      if(pars.overwritePolicy == Abort && 
	 !access(aFileName.c_str(),R_OK))
	{
	  m_saving._setSavingError(CtControl::SaveOverwriteError);
	  std::string output;
	  output = "Try to over write file: " + aFileName;
	  DEB_ERROR() << output;
	  throw LIMA_CTL_EXC(Error, output.c_str());
	}
      std::_Ios_Openmode openFlags = std::ios_base::out | std::ios_base::binary;
      if(pars.overwritePolicy == Append)
	openFlags |= std::ios_base::app;
      else if(pars.overwritePolicy == Overwrite)
	openFlags |= std::ios_base::trunc;

      for(int nbTry = 0;nbTry < 5;++nbTry)
	{
	  bool succeed = false;
	  try {
	    succeed = _open(aFileName,openFlags);
	  } catch (std::ios_base::failure &error) {
	    DEB_ERROR() << "Failure opening " << aFileName << ":" 
			<< error.what();
	  }

	  if(!succeed)
	    {
	      std::string output;

	      if(access(pars.directory.c_str(),W_OK))
		{
		  m_saving._setSavingError(CtControl::SaveAccessError);
		  output = "Can not write in directory: " + pars.directory;
		  DEB_ERROR() << output;
		  throw LIMA_CTL_EXC(Error,output.c_str());
		}
	    }
	  else
	    {
	      DEB_TRACE() << "Open file: " << aFileName;
	      m_file_opened = true;
	      break;
	    }
	}
    }
}

void CtSaving::SaveContainer::close()
{
  DEB_MEMBER_FUNCT();
  _close();
  m_file_opened = false;
  m_written_frames = 0;
  long idx;
  m_saving.getNextNumber(idx);
  m_saving.setNextNumber(idx + 1);
}
