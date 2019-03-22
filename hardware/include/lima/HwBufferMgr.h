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

#include "lima/LimaCompatibility.h"
#include "lima/HwFrameCallback.h"
#include "lima/HwBufferCtrlObj.h"
#include "lima/MemUtils.h"

#include <memory>
#include <vector>
#include <set>

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

	// BufferAllocMgr are **not** copy-constructible nor copy-assignable.
	BufferAllocMgr(const BufferAllocMgr&) = delete;
	BufferAllocMgr& operator=(const BufferAllocMgr&) = delete;

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
	virtual void allocBuffers(int nb_buffers, const FrameDim& frame_dim) override;
	virtual const FrameDim& getFrameDim();
	virtual void getNbBuffers(int& nb_buffers);
	virtual void releaseBuffers();

	virtual void *getBufferPtr(int buffer_nb);

 protected:
	typedef std::vector<MemBuffer> BufferList;
	typedef BufferList::const_reverse_iterator BufferListCRIt;

	FrameDim m_frame_dim;
	BufferList m_buffer_list;
};


#ifdef LIMA_USE_NUMA

/// MemBufferGroup is a group of buffers that share some properties
/// such as the allocator (but more could be added)
template <typename Iterator>
struct MemBufferGroup
{
	MemBufferGroup(unsigned long cpu_mask) : allocator(cpu_mask) {}

	NumaAllocator allocator;
	Iterator begin;
	Iterator end;
};

/// A SoftBufferAllocMgr that manages groups of buffer that have different allocators
class LIMACORE_API NumaSoftBufferAllocMgr : public SoftBufferAllocMgr
{
	DEB_CLASS(DebModHardware, "NumaSoftBufferAllocMgr");

 public:
	NumaSoftBufferAllocMgr() {}
	virtual ~NumaSoftBufferAllocMgr() = default;

	virtual void allocBufferGroup(size_t nb_buffers, unsigned long cpu_mask, const FrameDim& frame_dim)
	{
		Group group(cpu_mask);

		//Allocate the buffers per groups
		group.begin = m_buffer_list.end();
		m_buffer_list.resize(nb_buffers, MemBuffer(&group.allocator));
		group.end = m_buffer_list.end();

		m_buffer_groups.emplace_back(group);
	}

 private:
	typedef MemBufferGroup<BufferList::iterator> Group;
	typedef std::vector<Group> Groups;

	Groups m_buffer_groups;
};

#endif //LIMA_USE_NUMA

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
	virtual void acqFrameNb2BufferNb(int acq_frame_nb, int& buffer_nb,
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

	virtual void* getFrameBufferPtr(int frame_nb);
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

/// This class is a basic HwBufferCtrlObj software allocation implementation,
/// It can be directly provided to the control layer as a HwBufferCtrlObj.
class LIMACORE_API BasicSoftBufferCtrlObj : public HwBufferCtrlObj
{
public:
	BasicSoftBufferCtrlObj(BufferAllocMgr* buffer_alloc_mgr);
	virtual ~BasicSoftBufferCtrlObj() = default;

	virtual void setFrameDim(const FrameDim& frame_dim);
	virtual void getFrameDim(FrameDim& frame_dim);

	virtual void setNbBuffers(int  nb_buffers);
	virtual void getNbBuffers(int& nb_buffers);

	virtual void setNbConcatFrames(int nb_concat_frames);
	virtual void getNbConcatFrames(int& nb_concat_frames);

	virtual void getMaxNbBuffers(int& max_nb_buffers);

	virtual void *getBufferPtr(int buffer_nb,int concat_frame_nb = 0);
	virtual void *getFramePtr(int acq_frame_nb);

	virtual void getStartTimestamp(Timestamp& start_ts);
	virtual void getFrameInfo(int acq_frame_nb, HwFrameInfoType& info);

	virtual void registerFrameCallback(HwFrameCallback& frame_cb);
	virtual void unregisterFrameCallback(HwFrameCallback& frame_cb);

	StdBufferCbMgr&  getBuffer();

	int getNbAcquiredFrames();

	class LIMACORE_API Sync : public HwBufferCtrlObj::Callback
	{
		DEB_CLASS(DebModHardware, "SoftBufferCtrlObj::Sync");

		friend class SoftBufferCtrlObj;
	public:
		enum Status {
			AVAILABLE,TIMEOUT,INTERRUPTED
		};

		Sync(BasicSoftBufferCtrlObj& buffer_ctrl_obj, Cond& cond);
		virtual ~Sync();

		// Important: must be called with the cond.mutex locked!
		Status wait(int frame_number, double timeout = -1.);

	protected:
		virtual void map(void *address);
		virtual void release(void *address);
		virtual void releaseAll();

	private:
		typedef std::multiset<void *> BufferList;

		Cond&			m_cond;
		BasicSoftBufferCtrlObj& 	m_buffer_ctrl_obj;
		BufferList		m_buffer_in_use;
	};

	Sync* getBufferSync(Cond& cond);
    
	virtual HwBufferCtrlObj::Callback* getBufferCallback();

protected:
	using BufferAllocMgrPtr = std::unique_ptr<BufferAllocMgr>;

	BufferAllocMgrPtr	m_buffer_alloc_mgr;
	StdBufferCbMgr 		m_buffer_cb_mgr;
	BufferCtrlMgr		m_mgr;
	int				m_acq_frame_nb;
	std::unique_ptr<HwBufferCtrlObj::Callback> 	m_buffer_callback;
};


class LIMACORE_API SoftBufferCtrlObj : public BasicSoftBufferCtrlObj
{
public:
	SoftBufferCtrlObj() : BasicSoftBufferCtrlObj(new SoftBufferAllocMgr) {}
	virtual ~SoftBufferCtrlObj() = default;
};


#ifdef LIMA_USE_NUMA

/// This class is a NUMA HwBufferCtrlObj software allocation implementation,
/// It can be directly provided to the control layer as a HwBufferCtrlObj.
class LIMACORE_API NumaSoftBufferCtrlObj : public BasicSoftBufferCtrlObj
{
public:
	NumaSoftBufferCtrlObj() : BasicSoftBufferCtrlObj(new NumaSoftBufferAllocMgr) {}
	virtual ~NumaSoftBufferCtrlObj() = default;

	//TODO  Gives access to the groups

};

#endif // LIMA_USE_NUMA

} // namespace lima

#endif // HWBUFFERMGR_H
