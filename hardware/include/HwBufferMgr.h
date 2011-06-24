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
#ifndef HWBUFFERMGR_H
#define HWBUFFERMGR_H

#include "LimaCompatibility.h"
#include "HwFrameCallback.h"
#include "HwBufferCtrlObj.h"
#include "MemUtils.h"

#include <vector>

namespace lima
{

/*******************************************************************
 * \class BufferAllocMgr
 * \brief Abstract class defining interface for buffer allocation
 *
 * Specifies the basic functionality for allocating frame buffers
 *******************************************************************/

class LIMACORE_API BufferAllocMgr
{
	DEB_CLASS(DebModHardware, "BufferAllocMgr");

 public:
	BufferAllocMgr();
	virtual ~BufferAllocMgr();

	virtual int getMaxNbBuffers(const FrameDim& frame_dim) = 0;
	virtual void allocBuffers(int nb_buffers, 
				  const FrameDim& frame_dim) = 0;
	virtual const FrameDim& getFrameDim() = 0;
	virtual void getNbBuffers(int& nb_buffers) = 0;
	virtual void releaseBuffers() = 0;

	virtual void *getBufferPtr(int buffer_nb) = 0;

	virtual void clearBuffer(int buffer_nb);
	virtual void clearAllBuffers();
};


/*******************************************************************
 * \class SoftBufferAllocMgr
 * \brief Simple full software implementation of BufferAllocMgr
 *
 * This classes uses new and delete to allocate the memory buffers
 *******************************************************************/

class LIMACORE_API SoftBufferAllocMgr : public BufferAllocMgr
{
	DEB_CLASS(DebModHardware, "SoftBufferAllocMgr");

 public:
	SoftBufferAllocMgr();
	virtual ~SoftBufferAllocMgr();

	virtual int getMaxNbBuffers(const FrameDim& frame_dim);
	virtual void allocBuffers(int nb_buffers, 
				  const FrameDim& frame_dim);
	virtual const FrameDim& getFrameDim();
	virtual void getNbBuffers(int& nb_buffers);
	virtual void releaseBuffers();

	virtual void *getBufferPtr(int buffer_nb);
	
 private:
	typedef std::vector<MemBuffer *> BufferList;
	typedef BufferList::const_iterator BufferListCIt;

	FrameDim m_frame_dim;
	BufferList m_buffer_list;
};


/*******************************************************************
 * \class BufferCbMgr
 * \brief Abstract class with interface for buffer alloc. and callbacks
 *
 * Specifies the basic functionality for allocating frame buffers and
 * for managing the frame callbacks
 *******************************************************************/

class LIMACORE_API BufferCbMgr : public HwFrameCallbackGen
{
	DEB_CLASS(DebModHardware, "BufferCbMgr");

 public:
	enum Cap {
		Basic=0, Concat=1, Acc=2, // bit mask
	};

	BufferCbMgr();
	virtual ~BufferCbMgr();

	virtual Cap getCap() = 0;

	virtual int getMaxNbBuffers(const FrameDim& frame_dim, 
				    int nb_concat_frames) = 0;
	virtual void allocBuffers(int nb_buffers, int nb_concat_frames, 
				  const FrameDim& frame_dim) = 0;
	virtual const FrameDim& getFrameDim() = 0;
	virtual void getNbBuffers(int& nb_buffers) = 0;
	virtual void getNbConcatFrames(int& nb_concat_frames) = 0;
	virtual void releaseBuffers() = 0;

	virtual void *getBufferPtr(int buffer_nb, int concat_frame_nb) = 0;

	virtual void clearBuffer(int buffer_nb);
	virtual void clearAllBuffers();

	virtual void setStartTimestamp(Timestamp  start_ts);
	virtual void getStartTimestamp(Timestamp& start_ts);

	virtual void getFrameInfo(int acq_frame_nb, HwFrameInfoType& info) = 0;

	virtual void getBufferFrameDim(const FrameDim& single_frame_dim,
				       int nb_concat_frames, 
				       FrameDim& buffer_frame_dim);
	virtual void acqFrameNb2BufferNb(int acq_frame_nb,int& buffer_nb,
					 int& concat_frame_nb);

 private:
	Timestamp m_start_ts;
};

BufferCbMgr::Cap operator |(BufferCbMgr::Cap c1, BufferCbMgr::Cap c2);
BufferCbMgr::Cap operator &(BufferCbMgr::Cap c1, BufferCbMgr::Cap c2);


/*******************************************************************
 * \class StdBufferCbMgr
 * \brief Class providing standard buffer cb functionality
 *
 * This class implements the normal buffer mode using a basic 
 * BufferAllocMgr
 *******************************************************************/

class LIMACORE_API StdBufferCbMgr : public BufferCbMgr
{
	DEB_CLASS(DebModHardware, "StdBufferCbMgr");

 public:
	StdBufferCbMgr(BufferAllocMgr& alloc_mgr);
	virtual ~StdBufferCbMgr();

	virtual Cap getCap();

	virtual int getMaxNbBuffers(const FrameDim& frame_dim, 
				    int nb_concat_frames);
	virtual void allocBuffers(int nb_buffers, int nb_concat_frames, 
				  const FrameDim& frame_dim);
	virtual const FrameDim& getFrameDim();
	virtual void getNbBuffers(int& nb_buffers);
	virtual void getNbConcatFrames(int& nb_concat_frames);
	virtual void releaseBuffers();

	virtual void *getBufferPtr(int buffer_nb, int concat_frame_nb);

	virtual void clearBuffer(int buffer_nb);
	virtual void clearAllBuffers();

