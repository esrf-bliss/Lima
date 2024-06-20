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
#include "lima/RegExUtils.h"

using namespace lima;
using namespace std;


SimpleRegEx::SingleMatch::SingleMatch()
{
	start = end = {};
}

SimpleRegEx::SingleMatch::SingleMatch(const std::ssub_match& m)
{
	if (m.length() > 0) {
		start = m.first;
		end   = m.second;
	} else
		start = end = {};
}

bool SimpleRegEx::SingleMatch::found() const
{ 
	return start != end; 
}


SimpleRegEx::SingleMatch::operator string() const
{ 
	return string(start, end); 
}

string SimpleRegEx::SingleMatch::str() const
{ 
	return *this;
}


SimpleRegEx::SimpleRegEx(const string& regex_str)
{
	set(regex_str);
}


SimpleRegEx::SimpleRegEx(const SimpleRegEx& regex)
{
	set(regex.m_str);
}


SimpleRegEx& SimpleRegEx::operator =(const string& regex_str)
{
	set(regex_str);
	return *this;
}

SimpleRegEx& SimpleRegEx::operator +=(const string& regex_str)
{
	set(m_str + regex_str);
	return *this;
}

SimpleRegEx& SimpleRegEx::operator =(const SimpleRegEx& regex)
{
	set(regex.m_str);
	return *this;
}

SimpleRegEx& SimpleRegEx::operator +=(const SimpleRegEx& regex)
{
	set(m_str + regex.m_str);
	return *this;
}

void SimpleRegEx::set(const string& regex_str)
{
	if (regex_str == m_str)
		return;

	m_regex.assign(regex_str, std::regex_constants::extended);

	m_str = regex_str;
	m_nb_groups = m_regex.mark_count();
}

const string& SimpleRegEx::getRegExStr() const
{
	return m_str;
}

int SimpleRegEx::getNbGroups() const
{
	return m_nb_groups;
}

bool SimpleRegEx::singleSearch(const string& str, FullMatchType& match, 
			       int nb_groups, int match_idx) const
{
	if (match_idx < 0)
		throw LIMA_COM_EXC(InvalidValue, "Invalid match index");

	MatchListType match_list;
	multiSearch(str, match_list, nb_groups, match_idx + 1);
	if (int(match_list.size()) <= match_idx)
		return false;

	match = match_list[match_idx];
	return true;
}

void SimpleRegEx::multiSearch(const string& str, MatchListType& match_list,
			      int nb_groups, int max_nb_match) const
{
	if (m_str.empty())
		throw LIMA_COM_EXC(InvalidValue, "Regular expression not set");

	match_list.clear();

	typedef string::const_iterator StrIt;
	StrIt sbeg = str.begin();
	StrIt send = str.end();

	nb_groups = (nb_groups > 0) ? nb_groups : (m_nb_groups + 1);

	std::smatch match;

	StrIt it = sbeg; 
	for (int i = 0; it != send; i++) {
		if ((max_nb_match > 0) && (i == max_nb_match))
			break;

		std::regex_constants::match_flag_type flags{};
		if (it != sbeg)
			flags |= std::regex_constants::match_not_bol;
		if (!std::regex_search(it, send, match, m_regex, flags))
			break;

		FullMatchType full_match;
		for (int g = 0; g < nb_groups; ++g)
			full_match.emplace_back(match[g]);
		match_list.push_back(full_match);

		it = match[0].second;
	}
}

bool SimpleRegEx::match(const string& str, FullMatchType& match, 
			int nb_groups) const
{
	if (!singleSearch(str, match, nb_groups))
		return false;
	
	return (match[0].start == str.begin());
}

SimpleRegEx lima::operator +(const SimpleRegEx& re1, const SimpleRegEx& re2)
{
	SimpleRegEx re = re1;
	return re += re2;
}




RegEx::RegEx(const string& regex_str)
{
	set(regex_str);
}

RegEx::RegEx(const RegEx& regex)
{
	set(regex.m_str);
}

RegEx& RegEx::operator =(const string& regex_str)
{
	set(regex_str);
	return *this;
}

RegEx& RegEx::operator +=(const string& regex_str)
{
	set(m_str + regex_str);
	return *this;
}

