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
    struct LIMACORE_API Parameters 
    {
      DEB_CLASS_NAMESPC(DebModControl,"Buffer::Parameters","Control");
    public:
      Parameters();
      void reset();
      void resetNonPersistent();
      BufferMode mode;
      long	nbBuffers;
      short	maxMemory;
    };

    CtBuffer(HwInterface *hw);
    ~CtBuffer();

    void setPars(Parameters pars);
    void getPars(Parameters& pars) const;

    void setMode(BufferMode mode);
    void getMode(BufferMode& mode) const;

    void setNumber(long nb_buffers);
    void getNumber(long& nb_buffers) const;

    void getMaxNumber(long& nb_buffers) const;

    void setMaxMemory(short max_memory);
    void getMaxMemory(short& max_memory) const;

    void registerFrameCallback(CtControl *ct);
    void unregisterFrameCallback();
	
    void getFrame(Data&,int frameNumber,int readBlockLen=1);

    void reset();
    void setup(CtControl *ct);

    bool isAccumulationActive() const {return !!m_ct_accumulation;}

    void getDataFromHwFrameInfo(Data&,const HwFrameInfoType&,
				int readBlockLen=1);
    void transformHwFrameInfoToData(Data&,const HwFrameInfoType&,
					   int readBlockLen=1);

    bool waitBuffersReleased(double timeout=-1);

  private:
    class _DataBuffer;
    friend class _DataBuffer;

    void _release(_DataBuffer *buffer);

    Cond			m_cond;
    HwBufferCtrlObj* 		m_hw_buffer;
    CtBufferFrameCB* 		m_frame_cb;
    Parameters			m_pars;
    CtAccumulation* 		m_ct_accumulation;
    HwBufferCtrlObj::Callback* 	m_hw_buffer_cb;
    int				m_mapped_frames;
  };

  inline std::ostream& operator<<(std::ostream &os,
				  const CtBuffer::Parameters &params)
  {
    os << "<"
       << "mode=" << params.mode << ", "
       << "nbBuffers=" << params.nbBuffers << ", "
       << "maxMemory=" << params.maxMemory << ", "
       << ">";
    return os;
  }
} // namespace lima

#endif // CTBUFFER_H
