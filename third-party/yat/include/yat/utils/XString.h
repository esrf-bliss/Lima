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

#ifndef _YAT_XSTRING_H_
#define _YAT_XSTRING_H_

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include <yat/CommonHeader.h>

namespace yat 
{

// ============================================================================
// XString class
// ============================================================================
template <typename _T>
class XString
{
public:  
  
  //- converts string content to numeric type _T
  //- should also work for any "istringstream::operator>>" supported type
  //---------------------------------------------------------------------
  static _T to_num (const std::string& _s, bool _throw = true)
  {
    ISStream iss(_s.c_str());

    _T num_val;

    if ( (iss >> num_val) == false )
    {
      if (_throw)
      {
        OSStream desc;
        desc << "conversion from string to num failed [" 
             << _s
             << "]"
             << std::ends;
        THROW_YAT_ERROR ("SOFTWARE_ERROR",
                         desc.str().c_str(),
                         "XString::to_num");
      }
      return 0;
    }

    return num_val;
  } 

  //- converts from type _T to std::string
  //---------------------------------------------------------------------
  static std::string to_string (const _T & _t, bool _throw = true)
  {
    OSStream oss;

    if ( (oss << std::fixed << _t) == false )
    {
      if (_throw)
      {
        OSStream desc;
        desc << "conversion from num to string failed [" 
             << _t
             << "]"
             << std::ends;
        THROW_YAT_ERROR ("SOFTWARE_ERROR",
                         desc.str().c_str(),
                         "XString::to_string");
      }
      return std::string("");
    }

    return oss.str();
  } 

};

} //- namespace

#endif // _XSTRING_H_
