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
#include "lima/MemUtils.h"

#include <cstdlib>
#include <sstream>
#ifdef __unix
#include <sys/sysinfo.h>
#ifdef LIMA_USE_NUMA
#include <numa.h>
#include <numaif.h>
#endif
#include <sys/mman.h>
#ifdef __SSE2__
#include <emmintrin.h>
#endif
#else
#include <windows.h>
#endif
#include <limits.h>
#include <unistd.h>

using namespace lima;
using namespace std;

void lima::GetSystemMem(int& mem_unit, int& system_mem)
{
	if (mem_unit < 0)
		throw LIMA_COM_EXC(InvalidValue, "Invalid mem_unit value");
#ifdef __unix
        struct sysinfo s_info;
	if (sysinfo(&s_info) < 0)
		throw LIMA_COM_EXC(Error, "Error calling sysinfo");

        long long tot_mem = s_info.totalram;
	tot_mem *= s_info.mem_unit;
	if (mem_unit == 0) 
		mem_unit = s_info.mem_unit;
#else  // Windoze
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	
	GlobalMemoryStatusEx(&statex);
	long long tot_mem = (long long) statex.ullAvailPhys;
	if (mem_unit == 0) 
		mem_unit = 1;
#endif

	const bool platform_32 = (sizeof(void *) == 4);
	const long long two_gigas = 2LL * 1024 * 1024 * 1024;
	if ((platform_32) && (tot_mem > two_gigas))
		tot_mem = two_gigas;


	long long huge_blocks = tot_mem / mem_unit;
	if (huge_blocks > INT_MAX)
		huge_blocks = INT_MAX;

	system_mem = int(huge_blocks);
}

void lima::GetPageSize(int& page_size)
{
#ifdef __unix
	page_size = getpagesize();
#else
	SYSTEM_INFO system_info;
        GetSystemInfo (&system_info);
        page_size = system_info.dwPageSize;
#endif
}

int lima::GetDefMaxNbBuffers(const FrameDim& frame_dim)
{
	int frame_size = frame_dim.getMemSize();
	if (frame_size <= 0)
		throw LIMA_COM_EXC(InvalidValue, "Invalid FrameDim");

	int tot_buffers;
	GetSystemMem(frame_size, tot_buffers);
	return int(tot_buffers);
}
	
void lima::ClearBuffer(void *ptr, int nb_concat_frames, 
		       const FrameDim& frame_dim)
{
	memset(ptr, 0, nb_concat_frames * size_t(frame_dim.getMemSize()));
}

Allocator *Allocator::defaultAllocator()
{
	static Allocator::Ref allocator = new Allocator();
	return allocator;
}

Allocator::DataPtr Allocator::alloc(void* &ptr, size_t& size, size_t alignment)
{
#ifdef __unix
	int ret = posix_memalign(&ptr, alignment, size);
	if (ret != 0)
		throw LIMA_COM_EXC(Error, "Error in posix_memalign: ")
			<< strerror(ret);
#else
	ptr = _aligned_malloc(size, alignment);
	if (!ptr)
		throw LIMA_COM_EXC(Error, "Error in _aligned_malloc: ")
			<< "NULL pointer return";
#endif
	return DataPtr();
}

void Allocator::init(void* ptr, size_t size)
{
	// memset implementation is already vectorized
	memset(ptr, 0, size);
}

void Allocator::release(void* ptr, size_t /*size*/, DataPtr /*alloc_data*/)
{
#ifdef __unix
	free(ptr);
#else
	_aligned_free(ptr);
#endif
}

#ifdef __unix

int MMapAllocator::getPageAlignedSize(int size)
{
	int page_size;
	GetPageSize(page_size);
	int misaligned = size & (page_size - 1);
	if (misaligned)
		size += page_size - misaligned;
	return size;
}

// Allocate a buffer of a given size 
Allocator::DataPtr MMapAllocator::alloc(void* &ptr, size_t& size,
					size_t /*alignment = 16*/)
{
	ptr = allocMmap(size);
	return DataPtr();
}

// Free a buffer
void MMapAllocator::release(void* ptr, size_t size, DataPtr /*alloc_data*/)
{
	size = getPageAlignedSize(size);
	munmap(ptr, size);
}

