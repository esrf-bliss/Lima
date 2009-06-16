#ifndef HWSERIALLINE_H
#define HWSERIALLINE_H

#include <string>
#include "Exceptions.h"


namespace lima {


class HwSerialLine
{
  public :

	enum TimeoutConst {
		TMOUT_DEFAULT = -2,
		TMOUT_BLOCK_FOREVER = -1,
		TMOUT_NO_BLOCK = 0
	};

	HwSerialLine( const std::string& line_term="\r", double timeout=1.0,
	              int block_size=0, double block_delay=0 );

	virtual ~HwSerialLine();

	virtual void flush();

	virtual void read( std::string& buffer, int max_len, 
	                   double timeout=TMOUT_DEFAULT ) = 0;

	virtual void write( const std::string& buffer, bool no_wait=false ) = 0;

	virtual void readStr( std::string& buffer, int max_len, 
	                      const std::string& term, 
	                      double timeout=TMOUT_DEFAULT );

	virtual void readLine( std::string& buffer, int max_len, 
	                       double timeout=TMOUT_DEFAULT );

	virtual void writeRead( const std::string& writebuffer,
	                        std::string& readbuffer, int max_len,
	                        bool wr_no_wait=false, 
	                        double rd_timeout=TMOUT_DEFAULT );

	virtual void writeReadStr( const std::string& writebuffer, 
	                           std::string& readbuffer, 
	                           int max_len, const std::string& term, 
	                           bool wr_no_wait=false,
	                           double rd_timeout=TMOUT_DEFAULT );

	virtual void readAvailable( std::string& buffer, int max_len );

	virtual void getNumAvailBytes( int &avail ) = 0;

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
	double       m_timeout;  /* Will be used with TMOUT_DEFAULT */
	int          m_block_size;
	double       m_block_delay;
};


inline double HwSerialLine::checkDefTimeout( double timeout )
{
	return (TMOUT_DEFAULT == (int)timeout)? m_timeout : timeout; // XXX
}


}  // namespace lima

#endif /* HWSERIALLINE_H */
