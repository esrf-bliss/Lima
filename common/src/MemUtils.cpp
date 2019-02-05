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
#include "lima/Exceptions.h"

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
		throw LIMA_HW_EXC(InvalidValue, "Invalid mem_unit value");
#ifdef __unix
        struct sysinfo s_info;
	if (sysinfo(&s_info) < 0)
		throw LIMA_HW_EXC(Error, "Error calling sysinfo");

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
		throw LIMA_HW_EXC(InvalidValue, "Invalid FrameDim");

	int tot_buffers;
	GetSystemMem(frame_size, tot_buffers);
	return int(tot_buffers);
}
	
void lima::ClearBuffer(void *ptr, int nb_concat_frames, 
		       const FrameDim& frame_dim)
{
	memset(ptr, 0, nb_concat_frames * frame_dim.getMemSize());
}

MemBuffer::Allocator *MemBuffer::Allocator::getAllocator()
{
	static Allocator allocator;
	return &allocator;
}

void MemBuffer::Allocator::alloc(MemBuffer& buffer, int& size)
{
	void *ptr;
#ifdef __unix
	if (useMmap(size)) {
		ptr = allocMmap(size);
	} else {
		int ret = posix_memalign(&ptr, Alignment, size);
		if (ret != 0)
			throw LIMA_COM_EXC(Error, "Error in posix_memalign: ")
				<< strerror(ret);
	}
#else
	ptr = _aligned_malloc(size, Alignment);
	if (!ptr)
		throw LIMA_COM_EXC(Error, "Error in _aligned_malloc: ")
			<< "NULL pointer return";
#endif
	buffer.m_ptr = ptr;
	buffer.m_size = size;
}

void MemBuffer::Allocator::init(MemBuffer& buffer)
{
	char *ptr = (char *) buffer.getPtr();
	int size = buffer.getSize();

#ifdef __unix
	int page_size;
	GetPageSize(page_size);
#ifdef __SSE2__
	if (!((long) ptr & 15)) { // aligned to 128 bits
		__m128i zero = _mm_setzero_si128();
		for (long i = 0; i < size; i += page_size, ptr += page_size) {
			if (size_t(size - i) >= sizeof(__m128i))
				_mm_store_si128((__m128i *) ptr, zero);
			else
				*ptr = 0;
		}
		_mm_empty();
	} else {
#endif
		for (long i = 0; i < size; i += page_size, ptr += page_size)
			*ptr = 0;
#ifdef __SSE2__
	}
#endif

#else
	memset(ptr, 0, size);
#endif
}

void MemBuffer::Allocator::copy(MemBuffer& buffer, const MemBuffer& src)
{
	memcpy(buffer.getPtr(), src.getConstPtr(), src.getSize());
}

void MemBuffer::Allocator::clear(MemBuffer& buffer)
{
	ClearBuffer(buffer.getPtr(), 1, FrameDim(buffer.getSize(), 1, Bpp8));
}

void MemBuffer::Allocator::release(MemBuffer& buffer)
{
	void *ptr = buffer.getPtr();
	int size = buffer.getSize();

#ifdef __unix
	if (useMmap(size))
		munmap(ptr, size);
	else
		free(ptr);
#else
	_aligned_free(ptr);
#endif
	buffer.m_ptr = NULL;
	buffer.m_size = 0;
}

int MemBuffer::Allocator::getPageAlignedSize(int size)
{
	int page_size;
	GetPageSize(page_size);
	int misaligned = size & (page_size - 1);
	if (misaligned)
		size += page_size - misaligned;
	return size;
}

#ifdef __unix
bool MemBuffer::Allocator::useMmap(int size)
{
	return size >= 128 * 1024;
}

void *MemBuffer::Allocator::allocMmap(int& size)
{
	size = getPageAlignedSize(size);
	void *ptr = (char *) mmap(0, size, PROT_READ | PROT_WRITE,
				  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (!ptr)
		throw LIMA_COM_EXC(Error, "Error in mmap: ")
			<< strerror(errno);
	return ptr;
}
#endif

inline void MemBuffer::init()
{
	m_size = 0;
	m_ptr = NULL;
	m_allocator = NULL;
#ifdef LIMA_USE_NUMA
	m_cpu_mask = 0;
#endif
}

MemBuffer::MemBuffer()
{
	init();
}

MemBuffer::MemBuffer(int size)
{
	init();
	alloc(size);
}

MemBuffer::MemBuffer(const MemBuffer& buffer)
{
	init();
	deepCopy(buffer);
}

MemBuffer::~MemBuffer()
{
	release();
}

void MemBuffer::alloc(int size)
{
	allocMemory(size);
	initMemory();
}

void MemBuffer::allocMemory(int& size)
{
	if (m_size == size)
		return;

	release();

	if (!m_allocator)
		m_allocator = Allocator::getAllocator();

	m_allocator->alloc(*this, size);
}

void MemBuffer::initMemory()
{
	m_allocator->init(*this);
}

void MemBuffer::deepCopy(const MemBuffer& buffer)
{
	int size = buffer.getSize();
	allocMemory(size);
	m_allocator->copy(*this, buffer);
}

MemBuffer& MemBuffer::operator =(const MemBuffer& buffer)
{
	deepCopy(buffer);
	return *this;
}

void MemBuffer::release()
{
	if (m_size)
		m_allocator->release(*this);
}

void MemBuffer::clear()
{
	if (m_size)
		m_allocator->clear(*this);
}

#ifdef LIMA_USE_NUMA
void MemBuffer::setCPUAffinityMask(unsigned long cpu_mask)
{
	m_cpu_mask = cpu_mask;
	if (m_cpu_mask != 0)
		m_allocator = NumaAllocator::getAllocator();
}

void MemBuffer::NumaAllocator::alloc(MemBuffer& buffer, int& size)
{
	Allocator::alloc(buffer, size);

	if (!useMmap(size) || !buffer.m_cpu_mask)
		return;

	void *ptr = buffer.getPtr();
	unsigned long node_mask;
	int max_node;
	getNUMANodeMask(buffer.m_cpu_mask, node_mask, max_node);
	mbind(ptr, size, MPOL_BIND, &node_mask, max_node, 0);
}

void MemBuffer::NumaAllocator::init(MemBuffer& buffer)
{
	Allocator::init(buffer);
}

void MemBuffer::NumaAllocator::copy(MemBuffer& buffer, const MemBuffer& src)
{
	Allocator::copy(buffer, src);
}

void MemBuffer::NumaAllocator::clear(MemBuffer& buffer)
{
	Allocator::clear(buffer);
}

void MemBuffer::NumaAllocator::release(MemBuffer& buffer)
{
	Allocator::release(buffer);
}

MemBuffer::NumaAllocator *MemBuffer::NumaAllocator::getAllocator()
{
	static NumaAllocator allocator;
	return &allocator;
}

void MemBuffer::NumaAllocator::getNUMANodeMask(unsigned long cpu_mask,
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
#endif
