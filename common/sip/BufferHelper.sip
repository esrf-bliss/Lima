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


class BufferHelper
{
%TypeHeaderCode
#include "lima/BufferHelper.h"
using namespace lima;
%End
public:

  struct Parameters 
  {
  public:
    enum DurationPolicy {
      Ephemeral,
      Persistent,
    };

    enum PersistentSizePolicy {
      Automatic,
      Fixed,
    };

    bool initMem;
    DurationPolicy durationPolicy;
    PersistentSizePolicy sizePolicy;
    int reqMemSizePercent;

    Parameters();

    int getDefMaxNbBuffers(int size) const;

    SIP_PYOBJECT __repr__() const;
%MethodCode
    LIMA_REPR_CODE
%End

    static BufferHelper::Parameters fromString(std::string s);
    std::string toString() const;
  };

  void setParameters(const BufferHelper::Parameters& params);
  void getParameters(BufferHelper::Parameters& params /Out/) const;

  void prepareBuffers(int nb_buffers, int size);
  void releaseBuffers();
};

