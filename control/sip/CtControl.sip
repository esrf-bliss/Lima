class CtControl {
%TypeHeaderCode
#include <CtControl.h>
using namespace lima;
%End  
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
    ImageStatus getImageStatus() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipRes = new CtControl::ImageStatus();
sipCpp->getImageStatus(*sipRes);
Py_END_ALLOW_THREADS
%End


    void reset();
};
