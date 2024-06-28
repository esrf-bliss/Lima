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
#include "lima/BufferHelper.h"

#include <cmath>
#include <sstream>

using namespace lima;


BufferHelper::Parameters::Parameters()
	: initMem(false),
	  durationPolicy(Ephemeral),
	  sizePolicy(Automatic),
	  reqMemSizePercent(0.0)
{
	DEB_CONSTRUCTOR();
}

int BufferHelper::Parameters::getDefMaxNbBuffers(int size) const
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(size);
	
	int max_nb_buffers = GetDefMaxNbBuffers(FrameDim(Size(size, 1), Bpp8));
	int res = int(std::round(max_nb_buffers * reqMemSizePercent / 100.0));
	
	DEB_RETURN() << DEB_VAR1(res);
	
	return res;	
}

BufferHelper::Parameters BufferHelper::Parameters::fromString(std::string s)
{
	DEB_STATIC_FUNCT();
	DEB_PARAM() << DEB_VAR1(s);
	Parameters param;
	std::istringstream is(s);
	is >> param;
	DEB_RETURN() << DEB_VAR1(param);
	return param;
}

std::string BufferHelper::Parameters::toString() const
{
	DEB_MEMBER_FUNCT();
	std::ostringstream os;
	os << *this;
	return os.str();
}

std::istream& lima::operator>>(std::istream& is,
			       BufferHelper::Parameters& params)
{
	std::string s;
	if (!is.eof())
		is >> s;
	if (s.empty())
		throw LIMA_COM_EXC(InvalidValue, "Invalid BufferHelper params");
	bool enclosed = (s[0] == '<');
	if (enclosed)
		s.erase(0, 1);
	enum { Name, Equal, Value, Sep } stage = Name;
	std::string name;
	bool last_group = false;
	while (true) {
		if (s.empty()) {
			if (!is.eof() && !last_group)
				is >> s;
			if (s.empty() || last_group) {
				if ((stage == Name) || (stage == Sep))
					break;
				throw LIMA_COM_EXC(InvalidValue, "Invalid BufferHelper params");
			}
		}
		if (enclosed && (s.back() == '>')) {
			s.pop_back();
			last_group == true;
			continue;
		}
		if (stage == Name) {
			long pos = s.find('=');
			name = s.substr(0, pos);
			s.erase(0, pos);
			stage = Equal;
		} else if (stage == Equal) {
			if (s[0] != '=')
				throw LIMA_COM_EXC(InvalidValue, "Invalid BufferHelper params");
			s.erase(0, 1);
			stage = Value;
		} else if (stage == Value) {
			long pos = s.find(',');
			std::istringstream val(s.substr(0, pos));
			s.erase(0, pos);
			if (name == "initMem")
				val >> params.initMem;
			else if (name == "durationPolicy")
				val >> params.durationPolicy;
			else if (name == "sizePolicy" )
				val >> params.sizePolicy;
			else if (name == "reqMemSizePercent")
				val >> params.reqMemSizePercent;
			else
				throw LIMA_COM_EXC(InvalidValue, "Invalid BufferHelper params");
			stage = Sep;
		} else {
			if (s[0] != ',')
				throw LIMA_COM_EXC(InvalidValue, "Invalid BufferHelper params");
			s.erase(0, 1);
			stage = Name;
		}
	}
	return is;
}


class BufferHelper::_Impl
{
	DEB_CLASS_NAMESPC(DebModCommon, "BufferHelper::_Impl", "Common");

 public:
	virtual ~_Impl()
	{}

	virtual void setParameters(const Parameters& params)
	{ m_parameters = params; }

	virtual void getParameters(Parameters& params) const
	{ params = m_parameters; }

	virtual void prepareBuffers(int nb_buffers, int size)
	{}

	virtual void releaseBuffers()
	{}

	virtual std::shared_ptr<void> getBuffer(int size) = 0;

 protected:
	Parameters m_parameters;
};


class _BufferHelper_DefaultImpl : public BufferHelper::_Impl
{
	DEB_CLASS_NAMESPC(DebModCommon, "_BufferHelper_DefaultImpl",
			  "Common");

