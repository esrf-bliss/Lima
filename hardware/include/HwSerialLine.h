#ifndef HWSERIALLINE_H
#define HWSERIALLINE_H

#include <string>
#include "Exceptions.h"


namespace lima {


class HwSerialLine
{
  public :
	HwSerialLine( char line_term='\r', double timeout=1.0 );
	virtual ~HwSerialLine();

	virtual void getAvail( int &avail ) = 0;
	virtual void read( std::string& buffer, int& len, double timeout ) = 0;
	virtual void write( const std::string& buffer, int block_size = 0,
	                    double block_delay = 0, bool no_wait = false ) = 0;

	virtual void readStr( std::string& buffer, int max_len, 
	                            const std::string& term, double timeout );
	virtual void readLine( std::string& buffer, int max_len, 
	                             double timeout );

	virtual void writeRead(  );
	virtual void writeReadStr();

	virtual void flush();
	virtual void readAvailable();

  private :
	char   m_line_term;
	double m_timeout;
};


}

#endif /* HWSERIALLINE_H */
