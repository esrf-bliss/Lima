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
#include "lima/Debug.h"
#include "lima/MemUtils.h"

#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <limits>
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

DEB_GLOBAL_NAMESPC(DebModCommon, "MemUtils")


//--------------------------------------------------------------------
//  Memory Helper functions
//--------------------------------------------------------------------

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
	DEB_GLOBAL_FUNCT();
	
	int frame_size = frame_dim.getMemSize();
	if (frame_size <= 0)
	{
		DEB_ERROR() << "Invalid FrameDim: " << frame_dim;
		throw LIMA_COM_EXC(InvalidValue, "Invalid FrameDim");
	}

	int tot_buffers;
	GetSystemMem(frame_size, tot_buffers);
	return int(tot_buffers);
}
	
void lima::ClearBuffer(void *ptr, int nb_concat_frames, 
		       const FrameDim& frame_dim)
{
	memset(ptr, 0, nb_concat_frames * size_t(frame_dim.getMemSize()));
}


//--------------------------------------------------------------------
//  Allocator
//--------------------------------------------------------------------

Allocator::~Allocator()
{
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

std::string Allocator::toString() const
{
	return "Allocator()";
}


//--------------------------------------------------------------------
//  AllocatorFactory::DefaultChangeCallback
//--------------------------------------------------------------------

AllocatorFactory::DefaultChangeCallback::DefaultChangeCallback()
{
	get().registerDefaultChangeCallback(this);
}

AllocatorFactory::DefaultChangeCallback::~DefaultChangeCallback()
{
	get().unregisterDefaultChangeCallback(this);
}


//--------------------------------------------------------------------
//  AllocatorFactory::StringDecoder
//--------------------------------------------------------------------

AllocatorFactory::StringDecoder::StringDecoder()
	: m_name_re("(?P<name>[A-Za-z0-9_]+)"),
	  m_param_token_re("(?P<key>[A-Za-z0-9_]+)=(?P<value>[^,)]+)"),
	  m_params_re(getImplParamsRe(m_param_token_re)),
	  m_full_re(m_name_re + m_params_re)

{
	DEB_CONSTRUCTOR();

	DEB_TRACE() << "m_name_re=" << m_name_re.getRegExStr();
	DEB_TRACE() << "m_param_token_re=" << m_param_token_re.getRegExStr();
	DEB_TRACE() << "m_params_re=" << m_params_re.getRegExStr();
	DEB_TRACE() << "m_full_re=" << m_full_re.getRegExStr();
}

bool AllocatorFactory::StringDecoder::checkName(std::string name) const
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(name);
	RegEx::FullMatchType m;
	return getExactMatchRe(m_name_re).match(name, m);
}

RegEx AllocatorFactory::StringDecoder::getExactMatchRe(const RegEx& re)
{
	return "^" + re.getRegExStr() + "$";
}

RegEx AllocatorFactory::StringDecoder::getImplParamsRe(const RegEx& token_re)
{
	std::string unnamed_token = token_re.getSimpleRegEx().getRegExStr();
	std::string all_tokens = unnamed_token + "(," + unnamed_token + ")*";
	return "\\((?P<params>" + all_tokens + ")?\\)";
}

AllocatorFactory::StringDecoder::NameParams
AllocatorFactory::StringDecoder::decode(std::string s) const
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(s);
	RegEx::FullNameMatchType m;
	RegEx full_exact_re = getExactMatchRe(m_full_re);
	if (!full_exact_re.matchName(s, m))
		THROW_COM_ERROR(NotSupported)
			<< "AllocatorFactory::StringDecoder::decode: "
			<< "could not parse \"" << s << "\"";
	std::string name = m["name"];
	DEB_TRACE() << DEB_VAR2(name, m["params"].str());
	ParamList params;
	if (m["params"].found()) {
		RegEx::NameMatchListType ml;
		std::string param_str = m["params"];
		m_param_token_re.multiSearchName(param_str, ml);
		for (auto& m: ml)
			params.emplace_back(Impl::Param{m["key"], m["value"]});
	}
	if (DEB_CHECK_ANY(DebTypeReturn)) {
		DEB_RETURN() << DEB_VAR2(name, params.size());
		int i = 0;
		for (auto& par: params)
			DEB_RETURN() << i++ << ": "
				     << DEB_VAR2(par.key, par.value);
			
	}
	return std::make_pair(name, params);
}

