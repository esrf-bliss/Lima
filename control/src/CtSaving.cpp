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
#include <cmath>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef __linux__ 
#include <dirent.h>
#include <sys/statvfs.h>
#endif

#include "CtSaving.h"
#include "CtSaving_Edf.h"
#include "CtAcquisition.h"

#ifdef WITH_NXS_SAVING
#include "CtSaving_Nxs.h"
#endif

#ifdef WITH_CBF_SAVING
#include "CtSaving_Cbf.h"
#endif

#ifdef WITH_FITS_SAVING
#include "CtSaving_Fits.h"
#endif

#include "TaskMgr.h"
#include "SinkTask.h"

using namespace lima;

static const char DIR_SEPARATOR = '/';

/** @brief save task class
 */
class CtSaving::Stream::_SaveTask : public SinkTaskBase
{
    DEB_CLASS_NAMESPC(DebModControl,"CtSaving::Stream::_SaveTask","Control");
public:
  _SaveTask(CtSaving::Stream& stream) 
    : SinkTaskBase(), m_stream(stream) {}

  virtual void process(Data &aData)
  {
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(aData);

    m_stream.writeFile(aData, m_header);
  }

  CtSaving::HeaderMap	 m_header;
private:
  CtSaving::Stream& m_stream;
};
/** @brief save callback
 */
class CtSaving::Stream::_SaveCBK : public TaskEventCallback
{
    DEB_CLASS_NAMESPC(DebModControl,"CtSaving::Stream::_SaveCBK","Control");
public:
  _SaveCBK(Stream& stream) : m_stream(stream) {}
  virtual void finished(Data &aData)
  {
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(aData);

    m_stream.saveFinished(aData);
  }
private:
  Stream& m_stream;
};
/** @brief compression callback
 */
class CtSaving::Stream::_CompressionCBK : public TaskEventCallback
{
    DEB_CLASS_NAMESPC(DebModControl,"CtSaving::_CompressionCBK","Control");
public:
  _CompressionCBK(Stream& stream) : m_stream(stream) {}
  virtual void finished(Data &aData)
  {
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(aData);

    m_stream.compressionFinished(aData);
  }
private:
  Stream &m_stream;
};
/** @brief manual background saving
 */
class CtSaving::_ManualBackgroundSaveTask : public SinkTaskBase
{
public:
  _ManualBackgroundSaveTask(CtSaving& ct_saving,
			    HeaderMap &aHeader) :
    m_saving(ct_saving),
    m_header(aHeader)
  {
  }

  ~_ManualBackgroundSaveTask()
  {
    AutoMutex lock(m_saving.m_cond.mutex());
    m_saving.m_ready_flag = true;
    m_saving.m_cond.broadcast();
  }

  virtual void process(Data &aData)
  {
    m_saving._synchronousSaving(aData,m_header);
  }
private:
  CtSaving &m_saving;
  HeaderMap m_header;
};
/** @brief Parameters default constructor
 */
CtSaving::Parameters::Parameters()
  : nextNumber(0), fileFormat(RAW), savingMode(Manual), 
    overwritePolicy(Abort),managedMode(Software),
    indexFormat("%04d"),framesPerFile(1)
{
}

void CtSaving::Parameters::checkValid() const
{
  switch(fileFormat)
    {
#ifdef WITH_CBF_SAVING
    case CBFFormat :
      if(framesPerFile > 1)
	throw LIMA_CTL_EXC(InvalidValue, "CBF file format does not support "
			                 "multi frame per file");
      break;
#endif
#ifndef __unix
#pragma message ( "--- WARNING / TODO - no cases???" )
#endif
    default:
      break;
    }
}


//@brief constructor
CtSaving::Stream::Stream(CtSaving& aCtSaving, int idx)
  : m_saving(aCtSaving), m_idx(idx),
    m_save_cnt(NULL),
    m_pars_dirty_flag(false),
    m_active(false),
    m_compression_cbk(NULL)
{
  DEB_CONSTRUCTOR();

  createSaveContainer();
  m_saving_cbk = new _SaveCBK(*this);
  m_compression_cbk = new _CompressionCBK(*this);
}

//@brief destructor
CtSaving::Stream::~Stream()
{
  DEB_DESTRUCTOR();

  delete m_save_cnt;
  m_saving_cbk->unref();
  m_compression_cbk->unref();
}

const 
CtSaving::Parameters& CtSaving::Stream::getParameters(ParameterType type) const
{ 
  bool from_acq = (type == Acq) || ((type == Auto) && !m_pars_dirty_flag);
  return *(from_acq ? &m_acquisition_pars : &m_pars); 
}

CtSaving::Parameters& CtSaving::Stream::getParameters(ParameterType type)
{ 
  bool from_acq = (type == Acq) || ((type == Auto) && !m_pars_dirty_flag);
  return *(from_acq ? &m_acquisition_pars : &m_pars); 
}

void CtSaving::Stream::setParameters(const CtSaving::Parameters& pars)
{ 
  DEB_MEMBER_FUNCT();

  if (pars.nextNumber == m_acquisition_pars.nextNumber)
    m_pars.nextNumber = pars.nextNumber;

  if (pars == m_pars)
    return;

  pars.checkValid(); 
  m_pars_dirty_flag = true; 
  m_pars = pars; 

  DEB_TRACE() << "pars changed";
}

void CtSaving::Stream::setActive(bool active)
{
  DEB_MEMBER_FUNCT();

  if (active == m_active)
    return;
  
  if (!active)
    m_save_cnt->close();

  m_active = active;
}

void CtSaving::Stream::prepare()
{
  DEB_MEMBER_FUNCT();

  if (hasAutoSaveMode())
    {
      m_save_cnt->close();
      updateParameters();
      checkWriteAccess();
    }
}

