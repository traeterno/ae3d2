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

const u8 u8max = 0xFF;
const u16 u16max = 0xFFFF;
const u32 u32max = 0xFFFFFFFF;
const usize usizemax = 0xFFFFFFFF;

const i8 i8max = 0x7F;
const i8 i8min = 0x80;
const i16 i16max = 0x7FFF;
const i16 i16min = 0x8000;
const i32 i32max = 0x7FFFFFFF;
const i32 i32min = 0x80000000;
const isize isizemax = 0xFFFFFFFF;
const isize isizemin = 0x80000000;

};

#endif