
namespace Espia
{

class SerialLine : HwSerialLine
{

%TypeHeaderCode
#include "EspiaSerialLine.h"
#include "Espia.h"
%End

  public :
	SerialLine( Espia::Dev& edev, const std::string& line_term="\r", 
		    double timeout=1.0, int block_size=0, 
		    double block_delay=0 );
	~SerialLine();

	Espia::Dev& getDev();

	virtual void write( const std::string& buffer, bool no_wait=false );

	virtual void read( std::string& buffer /Out/, int max_len, 
	                   double timeout=HwSerialLine::TimeoutDefault );

	virtual void readStr( std::string& buffer /Out/, int max_len, 
	                      const std::string& term, 
	                      double timeout=HwSerialLine::TimeoutDefault );

	virtual void flush();

	virtual void getNbAvailBytes( int &avail_bytes /Out/ );

};


}; // namespace Espia
