//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2011
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################
/***************************************************************//**
 * @file   HwSerialLine.cpp
 * @brief  This file contains HwSerialLine class implementation
 *
 * @author A.Kirov, A.Homs
 * @date   03/06/2009
 *******************************************************************/

#include "lima/HwSerialLine.h"
#include "lima/Timestamp.h"

using namespace lima;
using namespace std;


/***************************************************************//**
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
	DEB_CONSTRUCTOR();
}


HwSerialLine::~HwSerialLine()
{
	DEB_DESTRUCTOR();
}


/***************************************************************//**
 * @brief Read the serial line until term.
 *******************************************************************/
void HwSerialLine::readStr( string& buffer, int max_len, 
                            const string& term, double timeout )
{
	DEB_MEMBER_FUNCT();

	Timestamp start=Timestamp::now();
	int match=0, n, term_len=int(term.length()), len=0;
	bool have_timeout=(timeout > 0), have_maxlen=(max_len > 0);
	double tout=checkDefTimeout(timeout);
	string buf;

	buffer = "";
	while( ((!have_timeout) || (tout > 0)) && 
	       ((!have_maxlen) || (len < max_len)) ) {
		n = 1;
		read( buf, n, tout );
		n = int(buf.length());
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


/***************************************************************//**
 * @brief Read the serial line until line term.
 *******************************************************************/
void HwSerialLine::readLine( string& buffer, int max_len, double timeout )
{
	DEB_MEMBER_FUNCT();
	readStr( buffer, max_len, m_line_term, timeout );
}


/***************************************************************//**
 * @brief Write and then immediately Read the serial line.
 *******************************************************************/
void HwSerialLine::writeRead( const string& writebuffer,
	                      string& readbuffer, int max_len,
	                      bool wr_no_wait, double rd_timeout )

{
	DEB_MEMBER_FUNCT();
	write( writebuffer, wr_no_wait );
	read( readbuffer, max_len, rd_timeout );
}


/***************************************************************//**
 * @brief Write and then immediately Read the serial line until term.
 *******************************************************************/
void HwSerialLine::writeReadStr( const string& writebuffer,
                                 string& readbuffer, int max_len, 
                                 const string& term, 
                                 bool wr_no_wait, double rd_timeout )

{
	DEB_MEMBER_FUNCT();
	write( writebuffer, wr_no_wait );
	readStr( readbuffer, max_len, term, rd_timeout );
}


/***************************************************************//**
 * @brief Read without blocking from the serial line.
 *******************************************************************/
void HwSerialLine::readAvailable( string& buffer, int max_len )
{
	DEB_MEMBER_FUNCT();
	read( buffer, max_len, TimeoutNoBlock );
}


/***************************************************************//**
 * @brief Flush the serial RX buffer
 *******************************************************************/
void HwSerialLine::flush()
{
	DEB_MEMBER_FUNCT();
	string buf;
	int len;

	getNbAvailBytes(len);
	read( buf, len, TimeoutNoBlock );
}


void HwSerialLine::setLineTerm( const string& line_term )
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(line_term);
	m_line_term = line_term;
}

void HwSerialLine::getLineTerm( string& line_term ) const
{
	DEB_MEMBER_FUNCT();
	line_term = m_line_term;
	DEB_RETURN() << DEB_VAR1(line_term);
}


void HwSerialLine::setTimeout( double timeout )
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(timeout);
	m_timeout = timeout;
}

void HwSerialLine::getTimeout( double& timeout ) const
{
	DEB_MEMBER_FUNCT();
	timeout = m_timeout;
	DEB_RETURN() << DEB_VAR1(timeout);
}


void HwSerialLine::setBlockSize( int block_size )
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(block_size);
	m_block_size = block_size;
}

void HwSerialLine::getBlockSize( int& block_size ) const
{
	DEB_MEMBER_FUNCT();
	block_size = m_block_size;
	DEB_RETURN() << DEB_VAR1(block_size);
}


void HwSerialLine::setBlockDelay( double block_delay )
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(block_delay);
	m_block_delay = block_delay;
}

void HwSerialLine::getBlockDelay( double& block_delay ) const
{
	DEB_MEMBER_FUNCT();
	block_delay = m_block_delay;
	DEB_RETURN() << DEB_VAR1(block_delay);
}
