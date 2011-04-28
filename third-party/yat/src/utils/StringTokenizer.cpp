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

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include <yat/utils/StringTokenizer.h>

namespace yat 
{

//=============================================================================
// StringTokenizer::StringTokenizer
//=============================================================================
StringTokenizer::StringTokenizer (const std::string & s, const std::string & d)
  :  m_delim_str(d), m_token_str(s)
{
  if ( ! s.length () || ! d.length() )
    return;

  std::string::size_type current_pos = 0;

  while (true)
  {
    current_pos = m_token_str.find(m_delim_str, current_pos);
    if ( current_pos != std::string::npos )
    {
      current_pos += m_delim_str.length ();
      while (m_token_str.find(m_delim_str, current_pos) == current_pos)
        m_token_str.erase(current_pos, m_delim_str.length());
    }
    else
      break;
  }
 
  if (! m_token_str.find(m_delim_str, 0))
    m_token_str.erase(0, m_delim_str.length ());

  current_pos = 0;
  if ((current_pos = m_token_str.rfind(m_delim_str)) != std::string::npos)
  {
    if (current_pos != (m_token_str.length() - m_delim_str.length()))
      return;
    m_token_str.erase(m_token_str.length() - m_delim_str.length(), m_delim_str.length());
  }
}

//=============================================================================
// StringTokenizer::count_tokens
//=============================================================================
int StringTokenizer::count_tokens () const
{
  std::string::size_type prev_pos = 0;

  if (m_token_str.length () > 0)
  {
    int num_tokens = 0;
    std::string::size_type current_pos = 0;
    while (true)
    {
      if ((current_pos = m_token_str.find (m_delim_str, current_pos)) != std::string::npos)
      {
        num_tokens++;
        prev_pos = current_pos;
        current_pos += m_delim_str.length ();
      }
      else
        break;
    }
    return ++num_tokens;
  }
  else
  {
    return 0;
  }
}

//=============================================================================
// StringTokenizer::has_more_tokens
//=============================================================================
bool StringTokenizer::has_more_tokens () const
{
  return m_token_str.length() > 0;
}

//=============================================================================
// StringTokenizer::next_token
//=============================================================================
std::string StringTokenizer::next_token ()
{
  if (m_token_str.length () == 0)
    return "";

  std::string tStr = "";
  std::string::size_type pos = m_token_str.find(m_delim_str, 0);

  if (pos != std::string::npos)
  {
    tStr = m_token_str.substr(0, pos);
    m_token_str = m_token_str.substr(pos + m_delim_str.length (), m_token_str.length () - pos);
  }
  else
  {
    tStr = m_token_str.substr (0, m_token_str.length ());
    m_token_str = "";
  }

  return tStr;
}

//=============================================================================
// StringTokenizer::next_int_token
//=============================================================================
int StringTokenizer::next_int_token ()
{
  return ::atoi (next_token ().c_str ());
}

//=============================================================================
// StringTokenizer::next_long_token
//=============================================================================
long StringTokenizer::next_long_token ()
{
  return ::atol(next_token ().c_str ());
}

//=============================================================================
// StringTokenizer::next_fp_token
//=============================================================================
double StringTokenizer::next_fp_token ()
{
  return static_cast<double>(::atof(next_token ().c_str ()));
}

//=============================================================================
// StringTokenizer::next_token
//=============================================================================
std::string StringTokenizer::next_token (const std::string & delimiter)
{
  if (m_token_str.length () == 0)
    return "";

  std::string tStr = "";

  std::string::size_type pos = m_token_str.find (delimiter, 0);

  if (pos != std::string::npos)
  {
    tStr = m_token_str.substr (0, pos);
    m_token_str = m_token_str.substr (pos + delimiter.length (), m_token_str.length () - pos);
  }
  else
  {
    tStr = m_token_str.substr (0, m_token_str.length ());
    m_token_str = "";
  }

  return tStr;
}

//=============================================================================
// StringTokenizer::remaining_string
//=============================================================================
std::string StringTokenizer::remaining_string ()
{
  return m_token_str;
}

//=============================================================================
// StringTokenizer::filter_next_token
//=============================================================================
std::string StringTokenizer::filter_next_token (const std::string & filterStr)
{
  std::string str = next_token ();

  std::string::size_type current_pos = 0;

  while ((current_pos = str.find (filterStr, current_pos)) != std::string::npos)
    str.erase (current_pos, filterStr.length ());

  return str;
}

} //- namespace