void CtSaving::Stream::updateParameters()
{
  DEB_MEMBER_FUNCT();

  if (!m_pars_dirty_flag)
    return;

  if (m_pars.fileFormat != m_acquisition_pars.fileFormat)
    createSaveContainer();

  m_acquisition_pars = m_pars;
  m_pars_dirty_flag = false;
}

void CtSaving::Stream::createSaveContainer()
{
  DEB_MEMBER_FUNCT();

  switch (m_pars.fileFormat) {
  case CBFFormat :
#ifndef WITH_CBF_SAVING
    THROW_CTL_ERROR(NotSupported) << "Lima is not compiled with the cbf "
                                     "saving option, not managed";  
#endif
    goto common;

  case NXS:
#ifndef WITH_NXS_SAVING
    THROW_CTL_ERROR(NotSupported) << "Lima is not compiled with the nxs "
                                     "saving option, not managed";  
#endif        
    goto common;

  case FITS:
#ifndef WITH_FITS_SAVING
    THROW_CTL_ERROR(NotSupported) << "Lima is not compiled with the fits "
                                     "saving option, not managed";  
#endif        
    goto common;

  case RAW:
  case EDF:

  common:
    if (m_save_cnt) {
      m_save_cnt->close();
      delete m_save_cnt;
    }
    break;

  default:
    THROW_CTL_ERROR(NotSupported) << "File format not yet managed";
  }

  switch(m_pars.fileFormat)
  {
  case RAW:
  case EDF:
    m_save_cnt = new SaveContainerEdf(*this);
    break;
#ifdef WITH_CBF_SAVING
  case CBFFormat:
    m_save_cnt = new SaveContainerCbf(*this);
    m_pars.framesPerFile = 1;
    break;
#endif
#ifdef WITH_NXS_SAVING
  case NXS:
    m_save_cnt = new SaveContainerNxs(*this);
    break;
#endif
#ifdef WITH_FITS_SAVING
  case FITS:
    m_save_cnt = new SaveContainerFits(*this);
    break;
#endif
  default:
    break;
  }
}

void CtSaving::Stream::writeFile(Data& data, HeaderMap& header)
{
  DEB_MEMBER_FUNCT();

  m_save_cnt->writeFile(data, header);
}


SinkTaskBase *CtSaving::Stream::getTask(TaskType type, const HeaderMap& header)
{
  DEB_MEMBER_FUNCT();

  SinkTaskBase *save_task;

  if (type == Compression) {
    if (!needCompression())
      return NULL;
    save_task = m_save_cnt->getCompressionTask(header);
    save_task->setEventCallback(m_compression_cbk);
  } else {
    _SaveTask *real_task = new _SaveTask(*this);
    real_task->m_header = header;
    save_task = real_task;
    save_task->setEventCallback(m_saving_cbk);
  }

  return save_task;
}

void CtSaving::Stream::compressionFinished(Data& data)
{
  DEB_MEMBER_FUNCT();
  m_saving._compressionFinished(data, *this);
}

void CtSaving::Stream::saveFinished(Data& data)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(data, getIndex());

  m_saving._saveFinished(data, *this);
}

class CtSaving::_NewFrameSaveCBK : public HwSavingCtrlObj::Callback
{
public:
  _NewFrameSaveCBK(CtSaving &ct_saving) :
    m_saving(ct_saving)
  {
  }
  bool newFrameWrite(int frame_id)
  {
    return m_saving._newFrameWrite(frame_id);
  }
private:
  CtSaving&	m_saving;
};

//@brief constructor
CtSaving::CtSaving(CtControl &aCtrl) :
  m_ctrl(aCtrl),
  m_stream(NULL),
  m_ready_flag(true),
  m_need_compression(false),
  m_nb_save_cbk(0),
  m_end_cbk(NULL)
{
  DEB_CONSTRUCTOR();

  m_nb_stream = 5;
  m_stream = new Stream *[m_nb_stream];
  for (int s = 0; s < m_nb_stream; ++s)
    m_stream[s] = new Stream(*this, s);

  m_stream[0]->setActive(true);

  resetLastFrameNb();

  HwInterface *hw = aCtrl.hwInterface();
  m_has_hwsaving = hw->getHwCtrlObj(m_hwsaving);
  if(m_has_hwsaving)
    {
      m_new_frame_save_cbk = new _NewFrameSaveCBK(*this);
      m_hwsaving->registerCallback(m_new_frame_save_cbk);
    }
  else
    m_new_frame_save_cbk = NULL;
}

//@brief destructor
CtSaving::~CtSaving()
{
  DEB_DESTRUCTOR();

  for (int s = 0; s < m_nb_stream; ++s)
    delete m_stream[s];
  delete [] m_stream;

  setEndCallback(NULL);
  if(m_has_hwsaving)
    {
      m_hwsaving->unregisterCallback(m_new_frame_save_cbk);
      delete m_new_frame_save_cbk;
    }
}

CtSaving::Stream& CtSaving::getStreamExc(int stream_idx) const
{
  DEB_MEMBER_FUNCT();
  THROW_CTL_ERROR(InvalidValue) << "Invalid " << DEB_VAR1(stream_idx);
}

/** @brief set saving parameter for a saving stream

    @param pars parameters for the saving stream
    @param stream_idx the id of the saving stream
 */
void CtSaving::setParameters(const CtSaving::Parameters &pars, int stream_idx)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(pars, stream_idx);

  AutoMutex aLock(m_cond.mutex());
  Stream& stream = getStream(stream_idx);
  stream.setParameters(pars);
}

/** @brief get the saving stream parameters

    @param pars the return parameters
    @param stream_idx the stream id
 */
