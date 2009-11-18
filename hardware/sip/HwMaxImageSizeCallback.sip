class HwMaxImageSizeCallback
{
%TypeHeaderCode
#include "HwMaxImageSizeCallback.h"
using namespace lima;
%End
 public:
	HwMaxImageSizeCallback();
	virtual ~HwMaxImageSizeCallback();

	HwMaxImageSizeCallbackGen *getMaxImageSizeCallbackGen() const;

 protected:
	virtual void maxImageSizeChanged(const Size& size, 
					 ImageType image_type) = 0;
};

class HwMaxImageSizeCallbackGen
{
%TypeHeaderCode
#include "HwMaxImageSizeCallback.h"
using namespace lima;
%End
 public:
	HwMaxImageSizeCallbackGen();
	virtual ~HwMaxImageSizeCallbackGen();

	void registerMaxImageSizeCallback(HwMaxImageSizeCallback& cb);
	void unregisterMaxImageSizeCallback(HwMaxImageSizeCallback& cb);

 protected:
	virtual void setMaxImageSizeCallbackActive(bool cb_active) = 0;
	void maxImageSizeChanged(const Size& size, ImageType image_type);
};

