/***************************************************************//**
 * @file   HwSerialLine.h
 * @brief  This file contains the abstract HwSerialLine class
 *
 * @author A.Kirov, A.Homs
 * @date   03/06/2009
 *******************************************************************/

#ifndef HWSERIALLINE_H
#define HWSERIALLINE_H

#include <string>
#include "Debug.h"
#include "Exceptions.h"


namespace lima {


/***************************************************************//**
 * @class  HwSerialLine
 * @brief  An abstract class containing common Serial Line operations
 *
 * Note: The default timeout for most of the methods is TimeoutDefault.
 *       This means that the internal variable m_timeout must be used.
 *       When implementing the pure virtual functions this has to be
 *       taken into account!
 *******************************************************************/
class HwSerialLine
{
	DEB_CLASS(DebModHardwareSerial, "HwSerialLine");

  public :

	enum TimeoutConst {
		TimeoutDefault = -2,  // The internal timeout must be used
		TimeoutBlockForever = -1,
		TimeoutNoBlock = 0
	};

	HwSerialLine( const std::string& line_term="\r", double timeout=1.0,
	              int block_size=0, double block_delay=0 );

	virtual ~HwSerialLine();

	virtual void flush();

	virtual void read( std::string& buffer, int max_len, 
	                   double timeout=TimeoutDefault ) = 0;

	virtual void write( const std::string& buffer, bool no_wait=false ) = 0;

	virtual void readStr( std::string& buffer, int max_len, 
	                      const std::string& term, 
	                      double timeout=TimeoutDefault );

	virtual void readLine( std::string& buffer, int max_len, 
	                       double timeout=TimeoutDefault );

	virtual void writeRead( const std::string& writebuffer,
	                        std::string& readbuffer, int max_len,
	                        bool wr_no_wait=false, 
	                        double rd_timeout=TimeoutDefault );

	virtual void writeReadStr( const std::string& writebuffer, 
	                           std::string& readbuffer, 
	                           int max_len, const std::string& term, 
	                           bool wr_no_wait=false,
	                           double rd_timeout=TimeoutDefault );

	virtual void readAvailable( std::string& buffer, int max_len );

	virtual void getNbAvailBytes( int &avail ) = 0;

	virtual void setLineTerm( const std::string& line_term );
	virtual void getLineTerm( std::string& line_term ) const;

	virtual void setTimeout( double timeout );
	virtual void getTimeout( double& timeout ) const;

	virtual void setBlockSize( int block_size );
	virtual void getBlockSize( int& block_size ) const;

	virtual void setBlockDelay( double block_delay );
	virtual void getBlockDelay( double& block_delay ) const;

	double checkDefTimeout( double timeout );

  private :
	std::string  m_line_term;
	double       m_timeout;  // Used with TimeoutDefault
	int          m_block_size;
	double       m_block_delay;
};


inline double HwSerialLine::checkDefTimeout( double timeout )
{
	return (TimeoutDefault == (int)timeout)? m_timeout : timeout; // XXX
}


}  // namespace lima

#endif /* HWSERIALLINE_H */
