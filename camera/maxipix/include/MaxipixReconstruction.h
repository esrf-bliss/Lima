//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2011
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################
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
      void setModel(Model);
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
