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

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <iostream>
#include <yat/CommonHeader.h>
#include <yat/Exception.h>

namespace yat 
{
  // ============================================================================
  // Error::Error
  // ============================================================================
  Error::Error ()
    :  reason ("unknown"),
       desc ("unknown error"),
       origin ("unknown"),
       code (-1),
       severity (yat::ERR)
  {
    //- noop
  }

  // ============================================================================
  // Error::Error
  // ============================================================================
  Error::Error (const char *_reason,
                const char *_desc,
                const char *_origin,
                int _code, 
                int _severity)
    :  reason (_reason),
       desc (_desc),
       origin (_origin),
       code (_code),
       severity (_severity)
  {
    //- noop
  }

  // ============================================================================
  // Error::Error
  // ============================================================================
  Error::Error (const std::string& _reason,
                const std::string& _desc,
                const std::string& _origin,
                int _code, 
                int _severity)
    :  reason (_reason),
       desc (_desc),
       origin (_origin),
       code (_code),
       severity (_severity)
  {
    //- noop
  }

  // ============================================================================
  // Error::Error
  // ============================================================================
  Error::Error (const Error& _src)
    :  reason (_src.reason),
       desc (_src.desc),
       origin (_src.origin),
       code (_src.code),
       severity (_src.severity)
  {
    //- noop
  }

  // ============================================================================
  // Error::~Error
  // ============================================================================
  Error::~Error ()
  {
    //- noop
  }

  // ============================================================================
  // Error::operator=
  // ============================================================================
  Error& Error::operator= (const Error& _src) 
  {
    //- no self assignment
    if (this == &_src)
      return *this;

    this->reason = _src.reason;
    this->desc = _src.desc;
    this->origin = _src.origin;
    this->code = _src.code;
    this->severity = _src.severity;

    return *this;
  }

  // ============================================================================
  // Exception::Exception
  // ============================================================================
  Exception::Exception () 
    : errors(0)
  {
    //- noop
  }

  // ============================================================================
  // Exception::Exception
  // ============================================================================
  Exception::Exception (const char *_reason,
                        const char *_desc,
                        const char *_origin,
                        int _code, 
                        int _severity) 
    : errors(0)
  {
    this->push_error(Error(_reason, _desc, _origin, _code, _severity));
  }

  // ============================================================================
  // Exception::Exception
  // ============================================================================
  Exception::Exception (const std::string& _reason,
                        const std::string& _desc,
                        const std::string& _origin,
                        int _code, 
                        int _severity) 
    : errors(0)
  {
    this->push_error(Error(_reason, _desc, _origin, _code, _severity));
  }

  // ============================================================================
  // Exception::Exception
  // ============================================================================
  Exception::Exception (const Exception& _src) 
    : errors(0)
  {
    for (unsigned int i = 0; i < _src.errors.size();  i++)
      this->push_error(_src.errors[i]);
  }

  // ============================================================================
  // Exception::Exception
  // ============================================================================
  Exception& Exception::operator= (const Exception& _src) 
  {
    //- no self assignment
    if (this == &_src)
      return *this;

    this->errors.clear();

    for (unsigned int i = 0; i < _src.errors.size();  i++)
      this->push_error(_src.errors[i]);

    return *this;
  }

  // ============================================================================
  // Exception::~Exception
  // ============================================================================
  Exception::~Exception ()
  {
    this->errors.clear();
  }

  // ============================================================================
  // Exception::push_error
  // ============================================================================
  void Exception::push_error (const char *_reason,
                              const char *_desc,
                              const char *_origin, 
                              int _code, 
                              int _severity)
  {
    this->errors.push_back(Error(_reason, _desc, _origin, _code, _severity));
  }

  // ============================================================================
  // Exception::push_error
  // ============================================================================
  void Exception::push_error (const std::string& _reason,
                              const std::string& _desc,
                              const std::string& _origin, 
                              int _code, 
                              int _severity)
  {
    this->errors.push_back(Error(_reason, _desc, _origin, _code, _severity));
  }

  // ============================================================================
  // Exception::push_error
  // ============================================================================
  void Exception::push_error (const Error& _error)
  {
    this->errors.push_back(_error);
  }
  
  // ============================================================================
  // Exception::dump
  // ============================================================================
  void Exception::dump () const
  {
    std::cout << "-- yat::Exception ---------------------------" << std::endl;
    for (size_t e = 0; e < this->errors.size(); e++)
    {
      std::cout << "\tErr[" 
                << e << "]:reason..." 
                << this->errors[e].reason 
                << std::endl; 
              
      std::cout << "\tErr[" 
                << e << "]:desc....." 
                << this->errors[e].desc
                << std::endl;  
              
      std::cout << "\tErr[" 
                << e 
                << "]:desc....." 
                << this->errors[e].origin 
                << std::endl; 
              
      std::cout << "\tErr[" 
                << e 
                << "]:code....." 
                << this->errors[e].code
                << std::endl; 
    }
    std::cout << "----------------------------------------------" << std::endl;
  }

} // namespace



