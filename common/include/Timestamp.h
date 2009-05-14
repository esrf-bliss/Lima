#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include "Exceptions.h"

namespace lima
{

class Timestamp
{
 public:
	Timestamp() : m_ts(Unset) {}
	Timestamp(double ts) : m_ts(ts) {}
	Timestamp(const Timestamp& ts) : m_ts(ts.m_ts) {}

	bool isSet() const
	{ return *this != Unset; }

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
	static const Timestamp Unset;

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


}

#endif // TIMESTAMP_H
