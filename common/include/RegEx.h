#ifndef REGEX_H
#define REGEX_H

#include "Exceptions.h"

#include <string>
#include <vector>
#include <map>
#include <sys/types.h>
#include <regex.h>

namespace lima
{

class SimpleRegEx
{
 public:
	typedef struct SingleMatch {
		typedef std::string::const_iterator StrIt;

		StrIt start;
		StrIt end;

		SingleMatch();
		SingleMatch(StrIt it, const regmatch_t& rm);
		operator bool() const;
	} SingleMatchType;

	typedef std::vector<SingleMatchType> FullMatchType;
	typedef std::vector<FullMatchType>   MatchListType;

	SimpleRegEx();
	SimpleRegEx(const std::string& regex_str);
	SimpleRegEx(const SimpleRegEx& regex);
	~SimpleRegEx();

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
	void free();

	static int findNbGroups(const std::string& regex_str);

	void throwError(int ret, std::string file, std::string func, 
			int line) const;

	std::string m_str;
	regex_t m_regex;
	int m_nb_groups;
};

SimpleRegEx operator +(const SimpleRegEx& re1, const SimpleRegEx& re2);


class RegEx {
 public:
	typedef SimpleRegEx::SingleMatchType           SingleMatchType;
	typedef SimpleRegEx::FullMatchType             FullMatchType;
	typedef SimpleRegEx::MatchListType             MatchListType;

	typedef std::map<std::string, SingleMatchType> FullNameMatchType;
	typedef std::vector<FullNameMatchType>         NameMatchListType;

	RegEx();
	RegEx(const std::string& regex_str);
	RegEx(const RegEx& regex);
	~RegEx();

	RegEx& operator  =(const std::string& regex_str);
	RegEx& operator +=(const std::string& regex_str);
	RegEx& operator  =(const RegEx& regex);
	RegEx& operator +=(const RegEx& regex);

	const std::string& getRegExStr() const;

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
	void free();
	void convertNameMatch(const FullMatchType& match, 
			      FullNameMatchType& name_match) const;

	std::string m_str;
	SimpleRegEx m_regex;
	NameMapType m_name_map;
};

RegEx operator +(const RegEx& re1, const RegEx& re2);

} // namespace lima

#endif // REGEX_H
