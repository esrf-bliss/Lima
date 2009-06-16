#ifndef ESPIASERIALLINE_H
#define ESPIASERIALLINE_H

#include <string>
#include "Exceptions.h"
#include "HwSerialLine.h"
#include "EspiaDev.h"


namespace lima {


class EspiaSerialLine : public HwSerialLine
{
  public :
	EspiaSerialLine( EspiaDev& edev, const std::string& line_term="\r", 
	                 double timeout=1.0, int block_size=0, 
	                 double block_delay=0 );

	~EspiaSerialLine();

	virtual void write( const std::string& buffer, bool no_wait=false );

	virtual void read( std::string& buffer, int max_len, 
	                   double timeout=TMOUT_DEFAULT );

	virtual void readStr( std::string& buffer, int max_len, 
	                      const std::string& term, 
	                      double timeout=TMOUT_DEFAULT );

	void flush();

	virtual void getNumAvailBytes( int &avail_bytes );

  private :
	EspiaDev& m_dev;
};


}

#endif /* ESPIASERIALLINE_H */
