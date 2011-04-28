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

#ifndef _YAT_LOGHELPER_H_
#define _YAT_LOGHELPER_H_

#include <yat/CommonHeader.h>

/*!
 *  Define the YAT_LOG & YAT_TRACE helper macro depending on the YAT_ENABLE_LOG macro
 *  These macros can be used by yat users to log in their applications
 */
#if defined (YAT_ENABLE_LOG)

#  include <iostream>

#  define YAT_LOG(s) \
    do \
    { \
       std::cout << "[this:" \
                 << std::hex \
                 << (void *)this \
                 << std::dec \
                 << "]::" \
                 << __FUNCTION__ \
                 << "::" \
                 << s \
                 << std::endl; \
    } while (0)
    
#  define YAT_LOG_STATIC(s) \
    do \
    { \
       std::cout << s << std::endl; \
    } while(0)

# if defined (YAT_ENABLE_TRACE)
    namespace yat
    {
      class YAT_DECL TraceHelper
      {
      public:
        TraceHelper::TraceHelper(const char* _func_name, const void * _this = 0 )
          :  instance(_this), func_name(_func_name)
        { 
          std::cout << func_name 
                    << " [this::" 
                    << std::hex 
                    << instance 
                    << std::dec 
                    << "] <-" 
                    << std::endl; 
        };

        TraceHelper::~TraceHelper()
        { 
          std::cout << func_name 
                    << " [this::" 
                    << std::hex 
                    << instance
                    << std::dec 
                    << "] ->" 
                    << std::endl; 
        };

      private:
        const void * instance;
        const char* func_name;
      };
    }
#   define YAT_TRACE(func_name) \
      yat::TraceHelper yat_trace_helper( (func_name), this )
#   define YAT_TRACE_STATIC(func_name) \
      yat::TraceHelper yat_trace_helper( (func_name) )
# else
#   define YAT_TRACE(func_name)
#   define YAT_TRACE_STATIC(func_name)
# endif

#else

# define YAT_LOG(x)
# define YAT_LOG_STATIC(x)
# define YAT_TRACE(x)
# define YAT_TRACE_STATIC(x)

#endif


#endif
