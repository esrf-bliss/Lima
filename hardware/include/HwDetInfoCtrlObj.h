#ifndef HWDETINFOCTRLOBJ_H
#define HWDETINFOCTRLOBJ_H

#include "SizeUtils.h"
#include <string>

namespace lima
{

class HwDetInfoCtrlObj
{
 public:
	virtual ~HwDetInfoCtrlObj();

	virtual void getMaxImageSize(Size& max_image_size) = 0;
	virtual void getCurrImageSize(Size& curr_image_size) = 0;

	virtual void getDefImageType(ImageType& def_image_type) = 0;
	virtual void getCurrImageType(ImageType& curr_image_type) = 0;
	virtual void setCurrImageType(ImageType  curr_image_type) = 0;

	virtual void getPixelSize(double& pixel_size) = 0;
	virtual void getDetectorType(std::string& det_type) = 0;
	virtual void getDetectorModel(std::string& det_model) = 0;
};


} // namespace lima


#endif // HWDETINFOCTRLOBJ_H
