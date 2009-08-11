#ifndef CTCONTROL_H
#define CTCONTROL_H

#include "HwInterface.h"

#include "ThreadUtils.h"

namespace lima {

  class CtSaving;
  class CtAcquisition;
  class CtBuffer;
  class CtDebug;

  class CtControl {
  public:

    enum ApplyPolicy {
      All,
      Changes,
      HwSyncChanges,
    };


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

    CtAcquisition* acquisition() { return m_ct_acq; }
    CtSaving* saving() { return m_ct_saving; }
    CtBuffer* buffer() { return m_ct_buffer; }

    void setDebug(short level);
    void getDebug(short& level) const;

    void setApplyPolicy(ApplyPolicy policy);
    void getApplyPolicy(ApplyPolicy &policy);

    void getAcqStatus(HwInterface::AcqStatus& status) const; // from HW
    void getImageStatus(ImageStatus& status) const;

    void reset();

  private:
    HwInterface		*m_hw;
    mutable Cond	m_cond;
    ImageStatus		m_img_status;
    CtSaving		*m_ct_saving;
    CtAcquisition	*m_ct_acq;
    CtBuffer		*m_ct_buffer;
    CtDebug		*m_ct_debug;
    ApplyPolicy		m_policy;
  };

} // namespace lima

#endif // CTCONTROL_H
