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

    CtControl(HwInterface *hw);
    ~CtControl();

    void prepareAcq();
    void startAcq();
    void stopAcq();

    CtAcquisition* acquisition();
    CtSaving* saving();
    CtImage* image();
    CtBuffer* buffer();

    void setDebug(short level);
    void getDebug(short& level /Out/) const;

    void setApplyPolicy(ApplyPolicy policy);
    void getApplyPolicy(ApplyPolicy &policy /Out/) const;

    void getAcqStatus(HwInterface::AcqStatus& status /Out/) const; // from HW
    void getImageStatus(ImageStatus &imageStatus /Out/) const;

    void reset();
  protected:
    void newFrameReady(Data& data);
    void newFrameToSave(Data& data);
    void newBaseImageReady(Data &data);
    void newImageReady(Data &data);
    void newCounterReady(Data &data);
    void newImageSaved(Data &data);
};
