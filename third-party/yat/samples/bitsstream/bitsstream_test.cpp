/*!
 * \file     
 * \brief    A yat::BitsStream example.
 * \author   N. Leclercq, J. Malik - Synchrotron SOLEIL
 */


#include <iostream>
#include <limits>

#include <yat/bitsstream/BitsRecord.h>
#include <yat/memory/DataBuffer.h>

//-----------------------------------------------------------------------------
// BITS_RECORD: TestRecord
//-----------------------------------------------------------------------------
BEGIN_BITS_RECORD(TestRecord)
  //- 1 bit members
  MEMBER(bool_0, 1, bool)
  MEMBER(bool_1, 1, bool)
  MEMBER(bool_2, 1, bool)
  MEMBER(bool_3, 1, bool)
  MEMBER(bool_4, 1, bool)
  MEMBER(bool_5, 1, bool)
  MEMBER(bool_6, 1, bool)
  MEMBER(bool_7, 1, bool)
  //- 1 byte members
  MEMBER(neg_signed_char, 8, char)
  MEMBER(mid_signed_char, 8, char)
  MEMBER(pos_signed_char, 8, char)
  MEMBER(min_unsigned_char, 8, unsigned char)
  MEMBER(mid_unsigned_char, 8, unsigned char)
  MEMBER(max_unsigned_char, 8, unsigned char)
  //- 2 bytes members
  MEMBER(neg_signed_short, 16, short)
  MEMBER(mid_signed_short, 16, short)
  MEMBER(pos_signed_short, 16, short)
  MEMBER(min_unsigned_short, 16, unsigned short)
  MEMBER(mid_unsigned_short, 16, unsigned short)
  MEMBER(max_unsigned_short, 16, unsigned short)
  //- 4 bytes members
  MEMBER(neg_signed_long, 32, long)
  MEMBER(mid_signed_long, 32, long)
  MEMBER(pos_signed_long, 32, long)
  MEMBER(min_unsigned_long, 32, unsigned long)
  MEMBER(mid_unsigned_long, 32, unsigned long)
  MEMBER(max_unsigned_long, 32, unsigned long)
END_BITS_RECORD(TestRecord)

BEGIN_BITS_RECORD_EXTRACTOR(TestRecord)
  EXTRACT_MEMBER(bool_0)
  EXTRACT_MEMBER(bool_1)
  EXTRACT_MEMBER(bool_2)
  EXTRACT_MEMBER(bool_3)
  EXTRACT_MEMBER(bool_4)
  EXTRACT_MEMBER(bool_5)
  EXTRACT_MEMBER(bool_6)
  EXTRACT_MEMBER(bool_7)
  EXTRACT_MEMBER(neg_signed_char)
  EXTRACT_MEMBER(mid_signed_char)
  EXTRACT_MEMBER(pos_signed_char)
  EXTRACT_MEMBER(min_unsigned_char)
  EXTRACT_MEMBER(mid_unsigned_char)
  EXTRACT_MEMBER(max_unsigned_char)
  EXTRACT_MEMBER(neg_signed_short)
  EXTRACT_MEMBER(mid_signed_short)
  EXTRACT_MEMBER(pos_signed_short)
  EXTRACT_MEMBER(min_unsigned_short)
  EXTRACT_MEMBER(mid_unsigned_short)
  EXTRACT_MEMBER(max_unsigned_short)
  EXTRACT_MEMBER(neg_signed_long)
  EXTRACT_MEMBER(mid_signed_long)
  EXTRACT_MEMBER(pos_signed_long)
  EXTRACT_MEMBER(min_unsigned_long)
  EXTRACT_MEMBER(mid_unsigned_long)
  EXTRACT_MEMBER(max_unsigned_long)
END_BITS_RECORD_EXTRACTOR(TestRecord)

