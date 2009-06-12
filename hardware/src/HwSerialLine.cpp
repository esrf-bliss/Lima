#include "HwSerialLine.h"
#include "Timestamp.h"

using namespace lima;
using namespace std;


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
	int match=0, n, term_len=term.length(), len=0;
	bool have_timeout=(timeout > 0), have_maxlen=(max_len > 0);
	double tout=timeout;
	string buf;

	buffer = "";
	while( ((!have_timeout) || (tout > 0)) && 
	       ((!have_maxlen) || (len < max_len)) ) {
		n = 1;
		read( buf, n, tout );
		if( 0 == n )
			break;  // ???
		buffer += buf;
		len += n;
		if( 0 == term.compare(match, n, buf) ) {
			match += n;
			if( match == term_len )
				return;
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
void HwSerialLine::writeRead( const std::string& writebuffer, int block_size,
	                      double block_delay, bool no_wait,
	                      std::string& readbuffer, /*int max_len, ???*/
	                      double timeout )

{
	write( writebuffer, block_size, block_delay, no_wait );
	readAvailable( readbuffer, timeout );
// Or:	read( readbuffer, max_len, timeout );
}


/**
 * @brief Write and then immediately Read the serial line until term.
 */
void HwSerialLine::writeReadStr( const std::string& writebuffer, 
	                         int block_size, double block_delay, 
	                         bool no_wait, std::string& readbuffer, 
	                         int max_len, const std::string& term, 
	                         double timeout )

{
	write( writebuffer, block_size, block_delay, no_wait );
	readStr( readbuffer, max_len, term, timeout );
}


/**
 * @brief Read the serial line until there is something to read
 */
void HwSerialLine::readAvailable( std::string& buffer, /*int max_len, ???*/
	                            double timeout )

{
	int max_len;
	getAvail( max_len );
	read( buffer, max_len, timeout );
}


/**
 * @brief Flush the serial RX buffer
 */
void HwSerialLine::flush()
{
	string buf;
	int n=1;  // We don't want to waste memory reading all that is available

	while( n ) {
		n = 1;
		read(buf, n, TMOUT_NO_BLOCK);  // timeout ???
	}
}

