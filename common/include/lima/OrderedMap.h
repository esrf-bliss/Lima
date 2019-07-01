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
#ifndef ORDEREDMAP_H
#define ORDEREDMAP_H

#include <vector>
#include <map>
#include <algorithm>

namespace lima {


template <class Key>
class OrderedMapComp
{
 public:
	using List = std::vector<Key>;
	using ListIt = typename List::iterator;
	using ListCIt = typename List::const_iterator;

	OrderedMapComp(List& l) : m_list(l)
	{}

	OrderedMapComp& operator =(const OrderedMapComp&)
	{
		return *this;
	}

	bool operator()(const Key& a, const Key& b) const
	{
		const ListCIt f = m_list.begin();
		const ListCIt l = m_list.end();
		const ListCIt ia = std::find(f, l, a);
		const ListCIt ib = std::find(f, l, b);
		if ((ia == l) && (ib == l))
			return std::less<Key>()(a, b);
		return ((ia != l) && ((ib == l) || (ia < ib)));
	}

 private:
	List& m_list;
};

template <class Key, class T>
class OrderedMap
{
 public:
	using Comp = OrderedMapComp<Key>;
	using Map = std::map<Key, T, Comp>;

	using value_type = typename Map::value_type;
	using iterator = typename Map::iterator;
	using const_iterator = typename Map::const_iterator;

	OrderedMap() : m_comp(m_list), m_map(m_comp)
	{}

	OrderedMap(const OrderedMap& other) 
		: m_list(other.m_list), m_comp(m_list), m_map(m_comp)
	{
		m_map = other.m_map;
	}

	OrderedMap(std::initializer_list<value_type> init)
		: m_comp(m_list), m_map(m_comp)
	{
		using InitList = std::initializer_list<value_type>;
		typename InitList::const_iterator it, l = init.end();
		for (it = init.begin(); it != l; ++it)
			insert(*it);
	}

	iterator begin()
	{ return m_map.begin(); }
	const_iterator begin() const
	{ return m_map.begin(); }
	iterator end()
	{ return m_map.end(); }
	const_iterator end() const
	{ return m_map.end(); }

	bool empty() const
	{ return m_map.empty(); }

	T& operator [](const Key& key)
	{
		_checkKey(key);
		return m_map[key];
	}

	void clear()
	{
		m_map.clear();
		m_list.clear();
	}

	std::pair<iterator, bool> insert(const value_type& value)
	{
		_checkKey(value.first);
		return m_map.insert(value);
	}

	template<class InputIt>
	void insert(InputIt first, InputIt last)
	{
		while (first != last)
			insert(*first++);
	}

	iterator find(const Key& key)
	{
		return m_map.find(key);
	}

	const_iterator find(const Key& key) const
	{
		return m_map.find(key);
	}

	iterator erase(const_iterator pos)
	{
		Key key = pos->first;
		iterator it = m_map.erase(pos);
		_eraseKey(key);
		return it;
	}

	// iterator erase(const_iterator first, const_iterator last);

	// void swap(OrderedMap& other);

	OrderedMap& operator =(const OrderedMap& other)
	{
		clear();
		m_list = other.m_list;
		m_map = other.m_map;
		return *this;
	}

 private:
	using List = typename Comp::List;
	using ListIt = typename Comp::ListIt;
	using ListCIt = typename Comp::ListCIt;

	void _checkKey(const Key& key)
	{
		const ListCIt f = m_list.begin();
		const ListCIt l = m_list.end();
		if (std::find(f, l, key) == l)
			m_list.push_back(key);
	}

	ListIt _eraseKey(const Key& key)
	{
		const ListCIt f = m_list.begin();
		const ListCIt l = m_list.end();
		const ListCIt it = std::find(f, l, key);
		if (it == l)
			throw std::out_of_range("Error erasing OrderedMap key");
		return m_list.erase(it);
	}

	List m_list;
	Comp m_comp;
	Map m_map;
};


} // namespace lima

#endif // ORDEREDMAP_H
