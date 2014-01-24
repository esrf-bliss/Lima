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
#include "MetaInterface.h"
#include "MetaDetInfoCtrlObj.h"
#include "MetaSyncCtrlObj.h"
#include "HwReconstructionCtrlObj.h"

#include "PoolThreadMgr.h"
#include "TaskMgr.h"
#include "LinkTask.h"

#include "CtBuffer.h"

using namespace lima;
using namespace lima::Meta;

#define FOR_ALL(fct)							\
  for(Interface::TileCnt::iterator i = m_tiles.begin();			\
      i != m_tiles.end();++i)						\
    {									\
      i->second->fct;							\
    }									

class Interface::_BufferFrameCBK : public HwFrameCallback
{
  DEB_CLASS_NAMESPC(DebModCamera,"_BufferFrameCBK","Meta");
public:
  class cbk : public TaskEventCallback
  {
  public:
    cbk(Interface::_BufferFrameCBK& i) : m_cnt(i) {}
    virtual void finished(Data& aData);
  private:
    Interface::_BufferFrameCBK& m_cnt;
  };
  friend class cbk;
  _BufferFrameCBK(Interface& i,
		  int row,int column,HwInterface* hwInterface) : 
    m_interface(i),
    m_row(row),
    m_column(column),
    m_hwinterface(hwInterface)
  {
    m_reconstruction_cbk = new cbk(*this);
  }
  ~_BufferFrameCBK()
  {
    delete m_reconstruction_cbk;
  }
protected:
  bool newFrameReady(const HwFrameInfoType& frame_info)
  {
    Data aFrameData;
    CtBuffer::transformHwFrameInfoToData(aFrameData,frame_info);

    HwReconstructionCtrlObj* reconstruction;
    m_hwinterface->getHwCtrlObj(reconstruction);
    if(reconstruction)
      {
	LinkTask* rTaskPt = reconstruction->getReconstructionTask();
	if(rTaskPt)
	  {
	    TaskMgr *mgr = new TaskMgr();
	    mgr->setInputData(aFrameData);
	    rTaskPt->setEventCallback(m_reconstruction_cbk);
	    mgr->setLinkTask(0,rTaskPt);
	    PoolThreadMgr::get().addProcess(mgr);
	    return true;
	  }
      }

    return m_interface._newFrameReady(m_row,m_column,aFrameData);
  }
  void newFrameReconstructed(Data& data)
  {
    m_interface._newFrameReady(m_row,m_column,data);
  }
private:
  Interface&	m_interface;
  int		m_row;
  int		m_column;
  HwInterface*	m_hwinterface;
  cbk*		m_reconstruction_cbk;
};

void Interface::_BufferFrameCBK::cbk::finished(Data& data)
{
  m_cnt.newFrameReconstructed(data);
}

Interface::Interface(Interface::Geometry geom) :
  m_geometry(geom),
  m_dirty_geom_flag(true)
{
  DEB_CONSTRUCTOR();

  m_det_info = new DetInfoCtrlObj(*this);
  m_sync = new SyncCtrlObj(*this);
}

Interface::~Interface()
{
  delete m_det_info;
  delete m_sync;
  for(BufferFrameCBKs::iterator i = m_buffer_cbks.begin();
      i != m_buffer_cbks.end();++i)
    delete *i;
}

void Interface::addInterface(int row,int column,HwInterface* i)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR3(row,column,i);

  m_det_info->_addInterface(i);

  HwBufferCtrlObj* hw_buffer;
  if(!i->getHwCtrlObj(hw_buffer))
    THROW_HW_ERROR(Error) <<  "Cannot get hardware buffer object";
  
  _BufferFrameCBK *cbk = new _BufferFrameCBK(*this,row,column,i);
  m_buffer_cbks.push_back(cbk);
  hw_buffer->registerFrameCallback(*cbk);

  m_tiles[ColumnRow(column,row)] = i;

  m_dirty_geom_flag = true;

}

void Interface::getCapList(CapList& cap_list) const
{
  cap_list.push_back(HwCap(m_det_info));
  cap_list.push_back(HwCap(m_sync));
  cap_list.push_back(HwCap(const_cast<SoftBufferCtrlObj*>(&m_buffer)));
}

void Interface::reset(ResetLevel reset_level)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(reset_level);

  FOR_ALL(reset(reset_level));
}

