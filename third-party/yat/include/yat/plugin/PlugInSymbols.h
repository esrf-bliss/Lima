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

#ifndef _YAT_PLUGIN_SYMBOLS_H_
#define _YAT_PLUGIN_SYMBOLS_H_

#include <yat/plugin/IPlugInInfo.h>
#include <yat/plugin/IPlugInFactory.h>

namespace yat
{
/*! \brief Templated generic factory implementing IPlugInFactory, used to instantiate the Object
  *         given to it during template instantiation
  */
template <class Object>
class GenericFactory : public IPlugInFactory
{
public:
  GenericFactory()
  {};

  virtual ~GenericFactory()
  {};

  virtual void create(IPlugInObject*& object,
                      const PlugInObjectParams&)
  {
    object = new Object();
  };
};

/*! \brief Prototypes for the exported symbols of a plugin
  */
typedef void            (*OnLoadFunc_t    ) ( void );
typedef void            (*OnUnLoadFunc_t  ) ( void );
typedef IPlugInInfo*    (*GetInfoFunc_t   ) ( void );
typedef IPlugInFactory* (*GetFactoryFunc_t) ( void );

/*! \brief Names of the exported symbols of a plugin
  */
const std::string kOnLoadSymbol      ( "OnLoad" );
const std::string kOnUnLoadSymbol    ( "OnUnLoad" );
const std::string kGetInfoSymbol     ( "GetInfo" );
const std::string kGetFactorySymbol  ( "GetFactory" );

/*! Helper macro to declare the OnLoad and OnUnLoad plugin exported symbols
  */
# define DECLARE_LOAD_UNLOAD_PLUGIN_EXPORTED_SYMBOLS \
  extern "C" YAT_DECL_EXPORT void OnLoad( void ); \
  extern "C" YAT_DECL_EXPORT void OnUnLoad( void );

/*! Helper macro to define an empty implementation of the OnLoad and OnUnLoad plugin exported symbols
  */
# define DEFINE_LOAD_UNLOAD_EXPORTED_SYMBOLS \
  void OnLoad( void )   {  }; \
  void OnUnLoad( void ) {  };

/*! Helper macro to define and declare default empty OnLoad and OnUnLoad plugin exported symbols
  */
# define EXPORT_DEFAULT_LOAD_UNLOAD \
  DECLARE_LOAD_UNLOAD_PLUGIN_EXPORTED_SYMBOLS \
  DEFINE_LOAD_UNLOAD_EXPORTED_SYMBOLS

/*! Helper macro to export the GetInfo symbol. The function will allocate and return an instance
  *  of the class given to it as parameter
  */
# define EXPORT_GETINFO( PlugInInfoClass ) \
  extern "C" YAT_DECL_EXPORT yat::IPlugInInfo *GetInfo( void ) { return new PlugInInfoClass(); }

/*! Helper macro to export the GetFactory symbol. The function will allocate and return an instance
  *  of the class given to it as parameter
  */
# define EXPORT_FACTORY( FactoryClass ) \
  extern "C" YAT_DECL_EXPORT yat::IPlugInFactory *GetFactory( void ) { return new FactoryClass(); }

/*! Helper macro to declare and define the exported symbols for a plugin containing a single class
  */
# define EXPORT_SINGLECLASS_PLUGIN( PlugInObjectClass, PlugInInfoClass ) \
  EXPORT_DEFAULT_LOAD_UNLOAD \
  EXPORT_GETINFO( PlugInInfoClass ) \
  EXPORT_FACTORY( yat::GenericFactory< PlugInObjectClass > )

} // namespace

#endif
