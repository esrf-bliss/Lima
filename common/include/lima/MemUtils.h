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
#include "lima/Exceptions.h"

namespace lima
{

void LIMACORE_API GetSystemMem(int& mem_unit, int& system_mem);
	
void LIMACORE_API GetPageSize(int& page_size);

int LIMACORE_API GetDefMaxNbBuffers(const FrameDim& frame_dim);

void LIMACORE_API ClearBuffer(void *ptr, int nb_concat_frames, const FrameDim& frame_dim);


struct LIMACORE_API Allocator
{
	DEB_STRUCT_NAMESPC(DebModCommon, "MemUtils", "Allocator");

	struct Data {
		virtual ~Data() = default;
	};
	typedef AutoPtr<Data> DataPtr;

	Allocator() : m_ref_count(0)
	{}

	Allocator(const Allocator& /*o*/) : m_ref_count(0)
	{}

	Allocator(Allocator&& o) : m_ref_count(0)
	{
		DEB_MEMBER_FUNCT();

		if (o.m_ref_count != 0)
			DEB_ERROR() << "Moved-from Allocator is not empty";
	}

	// Allocate a buffer of a given size and eventually return
	// the associated allocator data and potentially modified size
	virtual DataPtr alloc(void* &ptr, size_t& size, size_t alignment = 16);
	// Fill buffer with zeros (hot page)
	virtual void init(void* ptr, size_t size);
	// Free a buffer
	virtual void release(void* ptr, size_t size, DataPtr alloc_data);

	// Returns a static instance of the default allocator
	static Allocator *defaultAllocator();

	// All references to Allocators should be kept with this class
	class Ref
	{
	public:
		Ref(Allocator *alloc) : m_alloc(alloc->get())
		{}
		Ref(const Ref& o) : m_alloc(o.m_alloc->get())
		{}
		~Ref()
		{ m_alloc->put(); }

		Ref& operator =(const Ref& o)
		{
			if (m_alloc != o.m_alloc) {
				m_alloc->put();
				m_alloc = o.m_alloc->get();
			}
			return *this;
		}
		operator Allocator *() const
		{ return m_alloc; }
		Allocator *operator ->() const
		{ return m_alloc; }
	private:
		Allocator *m_alloc;
	};

 protected:
	friend class Ref;

	virtual ~Allocator()
	{
		DEB_MEMBER_FUNCT();

		if (m_ref_count != 0)
			DEB_ERROR() << "Error: destroying non-empty Allocator";
	}

	// The real resource management counter, triggered by Ref
	Allocator *get()
	{ return ++m_ref_count, this; }
	void put()
	{ if (--m_ref_count == 0) delete this; }

	// Keep track of allocated buffers pointing to this Allocator:
	// if greather than 0 this object cannot be moved
	unsigned m_ref_count;
};

inline bool operator ==(const Allocator::Ref& a, const Allocator::Ref& b)
{ return (Allocator *) a == (Allocator *) b; }

#ifdef __unix
// Allocator for virtual address mapping
class LIMACORE_API MMapAllocator : public Allocator
{
public:
	// Allocate a buffer of a given size 
	virtual DataPtr alloc(void* &ptr, size_t& size, size_t alignment = 16)
								override;

	// Free a buffer
	virtual void release(void* ptr, size_t size, DataPtr alloc_data)
								override;

	// Returns the size of a page aligned buffer (multiple of page size)
	static int getPageAlignedSize(int size);

protected:
	// Allocate a buffer with mmap (virtual address mapping)
	// The real, page-aligned buffer size, is returned in size arg
	void *allocMmap(size_t& size);
};
#endif //__unix

#ifdef LIMA_USE_NUMA
class LIMACORE_API NumaAllocator : public MMapAllocator
{
public:
	NumaAllocator(unsigned long cpu_mask) : m_cpu_mask(cpu_mask) {}

	unsigned long getCPUAffinityMask()
	{ return m_cpu_mask; }

	// Allocate a buffer and sets the NUMA memory policy with mbind
	virtual DataPtr alloc(void* &ptr, size_t& size, size_t alignment = 16)
								override;

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
	MemBuffer(Allocator *allocator = Allocator::defaultAllocator());
	MemBuffer(int size, Allocator *allocator =
		  				Allocator::defaultAllocator());
	~MemBuffer();

	// MemBuffer are copy constructible (deep copy, no aliasing)
	MemBuffer(const MemBuffer& buffer);
	MemBuffer& operator =(const MemBuffer& buffer);

	// MemBuffer are move-constructible or move-assignable.
	MemBuffer(MemBuffer&& rhs);
	MemBuffer& operator =(MemBuffer&& rhs);

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
	Allocator::Ref m_allocator;	//!< The allocator used to alloc and free the buffer
	Allocator::DataPtr m_alloc_data;
};

} // namespace lima


#endif // MEMUTILS_H
