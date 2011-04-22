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
 * \author N.Leclercq - Synchrotron SOLEIL
 */

#ifndef _YAT_STRING_TOKENIZER_H_
#define _YAT_STRING_TOKENIZER_H_

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

namespace yat 
{

// ============================================================================
// StringTokenizer class
// ============================================================================
class StringTokenizer
{
public:
  StringTokenizer (const std::string & str, const std::string & delim);

  ~StringTokenizer () {};

  int count_tokens () const;

  bool has_more_tokens () const;

  std::string next_token ();

  int next_int_token ();

  long next_long_token ();

  double next_fp_token ();

  std::string next_token (const std::string & delim);

  std::string remaining_string ();

  std::string filter_next_token (const std::string & filterStr);

private:
  std::string m_delim_str;
  std::string m_token_str;
};

} //- namespace

#endif


