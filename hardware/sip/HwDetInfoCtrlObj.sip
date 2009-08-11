class HwDetInfoCtrlObj
{
%TypeHeaderCode
#include "HwDetInfoCtrlObj.h"
using namespace lima;
%End
 public:
	virtual ~HwDetInfoCtrlObj();
	virtual void getMaxImageSize(Size& size /Out/) = 0;
	virtual void getDetectorImageSize(Size& size /Out/) = 0;
	virtual void getDefImageType(ImageType& def_image_type /Out/) = 0;
	virtual void getCurrImageType(ImageType& curr_image_type /Out/) = 0;
	virtual void setCurrImageType(ImageType  curr_image_type) = 0;

	virtual void getPixelSize(double& pixel_size /Out/) = 0;
	virtual void getDetectorType(std::string& det_type /Out/) = 0;
	virtual void getDetectorModel(std::string& det_model /Out/) = 0;
protected:
	virtual void setMaxImageSizeCallbackActive(bool cb_active) = 0;
	void maxImageSizeChanged(const Size& size, ImageType image_type);
};
