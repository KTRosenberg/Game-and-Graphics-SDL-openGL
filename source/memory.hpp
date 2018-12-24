#ifndef MEMORY_HPP
#define MEMORY_HPP

namespace mem {

struct Allocator {
    void* data;
    Fn_Memory_Allocator allocate;
    Fn_Memory_Deallocator free;
};

template <typename T>
T* alloc(void); 


template <typename T>
inline void zero(T* ptr);

}



#endif

#ifdef MEMORY_IMPLEMENTATION
#undef MEMORY_IMPLEMENTATION

#include "memory.cpp"

#endif