 public:
	std::shared_ptr<void> getBuffer(int size) override
	{
		DEB_MEMBER_FUNCT();
		DEB_PARAM() << DEB_VAR1(size);
		Allocator::Ref allocator = m_parameters.allocator;
		if (!allocator)
			allocator = AllocatorFactory::get().getDefaultAllocator();
		typedef std::shared_ptr<MemBuffer> Ptr;
		Ptr buffer = std::make_shared<MemBuffer>(size, allocator,
							 m_parameters.initMem);
		return std::shared_ptr<void>(buffer, buffer->getPtr());
	}
};


class _BufferHelper_PoolImpl : public BufferHelper::_Impl
{
	DEB_CLASS_NAMESPC(DebModCommon, "_BufferHelper_PoolImpl",
			  "Common");

 public:
	typedef BufferHelper::Parameters Parameters;

	void setParameters(const Parameters& params) override
	{
		DEB_MEMBER_FUNCT();
		DEB_PARAM() << DEB_VAR1(params);
		m_pool.setAllocator(params.allocator);
		m_pool.setInitMem(params.initMem);
		BufferHelper::_Impl::setParameters(params);
	}

	void prepareBuffers(int nb_buffers, int size) override
	{
		DEB_MEMBER_FUNCT();
		DEB_PARAM() << DEB_VAR2(nb_buffers, size);
		int max_nb_buffers = m_parameters.getDefMaxNbBuffers(size);
		if ((m_parameters.sizePolicy == Parameters::Automatic) &&
		    (nb_buffers > 0))
			max_nb_buffers = std::min(nb_buffers, max_nb_buffers);
		DEB_TRACE() << DEB_VAR2(max_nb_buffers, size);
		m_pool.allocBuffers(max_nb_buffers, size);
	}

	void releaseBuffers() override
	{
		DEB_MEMBER_FUNCT();
		m_pool.releaseBuffers();
	}

	std::shared_ptr<void> getBuffer(int size) override
	{
		DEB_MEMBER_FUNCT();
		DEB_PARAM() << DEB_VAR1(size);
		if (size != m_pool.getBufferSize())
			THROW_COM_ERROR(InvalidValue) << "Invalid buffer size";
		return m_pool.getBuffer();
	}

 private:
	BufferPool m_pool;
};


BufferHelper::BufferHelper()
	: m_impl(std::make_shared<_BufferHelper_DefaultImpl>())
{
	DEB_CONSTRUCTOR();
}

BufferHelper::~BufferHelper()
{
	DEB_DESTRUCTOR();
}

void BufferHelper::setParameters(const Parameters& params)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(params);
	if ((params.reqMemSizePercent < 0.0) ||
	    (params.reqMemSizePercent >= 100.0))
		THROW_COM_ERROR(InvalidValue)
			<< "Invalid BufferHelper::Parameters: "
			<< "reqMemSizePercent outside [0,100) range";
	Parameters curr_params;
	getParameters(curr_params);
	if (params.durationPolicy != curr_params.durationPolicy) {
		DEB_TRACE() << DEB_VAR2(curr_params.durationPolicy,
					params.durationPolicy);
		m_impl.reset();
		if (params.durationPolicy == Parameters::Ephemeral)
			m_impl = std::make_shared<_BufferHelper_DefaultImpl>();
		else
			m_impl = std::make_shared<_BufferHelper_PoolImpl>();
	}
	m_impl->setParameters(params);
}

void BufferHelper::getParameters(Parameters& params) const
{
	DEB_MEMBER_FUNCT();
	m_impl->getParameters(params);
}

void BufferHelper::prepareBuffers(int nb_buffers, int size)
{
	DEB_MEMBER_FUNCT();
	m_impl->prepareBuffers(nb_buffers, size);
}

void BufferHelper::releaseBuffers()
{
	DEB_MEMBER_FUNCT();
	m_impl->releaseBuffers();
}

std::shared_ptr<void> BufferHelper::getBuffer(int size)
{
	DEB_MEMBER_FUNCT();
	return m_impl->getBuffer(size);
}
