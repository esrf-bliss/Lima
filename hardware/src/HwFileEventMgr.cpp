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
#include <dirent.h>
#include <stdlib.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <unistd.h>
#include "HwFileEventMgr.h"
using namespace lima;

void HwFileEventCallbackHelper::prepare(const DirectoryEvent::Parameters& params)
{
  //Clean all pending Data
  for(DatasPendingType::iterator i = m_pending_frame_infos.begin();
      i != m_pending_frame_infos.end();m_pending_frame_infos.erase(i++))
    if(i->second.buffer_owner_ship != HwFrameInfoType::Managed)
      free(i->second.frame_ptr);

  m_pending_frame_infos.clear();

  m_cbk.prepare(params);
}

bool HwFileEventCallbackHelper::nextFileExpected(int file_number,
						 const char *full_path,
						 int &next_file_number_expected) throw()
{
  HwFrameInfoType aNewFrameInfo;
  bool continueFlag = true;
  try
    {
      continueFlag = m_cbk.getFrameInfo(file_number,full_path,
					Acquisition,aNewFrameInfo);
      bool continueAcq = m_cbk.newFrameReady(aNewFrameInfo);
      if(continueFlag) continueFlag = continueAcq;
      DatasPendingType::iterator i = m_pending_frame_infos.begin();
      while(i != m_pending_frame_infos.end() && continueFlag)
	{
	  ++file_number;
	  if(i->first == file_number)
	    {
	      next_file_number_expected = file_number + 1;
	      continueFlag = m_cbk.newFrameReady(i->second);
	      m_pending_frame_infos.erase(i);
	    }
	  else
	    break;
	  i = m_pending_frame_infos.begin();
	}
    }
  catch(...)		// File reading problem
    {
      next_file_number_expected = file_number;
    }
  return continueFlag;
}

bool HwFileEventCallbackHelper::newFile(int file_number,const char *full_path) throw()
{
  bool continueFlag = true;
  DatasPendingType::iterator i = m_pending_frame_infos.find(file_number);
  if(i == m_pending_frame_infos.end())
    {

      try
	{
	  HwFrameInfoType aNewFrameInfo;
	  continueFlag = m_cbk.getFrameInfo(file_number,full_path,
					    Acquisition,
					    aNewFrameInfo);
	  m_pending_frame_infos[file_number] = aNewFrameInfo;
	}
      catch(...)
	{
	}
    }
  return continueFlag;
}

/** @brief callback class of HwTmpfsBufferMgr
 */
class HwTmpfsBufferMgr::_CBK : public HwFileEventCallbackHelper::Callback
{
public:
  _CBK(HwTmpfsBufferMgr &cnt) : m_cnt(cnt) {}

  virtual void prepare(const DirectoryEvent::Parameters &params)
  {
    //Clean watch directory
    const char *path = params.watch_path.c_str();
    DIR* aWatchDir = opendir(path);
    if(aWatchDir)
      {
	while(1)
	  {
	    char fullPath[1024];
	    struct dirent aDirentStruct,*result;
	    int status = readdir_r(aWatchDir,
				   &aDirentStruct,&result);
	    if(status || !result)
	      break;
	    snprintf(fullPath,sizeof(fullPath),
		     "%s/%s",path,result->d_name);
	    unlink(fullPath);
	  }
	closedir(aWatchDir);
      }
    m_cnt.m_cbk.prepare(params);
  }
  virtual bool getFrameInfo(int image_number,const char* full_path,
			    HwFileEventCallbackHelper::CallFrom mode,
			    HwFrameInfoType &frame_info)
  {
    return m_cnt._getFrameInfo(image_number,full_path,mode,frame_info);
  }
  virtual bool newFrameReady(const HwFrameInfoType &frame_info)
  {
    int imageId2Remove = frame_info.acq_frame_nb - m_cnt.m_keep_nb_images;
    if(imageId2Remove >= 0)
      {
	char aFileName[128];
	snprintf(aFileName,sizeof(aFileName),
		 m_cnt.m_file_pattern.c_str(),imageId2Remove);
	char aFullPathBuffer[1024];
	snprintf(aFullPathBuffer,sizeof(aFullPathBuffer),
		 "%s/%s",m_cnt.m_tmpfs_path.c_str(),aFileName);
	unlink(aFullPathBuffer);
      }
    return m_cnt.newFrameReady(frame_info);
  }
private:
  HwTmpfsBufferMgr& m_cnt;
};
/** @brief Buffer manager for tmp file system.
 *
 *  This manager can be used when you can't get (from API) image from the
 *  detector. The only possible way is to read image back from a file.
 */
HwTmpfsBufferMgr::HwTmpfsBufferMgr(const char* tmpfs_path, 
				   const char* file_pattern,
				   Callback &cbk) :
  m_tmpfs_path(tmpfs_path),
  m_file_pattern(file_pattern),
  m_next_expected_image_number(0),
  m_keep_nb_images(1),
  m_memory_percent(0.5),
  m_internal_cbk(new _CBK(*this)),
  m_directory_cbk(*m_internal_cbk),
  m_directory_event(true,m_directory_cbk),
  m_cbk(cbk)
{
    
}

