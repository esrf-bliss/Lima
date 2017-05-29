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


MemBuffer::MemBuffer()
	: m_size(0), m_ptr(NULL)
{
}

MemBuffer::MemBuffer(int size)
	: m_size(0), m_ptr(NULL)
{
	alloc(size);
	char* ptr = (char*)m_ptr;
	
#ifdef __unix
	long page_size = sysconf(_SC_PAGESIZE);
        char* ptr = (char*)m_ptr;
#ifdef __SSE2__
	if(!((long)ptr & 15))	// aligned to 128 bits
	  {
              __m128i zero = _mm_setzero_si128();
	      for(long i = 0;i < size;i += page_size,ptr+=page_size)
		{
		  if(size_t(size - page_size) > sizeof(__m128i))
		    _mm_store_si128((__m128i*)ptr,zero);
		  else
		    *ptr = 0;
		}
          }
	else
	  {
#endif
	      for(long i = 0;i < size;i += page_size,ptr+=page_size)
		*ptr = 0;
#ifdef __SSE2__
          }
#endif

#else
	memset(getPtr(), 0, size);
#endif
}

MemBuffer::MemBuffer(const MemBuffer& buffer)
	: m_size(0), m_ptr(NULL)
{
	copy(buffer);
}

MemBuffer::~MemBuffer()
{
	release();
}


void MemBuffer::alloc(int size)
{
	if (m_size == size)
		return;

	release();

#ifdef __unix
	int ret = posix_memalign(&m_ptr, Alignment, size);
	if (ret != 0)
		throw LIMA_COM_EXC(Error, "Error in posix_memalign: ")
			<< strerror(ret);
#else
	m_ptr = _aligned_malloc(size,Alignment);
	if(!m_ptr)
	  throw LIMA_COM_EXC(Error, "Error in _aligned_malloc: NULL pointer return");
#endif
	m_size = size;
}

void MemBuffer::copy(const MemBuffer& buffer)
{
	int size = buffer.getSize();
	alloc(size);
	memcpy(getPtr(), buffer.getConstPtr(), size);
}

void MemBuffer::release()
{
	if (!m_size)
		return;

#ifdef __unix
	free(m_ptr);
#else
	_aligned_free(m_ptr);
#endif
	m_ptr = NULL;
	m_size = 0;
}

MemBuffer& MemBuffer::operator =(const MemBuffer& buffer)
{
	copy(buffer);
	return *this;
}

void MemBuffer::clear()
{
	ClearBuffer(getPtr(), 1, FrameDim(getSize(), 1, Bpp8));
}
