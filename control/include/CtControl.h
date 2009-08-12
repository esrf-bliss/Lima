#ifndef CTCONTROL_H
#define CTCONTROL_H

#include "ThreadUtils.h"

#include "HwInterface.h"
#include "SoftOpInternalMgr.h"

#include "Data.h"

namespace lima {

  class CtDebug;
  class CtAcquisition;
  class CtImage;
  class CtBuffer;
  class CtBufferFrameCB;
  class CtSaving;

  class CtControl {
  friend class CtBufferFrameCB;
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
    CtImage* image() { return m_ct_image; }
    CtBuffer* buffer() { return m_ct_buffer; }


    void setDebug(short level);
    void getDebug(short& level) const;

    void setApplyPolicy(ApplyPolicy policy);
    void getApplyPolicy(ApplyPolicy &policy) const;

    void getAcqStatus(HwInterface::AcqStatus& status) const; // from HW
    void getImageStatus(ImageStatus& status) const;

    void reset();

  protected:
    void newFrameReady(Data& data);
    void newFrameToSave(Data& data);

  private:
    struct SoftOpStage {
	SoftOpStage()	{ reset(); }
	void reset();
	int getNb()	{ return internal+ext_link+ext_sink; };
	int getNbLink() { return internal+ext_link; };

	int	internal;
	int	ext_link;
	int	ext_sink;
    };

    HwInterface		*m_hw;
    mutable Cond	m_cond;
    ImageStatus		m_img_status;
    CtSaving		*m_ct_saving;
    CtAcquisition	*m_ct_acq;
    CtImage		*m_ct_image;
    CtBuffer		*m_ct_buffer;
    CtDebug		*m_ct_debug;
    SoftOpInternalMgr   *m_op_int;
    SoftOpStage		m_op_stage;

    ApplyPolicy		m_policy;
    bool		m_ready;
    bool		m_autosave;
  };

} // namespace lima

#endif // CTCONTROL_H
