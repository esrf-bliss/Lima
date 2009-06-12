#ifndef HWSERIALLINE_H
#define HWSERIALLINE_H

#include <string>
#include "Exceptions.h"


namespace lima {


#define TMOUT_DEFAULT        -2
#define TMOUT_BLOCK_FOREVER  -1
#define TMOUT_NO_BLOCK        0


class HwSerialLine
{
  public :
	HwSerialLine( char line_term='\r', double timeout=1.0 );
	virtual ~HwSerialLine();

	virtual void getAvail( int &avail ) = 0;

	virtual void read( std::string& buffer, int& len, double timeout ) = 0;

	virtual void write( const std::string& buffer, int block_size = 0,
	                    double block_delay=0, bool no_wait=false ) = 0;

	virtual void readStr( std::string& buffer, int max_len, 
	                      const std::string& term, double timeout );

	virtual void readLine( std::string& buffer, int max_len, double timeout );

	virtual void writeRead( const std::string& writebuffer, int block_size,
	                        double block_delay, bool no_wait,
	                        std::string& readbuffer, /*int max_len, ???*/
	                        double timeout );

	virtual void writeReadStr( const std::string& writebuffer, 
	                           int block_size, double block_delay, 
	                           bool no_wait, std::string& readbuffer, 
	                           int max_len, const std::string& term, 
	                           double timeout );

	virtual void readAvailable( std::string& buffer, /*int max_len, ???*/
	                            double timeout );

	virtual void flush();

  private :
	char   m_line_term;
	double m_timeout;  /* Will be used with TMOUT_DEFAULT */
};


}

#endif /* HWSERIALLINE_H */
