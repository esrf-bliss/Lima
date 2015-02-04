namespace Meta
{
  class Interface : HwInterface
  {
%TypeHeaderCode
#include <MetaInterface.h>
%End
  public:
    enum Geometry {TILE};
    Interface(Geometry = TILE);
    virtual ~Interface();

    void addInterface(int row,int column,HwInterface*);

    //- From HwInterface
    //    virtual void 	getCapList(CapList& /Out/) const;
    virtual void	getCapList(std::vector<HwCap> &cap_list /Out/) const;
    virtual void	reset(ResetLevel reset_level);
    virtual void 	prepareAcq();
    virtual void 	startAcq();
    virtual void 	stopAcq();
    virtual void 	getStatus(StatusType& status /Out/);
    virtual int 	getNbHwAcquiredFrames();

  };
};
