class Timestamp
{
%TypeHeaderCode
#include <Timestamp.h>
using namespace lima;
%End
 public:
	Timestamp();
	Timestamp(double ts);
	Timestamp(const Timestamp& ts);

	bool isSet() const;

	operator double() const;

	Timestamp& operator +=(const Timestamp& ts);

	Timestamp& operator -=(const Timestamp& ts);

	Timestamp& operator *=(double factor);

	Timestamp& operator /=(double factor);
	
	static Timestamp now();
};