BEGIN_BITS_RECORD_DUMP(TestRecord)
  DUMP_MEMBER(bool_0)
  DUMP_MEMBER(bool_1)
  DUMP_MEMBER(bool_2)
  DUMP_MEMBER(bool_3)
  DUMP_MEMBER(bool_4)
  DUMP_MEMBER(bool_5)
  DUMP_MEMBER(bool_6)
  DUMP_MEMBER(bool_7)
  DUMP_MEMBER(neg_signed_char)
  DUMP_MEMBER(mid_signed_char)
  DUMP_MEMBER(pos_signed_char)
  DUMP_MEMBER(min_unsigned_char)
  DUMP_MEMBER(mid_unsigned_char)
  DUMP_MEMBER(max_unsigned_char)
  DUMP_MEMBER(neg_signed_short)
  DUMP_MEMBER(mid_signed_short)
  DUMP_MEMBER(pos_signed_short)
  DUMP_MEMBER(min_unsigned_short)
  DUMP_MEMBER(mid_unsigned_short)
  DUMP_MEMBER(max_unsigned_short)
  DUMP_MEMBER(neg_signed_long)
  DUMP_MEMBER(mid_signed_long)
  DUMP_MEMBER(pos_signed_long)
  DUMP_MEMBER(min_unsigned_long)
  DUMP_MEMBER(mid_unsigned_long)
  DUMP_MEMBER(max_unsigned_long)
END_BITS_RECORD_DUMP(TestRecord)

//-----------------------------------------------------------------------------
// forward declaration
//-----------------------------------------------------------------------------
void fill_buffers (yat::Buffer<unsigned char>& leb, yat::Buffer<unsigned char>& beb);

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------
int main (int argc, char* argv[])
{
  std::cout << "------------------------------------" << std::endl; 
  std::cout << "size of bool........" << sizeof(bool) * 8 << " bits" << std::endl;
  std::cout << "size of char........" << sizeof(char) * 8 << " bits" << std::endl;
  std::cout << "size of short......." << sizeof(short) * 8 << " bits" << std::endl;
  std::cout << "size of int........." << sizeof(int) * 8 << " bits" << std::endl;
  std::cout << "size of long........" << sizeof(long) * 8 << " bits" << std::endl;
  std::cout << "------------------------------------" << std::endl;

  try
  {
    //--------------------------------------------------------------------
    // HARDWARE DATA READING SIMULATION
    //--------------------------------------------------------------------
    //- big endian buffer
    yat::Buffer<unsigned char> big_endian_buffer;
    //- little endian buffer
    yat::Buffer<unsigned char> little_endian_buffer;
    //- each buffer will be filled according to its byte ordering
    //- this will allow to test the yat::BitsStream byte reordering
    fill_buffers(little_endian_buffer, big_endian_buffer);
  
    //--------------------------------------------------------------------
    // LITTLE ENDIAN TEST
    //--------------------------------------------------------------------
    //- instanciate the little endian BitsStream from data buffer
    yat::BitsStream little_endian_bs(little_endian_buffer.base(), 
                                     little_endian_buffer.size(), 
                                     yat::Endianness::BO_LITTLE_ENDIAN);
    //- push BitsStream content into a TestRecord then dump the result 
    TestRecord little_endian_tr;
    little_endian_bs >> little_endian_tr;
    std::cout << little_endian_tr << std::endl;

    //--------------------------------------------------------------------
    // BIG ENDIAN TEST
    //--------------------------------------------------------------------
    //- instanciate the big endian BitsStream from data buffer
    yat::BitsStream big_endian_bs(big_endian_buffer.base(),
                                  big_endian_buffer.size(),
                                  yat::Endianness::BO_BIG_ENDIAN);
    //- push BitsStream content into a TestRecord then dump the result 
    TestRecord big_endian_tr;
    big_endian_bs >> big_endian_tr;
    std::cout << big_endian_tr << std::endl;
  }
  catch (...)
  {
    std::cout << "Unknown exception caught" << std::endl;
  }

	return 0;  
}




//-----------------------------------------------------------------------------
// //- dump_val
//-----------------------------------------------------------------------------
template <typename _T> void dump_val (const char * _txt, const _T& _v)
{
  std::bitset<8 * sizeof(_T)> _v_bs(static_cast<unsigned long>(_v));
  std::cout << _txt 
            << _v
            << " ["
            << _v_bs.to_string()
            << "]"
            << std::endl;
  std::cout << "------------------------------------" << std::endl;
}

