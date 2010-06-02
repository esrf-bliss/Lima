

namespace Maxipix {

%TypeHeaderCode

#include "MaxipixReconstruction.h"
#include "Constants.h"

using namespace lima;
%End

class MaxipixDet
// : public HwMaxImageSizeCallbackGen
{

  public:

    enum Version {
      DUMMY,
      MPX2,
      MXR2,
      TPX1
    };

    enum Polarity {
      NEGATIVE,
      POSITIVE
    };

    static const int ChipSize;
    static const double PixelSize;
    static const int MaxChips;

    MaxipixDet(Maxipix::PriamAcq& priam_acq);
    ~MaxipixDet();

    // -- acquisition
    Maxipix::PriamAcq& priamAcq();
    void resetAllFifo();
    void resetAllChip();

    // -- geometry
    void setNbChip(int xchip, int ychip);
    void setPixelGap(int xgap, int ygap);
    void setPriamPort(int x, int y, int port);

    // -- detector info
    void getImageSize(Size& size);
    void getPixelSize(double& size);

    void getDetectorType(std::string& type);
    void getDetectorModel(std::string& model);

    // -- reconstruction
    bool needReconstruction();
    void getReconstruction(Maxipix::MaxipixReconstruction::Model& model);

    // -- config
    void setVersion(Version version);
    void getVersion(Version& version);
    void setPolarity(Polarity polarity);
    void getPolarity(Polarity& polarity);
    void setFrequency(float frequency);
    void getFrequency(float& frequency);
    void setFsr0(std::string fsr0);

    void setup();

};

};

