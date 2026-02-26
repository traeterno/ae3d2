#ifndef aeTypes
#define aeTypes

namespace ae
{

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef u32 usize;

typedef char i8;
typedef short i16;
typedef int i32;
typedef i32 isize;

typedef float f32;
typedef double f64;

const u8 U8_MAX = 0xFF;
const u16 U16_MAX = 0xFFFF;
const u32 U32_MAX = 0xFFFFFFFF;
const usize USIZE_MAX = 0xFFFFFFFF;

const i8 I8_MAX = 0x7F;
const i8 I8_MIN = 0x80;
const i16 I16_MAX = 0x7FFF;
const i16 I16_MIN = 0x8000;
const i32 I32_MAX = 0x7FFFFFFF;
const i32 I32_MIN = 0x80000000;
const isize ISIZE_MAX = 0xFFFFFFFF;
const isize ISIZE_MIN = 0x80000000;

};

#endif