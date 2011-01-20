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
