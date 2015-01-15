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
class MutexAttr
{
%TypeHeaderCode
#include "lima/ThreadUtils.h"
using namespace lima;
%End
 public:
	enum Type {
		Normal, Recursive, ErrorCheck,
	};

	MutexAttr(Type type = Recursive);
	MutexAttr(const MutexAttr& mutex_attr);
	~MutexAttr();

	void setType(Type type);
	Type getType() const;
};


class Mutex
{
%TypeHeaderCode
#include "lima/ThreadUtils.h"
using namespace lima;
%End
 public:
	Mutex(MutexAttr mutex_attr = MutexAttr::Recursive);
	~Mutex();

	void lock();
	void unlock();
	bool tryLock();

	MutexAttr getAttr();

	void __enter__();
%MethodCode
		sipCpp->lock();	
%End	

	void __exit__(SIP_PYOBJECT,SIP_PYOBJECT,SIP_PYOBJECT);
%MethodCode
		sipCpp->unlock();
%End
};


class Cond
{
%TypeHeaderCode
#include "lima/ThreadUtils.h"
using namespace lima;
%End
 public:
	Cond();
	~Cond();
	
	void acquire();
	void release();
	Mutex& mutex();

	bool wait(double timeout = -1.);
	void signal();
	void broadcast();

	void __enter__();
%MethodCode
		sipCpp->acquire();	
%End	

	void __exit__(SIP_PYOBJECT,SIP_PYOBJECT,SIP_PYOBJECT);
%MethodCode
		sipCpp->release();
%End
};


