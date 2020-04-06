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

#include "lima/CtSaving_ZBuffer.h"

#include <string.h> // For memcpy
#include <stdlib.h> // For posix_memalign
#include <malloc.h> // For _aligned_malloc

using namespace lima;

void ZBuffer::_alloc(int buffer_size)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(buffer_size);

  if (buffer_size == 0)
    THROW_CTL_ERROR(InvalidValue) << "Invalid NULL buffer_size";

  used_size = 0;
#ifdef __unix
  if(posix_memalign(&buffer,4*1024,buffer_size))
#else
  buffer = _aligned_malloc(buffer_size,4*1024);
  if(!buffer)
#endif
    THROW_CTL_ERROR(Error) << "Can't allocate buffer";
  alloc_size = buffer_size;
}

void ZBuffer::_deep_copy(const ZBuffer& o)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(o.alloc_size, o.used_size);
  if (o.used_size > o.alloc_size)
    THROW_CTL_ERROR(Error) << "Invalid " << DEB_VAR2(o.used_size, o.alloc_size);
  _alloc(o.alloc_size);
  memcpy(buffer, o.buffer, used_size);
  used_size = o.used_size;
}

void ZBuffer::_free()
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(alloc_size, used_size);
#ifdef __unix
  free(buffer);
#else
  _aligned_free(buffer);
#endif
  _setInvalid();
}

