//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2017
// European Synchrotron Radiation Facility
// CS40220 38043 Grenoble Cedex 9 
// FRANCE
//
// Contact: lima@esrf.fr
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

#ifndef CTSAVING_COMPRESSION_BUFFER_H
#define CTSAVING_COMPRESSION_BUFFER_H

#include "lima/Debug.h"

namespace lima {

struct ZBufferHelper
{
  DEB_CLASS_NAMESPC(DebModControl,"ZBufferHelper","Control");

public:
  ZBufferHelper(int buffer_size);
  ZBufferHelper(const ZBufferHelper& o) = delete;
  ZBufferHelper(ZBufferHelper&& o);
  ~ZBufferHelper();

  ZBufferHelper& operator =(const ZBufferHelper& o) = delete;
  ZBufferHelper& operator =(ZBufferHelper&& o);

  int used_size;
  void *buffer;

private:
  void _alloc(int buffer_size);
  void _free();
};

inline ZBufferHelper::ZBufferHelper(int buffer_size)
{
  DEB_CONSTRUCTOR();
  _alloc(buffer_size);
}

inline ZBufferHelper::ZBufferHelper(ZBufferHelper&& o)
  : used_size(std::move(o.used_size)), buffer(std::move(o.buffer))
{
  DEB_CONSTRUCTOR();
  o.buffer = NULL;
}

inline ZBufferHelper& ZBufferHelper::operator =(ZBufferHelper&& o)
{
  if (buffer)
    _free();
  used_size = std::move(o.used_size);
  buffer = std::move(o.buffer);
  o.buffer = NULL;
  return *this;
}

inline ZBufferHelper::~ZBufferHelper()
{
  if (buffer)
    _free();
}

typedef std::vector<ZBufferHelper> ZBufferType;
typedef std::map<int,ZBufferType> dataId2ZBufferType;

};  //namespace lima


#endif // CTSAVING_COMPRESSION_BUFFER_H