//--------------------------------------------------------------------
//  AllocatorFactory
//--------------------------------------------------------------------

// The AllocatorFactory singleton
AllocatorFactory& AllocatorFactory::get()
{
	DEB_STATIC_FUNCT();
	static AllocatorFactory factory;
	return factory;
}

AllocatorFactory::AllocatorFactory()
{
	DEB_CONSTRUCTOR();

	registerImplementation(&m_default_impl);
	m_default_allocator = fromString("Allocator()");
}

AllocatorFactory::~AllocatorFactory()
{
	DEB_DESTRUCTOR();
	unregisterImplementation(&m_default_impl);
}

void AllocatorFactory::registerImplementation(Impl *impl)
{
	DEB_MEMBER_FUNCT();
	std::string name = impl->getName();
	DEB_PARAM() << DEB_VAR1(name);
	if (!m_string_decoder.checkName(name))
		THROW_COM_ERROR(InvalidValue)
			<< "Invalid AllocatorFactory::Impl name: " << name;
	auto res = m_available_impls.insert(std::make_pair(name, impl));
	if (!res.second)
		THROW_COM_ERROR(InvalidValue)
			<< "AllocatorFactory::Impl already registered";
}

void AllocatorFactory::unregisterImplementation(Impl *impl)
{
	DEB_MEMBER_FUNCT();
	std::string name = impl->getName();
	DEB_PARAM() << DEB_VAR1(name);
	auto it = m_available_impls.find(name);
	if (it != m_available_impls.end())
		m_available_impls.erase(it);
}

void AllocatorFactory::setDefaultAllocator(Allocator::Ref def_alloc)
{
	DEB_MEMBER_FUNCT();
	if (!def_alloc)
		THROW_COM_ERROR(InvalidValue) << "Invalid default allocator";
	else if (def_alloc == getDefaultAllocator())
		return;

	DEB_TRACE() << "Changing default Allocator: "
		    << "prev=" << m_default_allocator->toString() << ", "
		    << "new=" << def_alloc->toString();

	for (auto cb: m_change_cb_list)
		cb->onDefaultAllocatorChange(m_default_allocator, def_alloc);
	
	m_default_allocator = def_alloc;
}

Allocator::Ref AllocatorFactory::getDefaultAllocator()
{
	return m_default_allocator;
}

void AllocatorFactory::registerDefaultChangeCallback(DefaultChangeCallback *cb)
{
	DEB_MEMBER_FUNCT();
	ChangeCbList::iterator it, end = m_change_cb_list.end();
	it = std::find(m_change_cb_list.begin(), end, cb);
	if (it != end)
		THROW_COM_ERROR(InvalidValue)
			<< "DefaultChangeCallback already registered";
	m_change_cb_list.push_back(cb);
}

void AllocatorFactory::unregisterDefaultChangeCallback(DefaultChangeCallback *cb)
{
	DEB_MEMBER_FUNCT();
	ChangeCbList::iterator it, end = m_change_cb_list.end();
	it = std::find(m_change_cb_list.begin(), end, cb);
	if (it == end)
		THROW_COM_ERROR(InvalidValue)
			<< "DefaultChangeCallback not registered";
	m_change_cb_list.erase(it);
}

Allocator::Ref AllocatorFactory::fromString(std::string s)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(s);
	StringDecoder::NameParams name_params = m_string_decoder.decode(s);
	ImplMap::iterator it = m_available_impls.find(name_params.first);
	if (it == m_available_impls.end())
		THROW_COM_ERROR(NotSupported)
			<< "AllocatorFactory::fromString: could not find "
			<< "allocator " << name_params.first;
	Allocator::Ref alloc = it->second->createFromParams(name_params.second);
	if (!alloc)
		THROW_COM_ERROR(InvalidValue) << it->first << " returned"
					      << "empty Allocator::Ref";
	DEB_RETURN() << DEB_VAR1(alloc->toString());
	return alloc;
}


#ifdef __unix

//--------------------------------------------------------------------
//  MMapAllocator
//--------------------------------------------------------------------

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

std::string MMapAllocator::toString() const
{
	return "MMapAllocator()";
}

//--------------------------------------------------------------------
//  MMapAllocatorFactory
//--------------------------------------------------------------------

