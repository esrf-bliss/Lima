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

#ifndef _YAT_EXCEPTION_H_
#define _YAT_EXCEPTION_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <string>
#include <vector>
#include <yat/CommonHeader.h>

namespace yat 
{

#define THROW_YAT_ERROR( reason, desc, origin ) \
  throw yat::Exception( (reason), (desc), (origin) )

#define RETHROW_YAT_ERROR( exception, reason, desc, origin ) \
  { \
    exception.push_error( (reason), (desc), (origin) ); \
    throw exception; \
  }

  // ============================================================================
  // Error Severity
  // ============================================================================
  typedef enum {
    WARN, 
    ERR, 
    PANIC
  } ErrorSeverity;

  // ============================================================================
  //! The Error class
  // ============================================================================
  //!  
  //! detailed description to be written
  //!
  // ============================================================================
  struct YAT_DECL Error
  {
    /**
    * Initialization. 
    */
    Error ();

    /**
    * Initialization. 
    */
    Error ( const char *reason,
            const char *desc,
            const char *origin,
            int err_code = -1, 
            int severity = yat::ERR);
    /**
    * Initialization. 
    */
    Error ( const std::string& reason,
            const std::string& desc,
            const std::string& origin, 
            int err_code = -1, 
            int severity = yat::ERR);

    /**
    * Copy constructor. 
    */
    Error (const Error& src);

    /**
    * Error details: code 
    */
    virtual ~Error ();

    /**
    * operator= 
    */
    Error& operator= (const Error& _src);

    /**
    * Error details: reason 
    */
    std::string reason;

    /**
    * Error details: description 
    */
    std::string desc;

    /**
    * Error details: origin 
    */
    std::string origin;

    /**
    * Error details: code 
    */
    int code;

    /**
    * Error details: severity 
    */
    int severity;
  };

  // ============================================================================
  //! The Exception class
  // ============================================================================
  //!  
  //! detailed description to be written
  //!
  // ============================================================================
  class YAT_DECL Exception
  {
  public:

    typedef std::vector<Error> ErrorList;

    /**
    * Ctor
    */
    Exception ();

    /**
    * Ctor
    */
    Exception ( const char *reason,
                const char *desc,
                const char *origin,
                int err_code = -1, 
                int severity = yat::ERR);

    /**
    * Ctor
    */
    Exception ( const std::string& reason,
                const std::string& desc,
                const std::string& origin, 
                int err_code = -1, 
                int severity = yat::ERR);

    /**
    * Ctor
    */
    Exception (const Error& error);

    /**
    * Copy ctor
    */
    Exception (const Exception& src);

    /**
    * operator=
    */
    Exception& operator= (const Exception& _src); 

    /**
    * Dtor
    */
    virtual ~Exception ();

    /**
    * Push the specified error into the errors list.
    */
    void push_error ( const char *reason,
                      const char *desc,
                      const char *origin, 
                      int err_code = -1, 
                      int severity = yat::ERR);

    /**
    * Push the specified error into the errors list.
    */
    void push_error ( const std::string& reason,
                      const std::string& desc,
                      const std::string& origin, 
                      int err_code = -1, 
                      int severity = yat::ERR);

    /**
    * Push the specified error into the errors list.
    */
    void push_error (const Error& error);
    
    /**
    * Dump.
    */
    virtual void dump () const;

    /**
    * The errors list
    */
    ErrorList errors;
  };

} // namespace

/*
#if defined (YAT_INLINE_IMPL)
# include <yat/Exception.i>
#endif // YAT_INLINE_IMPL
*/

#endif // _MESSAGE_H_