HwTmpfsBufferMgr::~HwTmpfsBufferMgr()
{
  m_directory_event.stop();
  delete m_internal_cbk;
}

void HwTmpfsBufferMgr::setMemoryPercent(double percent)
{
  DEB_MEMBER_FUNCT();

  if(percent >= 0. && percent <= 1.)
    m_memory_percent = percent;
  else
    THROW_HW_ERROR(Error) << "percent should be between 0. and 1.";
}
/** @brief next image number should we expected
 *  This parameters will be take into account after prepare
 */
void HwTmpfsBufferMgr::setNextExpectedImageNumber(int image_nb)
{
  m_next_expected_image_number = image_nb;
}

void HwTmpfsBufferMgr::prepare()
{
  DirectoryEvent::Parameters params;
  params.watch_path = m_tmpfs_path;
  params.file_pattern = m_file_pattern;
  params.next_file_number_expected = m_next_expected_image_number;
  m_directory_event.prepare(params);
  m_keep_nb_images = _calcNbMaxImages();
}

void HwTmpfsBufferMgr::start()
{
  m_directory_event.start();
  m_start_time = Timestamp::now();
}

void HwTmpfsBufferMgr::stop()
{
  m_directory_event.stop();
}

bool HwTmpfsBufferMgr::isStopped() const
{
  return m_directory_event.isStopped();
}

int HwTmpfsBufferMgr::getLastAcquiredFrame() const
{
  return m_directory_event.getNextFileNumberExpected() - 1;
}

void HwTmpfsBufferMgr::setFrameDim(const FrameDim&) 
{
  DEB_MEMBER_FUNCT();
}
void HwTmpfsBufferMgr::getFrameDim(FrameDim& frame_dim)
{
  m_cbk.getFrameDim(frame_dim);
}

void HwTmpfsBufferMgr::setNbBuffers(int)
{
  DEB_MEMBER_FUNCT();
}

void HwTmpfsBufferMgr::getNbBuffers(int& nb_buffers)
{
  nb_buffers = m_keep_nb_images;
}

void HwTmpfsBufferMgr::setNbConcatFrames(int nb_concat_frames)
{
  DEB_MEMBER_FUNCT();
  if(nb_concat_frames != 1)
    THROW_HW_ERROR(NotSupported) << "Not managed";
}
void HwTmpfsBufferMgr::getNbConcatFrames(int& nb_concat_frames)
{
  nb_concat_frames = 1;
}

void HwTmpfsBufferMgr::getMaxNbBuffers(int& max_nb_buffers)
{
  max_nb_buffers = _calcNbMaxImages();
}

void* HwTmpfsBufferMgr::getBufferPtr(int,int)
{
  return NULL;
}

void* HwTmpfsBufferMgr::getFramePtr(int)
{
  return NULL;
}

void HwTmpfsBufferMgr::getStartTimestamp(Timestamp& start_ts)
{
  start_ts = m_start_time;
}

void HwTmpfsBufferMgr::getFrameInfo(int acq_frame_nb, HwFrameInfoType& info)
{
  DEB_MEMBER_FUNCT();

  char filename[1024];
  snprintf(filename,sizeof(filename),
	   m_file_pattern.c_str(),acq_frame_nb);
  std::string fullPath = m_tmpfs_path + "/";
  fullPath += filename;
  m_cbk.getFrameInfo(acq_frame_nb,fullPath.c_str(),
		     HwFileEventCallbackHelper::OnDemand,
		     info);
}

void HwTmpfsBufferMgr::registerFrameCallback(HwFrameCallback& frame_cb)
{
  HwFrameCallbackGen::registerFrameCallback(frame_cb);
}

void HwTmpfsBufferMgr::unregisterFrameCallback(HwFrameCallback& frame_cb)
{
  HwFrameCallbackGen::unregisterFrameCallback(frame_cb);
}

int HwTmpfsBufferMgr::_calcNbMaxImages()
{
   DEB_MEMBER_FUNCT();

  struct statvfs fsstat;
  if(statvfs(m_tmpfs_path.c_str(),&fsstat))
    THROW_HW_ERROR(Error) << "Can't figure out what is the size of that filesystem: " 
			  << m_tmpfs_path;


  FrameDim anImageDim;
  getFrameDim(anImageDim);
  int nb_block_for_image = anImageDim.getMemSize() / fsstat.f_bsize;
  int aTotalImage = fsstat.f_blocks / nb_block_for_image;
  return aTotalImage * m_memory_percent;
}

bool HwTmpfsBufferMgr::_getFrameInfo(int image_number,const char* full_path,
				     HwFileEventCallbackHelper::CallFrom mode,
				     HwFrameInfoType &frame_info)
{
  return m_cbk.getFrameInfo(image_number,full_path,mode,frame_info);
}
