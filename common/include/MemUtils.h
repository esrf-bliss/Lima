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
#ifndef MEMUTILS_H
#define MEMUTILS_H

#include "SizeUtils.h"
#include "Debug.h"

namespace lima
{

void GetSystemMem(int& mem_unit, int& system_mem);
	
void GetPageSize(int& page_size);

int GetDefMaxNbBuffers(const FrameDim& frame_dim, double sys_mem_factor = 0);

void ClearBuffer(void *ptr, int nb_concat_frames, const FrameDim& frame_dim);


class MemBuffer 
{
 public:
	enum {
		Alignment = 16,
	};

	MemBuffer();
	MemBuffer(int size);
	MemBuffer(const MemBuffer& buffer);
	~MemBuffer();

	void alloc(int size);
	void copy(const MemBuffer& buffer);
	void release();

	int getSize() const;
	void *getPtr();
	const void *getConstPtr() const;

	void clear();

	operator void*();
	operator const void*() const;

	MemBuffer& operator =(const MemBuffer& buffer);

 private:
	int m_size;
	void *m_ptr;
};

inline int MemBuffer::getSize() const
{
	return m_size;
}

inline void *MemBuffer::getPtr()
{
	return m_ptr;
}

inline const void *MemBuffer::getConstPtr() const
{
	return m_ptr;
}

inline MemBuffer::operator void *()
{
	return getPtr();
}

inline MemBuffer::operator const void *() const
{
	return getConstPtr();
}



} // namespace lima


#endif // MEMUTILS_H
