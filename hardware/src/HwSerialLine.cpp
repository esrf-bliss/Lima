#include "HwSerialLine.h"
#include "Timestamp.h"

using namespace lima;
using namespace std;


#define check_null(ptr) if( NULL == ptr ) \
{\
	throw LIMA_HW_EXC(InvalidValue, #ptr " is NULL");\
}


HwSerialLine::HwSerialLine( char line_term, double timeout ) :
	m_line_term(line_term),
	m_timeout(timeout)
{
}


HwSerialLine::~HwSerialLine()
{
}


/**
 * @brief Read the serial line until term.
 */
void HwSerialLine::readStr( string& buffer, int max_len, 
                            const string& term, double timeout )
{
	Timestamp start=Timestamp::now();
	int match=0, n, term_len=term.length();
	bool have_timeout = (timeout > 0);
	double tout=timeout;
	string buf;

	buffer = "";
	while( ((!have_timeout) || (tout > 0)) && (buffer.length() < max_len) ) {
		n = 1;
		read( buf, n, tout );
		if( 0 == n )
			break;  // ???
		buffer += buf;
		if( 0 == term.compare(match, n, buf) ) {
			match += n;
			if( match == term_len ) {
				return;
			}
		} else {
			match = 0;
		}
		if( have_timeout )
			tout = timeout - double(Timestamp::now() - start);
	}
}


/**
 * @brief Read the serial line until line term.
 */
void HwSerialLine::readLine( string& buffer, int max_len, double timeout )
{
	readStr( buffer, max_len, string(1, m_line_term), timeout );
}


/**
 * @brief Write and then immediately Read the serial line until available?
 */
void HwSerialLine::writeRead()
{
}


/**
 * @brief Write and then immediately Read the serial line until term.
 */
void HwSerialLine::writeReadStr()
{
}


/**
 * @brief Flush the serial RX buffer
 */
void HwSerialLine::flush()
{
	// Get available
	// Read until there is something to read and thow it away
}


/**
 * @brief Read the serial line until there is something to read
 */
void HwSerialLine::readAvailable()
{
	// Get available
	// Read until there is something to read
}

