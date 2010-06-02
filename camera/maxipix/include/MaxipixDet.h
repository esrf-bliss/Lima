#ifndef MAXIPIXDET_H
#define MAXIPIXDET_H

#include "MaxipixReconstruction.h"
#include "SizeUtils.h"
#include "Debug.h"

namespace lima {
namespace Maxipix {

class PriamAcq;

class MaxipixDet
// : public HwMaxImageSizeCallbackGen
{
  DEB_CLASS_NAMESPC(DebModCamera, "Camera", "MaxipixDet");

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

    static const int ChipSize= 256*256;
    static const double PixelSize= 55.0;
    static const int MaxChips= 5;

    MaxipixDet(PriamAcq& priam_acq);
    ~MaxipixDet();

    // -- acquisition
    PriamAcq& priamAcq();
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
    void getReconstruction(MaxipixReconstruction::Model& model);

    // -- config
    void setVersion(Version version);
    void getVersion(Version& version);
    void setPolarity(Polarity polarity);
    void getPolarity(Polarity& polarity);
    void setFrequency(float frequency);
    void getFrequency(float& frequency);
    void setFsr0(std::string fsr0);

    void setup();

  private:
    PriamAcq& m_priam;

    bool m_init;
    Version m_version;
    Polarity m_polarity;
    float m_frequency;
    std::string m_fsr0;

    int m_xchip, m_ychip, m_nchip;
    int m_xgap, m_ygap;
    int m_port[MaxChips];
    bool m_reconstruct;
};

} // namespace Maxipix
} // namespace lima

#endif // MAXIPIXDET_H

