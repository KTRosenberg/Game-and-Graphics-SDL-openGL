#ifndef ENTITY_H
    #define ENTITY_H

    #if !(UNITY_BUILD)
        #define COMMON_UTILS_CPP_IMPLEMENTATION
        #include "common_utils_cpp.hpp"

        #include "types.h"
    #endif

//  enums
    #define entity_begin() PASTE(f_, ENTITY_NAME), 
    #define entity_field(type__, name__)
    #define entity_field_pointer()
    #define entity_field_array()
    #define entity_function(name__, body__)
    #define entity_end(max_count)

    enum struct FIELD_TYPE {
        #define KIND(a, b) PASTE(f_, a),
        PROPERTY_KINDS
        #undef KIND
        #include "./entities/entity_includes.hpp"
        COUNT_FIELD_TYPE
    };


    #undef entity_begin
    #undef entity_field
    #undef entity_field_pointer
    #undef entity_field_array
    #undef entity_function
    #undef entity_end

#ifndef METATESTING

//  struct
    #define entity_begin() struct PASTE(ENTITY_NAME,) { 
    #define entity_field(type__, name__) type__ name__;
    #define entity_field_pointer()
    #define entity_field_array()
    #define entity_function(name__, body__) \
        void name__(void* args) { body__ }
    #define entity_end(max_count) }; \
        PASTE(ENTITY_NAME,) PASTE(ENTITY_NAME, _array)[ \
            (((max_count +0) > 0 && (max_count +0) <= 0xFFFFFFFF) ? (max_count + 0) : 1) \
        ];\
        constexpr unsigned int PASTE(ENTITY_NAME, _array_count) = (((max_count +0) > 0 && (max_count +0) <= 0xFFFFFFFF) ? (max_count +0) : 1);

    #include "./entities/entity_includes.hpp"

    #undef entity_begin
    #undef entity_field
    #undef entity_field_pointer
    #undef entity_field_array
    #undef entity_function
    #undef entity_end

//  field metadata
    struct FieldMetaData {
        const char* name;
        const i32   offset;
        FIELD_TYPE  type;
        bool        is_pointer;
        bool        is_array;
    };

    #define entity_begin() FieldMetaData PASTE(ENTITY_NAME, _meta_data)[] = { 
    #define entity_field(type__, name__) { #name__, offsetof(PASTE(ENTITY_NAME,), name__), FIELD_TYPE::f_##type__, false, false},
    #define entity_field_pointer()
    #define entity_field_array()
    #define entity_function(name__, body__)
    #define entity_end(max_count) };

    #include "./entities/entity_includes.hpp"

    #undef entity_begin
    #undef entity_field
    #undef entity_field_pointer
    #undef entity_field_array
    #undef entity_function
    #undef entity_end

//  arrays of struct metadata
    struct ArrayMetaData {
        const char* name;
        void*       array;
        const i32   element_size;
        void*       meta_array;
    };

    #define entity_begin() { STRING(PASTE(ENTITY_NAME, _array)), PASTE(ENTITY_NAME, _array), sizeof(PASTE(ENTITY_NAME,)), PASTE(ENTITY_NAME, _meta_data)
    #define entity_field(type__, name__)
    #define entity_field_pointer()
    #define entity_field_array()
    #define entity_function(name__, body__)
    #define entity_end(max_count) },

    ArrayMetaData meta_arrays[] = {
        #include "./entities/entity_includes.hpp"
    };

/*
ArrayMetaData Arrays[] =
{
    { "Monsters", Monsters, sizeof(Monster), MonsterMetaData }
};
*/

    #undef entity_begin
    #undef entity_field
    #undef entity_field_pointer
    #undef entity_field_array
    #undef entity_function
    #undef entity_end
    

#endif

#ifdef ENTITY_IMPLEMENTATION
    #undef ENTITY_IMPLEMENTATION

#endif
#endif
    /*

    enum FieldType
{
    FieldType_Int,
    FieldType_Float,
    FieldType_Bool,
};

struct FieldMetaData
{
    char*     Name;
    int       Offset;
    FieldType Type;
};

struct Monster
{
    float Speed;
    int   Health;
    int   Damage;
};

struct ArrayMetaData
{
    char* Name;
    void* Array;
    int   ElementSize;
    void* MetaArray;
};

Monster Monsters[1024];

FieldMetaData MonsterMetaData[] =
{
    { "Speed",  offsetof(Monster, Speed),  FieldType_Float) },
    { "Health", offsetof(Monster, Health), FieldType_Int)},
    { "Damage", offsetof(Monster, Damage), FieldType_Int)},
};

ArrayMetaData Arrays[] =
{
    { "Monsters", Monsters, sizeof(Monster), MonsterMetaData }
};


(note that i'm just improvising this - i don't have an actual codebase to lift it from - so it's just to illustrate basic ideas)(edited)
given the above, do you see how you can parse an input like

Monster[2].Health = 75


and get "Monster" from that, which you can then use to find the first element in the Arrays[] array?
once you have that element, you know the address of the Monsters[] array and the size of each element. then, from the parsed string, you also have the index 2.
so you know that the data for the monster you want to update is located at memory address Arrays[0].Array + Arrays[0].ElementSize * 2

*/