namespace Prosilica
{
  class Interface : HwInterface
  {
%TypeHeaderCode
#include <ProsilicaInterface.h>
%End
  public:
    Interface(Prosilica::Camera* /KeepReference/);
    virtual ~Interface();

    virtual void getCapList(std::vector<HwCap> &cap_list /Out/) const;
    virtual void reset(ResetLevel reset_level);
    virtual void prepareAcq();
    virtual void startAcq();
    virtual void stopAcq();
    virtual void getStatus(StatusType& status);

    virtual int getNbAcquiredFrames();
    virtual int getNbHwAcquiredFrames();
  };

};
