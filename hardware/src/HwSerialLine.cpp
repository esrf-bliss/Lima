/*******************************************************************
 * @file   HwSerialLine.cpp
 * @brief  This file contains HwSerialLine class implementation
 *
 * @author A.Kirov, A.Homs
 * @date   03/06/2009
 *******************************************************************/

#include "HwSerialLine.h"
#include "Timestamp.h"

using namespace lima;
using namespace std;


/*******************************************************************
 * @brief      HwSerialLine class constructor
 *
 * @param[in]  line_term    Line terminator string. Default is "\r"
 * @param[in]  timeout      Input timeout. Default is 1.0 s
 * @param[in]  block_size   Write block size. Default is 0
 * @param[in]  block_delay  Block delay. Default is 0
 *******************************************************************/
HwSerialLine::HwSerialLine( const string& line_term, double timeout,
                            int block_size, double block_delay ) :
	m_line_term(line_term),
	m_timeout(timeout),
	m_block_size(block_size),
	m_block_delay(block_delay)
{
}


HwSerialLine::~HwSerialLine()
{
}


/*******************************************************************
 * @brief Read the serial line until term.
 *******************************************************************/
void HwSerialLine::readStr( string& buffer, int max_len, 
                            const string& term, double timeout )
{
	Timestamp start=Timestamp::now();
	int match=0, n, term_len=term.length(), len=0;
	bool have_timeout=(timeout > 0), have_maxlen=(max_len > 0);
	double tout=checkDefTimeout(timeout);
	string buf;

	buffer = "";
	while( ((!have_timeout) || (tout > 0)) && 
	       ((!have_maxlen) || (len < max_len)) ) {
		n = 1;
		read( buf, n, tout );
		n = buf.length();
		if( 0 == n )
			return;  // Or throw an exception?
		buffer += buf;
		len += n;
		if( 0 == term.compare(match, n, buf) ) {
			match += n;
			if( match == term_len )
				return;  // Terminator found
		} else {
			match = 0;
		}
		if( have_timeout )
			tout = timeout - double(Timestamp::now() - start);
	}
}


/*******************************************************************
 * @brief Read the serial line until line term.
 *******************************************************************/
void HwSerialLine::readLine( string& buffer, int max_len, double timeout )
{
	readStr( buffer, max_len, m_line_term, timeout );
}


/*******************************************************************
 * @brief Write and then immediately Read the serial line.
 *******************************************************************/
void HwSerialLine::writeRead( const string& writebuffer,
	                      string& readbuffer, int max_len,
	                      bool wr_no_wait, double rd_timeout )

{
	write( writebuffer, wr_no_wait );
	read( readbuffer, max_len, rd_timeout );
}


/*******************************************************************
 * @brief Write and then immediately Read the serial line until term.
 *******************************************************************/
void HwSerialLine::writeReadStr( const string& writebuffer,
                                 string& readbuffer, int max_len, 
                                 const string& term, 
                                 bool wr_no_wait, double rd_timeout )

{
	write( writebuffer, wr_no_wait );
	readStr( readbuffer, max_len, term, rd_timeout );
}


/*******************************************************************
 * @brief Read without blocking from the serial line.
 *******************************************************************/
void HwSerialLine::readAvailable( string& buffer, int max_len )
{
	read( buffer, max_len, TimeoutNoBlock );
}


/*******************************************************************
 * @brief Flush the serial RX buffer
 *******************************************************************/
void HwSerialLine::flush()
{
	string buf;
	int len;

	getNbAvailBytes(len);
	read( buf, len, TimeoutNoBlock );
}


void HwSerialLine::setLineTerm( const string& line_term )
{
	m_line_term = line_term;
}

void HwSerialLine::getLineTerm( string& line_term ) const
{
	line_term = m_line_term;
}


void HwSerialLine::setTimeout( double timeout )
{
	m_timeout = timeout;
}

void HwSerialLine::getTimeout( double& timeout ) const
{
	timeout = m_timeout;
}


void HwSerialLine::setBlockSize( int block_size )
{
	m_block_size = block_size;
}

void HwSerialLine::getBlockSize( int& block_size ) const
{
	block_size = m_block_size;
}


void HwSerialLine::setBlockDelay( double block_delay )
{
	m_block_delay = block_delay;
}

void HwSerialLine::getBlockDelay( double& block_delay ) const
{
	block_delay = m_block_delay;
}