class MMapAllocatorFactory
{
	struct Impl : AllocatorFactory::Impl
	{
		DEB_STRUCT_NAMESPC(DebModCommon, "MMapAllocatorFactory::Impl",
				   "MemUtils");

		std::string getName() const override
		{
			return "MMapAllocator";
		}

		Allocator::Ref createFromParams(const ParamList& pars) override
		{
			DEB_MEMBER_FUNCT();
			if (!pars.empty())
				THROW_COM_ERROR(InvalidValue) <<
					"Invalid MMapAllocator params";
			return std::make_shared<MMapAllocator>();
		}
	} m_impl;

public:
	MMapAllocatorFactory()
	{
		AllocatorFactory::get().registerImplementation(&m_impl);
	}
} mmap_allocator_factory;

#endif //__unix


//--------------------------------------------------------------------
//  MemBuffer
//--------------------------------------------------------------------

MemBuffer::MemBuffer(Allocator::Ref allocator /*= {}*/) :
	m_size(0),
	m_ptr(nullptr),
	m_allocator(allocator)
{
}

MemBuffer::MemBuffer(int size, Allocator::Ref allocator /*= {}*/,
		     bool init_mem /*= true*/) :
	m_size(0),
	m_ptr(nullptr),
	m_allocator(allocator)
{
	alloc(size, init_mem);
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

void MemBuffer::alloc(size_t size, bool init_mem /*= true*/)
{
	uninitializedAlloc(size);
	if (init_mem)
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


//--------------------------------------------------------------------
//  Local helper
//--------------------------------------------------------------------

template <typename X, typename Y>
static unsigned int ceiling_div(X x, Y y)
{
	return (x - 1) / y + 1;
}

//--------------------------------------------------------------------
//  Variable-length array hexadecimal string CODEC helpers
//--------------------------------------------------------------------

std::string lima::convert_2_string(const unsigned long *array, int nb_bits,
				   bool comma_sep)
{
	constexpr int NbULongBits = sizeof(unsigned long) * 8;
	constexpr int NbUIntBits = 32;
	constexpr int NbULongInts = NbULongBits / NbUIntBits;

	int nb_items = ceiling_div(nb_bits, NbULongBits);
	int missaligned_bits = nb_bits % NbULongBits;
	int first_bits = missaligned_bits ? missaligned_bits : NbULongBits;
	ostringstream os;
	os << hex << setfill('0');
	string sep;
	bool first = true;
	for (int i = nb_items - 1; i >= 0; --i) {
		unsigned long ul_val = array[i];
		int item_bits = first ? first_bits : NbULongBits;
		int nb_ints = ceiling_div(item_bits, NbUIntBits);
		first_bits %= NbUIntBits;
		for (int j = nb_ints - 1; j >= 0; --j, first = false) {
			uint32_t ui_val = (ul_val >> (j * NbUIntBits)) & 0xffffffff;
			int word_bits = first ? first_bits : NbUIntBits;
			os << sep << setw(ceiling_div(word_bits, 4)) << ui_val;
			if (sep.empty() && comma_sep)
				sep = ",";
		}
	}
	return os.str();
}

void lima::convert_from_string(const std::string& s, unsigned long *array,
			       int nb_bits)
{
	constexpr int NbULongBits = sizeof(unsigned long) * 8;
	constexpr int base = 16;

	auto convert_to_ulong = [&](const string& s) {
		size_t pos;
		unsigned long v = stoul(s, &pos, base);
		if (pos != s.size())
			throw LIMA_COM_EXC(InvalidValue, "Invalid mask: ") << s;
		return v;
	};

	int nb_items = ceiling_div(nb_bits, NbULongBits);
	const size_t bits_per_char = (base == 16) ? 4 : 1;
	const size_t chars_per_ulong = NbULongBits / bits_per_char;
	int idx = 0;
	string val;
	string::const_reverse_iterator it, end = s.rend();
	for (it = s.rbegin(); it != end; ++it) {
		if (*it == ',')
			continue;
		else if (idx == nb_items)
			throw LIMA_COM_EXC(InvalidValue, "Invalid mask: ")
							<< s;
		val.insert(0, 1, *it);
		if (val.size() == chars_per_ulong) {
			array[idx++] = convert_to_ulong(val);
			val.clear();
		}
	}
	if (!val.empty())
		array[idx++] = convert_to_ulong(val);
	while (idx < nb_items)
		array[idx++] = 0;
}


#ifdef LIMA_USE_NUMA

//--------------------------------------------------------------------
//  CPUMask
//--------------------------------------------------------------------

constexpr CPUMask::BitMask CPUMask::ULongMask;

void CPUMask::toULongArray(ULongArray& array) const
{
	for (int i = 0; i < NbULongs; ++i)
		array[i] = ((m_mask >> (i * NbULongBits)) & ULongMask).to_ulong();
}

CPUMask CPUMask::fromULongArray(const ULongArray& array)
{
	BitMask mask;
	for (int i = 0; i < NbULongs; ++i)
		mask |= BitMask(array[i]) << (i * NbULongBits);
	return mask;
}

string CPUMask::toString(int base, bool comma_sep) const
{
	if (base == 2)
		return m_mask.to_string();
	else if (base != 16)
		throw LIMA_COM_EXC(InvalidValue, "Invalid base: ") << base;

	ULongArray array;
	toULongArray(array);
	return convert_2_string(array, MaxNbCPUs, comma_sep);
}

CPUMask CPUMask::fromString(string aff_str, int base)
{
	if ((base != 2) && (base != 16))
		throw LIMA_COM_EXC(InvalidValue, "Invalid base: ") << base;
	
	if (!aff_str.empty() && (base == 16) && (aff_str.find("0x") == 0))
		aff_str.erase(0, 2);
	size_t len = aff_str.size();
	if (len && (aff_str.rfind('L') == len - 1))
		aff_str.erase(len - 1);
	if (aff_str.empty())
		throw LIMA_COM_EXC(InvalidValue, "Invalid mask stripped str");

	if (base == 2) {
		istringstream is(aff_str);
		BitMask mask;
		is >> mask;
		if (is.fail())
			throw LIMA_COM_EXC(InvalidValue, "Invalid base-2 mask: ")
				<< aff_str;
		return CPUMask(mask);
	} else {
		ULongArray array;
		convert_from_string(aff_str, array, MaxNbCPUs);
		return fromULongArray(array);
	}
}

std::ostream& lima::operator <<(std::ostream& os, const CPUMask& mask)
{
	return os << mask.toString();
}

std::istream& lima::operator >>(std::istream& is, CPUMask& mask)
{
	string s;
	is >> s;
	if (!is)
		return is;
	try {
		mask = CPUMask::fromString(s);
	} catch (...) {
		is.setstate(std::ios_base::failbit);
	}
	return is;
}

//--------------------------------------------------------------------
//  NumaNodeMask
//--------------------------------------------------------------------

int NumaNodeMask::getMaxNodes()
{
	static int max_nb_nodes = numa_max_node() + 1;
	return  max_nb_nodes;
}

int NumaNodeMask::getNbItems() { return ceiling_div(getMaxNodes(), ItemBits); }

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
	static Mutex mutex;
	AutoMutex l(mutex);
	NumaNodeList::iterator it, end = cpu_numa_node_list.end();
	for (it = cpu_numa_node_list.begin(); it != end; ++it)
		if (it->first == cpu_mask)
			return it->second;

	NumaNodeMask numa_node_mask;
	ItemArray& node_mask = numa_node_mask.m_array;

	for (unsigned int i = 0; i < CPUMask::MaxNbCPUs; ++i) {
		if (cpu_mask.m_mask.test(i)) {
			unsigned int n = numa_node_of_cpu(i);
			if (n >= getMaxNodes())
				throw LIMA_COM_EXC(Error, "Numa node too high");
			node_mask[n / ItemBits] |= 1L << (n % ItemBits);
		}
	}
	cpu_numa_node_list.emplace_back(std::make_pair(cpu_mask, node_mask));
	return numa_node_mask;
}

CPUMask NumaNodeMask::toCPUMask() const
{
	typedef std::list<std::pair<CPUMask, NumaNodeMask>> NumaNodeList;
	static NumaNodeList cpu_numa_node_list;
	static Mutex mutex;
	AutoMutex l(mutex);
	NumaNodeList::iterator it, end = cpu_numa_node_list.end();
	for (it = cpu_numa_node_list.begin(); it != end; ++it)
		if (it->second == *this)
			return it->first;

	CPUMask cpu_mask;
	const ItemArray& node_mask = m_array;

	for (unsigned int i = 0; i < CPUMask::MaxNbCPUs; ++i) {
		int n = numa_node_of_cpu(i);
		if (n < 0)
			continue;
		else if (n >= getMaxNodes())
			throw LIMA_COM_EXC(Error, "Numa node too high");
		if (node_mask[n / ItemBits] & (1L << (n % ItemBits)))
			cpu_mask.m_mask.set(i);
	}
	cpu_numa_node_list.emplace_back(std::make_pair(cpu_mask, node_mask));
	return cpu_mask;
}

std::string NumaNodeMask::toString(bool comma_sep) const
{
	return convert_2_string(getArray().data(), getMaxNodes(), comma_sep);
}

NumaNodeMask NumaNodeMask::fromString(std::string node_str)
{
	NumaNodeMask mask;
	convert_from_string(node_str, mask.getArray().data(), getMaxNodes());
	return mask;
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
	return os << "[" << mask.getMaxNodes() << "-bit]" << mask.toString(true);
}

std::istream& lima::operator >>(std::istream& is, NumaNodeMask& mask)
{
	string s;
	is >> s;
	try {
		mask = NumaNodeMask::fromString(s);
	} catch (...) {
		is.setstate(std::ios_base::failbit);
	}
	return is;
}


//--------------------------------------------------------------------
//  NumaAllocator
//--------------------------------------------------------------------

Allocator::DataPtr NumaAllocator::alloc(void* &ptr, size_t& size,
					size_t alignment)
{
	DataPtr alloc_data = MMapAllocator::alloc(ptr, size, alignment);

	if (m_cpu_mask.m_mask.none())
		return alloc_data;

	NumaNodeMask node_mask = NumaNodeMask::fromCPUMask(m_cpu_mask);
	node_mask.bind(ptr, size);
	return alloc_data;
}

std::string NumaAllocator::toString() const
{
	std::ostringstream os;
	os << "NumaAllocator(cpu_mask=0x" << m_cpu_mask << ")";
	return os.str();
}


//--------------------------------------------------------------------
//  NumaAllocatorFactory
//--------------------------------------------------------------------

class NumaAllocatorFactory
{
	struct Impl : AllocatorFactory::Impl
	{
		DEB_STRUCT_NAMESPC(DebModCommon, "NumaAllocatorFactory::Impl",
				   "MemUtils");

		std::string getName() const override
		{
			return "NumaAllocator";
		}

		template <class Mask>
		Allocator::Ref fromMaskStr(const std::string& mask_str)
		{
			DEB_MEMBER_FUNCT();

			if (mask_str.find("0x") != 0)
				THROW_COM_ERROR(InvalidValue)
					<< "Invalid hexadecimal-coded mask "
					<< "without explicit 0x prefix";
			Mask mask;
			std::istringstream is(mask_str.substr(2));
			is >> mask;
			if (!is)
				THROW_COM_ERROR(InvalidValue)
					<< "Invalid NumaAllocator mask: "
					<< mask_str;
			return std::make_shared<NumaAllocator>(mask);
		}

		Allocator::Ref createFromParams(const ParamList& pars) override
		{
			DEB_MEMBER_FUNCT();

			bool ok = (pars.size() == 1);
			bool is_cpu = (ok && (pars[0].key == "cpu_mask"));
			bool is_node = (ok && (pars[0].key == "node_mask"));
			if (!is_cpu && !is_node)
				THROW_COM_ERROR(InvalidValue)
					<< "Invalid param(s) string, must be: "
					<< "NumaAllocator(cpu_mask=0x<mask>) or "
					<< "NumaAllocator(node_mask=0x<mask>)";

			const std::string& mask_str = pars[0].value;
			if (is_cpu)
				return fromMaskStr<CPUMask>(mask_str);
			else
				return fromMaskStr<NumaNodeMask>(mask_str);

		}
	} m_impl;

public:
	NumaAllocatorFactory()
	{
		AllocatorFactory::get().registerImplementation(&m_impl);
	}
} numa_allocator_factory;

#endif //LIMA_USE_NUMA


//--------------------------------------------------------------------
//  BufferPool
//--------------------------------------------------------------------

class BufferPool::DefAllocChangeCb :
	public AllocatorFactory::DefaultChangeCallback
{
public:
	DefAllocChangeCb(BufferPool& pool) : m_pool(pool)
	{
	}

	virtual void onDefaultAllocatorChange(Allocator::Ref prev_alloc,
					      Allocator::Ref new_alloc)
	{
		m_pool.onDefaultAllocatorChange(prev_alloc, new_alloc);
	}

private:
	BufferPool& m_pool;
};


BufferPool::BufferPool(Allocator::Ref allocator, bool init_mem)
	: m_alloc(allocator), m_init_mem(init_mem), m_buffer_size(0)
{
	DEB_CONSTRUCTOR();
	DEB_PARAM() << DEB_VAR1(allocator);
	// can only create the cb once "this" is ready
	m_def_alloc_change_cb = new DefAllocChangeCb(*this);
}

BufferPool::~BufferPool()
{
	DEB_DESTRUCTOR();
	releaseBuffers();
	// destroy cb while "this" is still valid
	delete m_def_alloc_change_cb;
}

std::shared_ptr<void> BufferPool::getBuffer()
{
	DEB_MEMBER_FUNCT();
	void *p = nullptr;
	{
		AutoMutex l(m_mutex);
		if (m_available.empty())
			return {};
		p = m_available.front();
		m_available.pop();
	}
	auto releaser = [&](void *p) {
		AutoMutex l(m_mutex);
		m_available.push(p);
	};
	return std::shared_ptr<void>(p, releaser);
}

void BufferPool::setAllocator(Allocator::Ref allocator)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(allocator.get());

	if (allocator == m_alloc)
		return;

	releaseBuffers();
	m_alloc = allocator;
}

