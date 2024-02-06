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

#include <cassert>
#include <cstdlib>

#include <iostream>
#include <utility>

#include "lima/MemUtils.h"

using namespace lima;


void test_empty()
{
	//Default construction
	MemBuffer b;
	assert(b.getSize() == 0);

	//Copy construction
	MemBuffer c(b);
	assert(b.getSize() == 0);

	//Copy assignement
	MemBuffer d = b;

	//Move construction
	MemBuffer e(std::move(b));

	//Move assignement
	MemBuffer f = std::move(b);
}

void test_alloc()
{
	//Default construction
	MemBuffer b(1);
	assert(b.getSize() == 1);

	//Copy construction
	MemBuffer c(b);
	assert(c.getSize() == 1);
	assert(c.getConstPtr() != b.getConstPtr());

	//Copy assignement
	MemBuffer d = b;
	assert(d.getSize() == 1);
	assert(d.getConstPtr() != b.getConstPtr());

	//Move construction (from lvalue)
	LIMA_MAYBE_UNUSED const void *ptr = b.getConstPtr();
	MemBuffer e(std::move(b));
	assert(e.getSize() == 1);
	assert(e.getConstPtr() == ptr);
	assert(b.getConstPtr() == nullptr);
	assert(b.getSize() == 0);

	//Move assignement (from lvalue)
	MemBuffer f = std::move(e);
	assert(f.getSize() == 1);
	assert(f.getConstPtr() == ptr);
	assert(e.getConstPtr() == nullptr);
	assert(e.getSize() == 0);

	//Move construction (from rvalue)
	MemBuffer g(MemBuffer(1));
	assert(g.getSize() == 1);

	//Move assignement (from rvalue)
	MemBuffer h = MemBuffer(1);
	assert(g.getSize() == 1);
}


struct MockAllocator : lima::Allocator
{
	virtual DataPtr alloc(void* &ptr, size_t& size,
			      size_t /*alignment = 16*/) override
	{
		ptr = malloc(size);
		return DataPtr();
	}

	virtual void init(void* ptr, size_t size) override
	{
		memset(ptr, 0, size);
	}

	virtual void release(void* ptr, size_t /*size*/,
			     DataPtr /*alloc_data*/) override
	{
		assert(ptr);
		free(ptr);
	}

	static Allocator::Ref getAllocator()
	{
		static Allocator::Ref instance = std::make_shared<MockAllocator>();
		return instance;
	}
};


void test_custom_allocator()
{
	Allocator::Ref allocator = std::make_shared<MockAllocator>();

	//Default construction
	MemBuffer b(1, allocator);
	assert(b.getSize() == 1);
	assert(b.getAllocator() == allocator);

	//Copy construction
	MemBuffer c(b);
	assert(c.getSize() == 1);
	assert(c.getConstPtr() != b.getConstPtr());
	assert(c.getAllocator() == allocator);

	//Copy assignement
	MemBuffer d = b;
	assert(d.getSize() == 1);
	assert(d.getConstPtr() != b.getConstPtr());
	assert(d.getAllocator() == allocator);

	//Move construction
	LIMA_MAYBE_UNUSED const void *ptr = b.getConstPtr();
	MemBuffer e(std::move(b));
	assert(e.getSize() == 1);
	assert(e.getConstPtr() == ptr);
	assert(b.getConstPtr() == nullptr);
	assert(b.getSize() == 0);
	assert(e.getAllocator() == allocator);

	//Move assignement
	MemBuffer f = std::move(e);
	assert(f.getSize() == 1);
	assert(f.getConstPtr() == ptr);
	assert(e.getConstPtr() == nullptr);
	assert(e.getSize() == 0);
	assert(f.getAllocator() == allocator);
}


int main(int /*argc*/, char * /*argv*/ [])
{
	try {
		test_empty();

		test_alloc();

		test_custom_allocator();

	} catch (Exception e) {
		std::cerr << "LIMA Exception: " << e << std::endl;
	}

	return 0;
}
