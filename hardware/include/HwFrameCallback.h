#ifndef HWFRAMECALLBACK_H
#define HWFRAMECALLBACK_H

#include "SizeUtils.h"
#include "Timestamp.h"

namespace lima
{

class HwFrameCallback;


extern const double UnsetTimestamp;

/*******************************************************************
 * \typedef FrameInfoType
 * \brief Structure containing information about acquired frame
 *
 *
 *******************************************************************/

typedef struct FrameInfo {
	int acq_frame_nb;
	void *frame_ptr;
	const FrameDim *frame_dim;
	Timestamp frame_timestamp;

	FrameInfo() 
		: acq_frame_nb(-1), frame_ptr(NULL), frame_dim(),
		  frame_timestamp() {}

	FrameInfo(int frame_nb, void *ptr, const FrameDim *dim, 
		  Timestamp timestamp) 
		: acq_frame_nb(frame_nb), frame_ptr(ptr), frame_dim(dim),
		  frame_timestamp(timestamp) {}
} FrameInfoType;


/*******************************************************************
 * \class HwFrameCallbackGen
 * \brief Abstract class with interface for frame callback generation
 *
 * This class specifies the basic functionality for objects generating
 * frame callbacks
 *******************************************************************/

class HwFrameCallbackGen
{
 public:
	virtual ~HwFrameCallbackGen();

	void registerFrameCallback(HwFrameCallback *frame_cb);
	void unregisterFrameCallback(HwFrameCallback *frame_cb);

 protected:
	HwFrameCallbackGen();
	virtual void setFrameCallbackActive(bool cb_active) = 0;
	bool newFrameReady(const FrameInfoType& frame_info);
	
 private:
	HwFrameCallback *m_frame_cb;
};


/*******************************************************************
 * \class HwFrameCallback
 * \brief Base class for classes receiving frame callbacks
 *
 *******************************************************************/

class HwFrameCallback
{
 public:
	HwFrameCallback() : m_frame_cb_gen(NULL) {}
	virtual ~HwFrameCallback();

	HwFrameCallbackGen *getFrameCallbackGen() const;

 protected:
	virtual bool newFrameReady(const FrameInfoType& frame_info) = 0;

 private:
	friend class HwFrameCallbackGen;
	void setFrameCallbackGen(HwFrameCallbackGen *frame_cb_gen);

	HwFrameCallbackGen *m_frame_cb_gen;

};

inline HwFrameCallbackGen *HwFrameCallback::getFrameCallbackGen() const
{
	return m_frame_cb_gen;
}

} // namespace lima

#endif // HWFRAMECALLBACK_H