	virtual void getFrameInfo(int acq_frame_nb, HwFrameInfoType& info);

	bool newFrameReady(HwFrameInfoType& frame_info);

 protected:
	virtual void setFrameCallbackActive(bool cb_active);
	
 private:
	typedef std::vector<HwFrameInfoType> FrameInfoList;

	BufferAllocMgr *m_alloc_mgr;
	FrameDim m_frame_dim;			  
	int m_nb_concat_frames;
	FrameInfoList m_info_list;
	bool m_fcb_act;
};


/*******************************************************************
 * \class BufferCtrlMgr
 * \brief Class providing full buffer functionality for a given hardware
 *
 * This class implements all the buffer functionality required by the
 * hardware buffer interface. It can use different kinds of acq. buffer
 * managers and complement their missing functionality.
 *******************************************************************/

class LIMACORE_API BufferCtrlMgr : public HwFrameCallbackGen
{
	DEB_CLASS(DebModHardware, "BufferCtrlMgr");

 public:
	enum AcqMode {
		Normal, Concat, Acc,
	};

	BufferCtrlMgr(BufferCbMgr& acq_buffer_mgr);
	~BufferCtrlMgr();

	void setFrameDim(const FrameDim& frame_dim);
	void getFrameDim(      FrameDim& frame_dim);

	void setNbConcatFrames(int  nb_concat_frames);
	void getNbConcatFrames(int& nb_concat_frames);

	void setNbBuffers(int  nb_buffers);
	void getNbBuffers(int& nb_buffers);

	void getMaxNbBuffers(int& max_nb_buffers);

	void *getBufferPtr(int buffer_nb, int concat_frame_nb = 0);
	void *getFramePtr(int acq_frame_nb);

	void setStartTimestamp(Timestamp  start_ts);
	void getStartTimestamp(Timestamp& start_ts);

	void getFrameInfo(int acq_frame_nb, HwFrameInfoType& info);

	BufferCbMgr& getAcqBufferMgr();
	AcqMode getAcqMode();

 protected:
	virtual void setFrameCallbackActive(bool cb_active);
	
 private:
	class LIMACORE_API AcqFrameCallback : public HwFrameCallback
	{
		DEB_CLASS(DebModHardware, "BufferCtrlMgr::AcqFrameCallback");

	public:
		AcqFrameCallback(BufferCtrlMgr& buffer_mgr);
		~AcqFrameCallback();

	protected:
		virtual bool newFrameReady(const HwFrameInfoType& finfo);

	private:
		BufferCtrlMgr *m_buffer_mgr;
	};
	friend class AcqFrameCallback;

	void releaseBuffers();
	bool acqFrameReady(const HwFrameInfoType& acq_frame_info);

	int m_nb_concat_frames;
	BufferCbMgr *m_acq_buffer_mgr;
	FrameDim m_frame_dim;
	AcqFrameCallback m_frame_cb;
	bool m_frame_cb_act;
};
/** @brief this class is a basic software allocation class,
 *  It can be directly provide to the control layer as a HwBufferCtrlObj
 */
class SoftBufferCtrlMgr : public HwBufferCtrlObj
{
 public:
  SoftBufferCtrlMgr() :
    HwBufferCtrlObj(),
    m_buffer_cb_mgr(m_buffer_alloc_mgr),
    m_mgr(m_buffer_cb_mgr),
    m_acq_frame_nb(-1)
      {}

    virtual void setFrameDim(const FrameDim& frame_dim) {m_mgr.setFrameDim(frame_dim);}
    virtual void getFrameDim(FrameDim& frame_dim) {m_mgr.getFrameDim(frame_dim);}

    virtual void setNbBuffers(int  nb_buffers) {m_mgr.setNbBuffers(nb_buffers);}
    virtual void getNbBuffers(int& nb_buffers) {m_mgr.getNbBuffers(nb_buffers);}

    virtual void setNbConcatFrames(int nb_concat_frames) {m_mgr.setNbConcatFrames(nb_concat_frames);}
    virtual void getNbConcatFrames(int& nb_concat_frames) {m_mgr.getNbConcatFrames(nb_concat_frames);}

    virtual void getMaxNbBuffers(int& max_nb_buffers) {m_mgr.getMaxNbBuffers(max_nb_buffers);}

    virtual void *getBufferPtr(int buffer_nb,int concat_frame_nb = 0)
    {return m_mgr.getBufferPtr(buffer_nb,concat_frame_nb);}

    virtual void *getFramePtr(int acq_frame_nb)
    {return m_mgr.getFramePtr(acq_frame_nb);}

    virtual void getStartTimestamp(Timestamp& start_ts) 
    {m_mgr.getStartTimestamp(start_ts);}
    virtual void getFrameInfo(int acq_frame_nb, HwFrameInfoType& info)
    {m_mgr.getFrameInfo(acq_frame_nb,info);}

    virtual void   registerFrameCallback(HwFrameCallback& frame_cb)
    {m_mgr.registerFrameCallback(frame_cb);}
    virtual void unregisterFrameCallback(HwFrameCallback& frame_cb) 
    {m_mgr.unregisterFrameCallback(frame_cb);}

    StdBufferCbMgr&  getBuffer() {return m_buffer_cb_mgr;}

    int		     getNbAcquiredFrames() {return m_acq_frame_nb + 1;}
 protected:
    SoftBufferAllocMgr 	m_buffer_alloc_mgr;
    StdBufferCbMgr 	m_buffer_cb_mgr;
    BufferCtrlMgr	m_mgr;
    int			m_acq_frame_nb;
};
} // namespace lima

#endif // HWBUFFERMGR_H
