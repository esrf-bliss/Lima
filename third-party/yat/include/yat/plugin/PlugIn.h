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

#ifndef _YAT_PLUGIN_H_
#define _YAT_PLUGIN_H_

#include <yat/plugin/IPlugInInfo.h>
#include <yat/plugin/IPlugInFactory.h>

namespace yat
{

/*! \brief Platform independant layer to manage dynamic libraries.
  *
  * The PlugIn class provides a platform independent way to work with
  * dynamic library. It loads a specific dynamic library, and can return
  * specific symbol exported by the dynamic library.
  * It is inspired (when not copied) from the CppUnit implementation
  * (http://sourceforge.net/projects/cppunit)
  *
  * It can return a factory that constructs IPlugInObject implementations
  *
  */
class YAT_DECL PlugIn
{
public:

  /*! \brief Loads the specified library.
    *  \param library_file_name Name of the library to load.
    *  \exception DynamicLibraryManagerException if a failure occurs while loading
    *             the library (fail to found or load the library).
    */
  PlugIn( const std::string &library_file_name );

  /// Releases the loaded library.
  ~PlugIn();

  /*! \brief Returns information about the plugin
    */
  IPlugInInfo* info();

  /*! \brief Returns the factory used for constructing IPlugInObject implementation
    */
  IPlugInFactory* factory();

private:
  typedef void *LibraryHandle;
  typedef void *Symbol;

  /*! \brief Returns a pointer on the specified symbol exported by the library.
    *  \param symbol Name of the symbol exported by the library.
    *  \return Pointer on the symbol. Should be casted to the actual type. Never \c NULL.
    *  \exception yat::Exception if the symbol is not found.
    */
  Symbol find_symbol( const std::string &symbol );

  /*! Loads the specified library.
    * \param library_file_name Name of the library to load.
    * \exception DynamicLibraryManagerException if a failure occurs while loading
    *            the library (fail to found or load the library).
    */
  void load_library( const std::string &library_file_name );

  /*! Releases the loaded library.
    * 
    * \warning Must NOT throw any exceptions (called from destructor).
    */
  void release_library();

  /*! Loads the specified library.
    * 
    * May throw any exceptions (indicates failure).
    * \param library_file_name Name of the library to load.
    * \return Handle of the loaded library. \c NULL indicates failure.
    */
  LibraryHandle do_load_library( const std::string &library_file_name );

  /*! Releases the loaded library.
    *
    * The handle of the library to free is in \c m_libraryHandle. It is never
    * \c NULL.
    * \warning Must NOT throw any exceptions (called from destructor).
    */
  void do_release_library();

  /*! Returns a pointer on the specified symbol exported by the library.
    * 
    * May throw any exceptions (indicates failure).
    * \param symbol Name of the symbol exported by the library.
    * \return Pointer on the symbol. \c NULL indicates failure.
    */
  Symbol do_find_symbol( const std::string &symbol );

  /*! Returns detailed information about do_load_library() failure.
    *
    * Called just after a failed call to do_load_library() to get extra
    * error information.
    *
    * \return Detailed information about the failure of the call to
    *         do_load_library() that just failed.
    */
  std::string get_last_error_detail() const;

  /// Prevents the use of the copy constructor.
  PlugIn( const PlugIn &copy );

  /// Prevents the use of the copy operator.
  void operator =( const PlugIn &copy );

private:
  LibraryHandle m_libraryHandle;
  std::string m_libraryName;
};

} // namespace

#endif
