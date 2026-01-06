// Nonnon General Types
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_TYPE
#define _H_NONNON_NEUTRAL_TYPE




// [Needed] : Mac : long will be 64-bit

#ifdef __APPLE__


#include <stdint.h>


#define s8 int8_t
#define s16 int16_t
#define s32 int32_t
#define s64 int64_t

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t


#else  // #ifdef __APPLE__


#define s8           char
#define s16          short
#define s32          long
#define s64          long long

#define u8  unsigned char
#define u16 unsigned short
#define u32 unsigned long
#define u64 unsigned long long


#endif // #ifdef __APPLE__




// [!] : don't use "%lld" directly : use n_string_int2str() to convert into a string

#define n_type_gfx s32
#define n_type_int s64

// [!] : for example : 256 x 256 = 65025, so SHRT_MAX is insufficient
// [x] : Mac : array index type is not u64, maybe s32

#define n_type_index s64


// [!] : float is faster than double but very difficult to implement

//#define n_type_real float
#define n_type_real double




#endif // _H_NONNON_NEUTRAL_TYPE

