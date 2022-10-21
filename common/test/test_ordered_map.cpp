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

#include "lima/OrderedMap.h"
#include <iostream>
#include <string>
#include <cassert>

using namespace std;
using namespace lima;

template <class Key>
class test_ordered_map
{
public:
	test_ordered_map(string name, std::initializer_list<Key> v)
		: vals(v)
	{
		cout << "Testing OrderedMap<" << name << ", string>" << endl;
		exec();
	}

	void exec()
	{
		assert(vals.size() >= 4);

		using InitIt = typename initializer_list<Key>::const_iterator;
		InitIt v = vals.begin();

		using Map = OrderedMap<Key, string>;
		Map m {{*v++, "First"}, {*v++, "Second"}};
		InitIt v3 = v++;
		m[*v3] = "Third";
		m[*v++] = "Fourth";

		using MapCIt = typename Map::const_iterator;
		using MapIt = typename Map::iterator;

		MapCIt cit;
		for (cit = m.begin(); cit != m.end(); ++cit)
			cout << "m[" << cit->first << "]="
			     << cit->second << endl;

		MapIt it1 = m.find(*v3);
		MapIt it2 = m.erase(it1);
		cout << "erase(m[" << *v3 << "])=<" << it2->first << "," 
		     << it2->second << ">" << endl;
	
		for (cit = m.begin(); cit != m.end(); ++cit)
			cout << "m[" << cit->first << "]="
			     << cit->second << endl;
	}

private:
	initializer_list<Key> vals;
};

int main(int /*argc*/, char * /*argv*/ [])
{
	test_ordered_map<int> tint {"int", {2, 1, 0, 4}};
	test_ordered_map<string> tstr {"string", {"2", "1", "0", "4"}};
	return 0;
}
