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

#ifndef _YAT_BITS_RECORD_H_
#define _YAT_BITS_RECORD_H_

// ============================================================================
// WIN32 SPECIFIC
// ============================================================================
#if defined (WIN32)
# pragma warning(disable:4800)
#endif

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include <vector>
#include <iomanip>
#include <yat/bitsstream/BitsStream.h>

namespace yat 
{

//=============================================================================
// INDENT COUNTER
//=============================================================================
extern YAT_DECL size_t kINDENT_COUNTER;

//=============================================================================
// MACRO: DUMP_INC_INDENT
//=============================================================================
#define DUMP_INC_INDENT yat::kINDENT_COUNTER += 2;

//=============================================================================
// MACRO: DUMP_DEC_INDENT
//=============================================================================
#define DUMP_DEC_INDENT yat::kINDENT_COUNTER -= 2;

//=============================================================================
// MACRO: INDENT_STREAM
//=============================================================================
#define INDENT_STREAM \
  _os << std::setw(yat::kINDENT_COUNTER) << std::left << std::setfill(' ') << ""

//=============================================================================
// MACRO: DUMP_CLASS
//=============================================================================
#define DUMP_CLASS(_CLASS_NAME_) \
  INDENT_STREAM << "-------------------------------------------------------------" << std::endl;

//=============================================================================
// MACRO: DUMP_END
//=============================================================================
#define DUMP_END \
  INDENT_STREAM << "-------------------------------------------------------------";

//=============================================================================
// MACRO: BEGIN_BITS_RECORD_DUMP
//=============================================================================
#define BEGIN_BITS_RECORD_DUMP(_CLASS_NAME_) \
  inline std::ostream& operator<< (std::ostream& _os, const _CLASS_NAME_& _c) \
  { \
    _os << #_CLASS_NAME_ << std::endl; \
    DUMP_INC_INDENT; \
    DUMP_CLASS(_CLASS_NAME_) \
    DUMP_INC_INDENT;
                    
//=============================================================================
// MACRO: DUMP_MEMBER
//=============================================================================
#define DUMP_MEMBER(_MEMBER_NAME_) \
  INDENT_STREAM << " - "  \
                << std::setw(20)  \
                << std::left \
                << std::setfill('.') \
                << #_MEMBER_NAME_ \
                << _c._MEMBER_NAME_ \
                << std::endl;

//=============================================================================
// MACRO: DUMP_SKIP_BITS
//=============================================================================
#define DUMP_SKIP_BITS(_BITS_TO_SKIP_) \
  INDENT_STREAM << " - "  \
                << std::setw(20) \
                << std::left \
                << std::setfill('.') \
                << "skipping" \
                << _BITS_TO_SKIP_ \
                << " bits" \
                << std::endl;

//=============================================================================
// MACRO: END_BITS_RECORD_DUMP
//=============================================================================
#define END_BITS_RECORD_DUMP(_CLASS_NAME_) \
    DUMP_DEC_INDENT; \
    DUMP_END \
    DUMP_DEC_INDENT; \
    INDENT_STREAM << std::right; \
    return _os; \
  }

//=============================================================================
// operator<< for  BitsSet - dump value 
//=============================================================================
inline std::ostream& operator<< (std::ostream& _os, const yat::BitsSet<8,unsigned char>& _brm)
{
  _os << static_cast<unsigned int>(_brm()) << " [" << _brm.to_string() << "]";
  return _os;
}

//=============================================================================
// operator<< for  BitsSet - dump value 
//=============================================================================
inline std::ostream& operator<< (std::ostream& _os, const yat::BitsSet<8,char>& _brm)
{
  _os << static_cast<int>(_brm()) << " [" << _brm.to_string() << "]";
  return _os;
}

//=============================================================================
// operator<< for  BitsSet - dump value 
//=============================================================================
template <size_t _n, typename _T> 
inline std::ostream& operator<< (std::ostream& _os, const yat::BitsSet<_n,_T>& _brm)
{
  _os << _brm() << " [" << _brm.to_string() << "]";
  return _os;
}

//=============================================================================
// MACRO: BEGIN_BITS_RECORD
//=============================================================================
#define BEGIN_BITS_RECORD(_CLASS_NAME_) \
  class _CLASS_NAME_ \
  { \
    friend std::ostream& operator<< (std::ostream&, const _CLASS_NAME_&); \
  public: \
    void dump () const \
    { \
      std::ostream& _os = std::cout; \
      INDENT_STREAM << *this << std::endl; \
    }

//=============================================================================
// MACRO: END_BITS_RECORD
//=============================================================================
#define END_BITS_RECORD(_CLASS_NAME_) \
  };
   
//=============================================================================
// MACRO: MEMBER
//=============================================================================   
#define MEMBER(_MEMBER_NAME_, _BITS_, _CPP_TYPE_) \
  yat::BitsSet<_BITS_, _CPP_TYPE_> _MEMBER_NAME_;

//=============================================================================
// MACRO: IGNORE_MEMBER
//=============================================================================   
#define IGNORE_MEMBER(_MEMBER_NAME_, _BITS_, _CPP_TYPE_)

//=============================================================================
// MACRO: BEGIN_BITS_RECORD_EXTRACTOR
//=============================================================================
#define BEGIN_BITS_RECORD_EXTRACTOR(_CLASS_NAME_) \
  inline void operator>> (yat::BitsStream& _bs, _CLASS_NAME_& _br) \
  { \

//=============================================================================
// MACRO: EXTRACT_MEMBER
//=============================================================================
#define EXTRACT_MEMBER(_MEMBER_NAME_) \
  _bs >> _br._MEMBER_NAME_;

//=============================================================================
// MACRO: SKIP_BITS
//=============================================================================
#define SKIP_BITS(_BITS_TO_SKIP_) \
  _bs.skip_bits(_BITS_TO_SKIP_);

//=============================================================================
// MACRO: END_BITS_RECORD_EXTRACTOR
//=============================================================================
#define END_BITS_RECORD_EXTRACTOR(_CLASS_NAME_) \
  }

} //- namespace 
   
#endif //- _BITS_RECORD_H_


