#ifndef HWBUFFERMGR_H
#define HWBUFFERMGR_H

#include "HwFrameCallback.h"
#include "SizeUtils.h"

#include <vector>

namespace lima
{

/*******************************************************************
 * \class BufferAllocMgr
 * \brief Abstract class defining interface for buffer allocation
 *
 * Specifies the basic functionality for allocating frame buffers
 *******************************************************************/

class BufferAllocMgr
{
 public:
	virtual ~BufferAllocMgr();

	virtual int getMaxNbBuffers(const FrameDim& frame_dim) = 0;
	virtual void allocBuffers(int nb_buffers, 
				  const FrameDim& frame_dim) = 0;
	virtual const FrameDim& getFrameDim() = 0;
	virtual int getNbBuffers() = 0;
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

class SoftBufferAllocMgr : public BufferAllocMgr
{
 public:
	SoftBufferAllocMgr();
	virtual ~SoftBufferAllocMgr();

	virtual int getMaxNbBuffers(const FrameDim& frame_dim);
	virtual void allocBuffers(int nb_buffers, 
				  const FrameDim& frame_dim);
	virtual const FrameDim& getFrameDim();
	virtual int getNbBuffers();
	virtual void releaseBuffers();

	virtual void *getBufferPtr(int buffer_nb);
	
 private:
	typedef std::vector<char *> BufferList;
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

class BufferCbMgr : public HwFrameCallbackGen
{
 public:
	enum Cap {
		Basic=0, Concat=1, Acc=2, // bit mask
	};

	virtual ~BufferCbMgr();

	virtual Cap getCap() = 0;

	virtual int getMaxNbBuffers(const FrameDim& frame_dim) = 0;
	virtual void allocBuffers(int nb_buffers, 
				  const FrameDim& frame_dim) = 0;
	virtual const FrameDim& getFrameDim() = 0;
	virtual int getNbBuffers() = 0;
	virtual void releaseBuffers() = 0;

	virtual void *getBufferPtr(int buffer_nb) = 0;

	virtual void clearBuffer(int buffer_nb) = 0;
	virtual void clearAllBuffers() = 0;

	virtual void setStartTimestamp(Timestamp  start_ts) = 0;
	virtual void getStartTimestamp(Timestamp& start_ts) = 0;

	virtual void getFrameInfo(int acq_frame_nb, HwFrameInfoType& info) = 0;
};


/*******************************************************************
 * \class StdBufferCbMgr
 * \brief Class providing standard buffer cb functionality
 *
 * This class implements the normal buffer mode using a basic 
 * BufferAllocMgr
 *******************************************************************/

class StdBufferCbMgr : public BufferCbMgr
{
 public:
	StdBufferCbMgr(BufferAllocMgr& alloc_mgr);
	virtual ~StdBufferCbMgr();

	virtual Cap getCap();

	virtual int getMaxNbBuffers(const FrameDim& frame_dim);
	virtual void allocBuffers(int nb_buffers, 
				  const FrameDim& frame_dim);
	virtual const FrameDim& getFrameDim();
	virtual int getNbBuffers();
	virtual void releaseBuffers();

	virtual void *getBufferPtr(int buffer_nb);

	virtual void clearBuffer(int buffer_nb);
	virtual void clearAllBuffers();

	virtual void setStartTimestamp(Timestamp  start_ts);
	virtual void getStartTimestamp(Timestamp& start_ts);

	virtual void getFrameInfo(int acq_frame_nb, HwFrameInfoType& info);

	bool newFrameReady(HwFrameInfoType& frame_info);

 protected:
	virtual void setFrameCallbackActive(bool cb_active);
	
 private:
	typedef std::vector<HwFrameInfoType> FrameInfoList;

	BufferAllocMgr& m_alloc_mgr;
	FrameInfoList m_info_list;
	Timestamp m_start_ts;
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

class BufferCtrlMgr
{
 public:
	enum AcqMode {
		Normal, Concat, Acc,
	};

	BufferCtrlMgr(BufferCbMgr& acq_buffer_mgr);
	~BufferCtrlMgr();

	void setFrameDim(const FrameDim& frame_dim);
	void getFrameDim(      FrameDim& frame_dim);

	void setNbBuffers(int  nb_buffers);
	void getNbBuffers(int& nb_buffers);

	void setNbConcatFrames(int  nb_concat_frames);
	void getNbConcatFrames(int& nb_concat_frames);

	void setNbAccFrames(int  nb_acc_frames);
	void getNbAccFrames(int& nb_acc_frames);

	void getMaxNbBuffers(int& max_nb_buffers);

	void *getBufferPtr(int buffer_nb);
	void *getFramePtr(int acq_frame_nb);

	void setStartTimestamp(Timestamp  start_ts);
	void getStartTimestamp(Timestamp& start_ts);

	void getFrameInfo(int acq_frame_nb, HwFrameInfoType& info);

	void   registerFrameCallback(HwFrameCallback *frame_cb);
	void unregisterFrameCallback(HwFrameCallback *frame_cb);

	BufferCbMgr& getAcqBufferMgr();
	AcqMode getAcqMode();

 private:
	int m_nb_concat_frames;
	int m_nb_acc_frames;
	BufferCbMgr& m_acq_buffer_mgr;
	SoftBufferAllocMgr m_aux_alloc_mgr;
	StdBufferCbMgr m_aux_buffer_mgr;
	BufferCbMgr *m_effect_buffer_mgr;
	FrameDim m_frame_dim;
};




} // namespace lima

#endif // HWBUFFERMGR_H
