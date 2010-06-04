

namespace Maxipix {

%TypeHeaderCode

#include "HwMaxImageSizeCallback.h"
#include "MaxipixReconstruction.h"
#include "Constants.h"

using namespace lima;
%End

class MaxipixDet
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

    MaxipixDet();
    ~MaxipixDet();

    // -- version
    void setVersion(Version version);

    // -- geometry
    void setNbChip(int xchip, int ychip);
    void setPixelGap(int xgap, int ygap);

    // -- detector info
    void getImageSize(Size& size /Out/);
    void getPixelSize(double& size /Out/);
    void getImageType(ImageType& type /Out/);

    void getDetectorType(std::string& type /Out/);
    void getDetectorModel(std::string& model /Out/);

    // -- reconstruction
    bool needReconstruction();
    void getReconstruction(Maxipix::MaxipixReconstruction::Model& model /Out/);

};

};

