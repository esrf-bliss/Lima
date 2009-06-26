#ifndef CTCONTROL_H
#define CTCONTROL_H

#include "HwInterface.h"

#include "ThreadUtils.h"

namespace lima {

  class CtSaving;
  class CtControl {
  public:

    struct ImageStatus {
      ImageStatus();
      void reset();

      long	LastImageAcquired;
      long	LastBaseImageReady;
      long	LastImageReady;
      long	LastImageSaved;
      long	LastCounterReady;
    };

    CtControl(HwInterface *hw);
    ~CtControl();

    void prepareAcq();
    void startAcq();
    void stopAcq();

    void getAcqStatus(); // from HW
    void getImageStatus(ImageStatus& status) const;

    void reset();

  private:
    HwInterface		*m_hw;
    mutable Cond	m_cond;
    ImageStatus		m_img_status;
    CtSaving		*m_ct_saving;
  };

} // namespace lima

#endif // CTCONTROL_H
