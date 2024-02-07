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
	DEB_STRUCT_NAMESPC(DebModCommon, "Allocator", "MemUtils");

	struct Data {
		virtual ~Data() = default;
	};
	typedef std::shared_ptr<Data> DataPtr;
	typedef std::shared_ptr<Allocator> Ref;

	// Callback updating on default allocator change
	class DefaultChangeCallback
	{
	public:
		virtual ~DefaultChangeCallback() {};
		virtual void onDefaultAllocatorChange(Ref prev_alloc,
						      Ref new_alloc) = 0;
	};

	// Allocate a buffer of a given size and eventually return
	// the associated allocator data and potentially modified size
	virtual DataPtr alloc(void* &ptr, size_t& size, size_t alignment = 16);
	// Fill buffer with zeros (hot page)
	virtual void init(void* ptr, size_t size);
	// Free a buffer
	virtual void release(void* ptr, size_t size, DataPtr alloc_data);

	// Sets the static instance of the default allocator
	static void setDefaultAllocator(Ref def_alloc);
	// Returns the static instance of the default allocator
	static Ref getDefaultAllocator();

	static void registerDefaultChangeCallback(DefaultChangeCallback *cb);
	static void unregisterDefaultChangeCallback(DefaultChangeCallback *cb);

private:
	typedef std::vector<DefaultChangeCallback *> ChangeCbList;

	static Ref m_default_allocator;
	static ChangeCbList m_change_cb_list;
};

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

// Structure needed to bind memory to one or more CPU sockets
class NumaNodeMask
{
public:
	static constexpr int MaxNbCPUs = 128;
	typedef std::bitset<MaxNbCPUs> CPUMask;
	typedef std::vector<unsigned long> ItemArray;
	static constexpr int ItemBits = sizeof(ItemArray::value_type) * 8;

	NumaNodeMask();
	NumaNodeMask(const ItemArray& array);
	NumaNodeMask(const NumaNodeMask& o);
	NumaNodeMask(NumaNodeMask&& o);

	NumaNodeMask& operator =(const ItemArray& array);
	NumaNodeMask& operator =(const NumaNodeMask& o);
	NumaNodeMask& operator =(NumaNodeMask&& o);

	static int getMaxNodes();
	static int getNbItems();

	static NumaNodeMask fromCPUMask(const CPUMask& cpu_mask);

	void bind(void *ptr, size_t size);

	const ItemArray& getArray() const { return m_array; }

private:
	const ItemArray& checkArray(const ItemArray& array);

	ItemArray m_array;
};

std::ostream& operator <<(std::ostream& os, const NumaNodeMask& mask);

class LIMACORE_API NumaAllocator : public MMapAllocator
{
public:
	static constexpr int MaxNbCPUs = NumaNodeMask::MaxNbCPUs;	
	typedef NumaNodeMask::CPUMask CPUMask;

	NumaAllocator(const CPUMask& cpu_mask) : m_cpu_mask(cpu_mask) {}

	const CPUMask &getCPUAffinityMask()
	{ return m_cpu_mask; }

	// Allocate a buffer and sets the NUMA memory policy with mbind
	virtual DataPtr alloc(void* &ptr, size_t& size, size_t alignment = 16)
								override;

private:
	CPUMask m_cpu_mask; //<! if NUMA is used, keep cpu_mask for later use
};
#endif


class LIMACORE_API MemBuffer 
{
 public:
	//By default, construct a MemBuffer with the default allocator
	MemBuffer(Allocator::Ref allocator = {});
	MemBuffer(int size, Allocator::Ref allocator = {});
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
	Allocator::Ref getAllocator() const { return m_allocator; }

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