void CtSaving::getParameters(CtSaving::Parameters &pars, int stream_idx) const
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(stream_idx);

  AutoMutex aLock(m_cond.mutex());
  const Stream& stream = getStream(stream_idx);
  pars = stream.getParameters(Auto);

  DEB_RETURN() << DEB_VAR1(pars);
}
/** @brief set the saving directory for a saving stream
 */
void CtSaving::setDirectory(const std::string &directory, int stream_idx)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(directory, stream_idx);

  AutoMutex aLock(m_cond.mutex());
  Stream& stream = getStream(stream_idx);
  Parameters pars = stream.getParameters(Auto);
  pars.directory = directory;
  stream.setParameters(pars);
}
/** @brief get the saving directory for a saving stream
 */
void CtSaving::getDirectory(std::string& directory, int stream_idx) const
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(stream_idx);

  AutoMutex aLock(m_cond.mutex());
  const Stream& stream = getStream(stream_idx);
  const Parameters& pars = stream.getParameters(Auto);
  directory = pars.directory;

  DEB_RETURN() << DEB_VAR1(directory);
}
/** @brief set the filename prefix for a saving stream
 */
void CtSaving::setPrefix(const std::string &prefix, int stream_idx)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(prefix, stream_idx);

  AutoMutex aLock(m_cond.mutex());
  Stream& stream = getStream(stream_idx);
  Parameters pars = stream.getParameters(Auto);
  pars.prefix = prefix;
  stream.setParameters(pars);
}
/** @brief get the filename prefix for a saving stream
 */
void CtSaving::getPrefix(std::string& prefix, int stream_idx) const
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(stream_idx);

  AutoMutex aLock(m_cond.mutex());
  const Stream& stream = getStream(stream_idx);
  const Parameters& pars = stream.getParameters(Auto);
  prefix = pars.prefix;

  DEB_RETURN() << DEB_VAR1(prefix);
}
/** @brief set the filename suffix for a saving stream
 */
void CtSaving::setSuffix(const std::string &suffix, int stream_idx)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(suffix, stream_idx);

  AutoMutex aLock(m_cond.mutex());
  Stream& stream = getStream(stream_idx);
  Parameters pars = stream.getParameters(Auto);
  pars.suffix = suffix;
  stream.setParameters(pars);
}
/** @brief get the filename suffix for a saving stream
 */
void CtSaving::getSuffix(std::string& suffix, int stream_idx) const
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(stream_idx);

  AutoMutex aLock(m_cond.mutex());
  const Stream& stream = getStream(stream_idx);
  const Parameters& pars = stream.getParameters(Auto);
  suffix = pars.suffix;

  DEB_RETURN() << DEB_VAR1(suffix);
}
/** @brief set the next number for the filename for a saving stream
 */
void CtSaving::setNextNumber(long number, int stream_idx)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(number, stream_idx);

  AutoMutex aLock(m_cond.mutex());
  Stream& stream = getStream(stream_idx);
  Parameters pars = stream.getParameters(Auto);
  pars.nextNumber = number;
  stream.setParameters(pars);
}
/** @brief get the next number for the filename for a saving stream
 */
void CtSaving::getNextNumber(long& number, int stream_idx) const
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(stream_idx);
  
  AutoMutex aLock(m_cond.mutex());
  const Stream& stream = getStream(stream_idx);
  const Parameters& pars = stream.getParameters(Auto);
  number = pars.nextNumber;

  DEB_RETURN() << DEB_VAR1(number);
}
/** @brief set the saving format for a saving stream
 */
void CtSaving::setFormat(FileFormat format, int stream_idx)
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  Stream& stream = getStream(stream_idx);
  Parameters pars = stream.getParameters(Auto);
  pars.fileFormat = format;
  stream.setParameters(pars);
}
/** @brief get the saving format for a saving stream
 */
void CtSaving::getFormat(FileFormat& format, int stream_idx) const
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(stream_idx);

  AutoMutex aLock(m_cond.mutex());
  const Stream& stream = getStream(stream_idx);
  const Parameters& pars = stream.getParameters(Auto);
  format = pars.fileFormat;

  DEB_RETURN() << DEB_VAR1(format);
}
/** @brief set the saving mode for a saving stream
 */
void CtSaving::setSavingMode(SavingMode mode)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(mode);

  AutoMutex aLock(m_cond.mutex());
  for (int s = 0; s < m_nb_stream; ++s) {
    Stream& stream = getStream(s);
    Parameters pars = stream.getParameters(Auto);
    pars.savingMode = mode;
    stream.setParameters(pars);
  }
}
/** @brief get the saving mode for a saving stream
 */
void CtSaving::getSavingMode(SavingMode& mode) const
{ 
  DEB_MEMBER_FUNCT();
  
  AutoMutex aLock(m_cond.mutex());
  const Stream& stream = getStream(0);
  const Parameters& pars = stream.getParameters(Auto);
  mode = pars.savingMode;
  
  DEB_RETURN() << DEB_VAR1(mode);
}
/** @brief set the overwrite policy for a saving stream
 */
void CtSaving::setOverwritePolicy(OverwritePolicy policy, int stream_idx)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(policy, stream_idx);

  AutoMutex aLock(m_cond.mutex());
  Stream& stream = getStream(stream_idx);
  Parameters pars = stream.getParameters(Auto);
  pars.overwritePolicy = policy;
  stream.setParameters(pars);
}
/** @brief get the overwrite policy for a saving stream
 */
void CtSaving::getOverwritePolicy(OverwritePolicy& policy, 
				  int stream_idx) const
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(stream_idx);
  
  AutoMutex aLock(m_cond.mutex());
  const Stream& stream = getStream(stream_idx);
  const Parameters& pars = stream.getParameters(Auto);
  policy = pars.overwritePolicy;

  DEB_RETURN() << DEB_VAR1(policy);
}
/** @brief set the number of frame saved per file for a saving stream
 */
