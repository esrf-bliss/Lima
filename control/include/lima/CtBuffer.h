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
#ifndef CTBUFFER_H
#define CTBUFFER_H

#include "lima/Constants.h"
#include "lima/SizeUtils.h"
#include "lima/CtAcquisition.h"
#include "lima/CtImage.h"
#include "lima/HwInterface.h"
#include "lima/HwFrameCallback.h"

#include "processlib/Data.h"

namespace lima {

  class LIMACORE_API CtBufferFrameCB : public HwFrameCallback
  {
    DEB_CLASS_NAMESPC(DebModControl,"BufferFrameCB","Control");
    friend class CtBuffer;
  public:
    CtBufferFrameCB(CtControl *ct): m_ct(ct),m_ct_accumulation(NULL) {}
  protected:
    bool newFrameReady(const HwFrameInfoType& frame_info);
  private:
    CtControl* 		m_ct;
    CtAccumulation* 	m_ct_accumulation;
  };

  /// Controls buffer settings such as number of buffers, binning and rotation
  class LIMACORE_API CtBuffer 
  {
    DEB_CLASS_NAMESPC(DebModControl,"Buffer","Control");

  public:
    typedef BufferHelper::Parameters Parameters;

    CtBuffer(HwInterface *hw);
    ~CtBuffer();

    void setAllocParameters(const Parameters& params);
    void getAllocParameters(Parameters& params) const;

    void getNumber(long& nb_buffers) const;
    void getMaxHwNumber(long& nb_buffers) const;
    void getMaxAccNumber(long& nb_buffers) const;
    void getMaxNumber(long& nb_buffers) const;

    void registerFrameCallback(CtControl *ct);
    void unregisterFrameCallback();

    void getFrame(Data&,int frameNumber,int readBlockLen=1);

    void reset();
    void setup(CtControl *ct);

    bool isAccumulationActive() const {return !!m_ct_accumulation;}

    void getDataFromHwFrameInfo(Data& fdata,const HwFrameInfoType& frame_info,
                                int readBlockLen=1);
    template <class D>
    static void getDataFromAnonymousHwFrameInfo(Data& fdata,
                                                const HwFrameInfoType& frame_info,
                                                D&& deleter, int readBlockLen=1)
    {
      _initDataFromHwFrameInfo(fdata,frame_info,readBlockLen);
      BufferBase *fbuf;
      void *ptr= frame_info.frame_ptr;
      if(frame_info.buffer_owner_ship == HwFrameInfoType::Managed)
        fbuf= new MappedBuffer(ptr, std::forward<D>(deleter));
      else
        fbuf= new Buffer(ptr);
      fdata.setBuffer(fbuf);
      fbuf->unref();
    }

    bool waitBuffersReleased(double timeout=-1);

  private:
    class _DataBuffer;
    friend class _DataBuffer;

    void _release(_DataBuffer *buffer);

    static void _initDataFromHwFrameInfo(Data& fdata,
					 const HwFrameInfoType& frame_info,
                                         int readBlockLen);

    Cond			m_cond;
    HwBufferCtrlObj* 		m_hw_buffer;
    CtBufferFrameCB* 		m_frame_cb;
    Parameters			m_params;
    CtAccumulation* 		m_ct_accumulation;
    HwBufferCtrlObj::Callback* 	m_hw_buffer_cb;
    int				m_nb_buffers;
    int				m_mapped_frames;
  };

} // namespace lima

#endif // CTBUFFER_H
