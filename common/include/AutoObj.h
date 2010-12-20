#ifndef AUTOOBJ_H
#define AUTOOBJ_H

#include <stdio.h>	// For NULL

#include "Compatibility.h"

namespace lima
{

/********************************************************************
 * AutoCounter
 ********************************************************************/

class DLL_EXPORT AutoCounter
{
public:
	AutoCounter() : r(1)
	{}

	void get()
	{ r++; }

	bool put()
	{ return --r == 0; }

	int count() const
	{ return r; }

private:
	int r;
};


/********************************************************************
 * AutoLock
 ********************************************************************/

template <class M>
class DLL_EXPORT AutoLock
{
public:
	enum { UnLocked, Locked, TryLocked, PrevLocked };

	AutoLock(M& mutex, int state=Locked) 
	{ d = new AutoLockData(mutex, state); }

	AutoLock(const AutoLock& o) 
	{ d = o.getData(); }

	~AutoLock()
	{ putData(); }
	
	void lock()
	{ d->lock(); }

	void unlock()
	{ d->unlock(); }

	bool tryLock()
	{ return d->tryLock(); }

	M& mutex() const
	{ return d->mutex(); }

	bool locked() const
	{ return d->locked(); }

	void leaveLocked()
	{ d->leaveLocked(); }

	AutoLock& operator =(const AutoLock& o)
	{ 
		AutoLockData *od = o.getData(); // protects against "a = a"
		putData(); 
		d = od;
		return *this;
	}

private:
	class AutoLockData
	{
	public:
		AutoLockData(M& mutex, int state=Locked) 
			: m(mutex), l(false), ul_at_end(true)
		{
			switch (state) { 
			case Locked:     lock();    break;
			case TryLocked:  tryLock(); break;
			case PrevLocked: l = true;  break;
			default: break;
			}
		}

		~AutoLockData()
		{ 
			if (l && ul_at_end) 
				unlock(); 
		}

		AutoLockData *get()
		{ 
			c.get(); 
			return this; 
		}

		bool put()
		{ 
			return c.put(); 
		}

		void lock()
		{ 
			m.lock(); 
			l = true; 
		}

		void unlock()
		{ 
			m.unlock(); 
			l = false; 
		}

		bool tryLock()
		{ 
			l = m.tryLock(); 
			return l;
		}

		void leaveLocked()
		{
			ul_at_end = false;
		}

		M& mutex() const
		{ return m; }

		bool locked() const
		{ return l; }

	private:
		AutoCounter c;
		M& m;
		bool l;
		bool ul_at_end;
	};

	AutoLockData *getData() const
	{ return d->get(); }

	void putData()
	{ 
		if (d->put())
			delete d; 
		d = NULL;
	}

	AutoLockData *d;
};


/********************************************************************
 * AutoPtr
 ********************************************************************/

template <class T, bool array=false>
class DLL_EXPORT AutoPtr
{
public:
	AutoPtr() 
	{ d = new AutoPtrData(); }

	AutoPtr(T *ptr) 
	{ d = new AutoPtrData(ptr); }

	AutoPtr(const AutoPtr& o) 
	{ d = o.getData(); }

	~AutoPtr()
	{ putData(); }

	T *getPtr() const
	{ return d->getPtr(); }

	void setPtr(T *ptr)
	{ 
		putData(); 
		d = new AutoPtrData(ptr); 
	}

	operator T*() const
	{ return getPtr(); }

	T *operator ->() const
	{ return getPtr(); }

	T& operator[](int i)
	{ return getPtr()[i]; }

	const T& operator[](int i) const
	{ return getPtr()[i]; }


	AutoPtr& operator =(T *ptr)
	{ 
		setPtr(ptr); 
		return *this; 
	}

	AutoPtr& operator =(AutoPtr& o)
	{
		AutoPtrData *od = o.getData(); // protects against "a = a"
		putData(); 
		d = od;
		return *this;
	}

	void free()
	{ d->free(); }

	T *forget()
	{ 
		T *ptr = d->forget(); 
		putData();
		d = new AutoPtrData(); 
		return ptr;
	}

private:

	class AutoPtrData
	{
	public:
		AutoPtrData(T *ptr = NULL) : p(ptr)
		{}

		~AutoPtrData()
		{ 
			free();
		}

		AutoPtrData *get()
		{ 
			c.get(); 
			return this; 
		}

		bool put()
		{ 
			return c.put();
		}

		void free()
		{ 
			if (!p)
				return;
			if (array) 
				delete [] p; 
			else 
				delete p; 
			p = NULL; 
		}

		T *forget()
		{ 
			T *ptr = getPtr();
			if (c.count() == 1) 
				p = NULL; 
			return ptr;
		}

		T *getPtr() const
		{ return p; }

		void setPtr(T *ptr)
		{ p = ptr; }

	private:
		T *p;
		AutoCounter c;
	};

	AutoPtrData *getData() const
	{ return d->get(); }

	void putData()
	{ 
		if (d->put())
			delete d; 
		d = NULL;
	}

	AutoPtrData *d;
};


} // namespace lima

#endif // AUTOOBJ_H