void CtSaving::setFramesPerFile(unsigned long frames_per_file, int stream_idx)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(frames_per_file, stream_idx);

  AutoMutex aLock(m_cond.mutex());
  Stream& stream = getStream(stream_idx);
  Parameters pars = stream.getParameters(Auto);
  pars.framesPerFile = frames_per_file;
  stream.setParameters(pars);
}
/** @brief get the number of frame saved per file for a saving stream
 */
void CtSaving::getFramePerFile(unsigned long& frames_per_file, 
			       int stream_idx) const
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(stream_idx);

  AutoMutex aLock(m_cond.mutex());
  const Stream& stream = getStream(stream_idx);
  const Parameters& pars = stream.getParameters(Auto);
  frames_per_file = pars.framesPerFile;

  DEB_RETURN() << DEB_VAR1(frames_per_file);
}

/** @brief set who will manage the saving.
 *
 *  with this methode you can choose who will do the saving
 *   - if mode is set to Software, the saving will be managed by Lima core
 *   - if mode is set to Hardware then it's the sdk or the hardware of the camera that will manage the saving.
 *  @param mode can be either Software or Hardware
*/
void CtSaving::setManagedMode(CtSaving::ManagedMode mode)
{
  DEB_MEMBER_FUNCT();
  if(mode == Hardware && !m_has_hwsaving)
    THROW_CTL_ERROR(InvalidValue) << DEB_VAR1(mode) << "Not supported";

  AutoMutex aLock(m_cond.mutex());
  for (int s = 0; s < m_nb_stream; ++s)
    {
      Stream& stream = getStream(s);
      Parameters pars = stream.getParameters(Auto);
      pars.managedMode = mode;
      stream.setParameters(pars);
    }
}

void CtSaving::getManagedMode(CtSaving::ManagedMode &mode) const
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  const Stream& stream = getStream(0);
  const Parameters& pars = stream.getParameters(Auto);
  mode = pars.managedMode;
}

void CtSaving::_getTaskList(TaskType type, long frame_nr, 
			    const HeaderMap& header, TaskList& task_list)
{
  DEB_MEMBER_FUNCT();

  task_list.clear();
  for (int s = 0; s < m_nb_stream; ++s) {
    Stream& stream = getStream(s);
    if (stream.isActive()) {
      SinkTaskBase *save_task = stream.getTask(type, header);
      if (save_task)
	task_list.push_back(save_task);
    }
  }
  int nb_cbk = task_list.size();
  DEB_TRACE() << DEB_VAR1(nb_cbk);
  if (type == Compression) {
    FrameCbkCountMap::value_type map_pair(frame_nr, nb_cbk);
    m_nb_compression_cbk.insert(map_pair);
  } else
    m_nb_save_cbk = nb_cbk;
}
/** @brief clear the common header
 */
void CtSaving::resetCommonHeader()
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  ManagedMode managed_mode = getManagedMode();
  if(managed_mode == Software)
    m_common_header.clear();
  else
    {
      int hw_cap = m_hwsaving->getCapabilities();
      if(hw_cap & HwSavingCtrlObj::COMMON_HEADER)
	m_hwsaving->resetCommonHeader();
      else
	THROW_CTL_ERROR(NotSupported) << "Common header is not supported";
    }
}
/** @brief set the common header.
    This is the header which will be write for all frame for this acquisition
 */
void CtSaving::setCommonHeader(const HeaderMap &header)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(header);

  AutoMutex aLock(m_cond.mutex());
  ManagedMode managed_mode = getManagedMode();
  if(managed_mode == Software)
    m_common_header = header;
  else
    {
      int hw_cap = m_hwsaving->getCapabilities();
      if(hw_cap & HwSavingCtrlObj::COMMON_HEADER)
	m_hwsaving->setCommonHeader(header);
      else
	THROW_CTL_ERROR(NotSupported) << "Common header is not supported";
    }
}
/** @brief replace/add field in the common header
 */
void CtSaving::updateCommonHeader(const HeaderMap &header)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(header);

  AutoMutex aLock(m_cond.mutex());
  //Update
  for(HeaderMap::const_iterator i = header.begin();
      i != header.end();++i)
    {
      std::pair<HeaderMap::iterator,bool> result= 
	m_common_header.insert(HeaderMap::value_type(i->first,i->second));
      //if it exist, update
      if(!result.second)
	result.first->second = i->second;
    }
}
/** @brief get the current common header
 */
void CtSaving::getCommonHeader(HeaderMap& header) const
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  header = HeaderMap(m_common_header);

  DEB_RETURN() << DEB_VAR1(header);
}
/** @brief add/replace a header value in the current common header
 */
void CtSaving::addToCommonHeader(const HeaderValue &value)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(value);

  AutoMutex aLock(m_cond.mutex());
  m_common_header.insert(value);
}
/** @brief add/replace a header value in the current frame header
 */
void CtSaving::addToFrameHeader(long frame_nr,const HeaderValue &value)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(frame_nr,value);

  AutoMutex aLock(m_cond.mutex());
  m_frame_headers[frame_nr].insert(value);
}
/** @brief add/replace several value in the current frame header
 */
void CtSaving::updateFrameHeader(long frame_nr,const HeaderMap &header)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(frame_nr,header);

  AutoMutex aLock(m_cond.mutex());
  HeaderMap &frameHeader = m_frame_headers[frame_nr];
  for(HeaderMap::const_iterator i = header.begin();
      i != header.end();++i)
    {
      std::pair<HeaderMap::iterator,bool> result= 
	frameHeader.insert(HeaderMap::value_type(i->first,i->second));
      //if it exist, update
      if(!result.second)
	result.first->second = i->second;
    }
}
/** @brief validate a header for a frame.
    this mean that the header is ready and can now be save.
    If you are in AutoHeader this will trigger the saving if the data frame is available
 */
