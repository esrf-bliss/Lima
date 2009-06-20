#ifndef ESPIASERIALLINE_H
#define ESPIASERIALLINE_H

#include <string>
#include "Exceptions.h"
#include "HwSerialLine.h"
#include "EspiaDev.h"


namespace lima 
{

namespace Espia
{

class SerialLine : public HwSerialLine
{
  public :
	SerialLine( Dev& edev, const std::string& line_term="\r", 
		    double timeout=1.0, int block_size=0, 
		    double block_delay=0 );

	~SerialLine();

	virtual void write( const std::string& buffer, bool no_wait=false );

	virtual void read( std::string& buffer, int max_len, 
	                   double timeout=TimeoutDefault );

	virtual void readStr( std::string& buffer, int max_len, 
	                      const std::string& term, 
	                      double timeout=TimeoutDefault );

	virtual void flush();

	virtual void getNumAvailBytes( int &avail_bytes );

  private :
	Dev& m_dev;
};


} // namespace Espia

} // namespace lima

#endif /* ESPIASERIALLINE_H */
