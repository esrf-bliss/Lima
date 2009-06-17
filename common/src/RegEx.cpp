#include "RegEx.h"

using namespace lima;
using namespace std;

#define CHECK_CALL(ret)							\
	{								\
		int aux_ret = (ret);					\
		if (aux_ret != 0)					\
			throwError(aux_ret, __FILE__, __FUNCTION__,	\
				   __LINE__);				\
	}

SimpleRegEx::SimpleRegEx()
{
	set("");
}

SimpleRegEx::SimpleRegEx(const string& regex_str)
{
	set(regex_str);
}


SimpleRegEx::SimpleRegEx(const SimpleRegEx& regex)
{
	set(regex.m_str);
}


SimpleRegEx::~SimpleRegEx()
{
	free();
}

SimpleRegEx& SimpleRegEx::operator =(const SimpleRegEx& regex)
{
	set(regex.m_str);
	return *this;
}

SimpleRegEx& SimpleRegEx::operator +=(const SimpleRegEx& regex)
{
	string regex_str = m_str + regex.m_str;
	set(regex_str);
	return *this;
}

void SimpleRegEx::set(const string& regex_str)
{
	if (regex_str == m_str)
		return;

	free();

	if (!regex_str.empty())
		CHECK_CALL(regcomp(&m_regex, regex_str.c_str(), REG_EXTENDED));
	m_str = regex_str;
}

void SimpleRegEx::free()
{
	if (m_str.empty())
		return;

	regfree(&m_regex);
	m_str.clear();
}


const string& SimpleRegEx::getRegExStr() const
{
	return m_str;
}

bool SimpleRegEx::singleSearch(const string& str, FullMatchType& match, 
			       int nb_groups, int match_idx)
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
			      int nb_groups, int max_nb_match)
{
	if (m_str.empty())
		throw LIMA_COM_EXC(InvalidValue, "Regular expression not set");

	match_list.clear();

	typedef string::const_iterator StrIt;
	StrIt sbegin = str.begin();
	StrIt send   = str.end();

	if (nb_groups == 0)
		nb_groups = 255;
	regmatch_t reg_match[nb_groups];
	regmatch_t *mend = reg_match + nb_groups;

	StrIt it = sbegin; 
	for (int i = 0; it != send; i++) {
		if ((max_nb_match > 0) && (i == max_nb_match))
			break;

		string aux(it, send);
		int flags = (it != sbegin) ? REG_NOTBOL : 0;
		int ret = regexec(&m_regex, aux.c_str(), nb_groups, reg_match, 
				  flags);
		if (ret == REG_NOMATCH)
			break;
		CHECK_CALL(ret);

		StrIt match_end = send;
		FullMatchType full_match;
		for (regmatch_t *m = reg_match; m != mend; ++m) {
			if (m->rm_so == -1)
				break;

			SingleMatchType match;
			match.start = it + m->rm_so;
			match.end   = it + m->rm_eo;
			full_match.push_back(match);

			match_end = match.end;
		}		
		match_list.push_back(full_match);

		it = match_end;
	}
}

bool SimpleRegEx::match(const string& str, FullMatchType& match, 
			int nb_groups)
{
	if (!singleSearch(str, match, nb_groups))
		return false;
	
	return (match[0].start == str.begin());
}

void SimpleRegEx::throwError(int ret, string file, string func, int line)
{
	size_t len = regerror(ret, &m_regex, NULL, 0);
	string regerr(len, '\0');
	char *data = (char *) regerr.data();
	regerror(ret, &m_regex, data, regerr.size());
	string err_desc = string("regex: ") + regerr;
	throw Exception(Common, Error, err_desc, file, func, line);
}



