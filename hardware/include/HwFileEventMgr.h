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
#ifndef HWFILEEVENT_H
#define HWFILEEVENT_H
#ifdef __linux__

#include "DirectoryEventUtils.h"
#include "HwFrameInfo.h"
#include "HwFrameCallback.h"
#include "HwBufferCtrlObj.h"

namespace lima
{
  class HwFileEventCallbackHelper : public DirectoryEvent::Callback
  {
  public:
    enum CallFrom {Acquisition,OnDemand};
    class Callback
    {
    public:

      virtual void prepare(const DirectoryEvent::Parameters&) {}
      virtual bool getFrameInfo(int image_number,const char* full_path,
				CallFrom,HwFrameInfoType&) = 0;
      virtual bool newFrameReady(const HwFrameInfoType&) = 0;
    };

    HwFileEventCallbackHelper(Callback &cbk) : m_cbk(cbk) {};
    virtual void prepare(const DirectoryEvent::Parameters &params);
    virtual bool nextFileExpected(int file_number,
				  const char *full_path,
				  int &next_file_number_expected) throw();
    virtual bool newFile(int file_number,const char *full_path) throw();
    
    int getNbOfFramePending() const {return m_pending_frame_infos.size();}
  private:
    typedef std::map<int,HwFrameInfoType> DatasPendingType;

    DatasPendingType 	m_pending_frame_infos;
    Callback& 		m_cbk;
  };

  class HwTmpfsBufferMgr : public HwBufferCtrlObj, public HwFrameCallbackGen
  {
    DEB_CLASS_NAMESPC(DebModHardware, "HwTmpfsBufferMgr", "Hardware");
  public:
    class Callback
    {
    public:
      virtual void prepare(const DirectoryEvent::Parameters&) {}

      virtual bool getFrameInfo(int image_number,const char* full_path,
				HwFileEventCallbackHelper::CallFrom,
				HwFrameInfoType&) = 0;
      virtual void getFrameDim(FrameDim& frame_dim) = 0;
    };

    HwTmpfsBufferMgr(const char *tmpfs_path,const char* file_pattern,
		     Callback &cbk);
    ~HwTmpfsBufferMgr();
    
    void setMemoryPercent(double);
    void setNextExpectedImageNumber(int);
    void prepare();
    void start();
    void stop();
    bool isStopped() const;
    int getLastAcquiredFrame() const;

    int getNbOfFramePending() const {return m_directory_cbk.getNbOfFramePending();}

    //Raw access functions
    DirectoryEvent& getDirectoryEvent() {return m_directory_event;}
    //methodes used by Lima core
    virtual void setFrameDim(const FrameDim& frame_dim);
    virtual void getFrameDim(FrameDim& frame_dim);

    virtual void setNbBuffers(int nb_buffers);
    virtual void getNbBuffers(int& nb_buffers);

    virtual void setNbConcatFrames(int nb_concat_frames);
    virtual void getNbConcatFrames(int& nb_concat_frames);

    virtual void getMaxNbBuffers(int& max_nb_buffers);

    virtual void *getBufferPtr(int buffer_nb, int concat_frame_nb = 0);
    virtual void *getFramePtr(int acq_frame_nb);

    virtual void getStartTimestamp(Timestamp& start_ts);
    virtual void getFrameInfo(int acq_frame_nb, HwFrameInfoType& info);

    virtual void registerFrameCallback(HwFrameCallback& frame_cb);
    virtual void unregisterFrameCallback(HwFrameCallback& frame_cb);
  private:
    class _CBK;
    friend class _CBK;
    int _calcNbMaxImages();
    bool _getFrameInfo(int image_number,const char* full_path,
		       HwFileEventCallbackHelper::CallFrom,
		       HwFrameInfoType &frame_info);

    std::string 		m_tmpfs_path;
    std::string 		m_file_pattern;
    int				m_next_expected_image_number;
    int				m_keep_nb_images;
    double			m_memory_percent;
    _CBK* 			m_internal_cbk;
    HwFileEventCallbackHelper 	m_directory_cbk;
    DirectoryEvent 		m_directory_event;
    Callback&			m_cbk;
    Timestamp			m_start_time;
  };
}
#endif
#endif
