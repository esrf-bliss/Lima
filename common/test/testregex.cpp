#include "RegExUtils.h"

#include <iostream>

using namespace lima;
using namespace std;

typedef RegEx::SingleMatchType     SingleMatchType;
typedef RegEx::FullMatchType       FullMatchType;
typedef RegEx::MatchListType       MatchListType;
typedef RegEx::FullNameMatchType   FullNameMatchType;
typedef RegEx::NameMatchListType   NameMatchListType;

void print_match_list(const string& re, int nb_groups, const string& s, 
		      const MatchListType& match_list)
{
	cout << "re(" << nb_groups << ")=\"" << re << "\"" << endl;
	cout << "s=\"" << s << "\"" << endl;

	MatchListType::const_iterator iti = match_list.begin();
	string::const_iterator b = s.begin();
	for (int i = 0; iti != match_list.end(); ++i, ++iti) {
		const FullMatchType& fmatch = *iti;
		FullMatchType::const_iterator itj = fmatch.begin();
		for (int j = 0; itj != fmatch.end(); ++j, ++itj) {
			const SingleMatchType& smatch = *itj;
			cout << i << "-" << j << ": ";
			if (!smatch.found()) {
				cout << "No match" << endl;
				continue;
			}
			cout << smatch.start - b << "-" << smatch.end - b
			     << ": " << string(smatch) << endl;
		}
	}
	cout << endl;
}

void print_name_match_list(const string& re, int nb_name_groups, 
			   const string& s, 
			   const NameMatchListType& name_match_list)
{
	cout << "re(" << nb_name_groups << ")=\"" << re << "\"" << endl;
	cout << "s=\"" << s << "\"" << endl;

	NameMatchListType::const_iterator iti = name_match_list.begin();
	string::const_iterator b = s.begin();
	for (int i = 0; iti != name_match_list.end(); ++i, ++iti) {
		const FullNameMatchType& fmatch = *iti;
		FullNameMatchType::const_iterator itj = fmatch.begin();
		for (int j = 0; itj != fmatch.end(); ++j, ++itj) {
			const string& name = itj->first;
			const SingleMatchType& smatch = itj->second;
			
			cout << i << "-\"" << name << "\": ";
			if (!smatch.found()) {
				cout << "No match" << endl;
				continue;
			}
			cout << smatch.start - b << "-" << smatch.end - b
			     << ": " << string(smatch) << endl;
		}
	}
	cout << endl;
}

void test_simple_regex(const string& re_str, const string& s)
{
	SimpleRegEx re(re_str);
	MatchListType match_list;
	re.multiSearch(s, match_list);
	print_match_list(re_str, re.getNbGroups(), s, match_list);
}


void test_regex(const string& re_str, const string& s)
{
	RegEx re(re_str);
	MatchListType match_list;
	re.multiSearch(s, match_list);
	print_match_list(re_str, re.getNbGroups(), s, match_list);

	NameMatchListType name_match_list;
	re.multiSearchName(s, name_match_list);
	print_name_match_list(re_str, re.getNbNameGroups(), s, 
			      name_match_list);


}


int main(int argc, char *argv[])
{
	try {
		test_simple_regex("(b)?ab((ab)(\\3c))?", "abababc.bab");
		test_simple_regex("^(b)?ab((ab)(\\3c))?", "abababc.bab");

		test_regex("(b)?ab((ab)(\\3c))?", "abababc.bab");
		test_regex("^(b)?ab((ab)(\\3c))?", "abababc.bab");

		test_regex("(?P<pre>b)?ab((?P<cmd>ab)(?P<resp>\\3c))?", 
			   "abababc.bab");
	} catch (Exception e) {
		cerr << "LIMA Exception: " << e << endl;
	}

	return 0;
}
