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

#ifndef _YAT_CONFIG_LINUX_H_
#define _YAT_CONFIG_LINUX_H_

#define YAT_LINUX

/**
 *  Some integer types
 */
#include <stdint.h>
typedef uint8_t yat_uint8_t;
typedef int16_t  yat_int16_t;
typedef uint16_t yat_uint16_t;
typedef int32_t yat_int32_t;
typedef uint32_t yat_uint32_t;

/**
 *  Shared library related stuff
 */
#define YAT_DECL_EXPORT
#define YAT_DECL_IMPORT
#define YAT_DECL

/**
 *  <sstream> library related stuffs
 */
#undef YAT_HAS_SSTREAM
#if __GNUC__ >= 3
# if __GNUC__ == 3
#   if __GNUC_MINOR__ >= 2
#     define YAT_HAS_SSTREAM
#   endif
# else
#   define YAT_HAS_SSTREAM
# endif
#endif

/**
 *  pthread related stuffs
 */
#define YAT_HAS_PTHREAD_YIELD 1

/**
 *  Endianness related stuffs
 */
# if defined(i386) || defined(__i386__) || defined(__amd64__)
#  define YAT_HAS_PENTIUM 1
#  define YAT_LITTLE_ENDIAN_PLATFORM 1
# else
#  error "no support for this processor"
# endif

# if !defined(__GNUG__)
#  error "no support for this compiler - GCC compiler required"
# else
#  define YAT_HAS_STATIC_OBJ_MANAGER 0
# endif 

#endif
