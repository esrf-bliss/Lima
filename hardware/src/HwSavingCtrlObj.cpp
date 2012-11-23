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
#include "HwSavingCtrlObj.h"
#include "Exceptions.h"
#include <deque>
#include <algorithm>

using namespace lima;
const char* HwSavingCtrlObj::RAW_FORMAT_STR = "RAW"; ///< Raw format (no header)
const char* HwSavingCtrlObj::EDF_FORMAT_STR = "EDF"; ///< EDF format (Esrf Data Format)
const char* HwSavingCtrlObj::CBF_FORMAT_STR = "CBF"; ///< CBF format
#ifdef __linux__
class HwSavingCtrlObj::DirectoryCallback : public DirectoryEvent::Callback
{
public:
  DirectoryCallback(HwSavingCtrlObj& saving) : 
    m_saving(saving),
    m_start_file_number(0)
  {
  }
  void prepare(const DirectoryEvent::Parameters& params)
  {
    m_start_file_number = params.next_file_number_expected;
  }

  void clear()
  {
    m_image_ids.clear();
  }
  virtual bool nextFileExpected(int file_number,
				const char*,
				int &next_file_number_expected) throw()
  {
    int image_id = file_number - m_start_file_number;

    bool continueFlag = m_saving.m_callback ? 
      m_saving.m_callback->newFrameWritten(image_id) : false;

    int next_file_number = file_number + 1;

    std::sort(m_image_ids.begin(),m_image_ids.end());
    while(continueFlag &&
	  !m_image_ids.empty() && m_image_ids.front() == next_file_number)
      {
	image_id = next_file_number - m_start_file_number;
	continueFlag = m_saving.m_callback->newFrameWritten(image_id);
	++next_file_number;
	m_image_ids.pop_front();
      }
    next_file_number_expected = next_file_number_expected;
    return continueFlag;
  }

  virtual bool newFile(int file_number,const char*) throw()
  {
    m_image_ids.push_back(file_number);
    return true;
  }
private:
  HwSavingCtrlObj& 	m_saving;
  int 			m_start_file_number;
  std::deque<int> 	m_image_ids;
};
#endif

HwSavingCtrlObj::HwSavingCtrlObj(int capabilities) :
  m_caps(capabilities),
  m_active(false),
  m_callback(NULL)
#ifdef __linux__
  ,m_dir_cbk(new HwSavingCtrlObj::DirectoryCallback(*this)),
  m_dir_event(true,*m_dir_cbk)
#endif
{
}

HwSavingCtrlObj::~HwSavingCtrlObj()
{
#ifdef __linux__
  delete m_dir_cbk;
#endif
}

void HwSavingCtrlObj::setActive(bool flag)
{
  _setActive(flag);
  m_active = flag;
}

bool HwSavingCtrlObj::isActive() const
{
  return m_active;
}
void HwSavingCtrlObj::setDirectory(const std::string& directory)
{
  m_directory = directory;
}

void HwSavingCtrlObj::setPrefix(const std::string& prefix)
{
  m_prefix = prefix;
}
void HwSavingCtrlObj::setSuffix(const std::string& suffix)
{
  m_suffix = suffix;
}
void HwSavingCtrlObj::setNextNumber(long number)
{
  m_next_number = number;
}
void HwSavingCtrlObj::setIndexFormat(const std::string& indexFormat)
{
  m_index_format = indexFormat;
}
void HwSavingCtrlObj::setSaveFormat(const std::string &format)
{
  m_file_format = format;
}


/** @brief write manualy a frame
 */
void HwSavingCtrlObj::writeFrame(int,int)
{
  DEB_MEMBER_FUNCT();
  THROW_HW_ERROR(NotSupported) << "No available for this Hardware";
}
/** @brief write manualy a frame
 */
void HwSavingCtrlObj::readFrame(HwFrameInfoType&,int)
{
  DEB_MEMBER_FUNCT();
  THROW_HW_ERROR(NotSupported) << "No available for this Hardware";
}

/** @brief set frames' common header
 */
void HwSavingCtrlObj::setCommonHeader(const HeaderMap&)
{
  DEB_MEMBER_FUNCT();
  THROW_HW_ERROR(NotSupported) << "No available for this Hardware";
}

/** @brief clear common header
 */
void HwSavingCtrlObj::resetCommonHeader()
{
  DEB_MEMBER_FUNCT();
  THROW_HW_ERROR(NotSupported) << "No available for this Hardware";
}

void HwSavingCtrlObj::prepare()
{
  DEB_MEMBER_FUNCT();

  if(m_active)
    {
      _prepare();
#ifdef __linux__
      DirectoryEvent::Parameters params;
      params.watch_path = m_directory;
      params.file_pattern = m_prefix;
      params.file_pattern += m_index_format;
      params.file_pattern += m_suffix;
      params.next_file_number_expected = m_next_number;
      m_dir_event.prepare(params);

      if(m_callback)
	m_callback->prepare(params);
#endif
    }
}

void HwSavingCtrlObj::start()
{
  DEB_MEMBER_FUNCT();

  if(m_active)
    {
      _start();
#ifdef __linux__
      m_dir_event.start();
#endif
    }
}
void HwSavingCtrlObj::stop()
{
#ifdef __linux__
  m_dir_event.stop();
#endif
}

int HwSavingCtrlObj::getCapabilities() const
{
  return m_caps;
}

void HwSavingCtrlObj::registerCallback(HwSavingCtrlObj::Callback *cbk)
{
  DEB_MEMBER_FUNCT();

  if(m_callback)
    THROW_HW_ERROR(Error) << "Callback is already registered";
  m_callback = cbk;
}

void HwSavingCtrlObj::unregisterCallback(HwSavingCtrlObj::Callback *cbk)
{
  DEB_MEMBER_FUNCT();

  if(m_callback != cbk)
    THROW_HW_ERROR(Error) << "Try the unregister wrong callback object";
  m_callback = NULL;
}

std::string HwSavingCtrlObj::_getFullPath(int image_number) const
{
  char nbBuffer[32];
  snprintf(nbBuffer,sizeof(nbBuffer),
	   m_index_format.c_str(),image_number);
#ifdef __unix
  const char SEPARATOR = '/';
#else	 // WINDOW
  const char SEPARATOR = '\\';
#endif
  std::string fullpath;
  fullpath = m_directory + SEPARATOR;
  fullpath += m_prefix;
  fullpath += nbBuffer;
  fullpath += m_suffix;
  return fullpath;
}
