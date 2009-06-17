#include "RegEx.h"

#include <iostream>

using namespace lima;
using namespace std;

void test_simple_regex(const string& re_str, const string& s)
{
	SimpleRegEx re(re_str);

	cout << "re=\"" << re.getRegExStr() << "\"" << endl;
	cout << "s=\"" << s << "\"" << endl;

	typedef SimpleRegEx::SingleMatchType SingleMatchType;
	typedef SimpleRegEx::FullMatchType   FullMatchType;
	typedef SimpleRegEx::MatchListType   MatchListType;

	MatchListType match_list;
	re.multiSearch(s, match_list);

	MatchListType::const_iterator iti = match_list.begin();
	string::const_iterator b = s.begin();
	for (int i = 0; iti != match_list.end(); ++i, ++iti) {
		const FullMatchType& fmatch = *iti;
		FullMatchType::const_iterator itj = fmatch.begin();
		for (int j = 0; itj != fmatch.end(); ++j, ++itj) {
			const SingleMatchType& smatch = *itj;
			cout << i << "-" << j << ": " 
			     << smatch.start - b << "-" << smatch.end - b
			     << ": " << string(smatch.start, smatch.end) 
			     << endl;
		}
	}
	cout << endl;
}


int main(int argc, char *argv[])
{
	try {
		test_simple_regex("b?ab((ab)(\\2c))?", "abababc.bab");
		test_simple_regex("^b?ab((ab)(\\2c))?", "abababc.bab");
	} catch (Exception e) {
		cerr << "LIMA Exception: " << e << endl;
	}

	return 0;
}
