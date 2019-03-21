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

#include <unordered_map>

#include "lima/LimaCompatibility.h"
#include "lima/SizeUtils.h"
#include "lima/Debug.h"

namespace lima
{

void LIMACORE_API GetSystemMem(int& mem_unit, int& system_mem);
	
void LIMACORE_API GetPageSize(int& page_size);

int LIMACORE_API GetDefMaxNbBuffers(const FrameDim& frame_dim);

void LIMACORE_API ClearBuffer(void *ptr, int nb_concat_frames, const FrameDim& frame_dim);


struct LIMACORE_API Allocator
{
	// Allocate a buffer of a given size 
	virtual void alloc(void* &ptr, size_t size, size_t alignement = 16);
	// Fill buffer with zeros (hot page)
	virtual void init(void* ptr, size_t size);
	// Free a buffer
	virtual void release(void* ptr);

	// Returns a static instance of the default allocator
	static Allocator *getAllocator();

	// Returns the size of a page aligned buffer (multiple of page size)
	static int getPageAlignedSize(int size);
};


#ifdef __unix
// Allocator for virtual address mapping
class LIMACORE_API MMapAllocator : public Allocator
{
public:
	// Allocate a buffer of a given size 
	virtual void alloc(void* &ptr, size_t size, size_t alignement = 16) override;

	// Free a buffer
	virtual void release(void* ptr) override;

protected:
	// Returns true if mmap is available
	bool useMmap(size_t size);

	// Allocate a buffer with mmap (virtual address mapping)
	void *allocMmap(size_t size);

	std::unordered_map<void*, size_t> m_memory_maps;
};
#endif //__unix

#ifdef LIMA_USE_NUMA
class LIMACORE_API NumaAllocator : public MMapAllocator
{
public:
	NumaAllocator(unsigned long cpu_mask) : m_cpu_mask(cpu_mask) {}

	// Allocate a buffer and sets the NUMA memory policy with mbind
	virtual void alloc(void* &ptr, size_t size, size_t alignement = 16) override;

private:
	// Given a cpu_mask, returns the memory node mask
	// used by alloc to bind memory with the proper socket
	void getNUMANodeMask(unsigned long cpu_mask,
				    unsigned long& node_mask,
				    int& max_node);

	unsigned long m_cpu_mask; //<! if NUMA is used, keep the cpu_mask for later use
};
#endif


class LIMACORE_API MemBuffer 
{
 public:
	//By default, construct a MemBuffer with the default constructor
	MemBuffer(Allocator *allocator = Allocator::getAllocator());
	MemBuffer(int size, Allocator *allocator = Allocator::getAllocator());
	~MemBuffer();

	// MemBuffer are copy constructible (deep copy, no aliasing)
	MemBuffer(const MemBuffer&);
	MemBuffer& operator=(const MemBuffer&);

	// MemBuffer are move-constructible or move-assignable.
	MemBuffer(MemBuffer&&);
	MemBuffer& operator=(MemBuffer&&) = default;

	/// Allocate and initialized memory
	void alloc(size_t size);
	void deepCopy(const MemBuffer& buffer);
	void release();

	size_t getSize() const { return m_size; }
	void *getPtr() { return m_ptr; }
	const void *getConstPtr() const { return m_ptr; }

	void clear();

	operator void *() { return getPtr(); }
	operator const void *() const { return getConstPtr(); }

	/// Returns the allocator currently associated with MemBuffer
	Allocator *getAllocator() const { return m_allocator; }

 private:
	/// Call the allocator to (eventually) free the current buffer then allocate a new buffer
	void uninitializedAlloc(size_t size);

	/// Initialize the memory
	void initMemory();

	size_t m_size;	//!< The size of the buffer in bytes
	void *m_ptr;	//!< The pointer ot the buffer
	Allocator *m_allocator;	//!< The allocator used to alloc and free the buffer
};

} // namespace lima


#endif // MEMUTILS_H
