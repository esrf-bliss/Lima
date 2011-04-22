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

#ifndef _YAT_PLUGIN_MANAGER_H_
#define _YAT_PLUGIN_MANAGER_H_

#include <yat/plugin/PlugIn.h>
#include <deque>

namespace yat
{

/*! \brief Manages PlugIn.
 */
class YAT_DECL PlugInManager
{
public:
  /*! Constructs a PlugInManager object.
   */
  PlugInManager();

  /// Destructor.
  virtual ~PlugInManager();

  /*! \brief Loads the specified plug-in.
   *
   * After being loaded, the OnLoad() method is called.
   *
   * \param library_file_name Name of the file that contains the PlugIn.
   * \return Pointer on the IPlugInFactory associated to the library.
   *         Valid until the library is unloaded. Never \c NULL.
   * \exception yat::Exception is thrown if an error occurs during loading.
   */
  std::pair<IPlugInInfo*, IPlugInFactory*> load( const std::string &library_file_name );

  /*! \brief Unloads the specified plug-in.
   * \param library_file_name Name of the file that contains the TestPlugIn passed
   *                        to a previous call to load().
   */
  void unload( const std::string &library_file_name );
  
  /*! \brief Unloads all loaded plug-in.
   */
  void unload_all( void );

protected:
  /*! \brief (INTERNAL) Information about a specific plug-in.
   */
  struct PlugInEntry
  {
    std::string     m_fileName;
    PlugIn*         m_plugin;
    IPlugInInfo*    m_info;
    IPlugInFactory* m_factory;
  };

  /*! Unloads the specified plug-in.
   * \param plug_in Information about the plug-in.
   */
  void unload( PlugInEntry &plug_in );

private:
  /// Prevents the use of the copy constructor.
  PlugInManager( const PlugInManager &copy );

  /// Prevents the use of the copy operator.
  void operator =( const PlugInManager &copy );

private:
  typedef std::deque<PlugInEntry> PlugIns;
  PlugIns m_plugIns;
};

} // namespace

#endif
