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

#ifndef CTSAVING_ZBUFFER_H
#define CTSAVING_ZBUFFER_H

#include "lima/Debug.h"

#include <vector>
#include <map>

namespace lima {

struct ZBuffer
{
  DEB_CLASS_NAMESPC(DebModControl,"ZBuffer","Control");

public:
  ZBuffer(int buffer_size);
  ZBuffer(const ZBuffer& o);
  ZBuffer(ZBuffer&& o);
  ~ZBuffer();

  ZBuffer& operator =(const ZBuffer& o);
  ZBuffer& operator =(ZBuffer&& o);

  int alloc_size;
  int used_size;
  void *buffer;

private:
  void _alloc(int buffer_size);
  void _deep_copy(const ZBuffer& o);
  void _free();
};

inline ZBuffer::ZBuffer(int buffer_size)
{
  DEB_CONSTRUCTOR();
  _alloc(buffer_size);
}

inline ZBuffer::ZBuffer(const ZBuffer& o)
  : buffer(NULL)
{
  DEB_CONSTRUCTOR();
  if (o.buffer)
    _deep_copy(o);
}

inline ZBuffer::ZBuffer(ZBuffer&& o)
  : alloc_size(std::move(o.alloc_size)),
    used_size(std::move(o.used_size)), buffer(std::move(o.buffer))
{
  DEB_CONSTRUCTOR();
  o.buffer = NULL;
}

inline ZBuffer& ZBuffer::operator =(const ZBuffer& o)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(o.alloc_size, o.used_size);
  if (buffer)
    _free();
  if (o.buffer)
    _deep_copy(o);
  return *this;
}

inline ZBuffer& ZBuffer::operator =(ZBuffer&& o)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(o.alloc_size, o.used_size);
  if (buffer)
    _free();
  used_size = std::move(o.used_size);
  buffer = std::move(o.buffer);
  o.buffer = NULL;
  return *this;
}

inline ZBuffer::~ZBuffer()
{
  DEB_DESTRUCTOR();
  if (buffer)
    _free();
}

typedef std::vector<ZBuffer> ZBufferList;
typedef std::map<int,ZBufferList> dataId2ZBufferList;

};  //namespace lima


#endif // CTSAVING_ZBUFFER_H

