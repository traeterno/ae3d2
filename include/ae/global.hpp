#ifndef aeGlobal
#define aeGlobal

#include <string>
#include <json/value.h>

namespace ae
{

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;
typedef u64 usize;

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long i64;
typedef i64 isize;

typedef float f32;
typedef double f64;

};

namespace ae::fs
{
	std::string readText(std::string path);
	Json::Value readJSON(std::string path);
}

namespace ae::str
{
	void removeAll(std::string& base, std::string part);
}

#endif