class CtControl {
%TypeHeaderCode
#include <CtControl.h>
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
    };

    class ImageStatusCallback 
    {
    public:
      ImageStatusCallback();
      virtual ~ImageStatusCallback();
    protected:
      virtual void imageStatusChanged(const CtControl::ImageStatus& status)=0;
    };

    CtControl(HwInterface *hw);
    ~CtControl();

    void prepareAcq();
    void startAcq();
    void stopAcq();

    CtAcquisition* acquisition();
    CtSaving* saving();
    CtImage* image();
    CtBuffer* buffer();

    void setApplyPolicy(ApplyPolicy policy);
    void getApplyPolicy(ApplyPolicy &policy /Out/) const;

    void getAcqStatus(AcqStatus& status /Out/) const; // from HW
    void getImageStatus(ImageStatus &imageStatus /Out/) const;

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
};
