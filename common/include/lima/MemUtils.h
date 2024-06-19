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
#include "lima/ThreadUtils.h"
#include "lima/RegExUtils.h"

#include <memory>
#include <vector>
#include <queue>
#include <set>

namespace lima
{

//--------------------------------------------------------------------
//  Memory Helper functions
//--------------------------------------------------------------------

void LIMACORE_API GetSystemMem(int& mem_unit, int& system_mem);
	
void LIMACORE_API GetPageSize(int& page_size);

int LIMACORE_API GetDefMaxNbBuffers(const FrameDim& frame_dim);

void LIMACORE_API ClearBuffer(void *ptr, int nb_concat_frames, const FrameDim& frame_dim);


//--------------------------------------------------------------------
//  Allocator
//--------------------------------------------------------------------

struct LIMACORE_API Allocator
{
	DEB_STRUCT_NAMESPC(DebModCommon, "Allocator", "MemUtils");

	// Ancillary data associated to an allocation that may be necessary for
        // releasing the buffers. For instance, MemBuffers keep a reference to
        // these data and pass them to release.
	struct Data {
		virtual ~Data() = default;
	};
	typedef std::shared_ptr<Data> DataPtr;
	typedef std::shared_ptr<Allocator> Ref;

	virtual ~Allocator();

	// Allocate a buffer of a given size and eventually return
	// the associated allocator data and potentially modified size
	virtual DataPtr alloc(void* &ptr, size_t& size, size_t alignment = 16);
	// Fill buffer with zeros (hot page)
	virtual void init(void* ptr, size_t size);
	// Free a buffer
	virtual void release(void* ptr, size_t size, DataPtr alloc_data);

	// string representation for serialization
	virtual std::string toString() const;
};


//--------------------------------------------------------------------
//  AllocatorFactory
//--------------------------------------------------------------------

class LIMACORE_API AllocatorFactory
{
	DEB_CLASS_NAMESPC(DebModCommon, "AllocatorFactory", "MemUtils");
public:
	// The actual factory implementation 
	struct Impl
	{
		struct Param {
			std::string key;
			std::string value;
		};
		typedef std::vector<Param> ParamList;

		virtual std::string getName() const
		{
			return "Allocator";
		}

		virtual Allocator::Ref createFromParams(const ParamList& pars)
		{
			if (!pars.empty())
				throw LIMA_COM_EXC(InvalidValue,
						   "Invalid Allocator params");
			return std::make_shared<Allocator>();
		}
	};
	typedef Impl::ParamList ParamList;

	struct StringDecoder
	{
		DEB_STRUCT_NAMESPC(DebModCommon,
				   "AllocatorFactory::StringDecoder",
				   "MemUtils");

		typedef std::pair<std::string, ParamList> NameParams;

		RegEx m_name_re;
		RegEx m_param_token_re;
		RegEx m_params_re;
		RegEx m_full_re;

		StringDecoder();

		bool checkName(std::string name) const;
		NameParams decode(std::string s) const;

		static RegEx getExactMatchRe(const RegEx& re);
		static RegEx getImplParamsRe(const RegEx& token_re);
	};

	// Callback updating on default allocator change
	class DefaultChangeCallback
	{
	public:
		DefaultChangeCallback();
		DefaultChangeCallback(const DefaultChangeCallback& o) = delete;
		virtual ~DefaultChangeCallback();
		virtual void onDefaultAllocatorChange(Allocator::Ref prev_alloc,
						      Allocator::Ref new_alloc) = 0;
	};

	~AllocatorFactory();

	// The AllocatorFactory singleton
	static AllocatorFactory& get();

	// The AllocatorFactory singleton
	void registerImplementation(Impl *impl);
	void unregisterImplementation(Impl *impl);

	// Sets the static instance of the default allocator
	void setDefaultAllocator(Allocator::Ref def_alloc);
	// Returns the static instance of the default allocator
	Allocator::Ref getDefaultAllocator();

	void registerDefaultChangeCallback(DefaultChangeCallback *cb);
	void unregisterDefaultChangeCallback(DefaultChangeCallback *cb);

	// string representation for serialization
	Allocator::Ref fromString(std::string s);

