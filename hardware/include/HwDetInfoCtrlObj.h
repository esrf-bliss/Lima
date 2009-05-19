#ifndef HWDETINFOCTRLOBJ_H
#define HWDETINFOCTRLOBJ_H

#include "SizeUtils.h"
#include <string>

namespace lima
{

class HwDetInfoCtrlObj;

class HwMaxImageSizeCallback
{
 public:
	HwMaxImageSizeCallback() : m_det_info_ctrl_obj(NULL) {}
	virtual ~HwMaxImageSizeCallback();

	HwDetInfoCtrlObj *getDetInfoCtrlObj() const;

 protected:
	virtual void maxImageSizeChanged(const Size& size, 
					 ImageType& image_type) = 0;

 private:
	friend class HwDetInfoCtrlObj;
	void setDetInfoCtrlObj(HwDetInfoCtrlObj *det_info_ctrl_obj);

	HwDetInfoCtrlObj *m_det_info_ctrl_obj;
};

inline HwDetInfoCtrlObj *HwMaxImageSizeCallback::getDetInfoCtrlObj() const
{
	return m_det_info_ctrl_obj;
}


class HwDetInfoCtrlObj
{
 public:
	virtual ~HwDetInfoCtrlObj();

	virtual void getMaxImageSize(Size& max_image_size) = 0;
	virtual void getDetectorImageSize(Size& det_image_size) = 0;

	virtual void getDefImageType(ImageType& def_image_type) = 0;
	virtual void getCurrImageType(ImageType& curr_image_type) = 0;
	virtual void setCurrImageType(ImageType  curr_image_type) = 0;

	virtual void getPixelSize(double& pixel_size) = 0;
	virtual void getDetectorType(std::string& det_type) = 0;
	virtual void getDetectorModel(std::string& det_model) = 0;

	void registerMaxImageSizeCallback(HwMaxImageSizeCallback *cb);
	void unregisterMaxImageSizeCallback(HwMaxImageSizeCallback *cb);

 protected:
	virtual void setMaxImageSizeCallbackActive(bool cb_active) = 0;

 private:
	HwMaxImageSizeCallback *m_max_image_size_cb;
};


} // namespace lima


#endif // HWDETINFOCTRLOBJ_H
