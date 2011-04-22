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

#ifndef _YAT_IPLUGINOBJECT_WITH_ATTR_H_
#define _YAT_IPLUGINOBJECT_WITH_ATTR_H_

#include <yat/plugin/IPlugInObject.h>
#include <yat/plugin/PlugInTypes.h>

namespace yat
{

class YAT_DECL IPlugInObjectWithAttr : public yat::IPlugInObject
{
public:
  virtual void enumerate_attributes( yat::PlugInAttrInfoList& list) const
    throw (yat::Exception) = 0;

  virtual void enumerate_properties( yat::PlugInPropInfos& prop_infos ) const
    throw (yat::Exception) = 0;
  
  virtual void set_properties( yat::PlugInPropValues& prop_values )
    throw (yat::Exception) = 0;
};

} // namespace

#endif
