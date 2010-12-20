#ifndef HWBUFFERMGR_H
#define HWBUFFERMGR_H

#include "Compatibility.h"
#include "HwFrameCallback.h"
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

class DLL_EXPORT BufferAllocMgr
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

class DLL_EXPORT SoftBufferAllocMgr : public BufferAllocMgr
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

class DLL_EXPORT BufferCbMgr : public HwFrameCallbackGen
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

class DLL_EXPORT StdBufferCbMgr : public BufferCbMgr
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

class DLL_EXPORT BufferCtrlMgr : public HwFrameCallbackGen
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
	class DLL_EXPORT AcqFrameCallback : public HwFrameCallback
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




} // namespace lima

#endif // HWBUFFERMGR_H
