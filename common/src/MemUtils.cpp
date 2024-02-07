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

#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <iomanip>
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
#ifdef __unix
#include <unistd.h>
#else
#include <processlib/win/unistd.h>
#endif

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


// The default allocator
Allocator::Ref Allocator::m_default_allocator;

void Allocator::setDefaultAllocator(Allocator::Ref def_alloc)
{
	if (!def_alloc)
		throw LIMA_COM_EXC(InvalidValue, "Invalid default allocator");
	else if (def_alloc == getDefaultAllocator())
		return;

	ChangeCbList::iterator it, end = m_change_cb_list.end();
	for (it = m_change_cb_list.begin(); it != end; ++it)
		(*it)->onDefaultAllocatorChange(m_default_allocator, def_alloc);
	
	m_default_allocator = def_alloc;
}

Allocator::Ref Allocator::getDefaultAllocator()
{
	EXEC_ONCE(m_default_allocator = std::make_shared<Allocator>());
	return m_default_allocator;
}

// The DefaultChangeCallback list
Allocator::ChangeCbList Allocator::m_change_cb_list;

void Allocator::registerDefaultChangeCallback(DefaultChangeCallback *cb)
{
	ChangeCbList::iterator it, end = m_change_cb_list.end();
	it = std::find(m_change_cb_list.begin(), end, cb);
	if (it != end)
		throw LIMA_COM_EXC(InvalidValue,
				   "DefaultChangeCallback already registered");
	m_change_cb_list.push_back(cb);
}

void Allocator::unregisterDefaultChangeCallback(DefaultChangeCallback *cb)
{
	ChangeCbList::iterator it, end = m_change_cb_list.end();
	it = std::find(m_change_cb_list.begin(), end, cb);
	if (it == end)
		throw LIMA_COM_EXC(InvalidValue,
				   "DefaultChangeCallback not registered");
	m_change_cb_list.erase(it);
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

MemBuffer::MemBuffer(Allocator::Ref allocator /*= {}*/) :
	m_size(0),
	m_ptr(nullptr),
	m_allocator(allocator ? allocator : Allocator::getDefaultAllocator())
{
}

MemBuffer::MemBuffer(int size, Allocator::Ref allocator /*= {}*/) :
	m_size(0),
	m_ptr(nullptr),
	m_allocator(allocator ? allocator : Allocator::getDefaultAllocator())
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
int NumaNodeMask::getMaxNodes()
{
	static int max_nb_nodes = numa_max_node() + 1;
	return  max_nb_nodes;
}

int NumaNodeMask::getNbItems() { return (getMaxNodes() - 1) / ItemBits + 1; }

inline
const NumaNodeMask::ItemArray& NumaNodeMask::checkArray(const ItemArray& array)
{
	if (array.size() != getNbItems())
		throw LIMA_COM_EXC(Error, "NumaNodeMask array has bad size");
	return array;
}

NumaNodeMask::NumaNodeMask() : m_array(getNbItems(), 0) {}

NumaNodeMask::NumaNodeMask(const ItemArray& array) : m_array(checkArray(array))
{}

NumaNodeMask::NumaNodeMask(const NumaNodeMask& o) : m_array(o.m_array) {}

NumaNodeMask::NumaNodeMask(NumaNodeMask&& o) : m_array(std::move(o.m_array)) {}

NumaNodeMask& NumaNodeMask::operator =(const ItemArray& array)
{
	m_array = checkArray(array);
	return *this;
}

NumaNodeMask& NumaNodeMask::operator =(const NumaNodeMask& o)
{
	m_array = o.m_array;
	return *this;
}

NumaNodeMask& NumaNodeMask::operator =(NumaNodeMask&& o)
{
	m_array = std::move(o.m_array);
	return *this;
}

NumaNodeMask NumaNodeMask::fromCPUMask(const CPUMask& cpu_mask)
{
	typedef std::list<std::pair<CPUMask, NumaNodeMask>> NumaNodeList;
	static NumaNodeList cpu_numa_node_list;
	NumaNodeList::iterator it, end = cpu_numa_node_list.end();
	for (it = cpu_numa_node_list.begin(); it != end; ++it)
		if (it->first == cpu_mask)
			return it->second;;

	NumaNodeMask numa_node_mask;
	ItemArray& node_mask = numa_node_mask.m_array;

	for (unsigned int i = 0; i < MaxNbCPUs; ++i) {
		if (cpu_mask.test(i)) {
			unsigned int n = numa_node_of_cpu(i);
			if (n >= getMaxNodes())
				throw LIMA_COM_EXC(Error, "Numa node too high");
			node_mask[n / ItemBits] |= 1L << (n % ItemBits);
		}
	}
	cpu_numa_node_list.emplace_back(std::make_pair(cpu_mask, node_mask));
	return numa_node_mask;
}

void NumaNodeMask::bind(void *ptr, size_t size)
{
	int max_node = getMaxNodes() + 1; // Linux kernel decrements max_node(?)
	if (mbind(ptr, size, MPOL_BIND, &m_array[0], max_node, 0) != 0)
		throw LIMA_COM_EXC(Error, "Error in mbind: ")
			<< strerror(errno);
}

std::ostream& lima::operator <<(std::ostream& os, const NumaNodeMask& mask)
{
	os << "[" << mask.getMaxNodes() << "-bit]" << hex << setfill('0');
	bool first = true;
	int missaligned_bits = mask.getMaxNodes() % mask.ItemBits;
	int first_bits = missaligned_bits ? missaligned_bits : mask.ItemBits;
	const NumaNodeMask::ItemArray& array = mask.getArray();
	NumaNodeMask::ItemArray::const_reverse_iterator it, end = array.rend();
	for (it = array.rbegin(); it != end; ++it, first = false) {
		int word_bits = first ? first_bits : mask.ItemBits;
		os << (!first ? "," : "") << setw(word_bits / 4) << *it;
	}
	return os << setfill(' ') << dec;
}

Allocator::DataPtr NumaAllocator::alloc(void* &ptr, size_t& size,
					size_t alignment)
{
	DataPtr alloc_data = MMapAllocator::alloc(ptr, size, alignment);

	if (m_cpu_mask.none())
		return alloc_data;

	NumaNodeMask node_mask = NumaNodeMask::fromCPUMask(m_cpu_mask);
	node_mask.bind(ptr, size);
	return alloc_data;
}

#endif //LIMA_USE_NUMA