void BufferPool::onDefaultAllocatorChange(Allocator::Ref prev_alloc,
					  Allocator::Ref new_alloc)
{
	DEB_MEMBER_FUNCT();
	if (!m_alloc)
		releaseBuffers();
}

void BufferPool::setInitMem(bool init_mem)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(init_mem);

	if ((init_mem == m_init_mem) || m_init_mem)
		return;

	std::vector<MemBuffer>::iterator it, end = m_buffers.end();
	for (it = m_buffers.begin(); it != end; ++it)
		it->initMemory();

	m_init_mem = init_mem;
}

void BufferPool::allocBuffers(int nb_buffers, int size)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(nb_buffers, size);
	AutoMutex l(m_mutex);
	_allocBuffers(nb_buffers, size, l);
}

int BufferPool::getNbBuffers() const
{
	DEB_MEMBER_FUNCT();
	int nb_buffers =  m_buffers.size();
	DEB_RETURN() << DEB_VAR1(nb_buffers);
	return nb_buffers;
}

int BufferPool::getBufferSize() const
{
	DEB_MEMBER_FUNCT();
	int buffer_size =  m_buffer_size;
	DEB_RETURN() << DEB_VAR1(buffer_size);
	return buffer_size;
}

void BufferPool::releaseBuffers()
{
	DEB_MEMBER_FUNCT();
	AutoMutex l(m_mutex);
	_releaseBuffers(l);
}

void BufferPool::_allocBuffers(int nb_buffers, int size, AutoMutex& l)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR4(m_buffers.size(), nb_buffers,
				m_buffer_size, size);
	if (size != m_buffer_size)
		_releaseBuffers(l);
	if (nb_buffers == m_buffers.size())
		return;
	if (m_buffers.capacity() < nb_buffers)
		m_buffers.reserve(nb_buffers + 128);
	m_buffer_size = size;
	DEB_TRACE() << DEB_VAR2(m_buffers.size(), nb_buffers);
	// TODO: reduce the number of buffers
	Allocator::Ref allocator = m_alloc;
	if (!allocator)
		allocator = AllocatorFactory::get().getDefaultAllocator();
	while (m_buffers.size() < nb_buffers) {
		m_buffers.emplace_back(size, allocator, m_init_mem);
		m_available.push(m_buffers.back().getPtr());
	}
}

void BufferPool::_releaseBuffers(AutoMutex& l)
{
	DEB_MEMBER_FUNCT();
	if (m_available.size() != m_buffers.size())
		DEB_ERROR() << "Buffers still in use";
	while (m_available.size() > 0)
		m_available.pop();
	m_buffers.clear();
	m_buffer_size = 0;
}
