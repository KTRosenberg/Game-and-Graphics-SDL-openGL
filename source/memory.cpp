#ifndef MEMORY_HPP
#define MEMORY_HPP

namespace mem {

struct Allocator {
    void* data;
    Fn_Memory_allocate       allocate;
    Fn_Memory_deallocate     deallocate;
    Fn_Memory_resize         resize;
    Fn_Memory_deallocate_all deallocate_all;
};

template <typename T>
T* alloc(void); 


template <typename T>
inline void zero(T* ptr);

}

#endif

#ifdef MEMORY_IMPLEMENTATION
#undef MEMORY_IMPLEMENTATION

namespace mem {

TYPE_T
T* alloc(void) 
{
    return (T*)xmalloc(sizeof(T));
}

TYPE_T
inline void set_zero(T* ptr)
{
    memset(ptr, 0, sizeof(T));
}

}

#endif