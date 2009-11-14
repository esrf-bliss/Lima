#ifndef HWFRAMECALLBACK_H
#define HWFRAMECALLBACK_H

#include "SizeUtils.h"
#include "Timestamp.h"
#include "HwFrameInfo.h"
#include "Debug.h"

namespace lima
{

class HwFrameCallback;


/*******************************************************************
 * \class HwFrameCallbackGen
 * \brief Abstract class with interface for frame callback generation
 *
 * This class specifies the basic functionality for objects generating
 * frame callbacks
 *******************************************************************/

class HwFrameCallbackGen
{
	DEB_CLASS(DebModHardware, "HwFrameCallbackGen");

 public:
	HwFrameCallbackGen();
	virtual ~HwFrameCallbackGen();

	void registerFrameCallback(HwFrameCallback& frame_cb);
	void unregisterFrameCallback(HwFrameCallback& frame_cb);

 protected:
	virtual void setFrameCallbackActive(bool cb_active) = 0;
	bool newFrameReady(const HwFrameInfoType& frame_info);
	
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
	DEB_CLASS(DebModHardware, "HwFrameCallback");

 public:
	HwFrameCallback();
	virtual ~HwFrameCallback();

	HwFrameCallbackGen *getFrameCallbackGen() const;

 protected:
	virtual bool newFrameReady(const HwFrameInfoType& frame_info) = 0;

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
