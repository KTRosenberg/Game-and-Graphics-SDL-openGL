# 1 "prop.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 169 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "prop.c" 2
//#include <stdio.h>
//#include <stdlib.h>

typedef void (*Fn_PropertyStruct)(void);

enum struct PROPERTY_TYPE : u8 {
   PROP_i64,
   PROP_u64,
   PROP_usize,
   PROP_f64,
   PROP_i32,
   PROP_u32,
   PROP_f32,
   PROP_i16,
   PROP_u16,
   PROP_i8,
   PROP_u8,
   PROP_bool,
   PROP_char,
   PROP_string,
   PROP_function,
   COUNT_PROPERTY_TYPE
};

struct Functor {
   void* data,
   Fn_PropertyStruct function;
};








