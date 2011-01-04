#ifndef HWMAXIMAGESIZECALLBACK_H
#define HWMAXIMAGESIZECALLBACK_H

#include "LimaCompatibility.h"
#include "SizeUtils.h"
#include "Debug.h"

namespace lima 
{

class HwMaxImageSizeCallbackGen;

class LIMACORE_API HwMaxImageSizeCallback
{
	DEB_CLASS(DebModHardware, "HwMaxImageSizeCallback");

 public:
	HwMaxImageSizeCallback();
	virtual ~HwMaxImageSizeCallback();

	HwMaxImageSizeCallbackGen *getMaxImageSizeCallbackGen() const;

 protected:
	virtual void maxImageSizeChanged(const Size& size, 
					 ImageType image_type) = 0;

 private:
	friend class HwMaxImageSizeCallbackGen;
	void setMaxImageSizeCallbackGen(HwMaxImageSizeCallbackGen *mis_cb_gen);

	HwMaxImageSizeCallbackGen *m_mis_cb_gen;
};

inline HwMaxImageSizeCallbackGen *
HwMaxImageSizeCallback::getMaxImageSizeCallbackGen() const
{
	return m_mis_cb_gen;
}


class LIMACORE_API HwMaxImageSizeCallbackGen
{
	DEB_CLASS(DebModHardware, "HwMaxImageSizeCallbackGen");

 public:
	HwMaxImageSizeCallbackGen();
	virtual ~HwMaxImageSizeCallbackGen();

	void registerMaxImageSizeCallback(HwMaxImageSizeCallback& cb);
	void unregisterMaxImageSizeCallback(HwMaxImageSizeCallback& cb);

 protected:
	virtual void setMaxImageSizeCallbackActive(bool cb_active);
	void maxImageSizeChanged(const Size& size, ImageType image_type);

 private:
	HwMaxImageSizeCallback *m_mis_cb;

};


} // namespace lima

#endif // HWMAXIMAGESIZECALLBACK_H
