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

#include "lima/LimaCompatibility.h"
#include "lima/SizeUtils.h"
#include "lima/Debug.h"

namespace lima
{

void LIMACORE_API GetSystemMem(int& mem_unit, int& system_mem);
	
void LIMACORE_API GetPageSize(int& page_size);

int LIMACORE_API GetDefMaxNbBuffers(const FrameDim& frame_dim);

void LIMACORE_API ClearBuffer(void *ptr, int nb_concat_frames, const FrameDim& frame_dim);


class LIMACORE_API MemBuffer 
{
 public:
	enum {
		Alignment = 16,
	};

	MemBuffer();
	MemBuffer(int size);
	~MemBuffer();

	MemBuffer(const MemBuffer&);
	MemBuffer& operator=(const MemBuffer&);

	// MemBuffer are move-constructible or move-assignable.
	MemBuffer(MemBuffer&&) = default;
	MemBuffer& operator=(MemBuffer&&) = default;

#ifdef LIMA_USE_NUMA
	void setCPUAffinityMask(unsigned long cpu_mask);
#endif

	void alloc(int size);
	void deepCopy(const MemBuffer& buffer);
	void release();

	int getSize() const;
	void *getPtr();
	const void *getConstPtr() const;

	void clear();

	operator void *();
	operator const void *() const;

 private:
	class Allocator
	{
	public:
		virtual void alloc(MemBuffer& buffer, int& size);
		virtual void init(MemBuffer& buffer);
		virtual void copy(MemBuffer& buffer, const MemBuffer& src);
		virtual void clear(MemBuffer& buffer);
		virtual void release(MemBuffer& buffer);

		static Allocator *getAllocator();

		static int getPageAlignedSize(int size);
#ifdef __unix
		static bool useMmap(int size);
		static void *allocMmap(int& size);
#endif
	};
	friend class Allocator;

	void init();
	void allocMemory(int& size);
	void initMemory();

	int m_size;
	void *m_ptr;
	Allocator *m_allocator;

#ifdef LIMA_USE_NUMA
	class NumaAllocator : public Allocator
	{
	public:
		virtual void alloc(MemBuffer& buffer, int& size);
		virtual void init(MemBuffer& buffer);
		virtual void copy(MemBuffer& buffer, const MemBuffer& src);
		virtual void clear(MemBuffer& buffer);
		virtual void release(MemBuffer& buffer);

		static NumaAllocator *getAllocator();

		void getNUMANodeMask(unsigned long cpu_mask,
				     unsigned long& node_mask,
				     int& max_node);
	};
	friend class NumaAllocator;

	unsigned long m_cpu_mask;
#endif
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
