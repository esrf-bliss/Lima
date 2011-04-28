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

#ifndef _YAT_DATA_BUFFER_H_
#define _YAT_DATA_BUFFER_H_


// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <string.h>
#include <yat/threading/SharedObject.h>

namespace yat 
{

// ============================================================================
//! A buffer abstraction class.  
// ============================================================================
//!  
//! This template class provides a buffer abstraction. 
//! <operator=> must be defined for template parameter T.
//! 
// ============================================================================
template <typename T> 
class Buffer 
{
public:
  
  /**
   * Constructor. 
   * @param  capacity the maximum number of element of type T that can be stored into the buffer 
   * @param  clear clears the associated memory (i.e. set each byte to 0), does nothing ortherwise 
   */
  Buffer (size_t capacity = 0, bool clear = false)
    throw (Exception);
 
  /*
   * Memory copy constructor. Memory is copied from base to base + length.
   * @param  length the number of element of type T to be copied into the buffer. 
   * @param  base address of the block to copy.
   */
  Buffer (size_t length, T *base)
    throw (Exception);

  /**
   * Copy constructor 
   * @param  buf the source buffer.
   */
  Buffer (const Buffer<T> &buf)
    throw (Exception);

  /**
   * Destructor. Release resources.
   */
  virtual ~Buffer ();

  /**
   * operator= 
   */
  Buffer<T>& operator= (const Buffer<T> &src);

  /**
   * operator=. Memory is copied from base to base + Buffer::length_. 
   * @param base address of the block to copy.
   */
  Buffer<T>& operator= (const T *base);

  /**
   * operator=. Fill the buffer with a specified value.
   * @param val the value.
   */
  Buffer<T>& operator= (const T &val);
   
  /**
   * Fills the buffer with a specified value.
   * @param val the value.
   */
  void fill (const T& val);
  
  /**
   * Clears buffer's content. This is a low level clear: set memory
   * from Buffer::base_ to Buffer::base_ + Buffer::length_ to 0.
   */
  void clear ();

  /**
   * Returns a reference to the _ith element. No bound error checking.
   * @param i index of the element to return.
   * @return a reference to the ith element.
   */
  T& operator[] (size_t i);

  /**
   * Returns a const reference to the _ith element. No bound error checking.
   * @param i index of the element to return.
   * @return a reference to the ith element.
   */
  const T& operator[] (size_t i) const;

  /**
   * Returns the size of each element in bytes.
   * @return sizeof(T).
   */
  size_t elem_size () const;

  /**
   * Returns the number of bytes currently stored into the buffer. 
   * @return size of buffer content in bytes.
   */
  size_t size () const;

  /**
   * Returns the number of element currently stored into the buffer. 
   * @return current number of elements. 
   */
  size_t length () const;
  
  /**
   * Returns artificially change the buffer length. 
   * @param new_length new buffer capacity in num of elements.
   *
   *\remark
   * If new_length is greater then buffer capacity, then buffer length is set to buffer capacity.
   */
  void force_length (size_t new_length);
  
  /**
   * Returns the buffer capacity (i.e. max num of elements that can be stored into the buffer). 
   * @return the buffer capacity. 
   */
  size_t capacity () const;
  
  /**
   * Set the buffer capacity to _capacity
   * @param new_capacity new buffer capacity in num of elements.
   * @param keep_content if set to true, the current buffer content is maintained but may be troncated.
   */
  virtual void capacity (size_t new_capacity, bool keep_content = false)
    throw (Exception);
  
  /**
   * Returns true is the buffer is empty, false otherwise. 
   */
  bool empty () const;

  /**
   * Returns the buffer base address. 
   * @return the buffer base address. 
   */
  T * base () const;

protected:

  /**
   * the buffer base address. 
   */
  T * base_;

  /**
   * maximum number of element of type T.
   */
  size_t capacity_;
  
  /**
   * current number of element of type T.
   */
  size_t length_;
};

// ============================================================================
//! An image container abstraction class.  
// ============================================================================
//!  
//! This template class provides an image container abstraction. 
//! <operator=> must be defined for template parameter T.
//! 
// ============================================================================
template <typename T>
class ImageBuffer : public yat::Buffer<T>
{
public:
  /**
   * Constructor. 
   * @param width the width of the image in pixels
   * @param height the height of the image in pixels
   */
  ImageBuffer (size_t width = 0, size_t height = 0)
    throw (Exception);
 
  /**
   * Constructor. 
   * @param width the width of the image in pixels
   * @param height the height of the image in pixels
   * @param base address of the block to copy.
   */
  ImageBuffer (size_t width, size_t height, T *base)
    throw (Exception);

  /**
   * Copy Constructor. 
   * @param im
   */
  ImageBuffer (const ImageBuffer<T>& im)
    throw (Exception);

