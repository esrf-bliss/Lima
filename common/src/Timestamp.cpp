#include "Timestamp.h"
#include "Exceptions.h"

#include <sys/time.h>
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

