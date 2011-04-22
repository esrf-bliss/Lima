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
#include <iostream>
#include <yat/plugin/PlugIn.h>
#include <yat/plugin/PlugInSymbols.h>
#include <yat/plugin/IPlugInObjectWithAttr.h>

namespace yat
{

  IPlugInObject::IPlugInObject()
  {
  }

  IPlugInObject::~IPlugInObject()
  {
  }

  IPlugInFactory::IPlugInFactory()
  {
  }

  IPlugInFactory::~IPlugInFactory()
  {
  }

  IPlugInInfo::IPlugInInfo()
  {
  }

  IPlugInInfo::~IPlugInInfo()
  {
  }

  PlugIn::PlugIn( const std::string &library_file_name )
    : m_libraryHandle( NULL )
    , m_libraryName( library_file_name )
  {
    this->load_library( library_file_name );
  }


  PlugIn::~PlugIn()
  {
    this->release_library();
  }

  IPlugInInfo*
    PlugIn::info()
  {
    Symbol symbol;
    try
    {
      symbol = this->find_symbol(kGetInfoSymbol);
    }
    catch(yat::Exception& ex)
    {
      RETHROW_YAT_ERROR(ex,
                        "SHAREDLIBRARY_ERROR",
                        "Unable to find the 'GetInfo' symbol",
                        "PlugIn::info");
    }

    GetInfoFunc_t get_info_func = (GetInfoFunc_t)(symbol);

    IPlugInInfo* info = 0;
    try
    {
      info = (*get_info_func)();
    }
    catch(...)
    {
      THROW_YAT_ERROR("UNKNOWN_ERROR",
                      "Unknown error during IPlugInInfo instantiation",
                      "PlugIn::info");
    }
    return info;
  }

  IPlugInFactory* 
    PlugIn::factory()
  {
    Symbol symbol;
    try
    {
      symbol = this->find_symbol(kGetFactorySymbol);
    }
    catch(yat::Exception& ex)
    {
      RETHROW_YAT_ERROR(ex,
                        "SHAREDLIBRARY_ERROR",
                        "Unable to find the 'GetFactory' symbol",
                        "PlugIn::factory");
    }

    GetFactoryFunc_t get_factory_func = (GetFactoryFunc_t)(symbol);

    IPlugInFactory* factory = 0;
    try
    {
      factory = (*get_factory_func)();
    }
    catch(...)
    {
      THROW_YAT_ERROR("UNKNOWN_ERROR",
                      "Unknown error during factory instantiation",
                      "PlugIn::factory");
    }
    return factory;
  }


  PlugIn::Symbol 
    PlugIn::find_symbol( const std::string &symbol )
  {
    try
    {
      Symbol symbol_pointer = this->do_find_symbol( symbol );
      if ( symbol_pointer != NULL )
        return symbol_pointer;
    }
    catch ( ... )
    {
    }
    
    THROW_YAT_ERROR("SHAREDLIBRARY_ERROR",
                    "Unable to find the requested symbol",
                    "PlugIn::find_symbol");
    return NULL;    // keep compiler happy
  }

  void
    PlugIn::load_library( const std::string &library_file_name )
  {
    try
    {
      this->release_library();
      m_libraryHandle = this->do_load_library( library_file_name );
      if ( m_libraryHandle == NULL )
      {
        THROW_YAT_ERROR("SHAREDLIBRARY_ERROR",
                        "Unable to load the specified shared library",
                        "PlugIn::load_library");
      }

      Symbol sym = this->find_symbol(kOnLoadSymbol);

      OnLoadFunc_t load_func = (OnLoadFunc_t)(sym);
      load_func();

    }
    catch(yat::Exception& ex)
    {
      RETHROW_YAT_ERROR(ex,
                        "SHAREDLIBRARY_ERROR",
                        "Error while loading library",
                        "PlugIn::load_library");
    }
    catch (...)
    {
      THROW_YAT_ERROR("SHAREDLIBRARY_ERROR",
                      "Unknown error while loading library",
                      "PlugIn::load_library");
    }

  }


  void 
    PlugIn::release_library()
  {
    try
    {
      if ( m_libraryHandle != NULL )
      {
        Symbol sym = this->find_symbol(kOnUnLoadSymbol);

        OnUnLoadFunc_t unload_func = (OnUnLoadFunc_t)(sym);
        unload_func();

        this->do_release_library();
        m_libraryHandle = NULL;
      }
    }
    catch(yat::Exception& ex)
    {
      RETHROW_YAT_ERROR(ex,
                        "SHAREDLIBRARY_ERROR",
                        "Error while releasing library",
                        "PlugIn::release_library");
    }
    catch (...)
    {
      THROW_YAT_ERROR("SHAREDLIBRARY_ERROR",
                      "Unknown error while releasing library",
                      "PlugIn::release_library");
    }
  }


}
