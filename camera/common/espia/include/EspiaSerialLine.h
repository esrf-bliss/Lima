#ifndef ESPIASERIALLINE_H
#define ESPIASERIALLINE_H

#include <string>
#include "Exceptions.h"
#include "HwSerialLine.h"


namespace lima {


class EspiaSerialLine : public HwSerialLine
{
  public :
	EspiaSerialLine( char line_term='\r', double timeout=1.0 );
	~EspiaSerialLine();

	void serialGetAvail( int &avail );
	void serialRead( std::string& buffer, int& len, double timeout );
	void serialWrite( const std::string& buffer, int block_size = 0,
	                  double block_delay = 0, bool no_wait = false );

	void serialFlush();

  private :

};


}

#endif /* ESPIASERIALLINE_H */
