#ifndef CTIMAGE_H
#define CTIMAGE_H

#include "Constants.h"

namespace lima {

class CtImage {

    public:

	// Size, Type, Bin, XY from common

	CtImage();
	~CtImage();

	void getMaxImageSize(Size& size) const;
	void getImageSize(Size& size) const;
	void getImageType(ImageType& type) const;

    private:

}

} // namespace lima

#endif // CTIMAGE_H
