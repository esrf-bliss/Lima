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
#include "lima/Exceptions.h"

#include <vector>
#include <map>
#include <algorithm>

namespace lima {

class ZBuffer
{
  DEB_CLASS_NAMESPC(DebModControl,"ZBuffer","Control");

public:
  ZBuffer(int buffer_size);
  ZBuffer(const ZBuffer& o);
  ZBuffer(ZBuffer&& o);
  ~ZBuffer();

  ZBuffer& operator =(const ZBuffer& o);
  ZBuffer& operator =(ZBuffer&& o);

  void swap(ZBuffer& o);

  int used_size;

  void *ptr()
  { return buffer; }

private:
  bool _isValid() const;
  void _setInvalid();
  void _alloc(int buffer_size);
  void _deep_copy(const ZBuffer& o);
  void _free();

  int alloc_size;
  void *buffer;
};

inline bool ZBuffer::_isValid() const
{
  return buffer;
}

inline void ZBuffer::_setInvalid()
{
  used_size = 0;
  alloc_size = 0;
  buffer = NULL;
}

inline ZBuffer::ZBuffer(int buffer_size)
{
  DEB_CONSTRUCTOR();
  _alloc(buffer_size);
}

inline ZBuffer::ZBuffer(const ZBuffer& o)
{
  DEB_CONSTRUCTOR();
  if (o._isValid())
    _deep_copy(o);
  else
    _setInvalid();
}

inline ZBuffer::ZBuffer(ZBuffer&& o)
{
  DEB_CONSTRUCTOR();
  _setInvalid();
  swap(o);
}

inline ZBuffer& ZBuffer::operator =(const ZBuffer& o)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(o.alloc_size, o.used_size);
  if (std::addressof(o) == this) {
    DEB_TRACE() << "Copying this into itself";
    return *this;
  }
  if (_isValid())
    _free();
  if (o._isValid())
    _deep_copy(o);
  return *this;
}

inline ZBuffer& ZBuffer::operator =(ZBuffer&& o)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(o.alloc_size, o.used_size);
  if (std::addressof(o) == this)
    THROW_CTL_ERROR(InvalidValue) << "Trying to move this to itself";
  if (_isValid())
    _free();
  swap(o);
  return *this;
}

inline void ZBuffer::swap(ZBuffer& o)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR4(alloc_size, used_size, o.alloc_size, o.used_size);
  if (std::addressof(o) == this)
    return;
  std::swap(used_size, o.used_size);
  std::swap(alloc_size, o.alloc_size);
  std::swap(buffer, o.buffer);
}

inline ZBuffer::~ZBuffer()
{
  DEB_DESTRUCTOR();
  if (_isValid())
    _free();
}

typedef std::vector<ZBuffer> ZBufferList;
typedef std::map<int,ZBufferList> dataId2ZBufferList;

};  //namespace lima


#endif // CTSAVING_ZBUFFER_H