	const StringDecoder m_string_decoder;

private:
	typedef std::vector<DefaultChangeCallback *> ChangeCbList;
	typedef std::map<std::string, Impl*> ImplMap;

	AllocatorFactory();

	Allocator::Ref m_default_allocator;
	ChangeCbList m_change_cb_list;
	ImplMap m_available_impls;
	Impl m_default_impl;
};


#ifdef __unix

//--------------------------------------------------------------------
//  MMapAllocator
//--------------------------------------------------------------------

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

	// string representation for serialization
	std::string toString() const override;

protected:
	// Allocate a buffer with mmap (virtual address mapping)
	// The real, page-aligned buffer size, is returned in size arg
	void *allocMmap(size_t& size);
};

#endif //__unix

#ifdef LIMA_USE_NUMA

//--------------------------------------------------------------------
//  NumaNodeMask
//--------------------------------------------------------------------

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
std::ostream& operator <<(std::ostream& os, const NumaNodeMask::CPUMask& mask);


//--------------------------------------------------------------------
//  NumaAllocator
//--------------------------------------------------------------------

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

	// string representation for serialization
	std::string toString() const override;

private:
	CPUMask m_cpu_mask; //<! if NUMA is used, keep cpu_mask for later use
};
#endif


//--------------------------------------------------------------------
//  MemBuffer
//--------------------------------------------------------------------

class LIMACORE_API MemBuffer 
{
 public:
	//By default, construct a MemBuffer with the default allocator
	MemBuffer(Allocator::Ref allocator = AllocatorFactory::get().getDefaultAllocator());
	MemBuffer(int size,
		  Allocator::Ref allocator = AllocatorFactory::get().getDefaultAllocator(),
		  bool init_mem = true);
	~MemBuffer();

	// MemBuffer are copy constructible (deep copy, no aliasing)
	MemBuffer(const MemBuffer& buffer);
	MemBuffer& operator =(const MemBuffer& buffer);

	// MemBuffer are move-constructible or move-assignable.
	MemBuffer(MemBuffer&& rhs);
	MemBuffer& operator =(MemBuffer&& rhs);

	/// Allocate and initialize memory (if requested)
	void alloc(size_t size, bool init_mem = true);
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

	/// Initialize the memory
	void initMemory();

 private:
	/// Call the allocator to (eventually) free the current buffer then allocate a new buffer
	void uninitializedAlloc(size_t size);

	size_t m_size;	//!< The size of the buffer in bytes
	void *m_ptr;	//!< The pointer ot the buffer
	Allocator::Ref m_allocator;	//!< The allocator used to alloc and free the buffer
	Allocator::DataPtr m_alloc_data;
};


//--------------------------------------------------------------------
//  BufferPool
//--------------------------------------------------------------------

class LIMACORE_API BufferPool
{
	DEB_CLASS_NAMESPC(DebModCommon, "BufferPool", "MemUtils");
  
 public:
	BufferPool(Allocator::Ref allocator = {}, bool init_mem = true);
	~BufferPool();

	void setAllocator(Allocator::Ref allocator);
	Allocator::Ref getAllocator() const { return m_alloc; }

	void setInitMem(bool init_mem);
	bool getInitMem() const { return m_init_mem; }
	
	std::shared_ptr<void> getBuffer();

	void allocBuffers(int nb_buffers, int size);
	void releaseBuffers();
	int getNbBuffers() const;
	int getBufferSize() const;

 private:
	class DefAllocChangeCb;
	friend class DefAllocChangeCb;
	void onDefaultAllocatorChange(Allocator::Ref prev_alloc,
				      Allocator::Ref new_alloc);

	void _allocBuffers(int nb_buffers, int size, AutoMutex& l);
	void _releaseBuffers(AutoMutex& l);

	Mutex m_mutex;
	Allocator::Ref m_alloc;
	bool m_init_mem;
	std::vector<MemBuffer> m_buffers;
	std::queue<void *> m_available;
	int m_buffer_size;  
	DefAllocChangeCb *m_def_alloc_change_cb;
};

} // namespace lima


#endif // MEMUTILS_H
