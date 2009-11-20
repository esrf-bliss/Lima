#ifndef CTCONTROL_H
#define CTCONTROL_H

#include <set>

#include "ThreadUtils.h"

#include "HwInterface.h"

#include "Data.h"


namespace lima 
{

  class CtAcquisition;
  class CtImage;
  class CtBuffer;
  class CtBufferFrameCB;
  class CtSaving;

  class SoftOpInternalMgr;
  class SoftOpExternalMgr;

  class CtControl {
    DEB_CLASS_NAMESPC(DebModControl,"Control","Control");

    friend class CtBufferFrameCB;
  public:

    enum ApplyPolicy {
      All,
      Changes,
      HwSyncChanges,
    };


    struct ImageStatus {
      DEB_CLASS_NAMESPC(DebModControl,"Control::ImageStatus","Control");
    public:
      ImageStatus();
      void reset();

      long	LastImageAcquired;
      long	LastBaseImageReady;
      long	LastImageReady;
      long	LastImageSaved;
      long	LastCounterReady;
    };


    class ImageStatusCallback 
    {
      DEB_CLASS_NAMESPC(DebModControl,"Control::ImageStatusCallback", 
			"Control");
    public:
      ImageStatusCallback();
      virtual ~ImageStatusCallback();
    protected:
      virtual void imageStatusChanged(const ImageStatus& img_status) = 0;
    private:
      friend class CtControl;
      void setImageStatusCallbackGen(CtControl *cb_gen);
      CtControl *m_cb_gen;
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

    void setApplyPolicy(ApplyPolicy policy);
    void getApplyPolicy(ApplyPolicy &policy) const;

    void getAcqStatus(AcqStatus& status) const; // from HW
    void getImageStatus(ImageStatus& status) const;

    void ReadImage(Data&,long frameNumber = -1);
    void ReadBaseImage(Data&,long frameNumber = -1);

    void reset();

    void registerImageStatusCallback(ImageStatusCallback& cb);
    void unregisterImageStatusCallback(ImageStatusCallback& cb);

  protected:
    void newFrameReady(Data& data);
    void newFrameToSave(Data& data);
    void newBaseImageReady(Data &data);
    void newImageReady(Data &data);
    void newCounterReady(Data &data);
    void newImageSaved(Data &data);

  private:
    struct ltData
    {
      inline bool operator()(const Data& d1, const Data& d2) const
      {
	return d1.frameNumber < d2.frameNumber;
      }
    };
    class _LastBaseImageReadyCallback;
    friend class _LastBaseImageReadyCallback;
    class _LastImageReadyCallback;
    friend class _LastImageReadyCallback;
    class _LastImageSavedCallback;
    friend class _LastImageSavedCallback;
    class _LastCounterReadyCallback;
    friend class _LastCounterReadyCallback;

    HwInterface		*m_hw;
    mutable Cond	m_cond;
    ImageStatus		m_img_status;

    CtSaving		*m_ct_saving;
    CtAcquisition	*m_ct_acq;
    CtImage		*m_ct_image;
    CtBuffer		*m_ct_buffer;
    SoftOpInternalMgr   *m_op_int;
    SoftOpExternalMgr	*m_op_ext;

    bool		m_op_int_active;
    bool		m_op_ext_link_task_active;
    bool		m_op_ext_sink_task_active;

    std::set<Data,ltData> m_base_images_ready;
    std::set<Data,ltData> m_images_ready;

    ApplyPolicy		m_policy;
    bool		m_ready;
    bool		m_autosave;

    ImageStatusCallback *m_img_status_cb;
  };

  inline std::ostream& operator<<(std::ostream &os,
				  const CtControl::ImageStatus &status)
  {
    os << "<"
       << "LastImageAcquired=" << status.LastImageAcquired << ", "
       << "LastBaseImageReady=" << status.LastBaseImageReady << ", "
       << "LastImageReady=" << status.LastImageReady << ", "
       << "LastImageSaved=" << status.LastImageSaved << ", "
       << "LastCounterReady=" << status.LastCounterReady
       << ">";
    return os;
  }
} // namespace lima

#endif // CTCONTROL_H
