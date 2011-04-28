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

//=============================================================================
//- Endianness.cpp
//=============================================================================
// abstraction.......Optimized bytes swapping 
// class.............Endianness
// original authors..Aniruddha Gokhale, Carlos O'Ryan, ... (ACE lib)
// hacker............Nicolas Leclercq - SOLEIL
//=============================================================================

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include <yat/bitsstream/Endianness.h>

#if !defined (YAT_INLINE_IMPL)
# include <yat/bitsstream/Endianness.i>
#endif // YAT_INLINE_IMPL

namespace yat 
{

//=============================================================================
// BASE TYPE FOR POINTERS ARITH
//=============================================================================
typedef unsigned long ptr_arith_t;

//=============================================================================
// ALIGN_BINARY
//=============================================================================
#define ALIGN_BINARY(ptr, alignment) \
    ((ptr + ((ptr_arith_t)((alignment)-1))) & (~((ptr_arith_t)((alignment)-1))))

//=============================================================================
// PTR_ALIGN_BINARY
//=============================================================================
#define PTR_ALIGN_BINARY(ptr, alignment) \
        ((const char* const) ALIGN_BINARY (((ptr_arith_t) (ptr)), (alignment)))

//=============================================================================
// HOST ENDIANNESS
//=============================================================================
#if YAT_LITTLE_ENDIAN_PLATFORM == 1
  const Endianness::ByteOrder 
    Endianness::host_endianness = Endianness::BO_LITTLE_ENDIAN; 
  const Endianness::ByteOrder 
    Endianness::not_host_endianness = Endianness::BO_BIG_ENDIAN; 
#else
  const Endianness::ByteOrder 
    Endianness::host_endianness = Endianness::BO_BIG_ENDIAN; 
  const Endianness::ByteOrder 
    Endianness::not_host_endianness = Endianness::BO_LITTLE_ENDIAN; 
#endif

//=============================================================================
// Endianness::swap_2_array
//=============================================================================
void Endianness::swap_2_array (const char* orig, char* target, size_t n)
{
  const char* const o4 = PTR_ALIGN_BINARY(orig, 4);
  // this is an _if_, not a _while_. The mistmatch can only be by 2.
  if (orig != o4)
    {
      Endianness::swap_2 (orig, target);
      orig += 2;
      target += 2;
      --n;
    }

  if (n == 0)
    return;

  const char* const end = orig + 2 * (n & (~3));

  if (target == PTR_ALIGN_BINARY(target, 4))
  {
    while (orig < end)
    {
#if YAT_HAS_PENTIUM == 1
# if defined(__GNUG__)
    unsigned int a = *reinterpret_cast<const unsigned int*>(orig);
    unsigned int b = *reinterpret_cast<const unsigned int*>(orig + 4);
    asm( "bswap %1"      : "=r" (a) : "0" (a) );
    asm( "bswap %1"      : "=r" (b) : "0" (b) );
    asm( "rol $16, %1"   : "=r" (a) : "0" (a) );
    asm( "rol $16, %1"   : "=r" (b) : "0" (b) );
    *reinterpret_cast<unsigned int*>(target) = a;
    *reinterpret_cast<unsigned int*>(target + 4) = b;
# elif defined(_MSC_VER)
    __asm mov ecx, orig;
    __asm mov edx, target;
    __asm mov eax, [ecx];
    __asm mov ebx, 4[ecx];
    __asm bswap eax;
    __asm bswap ebx;
    __asm rol eax, 16;
    __asm rol ebx, 16;
    __asm mov [edx], eax;
    __asm mov 4[edx], ebx;
# endif
#endif
      orig += 8;
      target += 8;
    }
  }
  else
  {
    // We're out of luck. We have to write in 2 byte chunks.
    while (orig < end)
    {
#if YAT_HAS_PENTIUM == 1
# if defined(__GNUG__)
    unsigned int a = *reinterpret_cast<const unsigned int*>(orig);
    unsigned int b = *reinterpret_cast<const unsigned int*>(orig + 4);
    asm( "bswap %1" : "=r" (a) : "0" (a) );
    asm( "bswap %1" : "=r" (b) : "0" (b) );
    // We're little endian.
    *reinterpret_cast<unsigned short*>(target + 2) = (unsigned short)(a & 0xffff);
    *reinterpret_cast<unsigned short*>(target + 6) = (unsigned short)(b & 0xffff);
    asm( "shrl $16, %1" : "=r" (a) : "0" (a) );
    asm( "shrl $16, %1" : "=r" (b) : "0" (b) );
    *reinterpret_cast<unsigned short*>(target + 0) = (unsigned short)(a & 0xffff);
    *reinterpret_cast<unsigned short*>(target + 4) = (unsigned short)(b & 0xffff);
# elif defined(_MSC_VER)
    __asm mov ecx, orig;
    __asm mov edx, target;
    __asm mov eax, [ecx];
    __asm mov ebx, 4[ecx];
    __asm bswap eax;
    __asm bswap ebx;
    // We're little endian.
    __asm mov 2[edx], ax;
    __asm mov 6[edx], bx;
    __asm shr eax, 16;
    __asm shr ebx, 16;
    __asm mov 0[edx], ax;
    __asm mov 4[edx], bx;
# endif
#endif
      orig += 8;
      target += 8;
    }
  }

  // (n & 3) == (n % 4).
  switch (n&3) 
  {
    case 3:
      Endianness::swap_2 (orig, target);
      orig += 2;
      target += 2;
    case 2:
      Endianness::swap_2 (orig, target);
      orig += 2;
      target += 2;
    case 1:
      Endianness::swap_2 (orig, target);
  }
}

//=============================================================================
// Endianness::swap_4_array
//=============================================================================
void Endianness::swap_4_array (const char* orig, char* target, size_t n)
{
  if (n == 0)
    return;

  // (n & (~3)) is the greatest multiple of 4 not bigger than n.
  // In the while loop, orig will move over the array by 16 byte
  // increments (4 elements of 4 bytes).
  // ends marks our barrier for not falling outside.
  const char* const end = orig + 4 * (n & (~3));

  while (orig < end)
  {
#if YAT_HAS_PENTIUM == 1
# if defined(__GNUG__)
    register unsigned int a = *reinterpret_cast<const unsigned int*>(orig);
    register unsigned int b = *reinterpret_cast<const unsigned int*>(orig + 4);
    register unsigned int c = *reinterpret_cast<const unsigned int*>(orig + 8);
    register unsigned int d = *reinterpret_cast<const unsigned int*>(orig + 12);
    asm ("bswap %1" : "=r" (a) : "0" (a));
    asm ("bswap %1" : "=r" (b) : "0" (b));
    asm ("bswap %1" : "=r" (c) : "0" (c));
    asm ("bswap %1" : "=r" (d) : "0" (d));
    *reinterpret_cast<unsigned int*>(target) = a;
    *reinterpret_cast<unsigned int*>(target + 4) = b;
    *reinterpret_cast<unsigned int*>(target + 8) = c;
    *reinterpret_cast<unsigned int*>(target + 12) = d;
# elif defined(_MSC_VER) 
    __asm mov eax, orig
    __asm mov esi, target
    __asm mov edx, [eax]
    __asm mov ecx, 4[eax]
    __asm mov ebx, 8[eax]
    __asm mov eax, 12[eax]
    __asm bswap edx
    __asm bswap ecx
    __asm bswap ebx
    __asm bswap eax
    __asm mov [esi], edx
    __asm mov 4[esi], ecx
    __asm mov 8[esi], ebx
    __asm mov 12[esi], eax
# endif
#endif
    orig += 16;
    target += 16;
  }

  // (n & 3) == (n % 4).
  switch (n&3) 
  {
    case 3:
      Endianness::swap_4 (orig, target);
      orig += 4;
      target += 4;
    case 2:
      Endianness::swap_4 (orig, target);
      orig += 4;
      target += 4;
    case 1:
      Endianness::swap_4 (orig, target);
  }
}

//=============================================================================
// Endianness::swap_8_array
//=============================================================================
void Endianness::swap_8_array (const char* orig, char* target, size_t n)
{
  if (n == 0)
    return;

  const char* const end = orig + 8 * n;

  while (orig < end)
  {
    Endianness::swap_8(orig, target);
    orig += 8;
    target += 8;
  }
}

//=============================================================================
// Endianness::swap_16_array
//=============================================================================
void Endianness::swap_16_array (const char* orig, char* target, size_t n)
{
  if (n == 0)
    return;

  const char* const end = orig + 16 * n;

  while (orig < end)
  {
    Endianness::swap_16(orig, target);
    orig += 16;
    target += 16;
  }
}

} //- namespace 

