//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2026
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
%ModuleHeaderCode
#include "lima/MemUtils.h"
using namespace lima;
%End
void GetSystemMem(int& mem_unit, int& system_mem);
	
void GetPageSize(int& page_size);

int GetDefMaxNbBuffers(const FrameDim& frame_dim);

void ClearBuffer(void *ptr, int nb_concat_frames, const FrameDim& frame_dim);


struct Allocator {

        class Ref {
	public:
		std::string toString() const;
%MethodCode
        Py_BEGIN_ALLOW_THREADS
	try {
		Allocator *alloc = sipCpp->get();
		if (!alloc)
			throw LIMA_COM_EXC(InvalidValue,
					   "De-referencing empty Allocator::Ref");
		sipRes = new std::string(alloc->toString());
        } catch (Exception &sipExceptionRef) {
                Py_BLOCK_THREADS
		const std::string& tmpString = sipExceptionRef.getErrMsg();
		const char *detail = tmpString.c_str();
		PyErr_SetString(sipException_Exception, detail);
                return SIP_NULLPTR;
	} catch (...) {
                Py_BLOCK_THREADS
                sipRaiseUnknownException();
                return SIP_NULLPTR;
	}
        Py_END_ALLOW_THREADS
%End

		bool __eq__(const Allocator::Ref& o) const;
%MethodCode
	sipRes = (*sipCpp == *a0);
%End

		int __bool__() const;
%MethodCode
	sipRes = bool(*sipCpp);
%End
        };

	virtual ~Allocator();

private:
	Allocator();
	Allocator(const Allocator& o);
};


class AllocatorFactory {

public:
	// The AllocatorFactory singleton
	static AllocatorFactory& get();

	// Sets the static instance of the default allocator
	void setDefaultAllocator(Allocator::Ref def_alloc);
	// Returns the static instance of the default allocator
	Allocator::Ref getDefaultAllocator();

	Allocator::Ref fromString(std::string s);

private:
	AllocatorFactory();
	AllocatorFactory(const AllocatorFactory& o);
};