void CtSaving::validateFrameHeader(long frame_nr)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(frame_nr);

  AutoMutex aLock(m_cond.mutex());
  SavingMode saving_mode = getAcqSavingMode();
  if (saving_mode != CtSaving::AutoHeader)
    return;

  FrameMap::iterator frame_iter = m_frame_datas.find(frame_nr);
  bool data_available = (frame_iter != m_frame_datas.end());
  bool can_save = (m_ready_flag && (m_last_frameid_saved == frame_nr - 1));
  if (!data_available || !(m_need_compression || can_save))
    return;
  Data aData = frame_iter->second;

  HeaderMap task_header;
  FrameHeaderMap::iterator aHeaderIter;
  aHeaderIter = m_frame_headers.find(frame_nr);
  bool keep_header = m_need_compression;
  _takeHeader(aHeaderIter, task_header, keep_header);

  TaskType task_type = m_need_compression ? Compression : Save;
  TaskList task_list;
  _getTaskList(task_type, frame_nr, task_header, task_list);
  if (!m_need_compression) {
    m_frame_datas.erase(frame_iter);
    m_ready_flag = false, m_last_frameid_saved = frame_nr;
  }
  aLock.unlock();
  _postTaskList(aData, task_list);
}
/** @brief get the frame header.

    @param frame_nr the frame id
    @param header the current frame header
 */
void CtSaving::getFrameHeader(long frame_nr, HeaderMap& header) const
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(frame_nr);

  AutoMutex aLock(m_cond.mutex());
  FrameHeaderMap::const_iterator i = m_frame_headers.find(frame_nr);
  if(i != m_frame_headers.end())
    header.insert(i->second.begin(),i->second.end());

  DEB_RETURN() << DEB_VAR1(header);
}

/** @brief get the frame header and remove it from the container
 */
void CtSaving::takeFrameHeader(long frame_nr, HeaderMap& header)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(frame_nr);

  AutoMutex aLock(m_cond.mutex());
  FrameHeaderMap::iterator i = m_frame_headers.find(frame_nr);
  if(i != m_frame_headers.end())
    {
      header = i->second;
      m_frame_headers.erase(i);
    }
  
  DEB_RETURN() << DEB_VAR1(header);
}
/** @brief remove a frame header

    @param frame_nr the frame id
 */
void CtSaving::removeFrameHeader(long frame_nr)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(frame_nr);

  AutoMutex aLock(m_cond.mutex());
  m_frame_headers.erase(frame_nr);
}
/** @brief remove all frame header
 */
void CtSaving::removeAllFrameHeaders()
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  m_frame_headers.clear();
}

// Private methodes

void CtSaving::_getCommonHeader(HeaderMap &header)
{
  header.insert(m_common_header.begin(),m_common_header.end());
}
void CtSaving::_takeHeader(FrameHeaderMap::iterator& headerIter, 
			   HeaderMap& header, bool keep_in_map)
{
  _getCommonHeader(header);

  if (headerIter == m_frame_headers.end())
    return;

  HeaderMap &aFrameHeaderMap = headerIter->second;
  for(HeaderMap::iterator i = aFrameHeaderMap.begin();
      i != aFrameHeaderMap.end();++i)
    {
      std::pair<HeaderMap::iterator,bool> result = 
	header.insert(HeaderMap::value_type(i->first,i->second));
      if(!result.second)
	result.first->second = i->second;
    }

  if(!keep_in_map)
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

bool CtSaving::_controlIsFault()
{
  DEB_MEMBER_FUNCT();
  CtControl::Status status;
  m_ctrl.getStatus(status);
  bool fault = (status.AcquisitionStatus == AcqFault);
  DEB_RETURN() << DEB_VAR1(fault);
  return fault;
}

bool CtSaving::_newFrameWrite(int frame_id)
{
  if(m_end_cbk)
    {
      Data aData;
      aData.frameNumber = frame_id;
      m_end_cbk->finished(aData);
    }
  return !!m_end_cbk;
}

void CtSaving::frameReady(Data &aData)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(aData);

  if (_controlIsFault()) {
    DEB_WARNING() << "Skip saving data: " << aData;
    return;
  }

  AutoMutex aLock(m_cond.mutex());

  SavingMode saving_mode = getAcqSavingMode();
  bool auto_header = (saving_mode == AutoHeader);
  long frame_nr = aData.frameNumber;
  FrameHeaderMap::iterator aHeaderIter;
  aHeaderIter = m_frame_headers.find(frame_nr);
  bool header_available = (aHeaderIter != m_frame_headers.end());
  bool can_save = (m_ready_flag && (m_last_frameid_saved == frame_nr - 1));
  DEB_TRACE() << DEB_VAR5(saving_mode, m_need_compression, can_save,
			  auto_header, header_available);
  if (!(m_need_compression || can_save) || 
      (auto_header && !header_available) || (saving_mode == Manual)) {
    FrameMap::value_type map_pair(frame_nr, aData);
    m_frame_datas.insert(map_pair);
    return;
  }

  HeaderMap task_header;
  bool keep_header = m_need_compression;
  _takeHeader(aHeaderIter, task_header, keep_header);

  TaskType task_type = m_need_compression ? Compression : Save;
  TaskList task_list;
  _getTaskList(task_type, frame_nr, task_header, task_list);
  if (!m_need_compression)
    m_ready_flag = false, m_last_frameid_saved = frame_nr;

  aLock.unlock();
  _postTaskList(aData, task_list);
}
/** @brief get write statistic
    this is the last write time
 */
void CtSaving::getWriteTimeStatistic(std::list<double> &aReturnList,
				     int stream_idx) const
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(stream_idx);

  const Stream& stream = getStream(stream_idx);
  stream.getStatistic(aReturnList);
}
/** @brief set the size of the write time static list
 */