  /**
   * Constructor. 
   * @param width the width of the image in pixels
   * @param height the height of the image in pixels
   * @param buf the buffer to copy data from
   */
  ImageBuffer (size_t width, size_t height, const yat::Buffer<T>& buf)
    throw (Exception);

  /**
   * Destructor
   */
  virtual ~ImageBuffer();

  /**
   * Accessor for the image width 
   */
  size_t width () const;

  /**
   * Accessor for the image height 
   */
  size_t height () const;

  /**
   * Setter for the dimensions of the image (set the length at width * height)
   */
  void set_dimensions (size_t width, size_t height);

  /**
   * change the dimensions of the array and preserve data
   */
  void resize (size_t new_width, size_t new_height)
    throw (Exception);

  /**
   * operator =
   */
  ImageBuffer<T>& operator= (const ImageBuffer<T> &src);

  /**
   * operator=. Memory is copied from base to base + ImageBuffer::length_. 
   * @param base address of the block to copy.
   */
  ImageBuffer<T>& operator= (const T *base);

  /**
   * operator=. Fill the buffer with a specified value.
   * @param val the value.
   */
  ImageBuffer<T>& operator= (const T &val);

protected:
  size_t width_;
  size_t height_;
};

// ============================================================================
//! A thread safe shared buffer abstraction class.
// ============================================================================
//!
//! This template class provides a thread safe shared buffer abstraction.
//! <operator=> must be defined for template parameter T.
//!
// ============================================================================
template <typename T> 
class SharedBuffer : public Buffer<T>, private SharedObject
{
protected:
  /**
   * Constructor. 
   * @param  capacity the maximum number of element of type T 
   *         that can be stored into the buffer 
   */
  SharedBuffer (size_t capacity = 0)
    throw (Exception);
 
  /**
   * Memory copy constructor. Memory is copied from _base to _base + _length.
   * @param  length the maximum number of element of type T 
   *         that can be stored into the buffer. 
   * @param  base address of the block to copy.
   */
  SharedBuffer (size_t length, T *base)
    throw (Exception);

  /**
   * Copy constructor. Use allocator associated with the source buffer.
   * @param  buf the source buffer.
   */
  SharedBuffer (const Buffer<T> &buf)
    throw (Exception);

  /**
   * Destructor. Release resources.
   */
  virtual ~SharedBuffer ();

  /**
   * Duplicate (shallow copy) this shared buffer
   */
  SharedBuffer * duplicate ()
    throw (Exception);
};

// ============================================================================
//! A ciruclar buffer abstraction class.  
// ============================================================================
//!  
//! This template class provides a  (write only) circular buffer abstraction. 
//! <operator=> must be defined for template parameter T.
//! 
// ============================================================================
template <typename T, typename L = yat::NullMutex>
class CircularBuffer
{
public:
  /**
   * Constructor. 
   */
  CircularBuffer ()
    throw (Exception);

  /**
   * Constructor. 
   */
  CircularBuffer (size_t capacity)
    throw (Exception);
  
  /**
   * Destructor. Release resources.
   */
  virtual ~CircularBuffer ();

  /**
   * Clears buffer's content.
   */
  virtual void clear ();

  /**
   * Fills the buffer with a specified value.
   * @param val the value.
   */
  void fill (const T& val);
  
  /**
   * Pushes the specified data into the circular buffer.
   * The data buffer must have 
   */
  void push (T new_element)
    throw (Exception);

  /**
   * Freezes the buffer. 
   * Any data pushed into a frozen circular buffer is silently ignored.  
   */
  void freeze ();

  /**
   * Unfreeze 
   * Data pushed into a frozen circular buffer is silently ignored (see CircularBuffer::freeze).  
   */
  void unfreeze ();

  /**
   * Returns the "chronologically ordered" circular buffer's content
   */
  const yat::Buffer<T> & ordered_data ()
    throw (Exception);

  /**
   * Set the buffer capacity to _capacity
   */
  virtual void capacity (size_t capacity)
    throw (Exception);

private:
  /**
   * Locling stategy
   */
  L lock_;
  
  /**
   * The write pointer
   */
  T * wp_; 

  /**
   * Frozen flag
   */
  bool frozen_;

  /**
   * The main buffer
   */
  yat::Buffer<T> data_;
  
  /**
   * The ordered buffer
   */
  yat::Buffer<T> ordered_data_;

  /**
   * Num of cycles
   */
  unsigned long num_cycles_;

  // = Disallow these operations.
  //--------------------------------------------
  CircularBuffer& operator= (const CircularBuffer&);
  CircularBuffer(const CircularBuffer&);
};

} // namespace

#if defined (YAT_INLINE_IMPL)
# include <yat/memory/DataBuffer.i>
#endif // YAT_INLINE_IMPL

#include <yat/memory/DataBuffer.tpp>

#endif // _DATA_BUFFER_H_



