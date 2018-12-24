#include <stdlib.h>
#include <string>

typedef int8_t   int8;
typedef int16_t  int16;
typedef int32_t  int32;
typedef int64_t  int64;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float    float32; 
typedef double   float64;

typedef int8     i8;
typedef int16    i16;
typedef int32    i32;
typedef int64    i64;

typedef uint8    u8;
typedef uint16   u16;
typedef uint32   u32;
typedef uint64   u64;

typedef float32  f32; 
typedef float64  f64;

typedef u64 usize;

typedef void (*Proc_Properties)(Field* data);

typedef char* byteptr;

#define PRE_A PROP_
#define PRE_B f_

#define PROPERTY_KINDS \
   KIND(PROP_, f_, i64) \
   KIND(PROP_, f_, u64) \
   KIND(PROP_, f_, usize) \
   KIND(PROP_, f_, f64) \
   KIND(PROP_, f_, i32) \
   KIND(PROP_, f_, u32) \
   KIND(PROP_, f_, f32) \
   KIND(PROP_, f_, i16) \
   KIND(PROP_, f_, u16) \
   KIND(PROP_, f_, i8) \
   KIND(PROP_, f_, u8) \
   KIND(PROP_, f_, bool) \
   KIND(PROP_, f_, char) \
   KIND(PROP_, f_, string) \
   KIND(PROP_, f_, Functor) \
   KIND(PROP_, f_, Ptr_PropertyContainer) \
   KIND(PROP_, f_, byteptr) \



enum struct PROPERTY_TYPE {
    #define SEP , 
    #define KIND(a, b, c) a##c SEP
    PROPERTY_KINDS
    #undef PREFIX
    #undef SEP
    #undef KIND
    COUNT_PROPERTY_TYPE
};

union Field {
    #define SEP ; 
    #define KIND(a, b, c) c b##c SEP
    PROPERTY_KINDS
    #undef PREFIX
    #undef SEP
    #undef KIND
    COUNT_FIELD
};

struct Functor {
    Proc_Properties proc;
    Field data;
};


struct Property {
    std::string name;

    PROPERTY_TYPE type;
    Field data;

    usize entity_id;
};

#define MAX_PROPERTIES_OBJECTS (256)
#define MAX_PROPERTIES (16)
struct PropertyContainer {  
   Property array[MAX_PROPERTIES];
   usize count;
};

typedef PropertyContainer* Ptr_PropertyContainer;

struct Entity {
    std::string name;
    Property array[MAX_PROPERTIES];
    usize count;
}

int main()
{
    X x;

    return EXIT_SUCCESS;
}
