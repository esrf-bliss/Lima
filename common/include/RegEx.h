#ifndef REGEX_H
#define REGEX_H

#include "Exceptions.h"

#include <string>
#include <vector>
#include <sys/types.h>
#include <regex.h>

namespace lima
{

class SimpleRegEx
{
 public:
	typedef struct SingleMatch {
		std::string::const_iterator start;
		std::string::const_iterator end;
	} SingleMatchType;

	typedef std::vector<SingleMatchType> FullMatchType;
	typedef std::vector<FullMatchType>   MatchListType;

	SimpleRegEx();
	SimpleRegEx(const std::string& regex_str);
	SimpleRegEx(const SimpleRegEx& regex);
	~SimpleRegEx();

	SimpleRegEx& operator  =(const SimpleRegEx& regex);
	SimpleRegEx& operator +=(const SimpleRegEx& regex);

	const std::string& getRegExStr() const;

	bool singleSearch(const std::string& str, FullMatchType& match, 
			  int nb_groups = 0, int match_idx = 0);
	void multiSearch(const std::string& str, MatchListType& match_list,
			 int nb_groups = 0, int max_nb_match = 0);
	bool match(const std::string& str, FullMatchType& match, 
		   int nb_groups = 0);

 private:
	void set(const std::string& regex_str);
	void free();

	void throwError(int ret, std::string file, std::string func, int line);

	std::string m_str;
	regex_t m_regex;
};

inline SimpleRegEx operator +(const SimpleRegEx& re1, const SimpleRegEx& re2)
{
	SimpleRegEx re = re1;
	return re += re2;
}


} // namespace lima

#endif // REGEX_H