void *MMapAllocator::allocMmap(size_t& size)
{
	size = getPageAlignedSize(size);
	void *ptr = (char *) mmap(0, size, PROT_READ | PROT_WRITE,
				  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (!ptr)
		throw LIMA_COM_EXC(Error, "Error in mmap: ")
			<< strerror(errno);
	return ptr;
}
#endif //__unix

MemBuffer::MemBuffer(Allocator *allocator /*= Allocator::defaultAllocator()*/) :
	m_size(0),
	m_ptr(nullptr),
	m_allocator(allocator)
{
}

MemBuffer::MemBuffer(int size, Allocator *allocator /*=
					Allocator::defaultAllocator()*/):
	m_size(0),
	m_ptr(nullptr),
	m_allocator(allocator)
{
	alloc(size);
}

MemBuffer::MemBuffer(const MemBuffer& buffer) :
	m_size(0),
	m_ptr(nullptr),
	m_allocator(buffer.m_allocator)
{
	deepCopy(buffer);
}

MemBuffer& MemBuffer::operator =(const MemBuffer& buffer)
{
	if (m_allocator != buffer.m_allocator) {
		release();
		m_allocator = buffer.m_allocator;
	}
	deepCopy(buffer);
	return *this;
}

// Steal buffer ressource
MemBuffer::MemBuffer(MemBuffer&& rhs) :
	m_size(move(rhs.m_size)),
	m_ptr(move(rhs.m_ptr)),
	m_allocator(move(rhs.m_allocator))
{
	// Finish resource transfer: remove it from rhs so
	// it is not deallocated twice
	rhs.m_ptr = nullptr;
	rhs.m_size = 0;
}

MemBuffer& MemBuffer::operator =(MemBuffer&& rhs)
{
	// First release previous contents
	release();
	// Steal buffer ressource
	m_ptr = move(rhs.m_ptr);
	m_size = move(rhs.m_size);
	m_allocator = move(rhs.m_allocator);
	// Finish transfer
	rhs.m_ptr = nullptr;
	rhs.m_size = 0;
	return *this;
}

MemBuffer::~MemBuffer()
{
	release();
}

void MemBuffer::alloc(size_t size)
{
	uninitializedAlloc(size);
	initMemory();
}

void MemBuffer::release()
{
	if (m_size) {
		m_allocator->release(m_ptr, m_size, m_alloc_data);

		m_ptr = nullptr;
		m_size = 0;
	}
}

void MemBuffer::clear()
{
	ClearBuffer(getPtr(), 1, FrameDim((int) getSize(), 1, Bpp8));
}

void MemBuffer::uninitializedAlloc(size_t size)
{
	if (!m_allocator)
		throw LIMA_COM_EXC(InvalidValue, "No Allocator was defined");

	if (m_size == size)
		return;

	release();

	size_t real_size = size;
	m_alloc_data = m_allocator->alloc(m_ptr, real_size);

	m_size = size;
}

void MemBuffer::initMemory()
{
	m_allocator->init(m_ptr, m_size);
}

void MemBuffer::deepCopy(const MemBuffer& buffer)
{
	if (buffer.m_ptr) {
		uninitializedAlloc(buffer.getSize());
		memcpy(getPtr(), buffer.getConstPtr(), buffer.getSize());
	} else {
		release();
	}
}


#ifdef LIMA_USE_NUMA
Allocator::DataPtr NumaAllocator::alloc(void* &ptr, size_t& size,
					size_t alignment)
{
	DataPtr alloc_data = MMapAllocator::alloc(ptr, size, alignment);

	if (!m_cpu_mask)
		return alloc_data;

	unsigned long node_mask;
	int max_node;
	getNUMANodeMask(m_cpu_mask, node_mask, max_node);
	if (mbind(ptr, size, MPOL_BIND, &node_mask, max_node, 0) != 0)
		throw LIMA_COM_EXC(Error, "Error in mbind: ")
			<< strerror(errno);

	return alloc_data;
}

void NumaAllocator::getNUMANodeMask(unsigned long cpu_mask,
					       unsigned long& node_mask,
					       int& max_node)
{
	int nb_nodes = numa_max_node() + 1;
	max_node = nb_nodes + 1;

	node_mask = 0;
	for (unsigned int i = 0; i < sizeof(cpu_mask) * 8; ++i) {
		if ((cpu_mask >> i) & 1) {
			unsigned int n = numa_node_of_cpu(i);
			node_mask |= 1L << n;
		}
	}
}
#endif //LIMA_USE_NUMA
