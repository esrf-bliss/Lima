//----------------------------------------------------------------------------
// YAT LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2010  The Tango Community
//
// Part of the code comes from the ACE Framework (i386 asm bytes swaping code)
// see http://www.cs.wustl.edu/~schmidt/ACE.html for more about ACE
//
// The thread native implementation has been initially inspired by omniThread
// - the threading support library that comes with omniORB. 
// see http://omniorb.sourceforge.net/ for more about omniORB.
//
// Contributors form the TANGO community:
// Ramon Sune (ALBA) for the yat::Signal class 
//
// The YAT library is free software; you can redistribute it and/or modify it 
// under the terms of the GNU General Public License as published by the Free 
// Software Foundation; either version 2 of the License, or (at your option) 
// any later version.
//
// The YAT library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// See COPYING file for license details 
//
// Contact:
//      Nicolas Leclercq
//      Synchrotron SOLEIL
//------------------------------------------------------------------------------
/*!
 * \author N.Leclercq, J.Malik - Synchrotron SOLEIL
 */
 
#ifndef _YAT_PORTABILITY_H_
#define _YAT_PORTABILITY_H_

#if (defined WIN32 || defined _WIN32)
#  include <yat/config-win32.h>
#elif (defined _linux_ || defined __linux__)
#  include <yat/config-linux.h>
#elif (defined __APPLE__)
#  include <yat/config-macosx.h>
#else
# error "unknown/unsupported platform - sorry!"
#endif

/**
 *  Define portable string streams
 */
#ifdef YAT_HAS_SSTREAM

# include <sstream>
  namespace yat
  {
    typedef std::ostringstream OSStream;
    typedef std::istringstream ISStream;
    typedef std::stringstream  SStream;
  }

#else

# include <string>
# include <strstream>

  namespace yat
  {
    class OSStream : public std::ostrstream
    {
    public:
      std::string str()
      {
        //- in case it is not already done, add an 'end of string' character
        (*this) << '\0'; 
        //- create a string containing the data of the strstream
        std::string ret(std::ostrstream::str()); 
        //- call freeze such that the std::ostrstream will delete its internal string
        std::ostrstream::freeze(false);
        return ret;
      }
    };

    class ISStream : public std::istrstream
    {
    public:
      std::string str()
      {
        //- in case it is not already done, add an 'end of string' character
        (*this) << '\0'; 
        //- create a string containing the data of the strstream
        std::string ret(std::istrstream::str()); 
        //- call freeze such that the std::istrstream will delete its internal string
        std::istrstream::freeze(false);
        return ret;
      }
    };

    class SStream : public std::strstream
    {
    public:
      std::string str()
      {
        //- in case it is not already done, add an 'end of string' character
        (*this) << '\0'; 
        //- create a string containing the data of the strstream
        std::string ret(std::strstream::str()); 
        //- call freeze such that the std::strstream will delete its internal string
        std::strstream::freeze(false);
        return ret;
      }
    };
  }
#endif

#endif
