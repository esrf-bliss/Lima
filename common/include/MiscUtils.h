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
#ifndef MISCUTILS_H
#define MISCUTILS_H

#include <map>
#include <algorithm>

#include "LimaCompatibility.h"

namespace lima
{

#define C_LIST_SIZE(list)	(sizeof(list) / sizeof((list)[0]))
#define C_LIST_END(list)	((list) + C_LIST_SIZE(list))
#define C_LIST_ITERS(list)	list, C_LIST_END(list)

// Map search helpers

template <class V>
class LIMACORE_API SecondIs
{
 public:
	SecondIs(V v) : m_val(v) 
	{}

	template <class K>
	bool operator () (const std::pair<K, V>& p) 
	{ return (p.second == m_val); }

 private:
	V m_val;
};

template <class M, class V>
typename M::const_iterator FindMapValue(const M& m, V v)
{
	return find_if(m.begin(), m.end(), SecondIs<V>(v));
}

} // namespace lima


#endif // MISCUTILS_H
