#ifndef PROSILICADETINFOCTRLOBJ_H
#define PROSILICADETINFOCTRLOBJ_H

#include "Prosilica.h"
#include "HwDetInfoCtrlObj.h"
#include "Debug.h"

namespace lima
{
  namespace Prosilica
  {
    class Camera;
    class DetInfoCtrlObj : public HwDetInfoCtrlObj
    {
      DEB_CLASS_NAMESPC(DebModCamera, "DetInfoCtrlObj","Prosilica");

    public:
      DetInfoCtrlObj(Camera*);
      virtual ~DetInfoCtrlObj();

      virtual void getMaxImageSize(Size& max_image_size);
      virtual void getDetectorImageSize(Size& det_image_size);

      virtual void getDefImageType(ImageType& def_image_type);
      virtual void getCurrImageType(ImageType& curr_image_type);
      virtual void setCurrImageType(ImageType  curr_image_type);

      virtual void getPixelSize(double& pixel_size);
      virtual void getDetectorType(std::string& det_type);
      virtual void getDetectorModel(std::string& det_model);

      virtual void registerMaxImageSizeCallback(HwMaxImageSizeCallback& cb);
      virtual void unregisterMaxImageSizeCallback(HwMaxImageSizeCallback& cb);
    private:
      Camera* 			m_cam;
      tPvHandle& 		m_handle;
    };

  } // namespace Prosilica
} // namespace lima


#endif // PROSILICADETINFOCTRLOBJ_H
