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

#ifndef _YAT_COMMON_H_
#define _YAT_COMMON_H_

// ============================================================================
// IMPL OPTION - THIS ENABLES/DISABLES THE CACHE ON THE MESSAGE CLASS 
// ============================================================================
// DEFINE THE FOLLOWING IS YOU WANT TO TEST THE CACHED ALLOCATOR ON THE MESSAGE 
// CLASS. FIRST TESTS SHOW THAT THE GAIN IS NOT SIGNIFICANT ENOUGH FOR THE CACHE
// TO BE USED. MOREOVER, THE CACHED ALLOCATOR USAGE IS QUITE TRICKY SINCE NEW AND 
// DELETE OPERATORS MUST BE OVERLOADED FOR THE CLASS TO BE "CACHABLE" (SEE WHAT 
// HAS BEEN DONE IN THE YAT MESSAGE TASK). ANYWAY WE NOW HAVE A CACHED-ALL0CATOR 
// IN YAT! 
// ============================================================================
// #define _USE_MSG_CACHE_

#if (defined (_DEBUG) || defined (DEBUG))
# define YAT_DEBUG
# include <assert.h>
#endif

#include <yat/Portability.h>
#include <yat/Inline.h>
#include <yat/LogHelper.h>
#include <yat/Exception.h>

namespace yat 
{

// ============================================================================
// IMPL OPTION
// ============================================================================
#define ENABLE_TASK_STATS

// ============================================================================
// CONSTs
// ============================================================================
#define kDEFAULT_MSG_TMO_MSECS 2000

//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------
#define SAFE_DELETE_PTR(P) if (P) { delete P; P = 0; } else (void)0
//-----------------------------------------------------------------------------
#define SAFE_DELETE_ARRAY(P) if (P) { delete[] P; P = 0; } else (void)0
//-----------------------------------------------------------------------------
#define SAFE_RELEASE(P) if (P) { P->release(); P = 0; } else (void)0
//-----------------------------------------------------------------------------
#define _CPTC(X) static_cast<const char*>(X)

//-----------------------------------------------------------------------------
// ASSERTION
//-----------------------------------------------------------------------------
#if defined (YAT_DEBUG)
# define DEBUG_ASSERT(EXP) assert(EXP)
#else
# define DEBUG_ASSERT(EXP)
#endif

}

#endif
