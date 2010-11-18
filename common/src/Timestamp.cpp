#include "Timestamp.h"
#include "Exceptions.h"

#ifdef __unix
#include <sys/time.h>
#else
#include <time_compat.h>
#endif
#include <unistd.h>
#include <iostream>

using namespace lima;

const Timestamp Timestamp::Unset = -12345678901234567890.1234567890;


Timestamp Timestamp::now()
{
	struct timeval t;
	if (gettimeofday(&t, NULL) < 0)
		throw LIMA_HW_EXC(Error, "Error calling gettimeofday");

	return t.tv_sec + t.tv_usec * 1e-6;
}

double lima::Sleep(double sec)
{
	Timestamp t0 = Timestamp::now();
	if (sec > 0)
		usleep((unsigned long) (sec * 1e6 + 0.1));
	return Timestamp::now() - t0;
}
