namespace Maxipix
{

  class MaxipixReconstruction : LinkTask
  {
%TypeHeaderCode
#include "MaxipixReconstruction.h"
using namespace lima;
%End
  public:
    enum Type {RAW,ZERO,DISPATCH,MEAN};
    enum Model {M_2x2,M_5x1};
    explicit MaxipixReconstruction(Model = M_5x1,
				   Type = RAW);
    MaxipixReconstruction(const Maxipix::MaxipixReconstruction&);
    ~MaxipixReconstruction();
    
    void setType(Type);
    void setModel(Model);
    void setXnYGapSpace(int xSpace,int ySpace);
    virtual Data process(Data &aData);
  };

}; // namespace Maxipix
