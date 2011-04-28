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
 
#ifndef _TARGET_PLATFORM_H_
#define _TARGET_PLATFORM_H_

//-----------------------------------------------------------------------------
// PLATFORM CONFIGURATION
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// WINDOWS on INTEL PENTIUM OR AMD ATHLON-XP [*** TESTED ***]
//-----------------------------------------------------------------------------
// supported compiler: Microsoft Vsual C++ [>= 6.0]
// supported processors: 32 bits Intel Pentium and AMD Athlon-XP 
// endianness : little 
//-----------------------------------------------------------------------------
#if defined(WIN32)

# if (_M_IX86 > 400)
#  define _HAS_PENTIUM_ 1
#  define _LITTLE_ENDIAN_PLATFORM_ 1
# else
#  error "no support for this processor"
# endif

# if !defined(_MSC_VER)
#  error "no support for this WIN32 compiler - MSVC++ compiler required"
# elif (_MSC_VER < 1200)
#  error "microsoft visual C++ >= 6.0 required"
# else
#  define _HAS_STATIC_OBJ_MANAGER_ 0
# endif

//-----------------------------------------------------------------------------
// LINUX on INTEL PENTIUM OR AMD ATHLON-XP [*** TESTED ***]
//-----------------------------------------------------------------------------
// supported compiler: gcc [>= 3.2.0]
// supported processors: 32 bits Intel Pentium and AMD Athlon-XP 
// endianness : little 
//-----------------------------------------------------------------------------
#elif defined(_linux_) || defined (__linux__)

# if defined(i386) || defined(__i386__)
#  define _HAS_PENTIUM_ 1
#  define _LITTLE_ENDIAN_PLATFORM_ 1
# else
#  error "no support for this processor"
# endif

# if !defined(__GNUG__)
#  error "no support for this compiler - GCC compiler required"
# else
#  define _HAS_STATIC_OBJ_MANAGER_ 0
# endif 

//-----------------------------------------------------------------------------
// SunOS on SPARC or PENTIUM [*** NOT TESTED ***]
//-----------------------------------------------------------------------------
// supported compiler: sun pro cc [>= ?]
// supported processors: 32 bits SPARC or PENTIUM
// endianness : big on SPARC, little on Pentium 
//-----------------------------------------------------------------------------
// NOTE FOR SOLARIS USERS: add -Dsun to your compiler flags
//-----------------------------------------------------------------------------
#elif defined(__sun) || defined (sun) 
# if defined(__sparcv9)
#  error "no support for 64 bits mode"
# elif (__sparc) || defined (__sparc__)
#  define _HAS_PENTIUM_ 0
#  define _LITTLE_ENDIAN_PLATFORM_ 0
# elif defined(__i386) || defined(i386) || defined(__i386__)
#  define _HAS_PENTIUM_ 1
#  define _LITTLE_ENDIAN_PLATFORM_ 1
# else
#  error "no support for this processor"
# endif
# if !defined(__SUNPRO_CC) && !defined(__GNUG__)
#  error "no support for this compiler - Sun-CC or GNU-GCC compiler required"
# else
#  define _HAS_STATIC_OBJ_MANAGER_ 1
# endif
//-----------------------------------------------------------------------------
// UNKNOWN/UNSUPPORTED PLATEFORM
//-----------------------------------------------------------------------------
#else
#  error "no support for this platform"
#endif

#endif // _TARGET_PLATFORM_H_
