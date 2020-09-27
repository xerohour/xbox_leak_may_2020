#ifndef __TYPES_H

typedef   signed    char s8;
typedef unsigned    char u8;
typedef   signed   short s16;
typedef unsigned   short u16;
typedef   signed    long s32;
typedef unsigned    long u32;
typedef   signed __int64 s64;
typedef unsigned __int64 u64;

#define MAX_U8   0xFF               //                   255
#define MAX_S8   0x7F               //                   127
#define MIN_S8  -0x80               //                  -128
#define MAX_U16  0xFFFF             //                 65535
#define MAX_S16  0x7FFF             //                 32767
#define MIN_S16 -0x8000             //                -32768
#define MAX_U32  0xFFFFFFFF         //            4294967295
#define MAX_S32  0x7FFFFFFF         //            2147483647
#define MIN_S32 -0x80000000         //           -2147483648
#define MAX_U64  0xFFFFFFFFFFFFFFFF // 184467440737XXXXXXXX5 - not sure :)
#define MAX_S64  0x7FFFFFFFFFFFFFFF //  922337203685XXXXXXX7 - not sure :)
#define MIN_S64 -0x8000000000000000 // -922337203685XXXXXXX8 - not sure :)

#endif
