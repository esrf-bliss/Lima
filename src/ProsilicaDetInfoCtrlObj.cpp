#include <cstdlib>
#include "ProsilicaDetInfoCtrlObj.h"
#include "ProsilicaCamera.h"

using namespace lima;
using namespace lima::Prosilica;

DetInfoCtrlObj::DetInfoCtrlObj(Camera *cam):
  m_cam(cam),
  m_handle(cam->getHandle())
{
}

DetInfoCtrlObj::~DetInfoCtrlObj()
{
}

void DetInfoCtrlObj::getMaxImageSize(Size& max_image_size)
{
  tPvUint32 width,height;
  m_cam->getMaxWidthHeight(width,height);
  max_image_size = Size(int(width),int(height));
}

void DetInfoCtrlObj::getDetectorImageSize(Size& det_image_size)
{
  getMaxImageSize(det_image_size);
}

void DetInfoCtrlObj::getDefImageType(ImageType& def_image_type)
{
  def_image_type = Bpp16;
}

void DetInfoCtrlObj::getCurrImageType(ImageType& curr_image_type)
{
  char modeStr[64];
  tPvUint32 psize;
  tPvErr error = PvAttrEnumGet(m_handle,"PixelFormat",modeStr,
			       sizeof(modeStr),&psize);
  int pixelSize = (modeStr[0] == 'B') ? 
    atoi(&modeStr[5]) : atoi(&modeStr[4]);
  curr_image_type = (pixelSize == 16) ? Bpp16 : Bpp8;
}

void DetInfoCtrlObj::setCurrImageType(ImageType curr_image_type)
{
  VideoMode aMode = m_cam->getVideoMode();
  VideoMode aNextMode;
  switch(curr_image_type)
    {
    case Bpp16:
      if(aMode == Y8 || aMode == Y16)
	aNextMode = Y16;
      else
	aNextMode = BAYER_RG16;
      break;
    case Bpp8:
      if(aMode == Y8 || aMode == Y16)
	aNextMode = Y8;
      else
	aNextMode = BAYER_RG8;
      break;
    default:
      throw LIMA_HW_EXC(InvalidValue,"This image type is not Managed");
    }

  m_cam->setVideoMode(aNextMode);
}

void DetInfoCtrlObj::getPixelSize(double& pixel_size)
{  
  pixel_size = -1.;		// @todo don't know
}

void DetInfoCtrlObj::getDetectorType(std::string& det_type)
{
  det_type = "Prosilica";
}

void DetInfoCtrlObj::getDetectorModel(std::string& det_model)
{
  m_cam->getCameraName(det_model);
}

void DetInfoCtrlObj::registerMaxImageSizeCallback(HwMaxImageSizeCallback& cb)
{
  m_cam->registerMaxImageSizeCallback(cb);
}

void DetInfoCtrlObj::unregisterMaxImageSizeCallback(HwMaxImageSizeCallback& cb)
{
  m_cam->unregisterMaxImageSizeCallback(cb);
}

