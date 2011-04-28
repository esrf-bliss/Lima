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

#ifndef _YAT_PLUGINTYPES_H_
#define _YAT_PLUGINTYPES_H_

#include <map>
#include <string>
#include <vector>
#include <yat/CommonHeader.h>
#include <yat/utils/Callback.h>
#include <yat/any/Any.h>

namespace yat
{

  YAT_DEFINE_CALLBACK ( SetAttrCB, const yat::Any&  );
  YAT_DEFINE_CALLBACK ( GetAttrCB, yat::Any& );

  struct PlugInDataType
  { 
    enum
    {
      BOOLEAN,
      UINT8,
      INT16,
      UINT16,
      INT32,
      UINT32,
      FLOAT,
      DOUBLE,
      STRING
    };
  };

  struct PlugInAttrWriteType
  { 
    enum
    {
      READ,
      WRITE,
      READ_WRITE
    };
  };

  class PlugInAttrInfo
  {
  public:
    std::string name;
    int data_type;
    int write_type;

    std::string label;
    std::string desc;
    std::string unit;
    std::string display_format;

    SetAttrCB set_cb;
    GetAttrCB get_cb;
  };

  typedef std::vector<PlugInAttrInfo> PlugInAttrInfoList;

  struct PlugInPropType
  {
    enum
    {
      _UNDEFINED = -1,
      BOOLEAN,
      INT16,
      UINT8,
      UINT16,
      INT32,
      UINT32,
      FLOAT,
      DOUBLE,
      STRING,
      STRING_VECTOR,
      INT16_VECTOR,
      UINT16_VECTOR,
      INT32_VECTOR,
      UINT32_VECTOR,
      FLOAT_VECTOR,
      DOUBLE_VECTOR
    };
  };

  typedef std::map<std::string , int> PlugInPropInfos;
  typedef std::map<std::string , yat::Any> PlugInPropValues;

} // namespace

#endif
