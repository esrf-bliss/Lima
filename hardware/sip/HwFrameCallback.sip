class HwFrameCallbackGen
{
%TypeHeaderCode
#include <HwFrameCallback.h>
using namespace lima;
%End
 public:
	HwFrameCallbackGen();
	virtual ~HwFrameCallbackGen();

	void registerFrameCallback(HwFrameCallback& frame_cb);
	void unregisterFrameCallback(HwFrameCallback& frame_cb);

 protected:
	virtual void setFrameCallbackActive(bool cb_active) = 0;
	bool newFrameReady(const HwFrameInfoType& frame_info);
	
};

class HwFrameCallback
{
%TypeHeaderCode
#include <HwFrameCallback.h>
using namespace lima;
%End
 public:
	HwFrameCallback();
	virtual ~HwFrameCallback();

	HwFrameCallbackGen *getFrameCallbackGen() const;

 protected:
	virtual bool newFrameReady(const HwFrameInfoType& frame_info) = 0;
};
