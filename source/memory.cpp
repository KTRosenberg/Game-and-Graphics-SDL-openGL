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

struct System_Context {
    mem::Allocator allocator;
    mem::Allocator array_allocator;
};

extern System_Context sys_ctx;

inline mem::Allocator const& get_allocator(System_Context* sc)
{
    return sc->allocator;
}
inline mem::Allocator const& get_array_allocator(System_Context* sc)
{
    return sc->array_allocator;
}
inline mem::Allocator const& get_sys_context_allocator(void)
{
    return sys_ctx.allocator;
}
inline mem::Allocator const& get_sys_context_array_allocator(void)
{
    return sys_ctx.array_allocator;
}

void push_sys_context(System_Context* mc, mem::Allocator const& a);
void pop_sys_context(System_Context* mc);
void push_sys_context(void);
void pop_sys_context(void);

template <typename T>
T* alloc(void); 


TYPE_T
inline void set_zero(T* ptr)
{
    memset(ptr, 0, sizeof(T));
}

}

#endif

#ifdef MEMORY_IMPLEMENTATION
#undef MEMORY_IMPLEMENTATION

namespace mem {

System_Context sys_ctx = {
    .allocator = {
        .allocate = mem_alloc,
        .deallocate = mem_free,
        .resize = mem_resize,
        .deallocate_all = mem_free_all
    },
    .array_allocator = {
        .allocate = mem_alloc,
        .deallocate = mem_free,
        .resize = mem_resize,
        .deallocate_all = mem_free_all
    },
};

void push_sys_context(System_Context* sc, mem::Allocator const& a)
{
}
void pop_sys_context(System_Context* sc)
{
}
void push_sys_context(void)
{
}
void pop_sys_context(void)
{
}

TYPE_T
T* alloc(void) 
{
    return (T*)xmalloc(sizeof(T));
}

}

#endif