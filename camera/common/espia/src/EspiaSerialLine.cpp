#include "EspiaSerialLine.h"

using namespace lima;
using namespace std;


#define check_null(ptr) if( NULL == ptr ) \
{\
	throw LIMA_HW_EXC(InvalidValue, #ptr " is NULL");\
}


EspiaSerialLine::EspiaSerialLine( char line_term, double timeout ) :
	HwSerialLine(line_term, timeout)
{
}


EspiaSerialLine::~EspiaSerialLine()
{
}


void EspiaSerialLine::serialGetAvail( int &avail )
{
}


void EspiaSerialLine::serialRead( string& buffer, int& len, double timeout )
{
}


void EspiaSerialLine::serialWrite( const string& buffer, int block_size,
	                           double block_delay, bool no_wait )
{
}


void EspiaSerialLine::serialFlush()
{
}
