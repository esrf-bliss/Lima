class HwDetInfoCtrlObj
{
%TypeHeaderCode
#include "HwDetInfoCtrlObj.h"
using namespace lima;
%End
 public:
	virtual ~HwDetInfoCtrlObj();
	virtual Size getMaxImageSize() [void (Size&)];
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipRes = new Size();
sipCpp->getMaxImageSize(*sipRes);
Py_END_ALLOW_THREADS
%End
	virtual Size getDetectorImageSize() [void (Size&)];
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipRes = new Size();
sipCpp->getDetectorImageSize(*sipRes);
Py_END_ALLOW_THREADS
%End

	virtual void getDefImageType(ImageType& def_image_type) = 0;
	virtual void getCurrImageType(ImageType& curr_image_type) = 0;
	virtual void setCurrImageType(ImageType  curr_image_type) = 0;

	virtual void getPixelSize(double& pixel_size) = 0;
	virtual void getDetectorType(std::string& det_type) = 0;
	virtual void getDetectorModel(std::string& det_model) = 0;
protected:
	virtual void setMaxImageSizeCallbackActive(bool cb_active) = 0;
	void maxImageSizeChanged(const Size& size, ImageType image_type);
};
