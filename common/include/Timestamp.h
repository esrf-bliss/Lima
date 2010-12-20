#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include "Compatibility.h"
#include "Exceptions.h"

namespace lima
{

class DLL_EXPORT Timestamp
{
 public:
	Timestamp();
	Timestamp(double ts) : m_ts(ts) {}
	Timestamp(const Timestamp& ts) : m_ts(ts.m_ts) {}

	bool isSet() const;
	
	operator double() const
	{ return m_ts; }

	Timestamp& operator  =(const Timestamp& ts)
	{ m_ts = ts.m_ts; return *this; }

	Timestamp& operator +=(const Timestamp& ts)
	{ validValue() += ts.validValue(); return *this; }

	Timestamp& operator -=(const Timestamp& ts)
	{ validValue() -= ts.validValue(); return *this; }

	Timestamp& operator *=(double factor)
	{ validValue() *= factor; return *this; }

	Timestamp& operator /=(double factor)
	{ validValue() /= factor; return *this; }
	
	static Timestamp now();

 private:
	double& validValue();
	const double& validValue() const;

	double m_ts;
};

inline double& Timestamp::validValue()
{
	if (!isSet())
		throw LIMA_COM_EXC(InvalidValue, "Timestamp not set");
	return m_ts;
}

inline const double& Timestamp::validValue() const
{
	if (!isSet())
		throw LIMA_COM_EXC(InvalidValue, "Timestamp not set");
	return m_ts;
}


inline Timestamp operator +(Timestamp t1, Timestamp t2)
{
	return t1 += t2;
}

inline Timestamp operator -(Timestamp t1, Timestamp t2)
{
	return t1 -= t2;
}

inline Timestamp operator *(Timestamp t, double factor)
{
	return t *= factor;
}

inline Timestamp operator /(Timestamp t, double factor)
{
	return t /= factor;
}

DLL_EXPORT double Sleep(double sec);

}

#endif // TIMESTAMP_H