void CtSaving::setStatisticHistorySize(int aSize, int stream_idx)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(aSize, stream_idx);

  Stream& stream = getStream(stream_idx);
  stream.setStatisticSize(aSize);
}
/** @brief activate/desactivate a stream
 */
void CtSaving::setStreamActive(int stream_idx, bool  active)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(stream_idx, active);

  if ((stream_idx == 0) && !active)
    THROW_CTL_ERROR(InvalidValue) << "Cannot deactivate file stream 0!";

  Stream& stream = getStream(stream_idx);
  stream.setActive(active);
}
/** @brief get if stream is active
 */
void CtSaving::getStreamActive(int stream_idx, bool& active) const
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(stream_idx);
  const Stream& stream = getStream(stream_idx);
  active = stream.isActive();
  DEB_RETURN() << DEB_VAR1(active);
}
/** @brief clear everything.
    - all header
    - all waiting data to be saved
    - close all stream
*/
void CtSaving::clear()
{
  DEB_MEMBER_FUNCT();

  resetLastFrameNb();

  for (int s = 0; s < m_nb_stream; ++s) {
    Stream& stream = getStream(s);
    stream.clear();
  }

  AutoMutex aLock(m_cond.mutex());
  m_frame_headers.clear();
  m_common_header.clear();	// @fix Should we clear common header???
  m_frame_datas.clear();
  
}
/** @brief write manually a frame

    @param aFrameNumber the frame id you want to save
    @param aNbFrames the number of frames you want to concatenate
 */
void CtSaving::writeFrame(int aFrameNumber, int aNbFrames,bool synchronous)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(aFrameNumber);

  class WaitAndCleanupReadyFlag
  {
  public:
    WaitAndCleanupReadyFlag(bool &ready_flag,Cond& aCond,bool cleanupReadyFlag) :
      m_lock(aCond.mutex()),m_cond(aCond),m_ready_flag(ready_flag),
      m_clean_ready_flag(cleanupReadyFlag){}

    ~WaitAndCleanupReadyFlag() 
    {
      m_lock.lock();
      if(m_clean_ready_flag)
	{
	  m_ready_flag = true;
	  m_cond.broadcast();
	}
    }
    void toggleReadyFlag() { m_ready_flag = false;m_lock.unlock();}
  private:
    AutoMutex 	m_lock;
    Cond&      	m_cond;
    bool&	m_ready_flag;
    bool	m_clean_ready_flag;
  } wait_and_cleanup_ready_flag(m_ready_flag,m_cond,synchronous);

  _updateParameters();

  SavingMode saving_mode = getAcqSavingMode();
  ManagedMode managed_mode = getManagedMode();
  wait_and_cleanup_ready_flag.toggleReadyFlag();

  if (saving_mode != Manual)
    THROW_CTL_ERROR(Error) << "Manual saving is only permitted when "
      "saving mode == Manual";

  if(managed_mode == Software)
    {
      Data anImage2Save;
      m_ctrl.ReadImage(anImage2Save,aFrameNumber,aNbFrames);

      // Saving
      HeaderMap header;
      FrameHeaderMap::iterator aHeaderIter;
      aHeaderIter = m_frame_headers.find(anImage2Save.frameNumber);
      _takeHeader(aHeaderIter, header, false);
      if(synchronous)
	_synchronousSaving(anImage2Save,header);
      else
	{
	  TaskMgr *aSavingManualMgrPt = new TaskMgr();
	  Data copyImage = anImage2Save.copy();
	  aSavingManualMgrPt->setInputData(copyImage);
	  SinkTaskBase *aTaskPt = new CtSaving::_ManualBackgroundSaveTask(*this,
									  header);
	  aSavingManualMgrPt->addSinkTask(0,aTaskPt);
	  aTaskPt->unref();
	  
	  PoolThreadMgr::get().addProcess(aSavingManualMgrPt);
	}
    }
  else
    {
      int hw_cap = m_hwsaving->getCapabilities();
      if(hw_cap & HwSavingCtrlObj::MANUAL_WRITE)
	m_hwsaving->writeFrame(aFrameNumber,aNbFrames);
      else
	THROW_CTL_ERROR(NotSupported) << "Manual write is not available";
    }
}

void CtSaving::_updateParameters()
{
  // wait until the saving is no more used
  while(!m_ready_flag) 
    m_cond.wait();

  for (int s = 0; s < m_nb_stream; ++s) {
    Stream& stream = getStream(s);
    if (stream.isActive())
      stream.updateParameters();
  }
}

void CtSaving::_synchronousSaving(Data &anImage2Save,HeaderMap &header)
{
  for (int s = 0; s < m_nb_stream; ++s) {
    Stream& stream = getStream(s);
    if (!stream.isActive())
      continue;

    if(stream.needCompression()) {
      SinkTaskBase *aCompressionTaskPt;
      aCompressionTaskPt = stream.getTask(Compression, header);
      aCompressionTaskPt->setEventCallback(NULL);
      aCompressionTaskPt->process(anImage2Save);
      aCompressionTaskPt->unref();
    }

    stream.writeFile(anImage2Save, header);
  }
}
void CtSaving::_postTaskList(Data& aData, const TaskList& task_list)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(aData, task_list.size());

  TaskMgr *aSavingMgrPt = new TaskMgr();

  TaskList::const_iterator it, end = task_list.end();
  for (it = task_list.begin(); it != end; ++it) {
    SinkTaskBase *save_task = *it;
    aSavingMgrPt->addSinkTask(0, save_task);
    save_task->unref();
  }
  aSavingMgrPt->setInputData(aData);

  PoolThreadMgr::get().addProcess(aSavingMgrPt);
}

