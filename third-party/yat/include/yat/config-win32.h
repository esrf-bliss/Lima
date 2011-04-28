//----------------------------------------------------------------------------
// YAT LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2009  The Tango Community
//
// Part of the code comes from the ACE Framework
// see http://www.cs.wustl.edu/~schmidt/ACE.html for more about ACE
//
// The thread native implementation has been initially inspired by omniThread
// - the threading support library that comes with omniORB. 
// see http://omniorb.sourceforge.net/ for more about omniORB.
// Contributors form the TANGO community:
// Ramon Sunes (ALBA) 
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

#ifndef _YAT_CONFIG_WIN32_H_
#define _YAT_CONFIG_WIN32_H_

#define YAT_WIN32

#include <Basetsd.h>

typedef unsigned char yat_uint8_t;

typedef short yat_int16_t;
typedef unsigned short yat_uint16_t;

#if defined(_WIN64)
  typedef int yat_int32_t;
  typedef unsigned int yat_uint32_t;
#else
  typedef long yat_int32_t;
  typedef unsigned long yat_uint32_t;
#endif

/**
 *  Disable some annoying warnings
 */
//- 'identifier' : identifier was truncated to 'number' characters
#pragma warning(disable:4786) 
//- 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2' 
#pragma warning(disable:4251) 
//- non – DLL-interface classkey 'identifier' used as base for DLL-interface classkey 'identifier' 
#pragma warning(disable:4275) 
//- C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
#pragma warning(disable:4290)
//- 'function': was declared deprecated
#pragma warning(disable:4996)
//- 'function': possible loss of data
#pragma warning(disable:4267)
//- 'function': forcing value to bool 'true' or 'false' (performance warning)
#pragma warning(disable:4800)

#ifndef _WIN32_WINNT
//- the following macro must be set sa as ::SignalObjectAndWait() to be defined
#define _WIN32_WINNT 0x400
#endif

#include <windows.h>

/**
 *  <sstream> library related stuffs
 */
#define YAT_HAS_SSTREAM

/**
 *  Shared library related stuffs
 */
# if defined(YAT_DLL)
#   define YAT_DECL_EXPORT __declspec(dllexport)
#   define YAT_DECL_IMPORT __declspec(dllimport)
#   if defined (YAT_BUILD)
#     define YAT_DECL YAT_DECL_EXPORT
#   else
#     define YAT_DECL YAT_DECL_IMPORT
#   endif
# elif defined(YAT_PLUGIN)
#   define YAT_DECL_EXPORT __declspec(dllexport)
#   define YAT_DECL_IMPORT __declspec(dllimport)
#   define YAT_DECL
# else
#   define YAT_DECL_EXPORT
#   define YAT_DECL_IMPORT
#   define YAT_DECL
# endif

/**
 *  Endianness related stuffs
 */
# if (_M_IX86 > 400)
#  define YAT_HAS_PENTIUM 1
#  define YAT_LITTLE_ENDIAN_PLATFORM 1
# else
#  error "no support for this processor"
# endif

# if !defined(_MSC_VER)
#  error "no support for this WIN32 compiler - MSVC++ compiler required"
# elif (_MSC_VER < 1200)
#  error "microsoft visual C++ >= 6.0 required"
# else
#  define YAT_HAS_STATIC_OBJ_MANAGER 0
# endif

#endif
