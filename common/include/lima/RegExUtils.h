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
#ifndef REGEX_H
#define REGEX_H

#include "lima/LimaCompatibility.h"
#include "lima/Debug.h"
#include "lima/Exceptions.h"

#include <string>
#include <vector>
#include <map>
#include <sys/types.h>
#include <regex>

namespace lima
{

class LIMACORE_API SimpleRegEx
{
	DEB_CLASS(DebModCommon, "SimpleRegEx");
 public:
	typedef struct LIMACORE_API SingleMatch {
		typedef std::string::const_iterator StrIt;

		StrIt start;
		StrIt end;

		SingleMatch();
		SingleMatch(const std::ssub_match& m);

		bool found() const;
		operator std::string() const;
		std::string str() const;
	} SingleMatchType;

	typedef std::vector<SingleMatchType> FullMatchType;
	typedef std::vector<FullMatchType>   MatchListType;

	SimpleRegEx() = default;
	SimpleRegEx(const std::string& regex_str);
	SimpleRegEx(const SimpleRegEx& regex);

	SimpleRegEx& operator  =(const std::string& regex_str);
	SimpleRegEx& operator +=(const std::string& regex_str);
	SimpleRegEx& operator  =(const SimpleRegEx& regex);
	SimpleRegEx& operator +=(const SimpleRegEx& regex);

	const std::string& getRegExStr() const;

	bool singleSearch(const std::string& str, FullMatchType& match, 
			  int nb_groups = 0, int match_idx = 0) const;
	void multiSearch(const std::string& str, MatchListType& match_list,
			 int nb_groups = 0, int max_nb_match = 0) const;
	bool match(const std::string& str, FullMatchType& match, 
		   int nb_groups = 0) const;

	int getNbGroups() const;

 private:
	void set(const std::string& regex_str);

	std::string m_str;
	std::regex m_regex;
	int m_nb_groups{0};
};

SimpleRegEx operator +(const SimpleRegEx& re1, const SimpleRegEx& re2);


class LIMACORE_API RegEx {
	DEB_CLASS(DebModCommon, "RegEx");
 public:
	typedef SimpleRegEx::SingleMatchType           SingleMatchType;
	typedef SimpleRegEx::FullMatchType             FullMatchType;
	typedef SimpleRegEx::MatchListType             MatchListType;

	typedef std::map<std::string, SingleMatchType> FullNameMatchType;
	typedef std::vector<FullNameMatchType>         NameMatchListType;

	RegEx() = default;
	RegEx(const std::string& regex_str);
	RegEx(const RegEx& regex);

	RegEx& operator  =(const std::string& regex_str);
	RegEx& operator +=(const std::string& regex_str);
	RegEx& operator  =(const RegEx& regex);
	RegEx& operator +=(const RegEx& regex);

	const std::string& getRegExStr() const;
	const SimpleRegEx& getSimpleRegEx() const;

	bool singleSearch(const std::string& str, 
			  FullMatchType& match, 
			  int nb_groups = 0, int match_idx = 0) const;
	void multiSearch(const std::string& str, 
			 MatchListType& match_list,
			 int nb_groups = 0, int max_nb_match = 0) const;
	bool match(const std::string& str, 
		   FullMatchType& match, 
		   int nb_groups = 0) const;

	bool singleSearchName(const std::string& str, 
			      FullNameMatchType& name_match, 
			      int match_idx = 0) const;
	void multiSearchName(const std::string& str, 
			     NameMatchListType& name_match_list,
			     int max_nb_match = 0) const;
	bool matchName(const std::string& str, 
		       FullNameMatchType& name_match) const;

	int getNbGroups() const;
	int getNbNameGroups() const;

 private:
	typedef std::map<std::string, int> NameMapType;

	void set(const std::string& regex_str);
	void convertNameMatch(const FullMatchType& match, 
			      FullNameMatchType& name_match) const;

	std::string m_str;
	SimpleRegEx m_regex;
	NameMapType m_name_map;
};

RegEx LIMACORE_API operator +(const RegEx& re1, const RegEx& re2);

} // namespace lima

#endif // REGEX_H
