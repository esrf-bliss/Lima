#ifndef MAXIPIXRECONSTRUCTION_H
#define MAXIPIXRECONSTRUCTION_H

#include "LinkTask.h"

namespace lima
{

  namespace Maxipix
  {

    class MaxipixReconstruction : public LinkTask
    {
    public:
      enum Type {RAW,ZERO,DISPATCH,MEAN};
      enum Model {M_2x2,M_5x1};
      explicit MaxipixReconstruction(Model = M_5x1,Type = RAW);
      MaxipixReconstruction(const MaxipixReconstruction&);
      ~MaxipixReconstruction();
      
      void setType(Type);
      void setXnYGapSpace(int xSpace,int ySpace);
      virtual Data process(Data &aData);
    private:
      Type	mType;
      Model	mModel;
      int	mXSpace;
      int	mYSpace;
    };

  } // namespace Maxipix
} // namespace lima
#endif	// MAXIPIXRECONSTRUCTION_H