RegEx& RegEx::operator =(const RegEx& regex)
{
	set(regex.m_str);
	return *this;
}

RegEx& RegEx::operator +=(const RegEx& regex)
{
	set(m_str + regex.m_str);
	return *this;
}

void RegEx::set(const string& regex_str)
{
	if (regex_str == m_str)
		return;

	static string re = "([^(])?(\\()(\\?P<([A-Za-z][A-Za-z0-9_]*)>)?[^\\(]*";
	static SimpleRegEx grp_start_re(re);

	typedef SimpleRegEx::SingleMatchType SingleMatchType;
	typedef SimpleRegEx::FullMatchType   FullMatchType;
	typedef SimpleRegEx::MatchListType   MatchListType;

	MatchListType grp_list;
	grp_start_re.multiSearch(regex_str, grp_list);

	string::const_iterator sit = regex_str.begin();
	int grp_nb = 0;
	string simple_regex_str;

	for (auto& fm: grp_list) {
		const SingleMatchType& grp_start   = fm[0];
		const SingleMatchType& pre_grp_chr = fm[1];
		const SingleMatchType& grp_open    = fm[2];
		const SingleMatchType& grp_ext     = fm[3];
		const SingleMatchType& grp_name    = fm[4];

		simple_regex_str += string(sit, grp_open.end);
		sit = grp_open.end;

		bool is_grp = (!pre_grp_chr.found() || 
			       (*pre_grp_chr.start != '\\'));
		if (is_grp)
			grp_nb++;
		if (is_grp && grp_ext.found()) {
			m_name_map[grp_name] = grp_nb;
			sit = grp_ext.end;
		}
		simple_regex_str += string(sit, grp_start.end);
		sit = grp_start.end;
	}
	simple_regex_str += string(sit, regex_str.end());
	
	m_str = regex_str;
	m_regex = simple_regex_str;

	if (m_regex.getNbGroups() != grp_nb)
		throw LIMA_COM_EXC(Error, "RegEx nb of groups mismatch");
}

const string& RegEx::getRegExStr() const
{
	return m_str;
}

const SimpleRegEx& RegEx::getSimpleRegEx() const
{
	return m_regex;
}

int RegEx::getNbGroups() const
{
	return m_regex.getNbGroups();
}

int RegEx::getNbNameGroups() const
{
	return m_name_map.size();
}

bool RegEx::singleSearch(const string& str, FullMatchType& match, 
			 int nb_groups, int match_idx) const
{
	return m_regex.singleSearch(str, match, nb_groups, match_idx);
}

bool RegEx::singleSearchName(const string& str, 
			     FullNameMatchType& name_match, 
			     int match_idx) const
{
	name_match.clear();

	FullMatchType full_match;
	if (!singleSearch(str, full_match, 0, match_idx))
		return false;

	convertNameMatch(full_match, name_match);
	return true;
}

void RegEx::convertNameMatch(const FullMatchType& full_match, 
			     FullNameMatchType& name_match) const
{
	for (auto& m: m_name_map) {
		const SingleMatchType& match = full_match[m.second];
		name_match[m.first] = match;
	}
}

void RegEx::multiSearch(const string& str, MatchListType& match_list,
			int nb_groups, int max_nb_match) const
{
	m_regex.multiSearch(str, match_list, nb_groups, max_nb_match);
}

void RegEx::multiSearchName(const string& str, 
			    NameMatchListType& name_match_list,
			    int max_nb_match) const
{
	name_match_list.clear();

	MatchListType match_list;
	multiSearch(str, match_list, 0, max_nb_match);

	for (auto& m: match_list) {
		FullNameMatchType name_match;
		convertNameMatch(m, name_match);
		name_match_list.push_back(name_match);
	}
}

bool RegEx::match(const string& str, FullMatchType& match, 
		  int nb_groups) const
{
	return m_regex.match(str, match, nb_groups);
}


bool RegEx::matchName(const string& str, 
		      FullNameMatchType& name_match) const
{
	name_match.clear();

	FullMatchType full_match;
	if (!match(str, full_match, 0))
		return false;

	convertNameMatch(full_match, name_match);
	return true;
}

RegEx lima::operator +(const RegEx& re1, const RegEx& re2)
{
	RegEx re = re1;
	return re += re2;
}