//-----------------------------------------------------------------------------
// fill_buffers
//-----------------------------------------------------------------------------
void fill_buffers (yat::Buffer<unsigned char>& leb, yat::Buffer<unsigned char>& beb)
{
  const size_t num_val_per_type = 3;

  const unsigned char bools(0xAA);

  char chars[num_val_per_type] = 
  { 
    std::numeric_limits<char>::min(), 
    std::numeric_limits<char>::max() / 2, 
    std::numeric_limits<char>::max() 
  }; 

  unsigned char uchars[num_val_per_type] = 
  { 
    std::numeric_limits<unsigned char>::min(),
    std::numeric_limits<unsigned char>::max() / 2,
    std::numeric_limits<unsigned char>::max()
  };

  short shorts[num_val_per_type] = 
  { 
    std::numeric_limits<short>::min(), 
    std::numeric_limits<short>::max() / 2,
    std::numeric_limits<short>::max()
  }; 

  unsigned short ushorts[num_val_per_type] = 
  { 
    std::numeric_limits<unsigned short>::min(),
    std::numeric_limits<unsigned short>::max() / 2,
    std::numeric_limits<unsigned short>::max()
  }; 

  long longs[num_val_per_type] = 
  { 
    std::numeric_limits<long>::min(), 
    std::numeric_limits<long>::max() / 2,
    std::numeric_limits<long>::max()
  }; 

  unsigned long ulongs[num_val_per_type] = 
  { 
    std::numeric_limits<unsigned long>::min(), 
    std::numeric_limits<unsigned long>::max() / 2,
    std::numeric_limits<unsigned long>::max()
  }; 

  const size_t buffer_size = sizeof(bools)
                           + sizeof(chars)
                           + sizeof(uchars)
                           + sizeof(shorts)
                           + sizeof(ushorts)
                           + sizeof(longs)
                           + sizeof(ulongs);

  leb.capacity(buffer_size);
  leb.force_length(buffer_size);
  beb.capacity(buffer_size);
  beb.force_length(buffer_size);

  size_t offset = 0;

  ::memcpy(leb.base() + offset, &bools, sizeof(bools));
  //- dump_val("leb[bools]= ", static_cast<int>(bools));

  ::memcpy(beb.base() + offset, &bools, sizeof(bools));
  //- dump_val("beb[bools]= ", static_cast<int>(bools));

  offset += sizeof(bools);

  ::memcpy(leb.base() + offset, chars, sizeof(chars));
  char * cp = reinterpret_cast<char*>(leb.base() + offset);
  //- dump_val("leb[char:0]= ", static_cast<int>(*(cp + 0)));
  //- dump_val("leb[char:1]= ", static_cast<int>(*(cp + 1)));
  //- dump_val("leb[char:2]= ", static_cast<int>(*(cp + 2)));

  ::memcpy(beb.base() + offset, chars, sizeof(chars));
  cp = reinterpret_cast<char*>(beb.base() + offset);
  //- dump_val("beb[char:0]= ", static_cast<int>(*(cp + 0)));
  //- dump_val("beb[char:1]= ", static_cast<int>(*(cp + 1)));
  //- dump_val("beb[char:2]= ", static_cast<int>(*(cp + 2)));

  offset += num_val_per_type * sizeof(char);

  ::memcpy(leb.base() + offset, uchars, sizeof(uchars));
  unsigned char * ucp = reinterpret_cast<unsigned char*>(leb.base() + offset);
  //- dump_val("leb[uchar:0]= ", static_cast<unsigned int>(*(ucp + 0)));
  //- dump_val("leb[uchar:1]= ", static_cast<unsigned int>(*(ucp + 1)));
  //- dump_val("leb[uchar:2]= ", static_cast<unsigned int>(*(ucp + 2)));

  ::memcpy(beb.base() + offset, uchars, sizeof(uchars));
  ucp = reinterpret_cast<unsigned char*>(beb.base() + offset);
  //- dump_val("beb[char:0]= ", static_cast<int>(*(ucp + 0)));
  //- dump_val("beb[char:1]= ", static_cast<int>(*(ucp + 1)));
  //- dump_val("beb[char:2]= ", static_cast<int>(*(ucp + 2)));

  offset += num_val_per_type * sizeof(unsigned char);

  ::memcpy(leb.base() + offset, shorts, sizeof(shorts));
  short * sp = reinterpret_cast<short*>(leb.base() + offset);
  //- dump_val("leb[shorts:0]= ", *(sp + 0));
  //- dump_val("leb[shorts:1]= ", *(sp + 1));
  //- dump_val("leb[shorts:2]= ", *(sp + 2));

  ::memcpy(beb.base() + offset, shorts, sizeof(shorts));
  sp = reinterpret_cast<short*>(beb.base() + offset);
  yat::Endianness::swap_2_array(reinterpret_cast<char*>(sp), 
                                reinterpret_cast<char*>(sp), 
                                num_val_per_type);
  //- dump_val("beb[shorts:0]= ", *(sp + 0));
  //- dump_val("beb[shorts:1]= ", *(sp + 1));
  //- dump_val("beb[shorts:1]= ", *(sp + 2));

  offset += num_val_per_type * sizeof(short);

  ::memcpy(leb.base() + offset, ushorts, sizeof(ushorts));
  unsigned short * usp = reinterpret_cast<unsigned short*>(leb.base() + offset);
  //- dump_val("leb[unsigned shorts:0]= ", *(usp + 0));
  //- dump_val("leb[unsigned shorts:1]= ", *(usp + 1));
  //- dump_val("leb[unsigned shorts:2]= ", *(usp + 2));

  ::memcpy(beb.base() + offset, ushorts, sizeof(ushorts));
  usp = reinterpret_cast<unsigned short*>(beb.base() + offset);
  yat::Endianness::swap_2_array(reinterpret_cast<char*>(usp), 
                                reinterpret_cast<char*>(usp), 
                                num_val_per_type);
  //- dump_val("beb[unsigned shorts:0]= ", *(usp + 0));
  //- dump_val("beb[unsigned shorts:1]= ", *(usp + 1));
  //- dump_val("beb[unsigned shorts:2]= ", *(usp + 2));

  offset += num_val_per_type * sizeof(unsigned short);

  ::memcpy(leb.base() + offset, longs, sizeof(longs));
  long * lp = reinterpret_cast<long*>(leb.base() + offset);
  //- dump_val("leb[long:0]= ", *(lp + 0));
  //- dump_val("leb[long:1]= ", *(lp + 1));
  //- dump_val("leb[long:2]= ", *(lp + 2));

  ::memcpy(beb.base() + offset, longs, sizeof(longs));
  lp = reinterpret_cast<long*>(beb.base() + offset);
  yat::Endianness::swap_4_array(reinterpret_cast<char*>(lp), 
                                reinterpret_cast<char*>(lp), 
                                num_val_per_type);
  //- dump_val("beb[long:0]= ", *(lp + 0));
  //- dump_val("beb[long:1]= ", *(lp + 1));
  //- dump_val("beb[long:2]= ", *(lp + 2));

  offset += num_val_per_type * sizeof(long);

  ::memcpy(leb.base() + offset, ulongs, sizeof(ulongs));
  unsigned long * ulp = reinterpret_cast<unsigned long*>(leb.base() + offset);
  //- dump_val("leb[unsigned long:0]= ", *(ulp + 0));
  //- dump_val("leb[unsigned long:1]= ", *(ulp + 1));
  //- dump_val("leb[unsigned long:2]= ", *(ulp + 2));

  ::memcpy(beb.base() + offset, ulongs, sizeof(ulongs));
  ulp = reinterpret_cast<unsigned long*>(beb.base() + offset);
  yat::Endianness::swap_4_array(reinterpret_cast<char*>(ulp), 
                                reinterpret_cast<char*>(ulp), 
                                num_val_per_type);
  //- dump_val("beb[unsigned long:0]= ", *(ulp + 0));
  //- dump_val("beb[unsigned long:1]= ", *(ulp + 1));
  //- dump_val("beb[unsigned long:2]= ", *(ulp + 2));
}
