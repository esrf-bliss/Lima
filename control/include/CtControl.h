#ifndef CTCONTROL_H
#define CTCONTROL_H

#include <set>

#include "ThreadUtils.h"

#include "HwInterface.h"

#include "Data.h"
#include "LinkTask.h"


namespace lima 
{

  class CtAcquisition;
  class CtImage;
  class CtBuffer;
  class CtBufferFrameCB;
  class CtSaving;
  class CtSpsImage;

  class SoftOpInternalMgr;
  class SoftOpExternalMgr;

  class CtControl {
    DEB_CLASS_NAMESPC(DebModControl,"Control","Control");

    friend class CtBufferFrameCB;
    friend class CtSaving;	// just to set saving error in stat
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

    enum ErrorCode {NoError,
		    SaveUnknownError,SaveAccessError,SaveOverwriteError,SaveDiskFull,SaveOverun,
		    ProcessingOverun,
		    CameraError};
    
    enum CameraErrorCode {NoCameraError}; /* @todo fix this */

    struct Status
    {
      DEB_CLASS_NAMESPC(DebModControl,"Control::Status","Control");
    public:
      Status();
      void reset();

      AcqStatus		AcquisitionStatus;
      ErrorCode		Error;
      CameraErrorCode	CameraStatus;
      ImageStatus	ImageCounters;
    };

    CtControl(HwInterface *hw);
    ~CtControl();

    void prepareAcq();
    void startAcq();
    void stopAcq();

    CtAcquisition* acquisition() { return m_ct_acq; }
    CtSaving* saving() { return m_ct_saving; }
    CtSpsImage* display() { return m_ct_sps_image; }
    CtImage* image() { return m_ct_image; }
    CtBuffer* buffer() { return m_ct_buffer; }

    SoftOpExternalMgr* externalOperation() {return m_op_ext;}

    HwInterface* interface() {return m_hw;}
    
    void setApplyPolicy(ApplyPolicy policy);
    void getApplyPolicy(ApplyPolicy &policy) const;

    void getStatus(Status& status) const; // from HW
    void getImageStatus(ImageStatus& status) const;

    void ReadImage(Data&,long frameNumber = -1);
    void ReadBaseImage(Data&,long frameNumber = -1);

    void reset();
    void resetStatus(bool only_acq_status);

    void registerImageStatusCallback(ImageStatusCallback& cb);
    void unregisterImageStatusCallback(ImageStatusCallback& cb);

    void setReconstructionTask(LinkTask*);

  protected:
    bool newFrameReady(Data& data);
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
    mutable Status      m_status;
    
    CtSaving		*m_ct_saving;
    CtSpsImage		*m_ct_sps_image;
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
    bool		m_display_active_flag;

    ImageStatusCallback *m_img_status_cb;

    inline bool _checkOverrun(Data&) const;
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

  
  inline std::ostream& operator<<(std::ostream &os,
				  const CtControl::ErrorCode &err_code)
  {
    const char *desc = "Unknown";
    switch (err_code)
    {
    case CtControl::NoError:           desc = "No error"; break;
    case CtControl::SaveUnknownError:  desc = "Saving  error"; break;
    case CtControl::SaveAccessError:   desc = "Save access error"; break;
    case CtControl::SaveOverwriteError: desc = "Save overwrite error"; break;
    case CtControl::SaveDiskFull:      desc = "Save disk full"; break;
    case CtControl::SaveOverun:        desc = "Save overrun"; break;
    case CtControl::ProcessingOverun:  desc = "Soft Processing overrun"; break;
      // should read CameraStatus instead @todo fix me
    case CtControl::CameraError:       desc = "Camera Error"; break;
    }
    return os << desc;
  }
    
  inline std::ostream& operator<<(std::ostream &os,
				  const CtControl::Status &status)
  {
    os << "<";
    os << "AcquisitionStatus=" << status.AcquisitionStatus;
    if(status.AcquisitionStatus == AcqFault)
	os << ", Error=" << status.Error;
    os << ", ImageCounters=" << status.ImageCounters;
    return os;
  }
} // namespace lima

#endif // CTCONTROL_H
