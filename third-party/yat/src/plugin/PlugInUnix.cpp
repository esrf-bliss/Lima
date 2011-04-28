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

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/plugin/PlugIn.h>


#if defined(YAT_LINUX) || defined(YAT_MACOSX)

#include <dlfcn.h>
#include <unistd.h>

namespace yat
{

  PlugIn::LibraryHandle PlugIn::do_load_library( const std::string &library_file_name )
  {
    return ::dlopen( library_file_name.c_str(), RTLD_NOW | RTLD_GLOBAL );
  }


  void PlugIn::do_release_library()
  {
    ::dlclose( m_libraryHandle);
  }


  PlugIn::Symbol PlugIn::do_find_symbol( const std::string &symbol )
  {
    return ::dlsym ( m_libraryHandle, symbol.c_str() );
  }


  std::string PlugIn::get_last_error_detail() const
  {
    return "";
  }


}

#endif