void CtSaving::_compressionFinished(Data& aData, Stream& stream)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(aData, stream.getIndex());

  AutoMutex aLock(m_cond.mutex());

  long frame_nr = aData.frameNumber;
  FrameCbkCountMap::iterator count_it = m_nb_compression_cbk.find(frame_nr);
  if (--count_it->second > 0)
    return;
  m_nb_compression_cbk.erase(count_it);

  if (!m_ready_flag || (m_last_frameid_saved != frame_nr - 1)) {
    FrameMap::value_type map_pair(frame_nr, aData);
    m_frame_datas.insert(map_pair);
    return;
  }

  HeaderMap header;
  FrameHeaderMap::iterator header_it = m_frame_headers.find(frame_nr);
  _takeHeader(header_it, header, false);

  TaskList task_list;
  _getTaskList(Save, frame_nr, header, task_list);
  m_ready_flag = false,m_last_frameid_saved = frame_nr;

  aLock.unlock();
  _postTaskList(aData, task_list);
}

void CtSaving::_saveFinished(Data &aData, Stream& stream)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(aData, stream.getIndex());

  AutoMutex aLock(m_cond.mutex());

  if(--m_nb_save_cbk > 0)
    return;

  //@todo check if the frame is still available
  if(m_end_cbk) {
    aLock.unlock();
    m_end_cbk->finished(aData);
    aLock.lock();
  }

  SavingMode saving_mode = getAcqSavingMode();
  bool auto_saving = (saving_mode == AutoFrame) || (saving_mode == AutoHeader);

  int next_frame = m_last_frameid_saved + 1;
  FrameMap::iterator nextDataIter = m_frame_datas.find(next_frame);
  bool data_available = (nextDataIter != m_frame_datas.end());
  FrameHeaderMap::iterator aHeaderIter = m_frame_headers.find(next_frame);
  bool header_available = (aHeaderIter != m_frame_headers.end());
  if (!auto_saving || !data_available || 
      ((saving_mode == AutoHeader) && !header_available)) {
    m_ready_flag = true;
    m_cond.signal();
    return;
  }
  Data aNewData = nextDataIter->second;

  HeaderMap task_header;
  _takeHeader(aHeaderIter, task_header, false);

  TaskList task_list;
  _getTaskList(Save, next_frame, task_header, task_list);
  m_last_frameid_saved = next_frame;
  m_frame_datas.erase(nextDataIter);
  aLock.unlock();
  _postTaskList(aNewData, task_list);
}

/** @brief this methode set the error saving status in CtControl
 */
void CtSaving::_setSavingError(CtControl::ErrorCode anErrorCode)
{
  DEB_MEMBER_FUNCT();
  // We do not stop the acquisition if we are in Manual Saving
  SavingMode saving_mode = getAcqSavingMode();
  if (saving_mode == Manual)
    return;

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
/** @brief preparing new acquisition
    this methode will resetLastFrameNb if mode is AutoSave
    and validate the parameter for this new acquisition
 */
void CtSaving::_prepare()
{
  DEB_MEMBER_FUNCT();

  if(hasAutoSaveMode()) 
    resetLastFrameNb();
  else
    DEB_TRACE() << "No auto save activated";

  AutoMutex aLock(m_cond.mutex());

  m_need_compression = false;

  //prepare all the active streams
  for (int s = 0; s < m_nb_stream; ++s) {
    Stream& stream = getStream(s);
    if (stream.isActive()) {
      stream.prepare();
      if (stream.needCompression())
	m_need_compression = true;
    }
  }

  m_nb_save_cbk = 0;
  m_nb_compression_cbk.clear();
}

CtSaving::SaveContainer::SaveContainer(Stream& stream) 
  : m_written_frames(0), m_stream(stream), m_statistic_size(16),
    m_file_opened(false)
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

  const CtSaving::Parameters& pars = m_stream.getParameters(Acq);

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
	      m_stream.setSavingError(CtControl::SaveDiskFull);
	      try {
		close();
	      } catch (...) {
	      }
	      THROW_CTL_ERROR(Error) << "Disk full!!!";
	    }
	};

  
#endif
      m_stream.setSavingError(CtControl::SaveUnknownError);
      try {
	close();
      } catch (...) {
      }
      THROW_CTL_ERROR(Error) << "Save unknown error";
    }
    catch(...)
    {
      m_stream.setSavingError(CtControl::SaveUnknownError);
      try {
	close();
      } catch (...) {
      }
      THROW_CTL_ERROR(Error) << "Save unknown error";
    }

  if(++m_written_frames == pars.framesPerFile) {
    try {
      close();
    } catch (...) {
      m_stream.setSavingError(CtControl::SaveCloseError);
      THROW_CTL_ERROR(Error) << "Save file close error";
    }
  }

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


void CtSaving::SaveContainer::getParameters(CtSaving::Parameters& pars) const
{
  pars = m_stream.getParameters(Acq);
}


