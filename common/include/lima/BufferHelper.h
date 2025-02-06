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
#ifndef BUFFERHELPER_H
#define BUFFERHELPER_H

#include "lima/MemUtils.h"
#include "lima/Debug.h"

#include <algorithm>
#include <map>

namespace lima {

/// Helper providing memory buffer functionality
class LIMACORE_API BufferHelper
{
	DEB_CLASS_NAMESPC(DebModCommon, "Buffer_Helper", "Common");

 public:
	struct LIMACORE_API Parameters 
	{
		DEB_CLASS_NAMESPC(DebModCommon, "BufferHelper::Parameters",
				  "Common");

	public:
		enum DurationPolicy {
			Ephemeral,
			Persistent,
		};

		enum PersistentSizePolicy {
			Automatic,
			Fixed,
		};

		Allocator::Ref allocator;
		bool initMem;
		DurationPolicy durationPolicy;
		PersistentSizePolicy sizePolicy;
		double reqMemSizePercent;

		Parameters();

		int getDefMaxNbBuffers(int size) const;

		static Parameters fromString(std::string s);
		std::string toString() const;
	};

	BufferHelper();
	~BufferHelper();

	void setParameters(const Parameters& params);
	void getParameters(Parameters& params) const;

	void prepareBuffers(int nb_buffers, int size);
	void releaseBuffers();

	std::map<int, int> getSize2NbAllocBuffersMap() const;

	std::shared_ptr<void> getBuffer(int size);

	class _Impl;

 private:
	std::shared_ptr<_Impl> m_impl;
};


inline
bool operator ==(const BufferHelper::Parameters& lhs,
		 const BufferHelper::Parameters& rhs)
{
	return ((lhs.allocator == rhs.allocator) &&
		(lhs.initMem == rhs.initMem) &&
		(lhs.durationPolicy == rhs.durationPolicy) &&
		(lhs.sizePolicy == rhs.sizePolicy) &&
		(lhs.reqMemSizePercent == rhs.reqMemSizePercent));
}

inline
bool operator !=(const BufferHelper::Parameters& lhs,
		 const BufferHelper::Parameters& rhs)
{
	return !(lhs == rhs);
}


inline
const char *convert_2_string(BufferHelper::Parameters::DurationPolicy pol)
{
	typedef BufferHelper::Parameters Parameters;

	const char *name;
	switch (pol)
	{
	case Parameters::Ephemeral:
		name = "EPHEMERAL"; break;
	case Parameters::Persistent:
		name = "PERSISTENT"; break;
	default:
		name = "UNKNOWN";
	}
	return name;
}

inline
void convert_from_string(const std::string& name,
                         BufferHelper::Parameters::DurationPolicy& pol)
{
	typedef BufferHelper::Parameters Parameters;

	std::string buffer = name;
	std::transform(buffer.begin(), buffer.end(),buffer.begin(), ::tolower);

	if (buffer == "ephemeral")
		pol = Parameters::Ephemeral;
	else if (buffer == "persistent")
		pol = Parameters::Persistent;
	else {
		std::ostringstream msg;
		msg << "BufferHelper::Parameter::DurationPolicy can't be: "
		    << name;
		throw LIMA_EXC(Common, InvalidValue, msg.str());
	}
}

inline
std::ostream& operator <<(std::ostream& os,
                          BufferHelper::Parameters::DurationPolicy pol)
{
	return os << convert_2_string(pol);
}

inline
std::istream& operator >>(std::istream& is,
                          BufferHelper::Parameters::DurationPolicy& pol)
{
	std::string s;
	is >> s;
	convert_from_string(s, pol);
	return is;
}

inline
const char *convert_2_string(BufferHelper::Parameters::PersistentSizePolicy pol)
{
	typedef BufferHelper::Parameters Parameters;

	const char *name;
	switch (pol)
	{
	case Parameters::Automatic:
		name = "AUTOMATIC"; break;
	case Parameters::Fixed:
		name = "FIXED"; break;
	default:
		name = "UNKNOWN";
	}
	return name;
}

inline
void convert_from_string(const std::string& name,
                         BufferHelper::Parameters::PersistentSizePolicy& pol)
{
	typedef BufferHelper::Parameters Parameters;

	std::string buffer = name;
	std::transform(buffer.begin(), buffer.end(),buffer.begin(), ::tolower);

	if (buffer == "automatic")
		pol = Parameters::Automatic;
	else if (buffer == "fixed")
		pol = Parameters::Fixed;
	else {
		std::ostringstream msg;
		msg << "BufferHelper::Parameter::PersistentSizePolicy can't be: "
		    << name;
		throw LIMA_EXC(Common, InvalidValue, msg.str());
	}
}

inline
std::ostream& operator <<(std::ostream& os,
                          BufferHelper::Parameters::PersistentSizePolicy pol)
{
	return os << convert_2_string(pol);
}

inline
std::istream& operator >>(std::istream& is,
                          BufferHelper::Parameters::PersistentSizePolicy& pol)
{
	std::string s;
	is >> s;
	convert_from_string(s, pol);
	return is;
}

inline
std::ostream& operator<<(std::ostream& os,
                         const BufferHelper::Parameters& params)
{
	const char* duration_pol = convert_2_string(params.durationPolicy);
	const char* size_pol = convert_2_string(params.sizePolicy);

	os << "<"
	   << "durationPolicy=" << duration_pol << ", "
	   << "sizePolicy=" << size_pol << ", "
	   << "initMem=" << params.initMem << ", "
	   << "reqMemSizePercent=" << params.reqMemSizePercent
	   << ">";
	return os;
}

std::istream& operator>>(std::istream& is, BufferHelper::Parameters& params);

} // namespace lima

#endif // BUFFERHELPER_H
