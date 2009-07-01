class HwSerialLine
{
%TypeHeaderCode
#include <HwSerialLine.h>
using namespace lima;
%End
  public :

	enum TimeoutConst {
		TimeoutDefault = -2,
		TimeoutBlockForever = -1,
		TimeoutNoBlock = 0
	};

	HwSerialLine( const std::string& line_term="\r", double timeout=1.0,
	              int block_size=0, double block_delay=0 );

	virtual ~HwSerialLine();

	virtual void flush();

	virtual void read( std::string& buffer /Out/, int max_len, 
	                   double timeout=TimeoutDefault ) = 0;

	virtual void write( const std::string& buffer, bool no_wait=false ) = 0;

	virtual void readStr( std::string& buffer /Out/, int max_len, 
	                      const std::string& term, 
	                      double timeout=TimeoutDefault );

	virtual void readLine( std::string& buffer /Out/, int max_len, 
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

	virtual void readAvailable( std::string& buffer /Out/, int max_len );

	virtual void getNbAvailBytes( int &avail /Out/) = 0;

	virtual void setLineTerm( const std::string& line_term );
	virtual void getLineTerm( std::string& line_term /Out/) const;

	virtual void setTimeout( double timeout );
	virtual void getTimeout( double& timeout /Out/) const;

	virtual void setBlockSize( int block_size );
	virtual void getBlockSize( int& block_size /Out/) const;

	virtual void setBlockDelay( double block_delay );
	virtual void getBlockDelay( double& block_delay /Out/) const;

	double checkDefTimeout( double timeout );

};