void CtSaving::SaveContainer::clear()
{
  AutoMutex aLock(m_cond.mutex());
  m_statistic_list.clear();
  this->close();
  _clear();			// call inheritance if needed
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
	  m_stream.setSavingError(CtControl::SaveOverwriteError);
	  std::string output;
	  output = "Try to over write file: " + aFileName;
	  THROW_CTL_ERROR(Error) << output;
	}
	  std::ios_base::openmode openFlags = std::ios_base::out | std::ios_base::binary;
      if(pars.overwritePolicy == Append)
	openFlags |= std::ios_base::app;
      else if(pars.overwritePolicy == Overwrite)
	openFlags |= std::ios_base::trunc;

      std::string error_desc;
      for(int nbTry = 0;nbTry < 5;++nbTry)
	{
	  bool succeed = false;
	  try {
	    succeed = _open(aFileName,openFlags);
	  } catch (std::ios_base::failure &error) {
	    error_desc = error.what();
	    DEB_WARNING() << "Could not open " << aFileName << ": " 
			  << error_desc;
	  } catch (...) {
	    error_desc = "Unknown error";
	    DEB_WARNING() << "Could not open " << aFileName << ": " 
			  << error_desc;
	  }

	  if(!succeed)
	    {
	      std::string output;

	      if(access(pars.directory.c_str(),W_OK))
		{
		  m_stream.setSavingError(CtControl::SaveAccessError);
		  output = "Can not write in directory: " + pars.directory;
		  THROW_CTL_ERROR(Error) << output;
		}
	    }
	  else
	    {
	      DEB_TRACE() << "Open file: " << aFileName;
	      m_file_opened = true;
	      break;
	    }
	}

      if (!m_file_opened) 
	{
	  m_stream.setSavingError(CtControl::SaveOpenError);
	  std::string output;
	  output = "Failure opening " + aFileName;
	  if (!error_desc.empty())
	    output += ": " + error_desc;
	  THROW_CTL_ERROR(Error) << output;
	}
    }
}

void CtSaving::SaveContainer::close()
{
  DEB_MEMBER_FUNCT();
  if (!m_file_opened)
    return;

  _close();
  m_file_opened = false;
  m_written_frames = 0;
  Parameters& pars = m_stream.getParameters(Acq);
  ++pars.nextNumber;
}

/** @brief check if all file can be written
 */

void CtSaving::Stream::checkWriteAccess()
{
  DEB_MEMBER_FUNCT();
  std::string output;
  // check if directory exist
  DEB_TRACE() << "Check if directory exist";
  if(!access(m_pars.directory.c_str(),F_OK))
    {
      // check if it's a directory
      struct stat aDirectoryStat;
      if(stat(m_pars.directory.c_str(),&aDirectoryStat))
	{
	  output = "Can stat directory : " + m_pars.directory;
	  THROW_CTL_ERROR(Error) << output;
	}
      DEB_TRACE() << "Check if it's really a directory";
      if(!S_ISDIR(aDirectoryStat.st_mode))
	{
	  output = "Path : " + m_pars.directory + " is not a directory";
	  THROW_CTL_ERROR(Error) << output;
	}

      // check if it's writtable
      DEB_TRACE() << "Check if directory is writtable";
      if(access(m_pars.directory.c_str(),W_OK))
	{
	  output = "Directory : " + m_pars.directory + " is not writtable";
	  THROW_CTL_ERROR(Error) << output;
	}
    }
  else
    {
      output = "Directory : " + m_pars.directory + " doesn't exist";
      THROW_CTL_ERROR(Error) << output;
    }

  // test all file is mode == Abort
  if(m_pars.overwritePolicy == Abort)
    {
      
      CtAcquisition *anAcq = m_saving.m_ctrl.acquisition();
      int nbAcqFrames;
      anAcq->getAcqNbFrames(nbAcqFrames);
      int framesPerFile = m_pars.framesPerFile;
      int nbFiles = (nbAcqFrames + framesPerFile - 1) / framesPerFile;
      int firstFileNumber = m_acquisition_pars.nextNumber;
      int lastFileNumber = m_acquisition_pars.nextNumber + nbFiles - 1;

#ifdef WIN32
      HANDLE hFind;
      WIN32_FIND_DATA FindFileData;
      const int maxNameLen = FILENAME_MAX;
      char filesToSearch[ maxNameLen];

      sprintf_s(filesToSearch,FILENAME_MAX, "%s/*.*", m_pars.directory.c_str());
      if((hFind = FindFirstFile(filesToSearch, &FindFileData)) == INVALID_HANDLE_VALUE)
#else
      struct dirent buffer;
      struct dirent* result;
      const int maxNameLen = 256;

      DIR *aDirPt = opendir(m_pars.directory.c_str());
      if(!aDirPt)
#endif
	{
	  output = "Can't open directory : " + m_pars.directory;
	  THROW_CTL_ERROR(Error) << output;
	}

      
      bool errorFlag = false;
      char testString[maxNameLen];
      snprintf(testString,sizeof(testString),
	       "%s%s%s",
	       m_pars.prefix.c_str(),
	       m_pars.indexFormat.c_str(),
	       m_pars.suffix.c_str());

      char firstFileName[maxNameLen],lastFileName[maxNameLen];
      snprintf(firstFileName, maxNameLen, testString, firstFileNumber);
      snprintf(lastFileName, maxNameLen, testString, lastFileNumber);
      DEB_TRACE() << "Test if file between: " DEB_VAR2(firstFileName,lastFileName);

      char *fname;

      int fileIndex;

#ifdef WIN32
      BOOL doIt = true;
      while(!errorFlag && doIt) {
        fname = FindFileData.cFileName;
        doIt = FindNextFile(hFind, &FindFileData);

	if(sscanf_s(fname,testString, &fileIndex) == 1)
#else
      int returnFlag;    // not used???
      while(!errorFlag && 
          !(returnFlag = readdir_r(aDirPt,&buffer,&result)) && result){
        fname = result->d_name;
	if(sscanf(result->d_name,testString,&fileIndex) == 1)
#endif
	    {
	      char auxFileName[maxNameLen];
	      snprintf(auxFileName,maxNameLen,testString,fileIndex);
	      if((strncmp(fname, auxFileName, maxNameLen) == 0) &&
		        (fileIndex >= firstFileNumber) && (fileIndex <= lastFileNumber))
		      {
		        output = "File : ";
		        output += fname;
		        output += " already exist";
		        errorFlag = true;
		      }
	    } // if sscanf
  } // while


#ifdef WIN32
      FindClose(hFind);
#else
      closedir(aDirPt);
#endif


      if(errorFlag)
	        THROW_CTL_ERROR(Error) << output;
    } // if(m_pars.overwritePolicy == Abort)
}
