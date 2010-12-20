#ifndef MEMUTILS_H
#define MEMUTILS_H

#include "Compatibility.h"
#include "SizeUtils.h"
#include "Debug.h"

namespace lima
{

void DLL_EXPORT GetSystemMem(int& mem_unit, int& system_mem);
	
void DLL_EXPORT GetPageSize(int& page_size);

int DLL_EXPORT GetDefMaxNbBuffers(const FrameDim& frame_dim, double sys_mem_factor = 0);

void DLL_EXPORT ClearBuffer(void *ptr, int nb_concat_frames, const FrameDim& frame_dim);


class DLL_EXPORT MemBuffer 
{
 public:
	enum {
		Alignment = 16,
	};

	MemBuffer();
	MemBuffer(int size);
	MemBuffer(const MemBuffer& buffer);
	~MemBuffer();

	void alloc(int size);
	void copy(const MemBuffer& buffer);
	void release();

	int getSize() const;
	void *getPtr();
	const void *getConstPtr() const;

	void clear();

	operator void*();
	operator const void*() const;

	MemBuffer& operator =(const MemBuffer& buffer);

 private:
	int m_size;
	void *m_ptr;
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
