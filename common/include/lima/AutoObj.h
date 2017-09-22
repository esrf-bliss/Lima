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
#ifndef AUTOOBJ_H
#define AUTOOBJ_H

#include <stdio.h>	// For NULL

#include "lima/LimaCompatibility.h"

namespace lima
{

/********************************************************************
 * AutoCounter
 ********************************************************************/

class AutoCounter
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
class AutoLock
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
			: m(mutex), l(0), ul_at_end(true)
		{
			switch (state) { 
			case Locked:     lock();    break;
			case TryLocked:  tryLock(); break;
			case PrevLocked: l = 1;     break;
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
			if (!l++)
				m.lock(); 
		}

		void unlock()
		{ 
			if (!--l)
				m.unlock(); 
		}

		bool tryLock()
		{ 
			if (!l)
				l = m.tryLock(); 
			else
				l++;
			return !!l;
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
		char l;
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


template <class M>
class AutoUnlock
{
 public:
	AutoUnlock(const AutoUnlock& o)
		: l(o.l)
	{ l.unlock(); }
	 
	AutoUnlock(AutoLock<M>& p)
		: l(p)
	{ l.unlock(); }
	 
	~AutoUnlock()
	{ l.lock(); }
	 
 private:
	 AutoLock<M>& l;
 };


/********************************************************************
 * AutoPtr
 ********************************************************************/

template <class T, bool array=false>
class AutoPtr
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
