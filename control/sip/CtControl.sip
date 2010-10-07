class CtControl {
%TypeHeaderCode
#include <CtControl.h>
#include <LinkTask.h>
using namespace lima;
%End  
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

      const char* __repr__();
%MethodCode
	std::ostringstream str;
	str << *sipCpp;	
	sipRes = str.str().c_str();
%End
    };

    class ImageStatusCallback 
    {
    public:
      ImageStatusCallback();
      virtual ~ImageStatusCallback();
    protected:
      virtual void imageStatusChanged(const CtControl::ImageStatus& status)=0;
    };

    enum ErrorCode {NoError,
		    SaveAccessError,SaveDiskFull,SaveOverun,
		    ProcessingOverun,
		    CameraError};
    
    enum CameraErrorCode {NoCameraError}; /* @todo fix this */

    struct Status
    {
      Status();
      void reset();

      AcqStatus		         AcquisitionStatus;
      CtControl::ErrorCode       Error;
      CtControl::CameraErrorCode CameraStatus;
      CtControl::ImageStatus	 ImageCounters;

      const char* __repr__();
%MethodCode
	std::ostringstream str;
	str << *sipCpp;	
	sipRes = str.str().c_str();
%End
    };

    CtControl(HwInterface *hw);
    ~CtControl();

    void prepareAcq();
    void startAcq();
    void stopAcq();

    CtAcquisition* acquisition();
    CtSaving* saving();
    CtSpsImage* display();
    CtImage* image();
    CtBuffer* buffer();
    CtAccumulation* accumulation();

    SoftOpExternalMgr* externalOperation();
    
    HwInterface* interface();

    void setApplyPolicy(ApplyPolicy policy);
    void getApplyPolicy(ApplyPolicy &policy /Out/) const;

    void getStatus(Status& status /Out/) const;
    void getImageStatus(ImageStatus &imageStatus /Out/) const;

    void ReadImage(Data& data /Out/,long frameNumber = -1);
    void ReadBaseImage(Data& data /Out/,long frameNumber = -1);

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
};