void Interface::prepareAcq()
{
  DEB_MEMBER_FUNCT();
  
  m_continue_acq = true;
  
  // preparing buffer
  int nb_frames;
  m_sync->getNbHwFrames(nb_frames);
  if(nb_frames > 8 || !nb_frames) nb_frames = 8;
  for(Interface::TileCnt::iterator i = m_tiles.begin();
      i != m_tiles.end();++i)
    {
      HwBufferCtrlObj* hw_buffer;
      if(!i->second->getHwCtrlObj(hw_buffer))
	THROW_HW_ERROR(Error) <<  "Cannot get hardware buffer object";

      HwDetInfoCtrlObj* hw_det;
      if(!i->second->getHwCtrlObj(hw_det))
	THROW_HW_ERROR(Error) << "Cannot get hardware detector info";

      Size max_size;
      hw_det->getMaxImageSize(max_size);
      ImageType img_type;
      hw_det->getCurrImageType(img_type);

      FrameDim fdim(max_size,img_type);
      hw_buffer->setFrameDim(fdim);
      hw_buffer->setNbConcatFrames(1);
      
      int max_nbuffers;
      hw_buffer->getMaxNbBuffers(max_nbuffers);
      int nb_buffer = nb_frames > max_nbuffers ? max_nbuffers : nb_frames;
      hw_buffer->setNbBuffers(nb_buffer);
    }

  FOR_ALL(prepareAcq());

  if(m_dirty_geom_flag)
    {
      m_dirty_geom_flag = false;
      m_tiles_offset.clear();

      Size det_image_size;
      m_det_info->getDetectorImageSize(det_image_size);
      
      m_det_info->getCurrImageType(m_curr_type);
      int depth = FrameDim::getImageTypeDepth(m_curr_type);
      m_width_step = det_image_size.getWidth() * depth;


      Interface::TileCnt::iterator i = m_tiles.begin();
      HwDetInfoCtrlObj* local_det_info;
      if(i == m_tiles.end())
	THROW_HW_ERROR(Error) << "Meta doesn't have any sub detector";
      if(!i->second->getHwCtrlObj(local_det_info))
	THROW_HW_ERROR(Error) << "Cannot get hardware det info";

      Size local_max_image_size;
      int prev_row = i->first.second;
      local_det_info->getMaxImageSize(local_max_image_size);
      int row_width = local_max_image_size.getWidth();
      int row_height = local_max_image_size.getHeight();

      int offset = 0;
      m_tiles_offset[ColumnRow(i->first.first,i->first.second)] = offset;


      for(++i;i != m_tiles.end();++i)
	{
	  if(!i->second->getHwCtrlObj(local_det_info))
	    THROW_HW_ERROR(Error) << "Cannot get hardware det info";
	  local_det_info->getMaxImageSize(local_max_image_size);
	  
	  if(prev_row == i->first.second) // new column, same row
	    {
	      m_tiles_offset[ColumnRow(i->first.first,
				       i->first.second)] = offset + row_width * depth;
	      row_height = std::max(row_height,
				    local_max_image_size.getHeight());
	      row_width += local_max_image_size.getWidth();
	    }
	  else			// new row
	    {
	      prev_row = i->first.second;
	      offset += row_width * row_height * depth;
	      
	      m_tiles_offset[ColumnRow(i->first.first,i->first.second)] = offset;

	      row_height = local_max_image_size.getHeight();
	      row_width = local_max_image_size.getWidth();
	    }
	}
    }
}

void Interface::startAcq()
{
  DEB_MEMBER_FUNCT();
  m_buffer.getBuffer().setStartTimestamp(Timestamp::now());
  FOR_ALL(startAcq());
}

void Interface::stopAcq()
{
  DEB_MEMBER_FUNCT();
  
  FOR_ALL(stopAcq());
}

void Interface::getStatus(StatusType& status)
{
  DEB_MEMBER_FUNCT();

  Interface::TileCnt::iterator i = m_tiles.begin();
  i->second->getStatus(status);

  for(++i;i != m_tiles.end();++i)
    {
      StatusType local_status;
      i->second->getStatus(local_status);
      if(local_status.acq == AcqFault)
	{
	  status = local_status;
	  break;
	}
      else if(local_status.acq != AcqReady)
	status = local_status;
    }
  DEB_RETURN() << DEB_VAR1(status);
}

int Interface::getNbHwAcquiredFrames()
{
  DEB_MEMBER_FUNCT();

  Interface::TileCnt::iterator i = m_tiles.begin();
  int nb_frames = i->second->getNbHwAcquiredFrames();
  for(++i;i != m_tiles.end();++i)
    {
      int local_nb_frames = i->second->getNbHwAcquiredFrames();
      if(local_nb_frames < nb_frames)
	nb_frames = local_nb_frames;
    }

  DEB_RETURN() << DEB_VAR1(nb_frames);
  return nb_frames;
}

bool Interface::_newFrameReady(int row,int column,
			       const Data& frame)
{
  int offset = m_tiles_offset[ColumnRow(column,row)];
  int frame_width = frame.dimensions[0];
  int frame_height = frame.dimensions[1];
  int depth = frame.depth();
  StdBufferCbMgr& buffer_mgr = m_buffer.getBuffer();
  char *ptr = (char*)buffer_mgr.getFrameBufferPtr(frame.frameNumber);
  ptr += offset;

  if(column == 0 && frame_width * depth == m_width_step)
    memcpy(ptr,frame.data(),frame.size());
  else
    {
      int nb_row = frame_height;
      int src_width_step = frame_width * depth;
      char* srcPt = (char*)frame.data();
      for(int row = 0;row < nb_row;++row,srcPt += src_width_step,ptr += m_width_step)
	memcpy(ptr,srcPt,src_width_step);
    }
  AutoMutex aLock(m_lock);
  std::pair<PendingFrames::iterator,bool> result= 
    m_pending_frames.insert(std::pair<int,int>(frame.frameNumber,1));
  //if it exist, increment
  if(!result.second) ++result.first->second;
 
  if(result.first->second == int(m_tiles.size())) // frame is finished
    {
      for(PendingFrames::iterator k = m_pending_frames.begin();
	  m_continue_acq && k != m_pending_frames.end();
	  m_pending_frames.erase(k++))
	{
	  if(k->second == int(m_tiles.size()))
	    {
	      HwFrameInfoType new_frame_info;
	      new_frame_info.acq_frame_nb = k->first;
	      if(!buffer_mgr.newFrameReady(new_frame_info))
		m_continue_acq = false;
	    }
	  else
	    break;
	}
    }
  
  return m_continue_acq;
}
