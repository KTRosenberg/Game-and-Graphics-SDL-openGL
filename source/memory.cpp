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

struct Memory_Context {
    mem::Allocator allocator;
    mem::Allocator array_allocator;
};

inline mem::Allocator const& get_allocator(Memory_Context* mc);
inline mem::Allocator const& get_array_allocator(Memory_Context* mc);
inline mem::Allocator const& get_mem_context_allocator(void);
inline mem::Allocator const& get_mem_context_array_allocator(void);

void push_mem_context(Memory_Context* mc, mem::Allocator const& a);
void pop_mem_context(Memory_Context* mc);
void push_mem_context(void);
void pop_mem_context(void);

template <typename T>
T* alloc(void); 


template <typename T>
inline void zero(T* ptr);

}

#endif

#ifdef MEMORY_IMPLEMENTATION
#undef MEMORY_IMPLEMENTATION

namespace mem {

static Memory_Context mem_ctx = {
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

inline mem::Allocator const& get_allocator(Memory_Context* mc)
{
    return mc->allocator;
}
inline mem::Allocator const& get_array_allocator(Memory_Context* mc)
{
    return mc->array_allocator;
}
inline mem::Allocator const& get_mem_context_allocator(void)
{
    return mem_ctx.allocator;
}
inline mem::Allocator const& get_mem_context_array_allocator(void)
{
    return mem_ctx.array_allocator;
}

void push_mem_context(Memory_Context* mc, mem::Allocator const& a)
{
}
void pop_mem_context(Memory_Context* mc)
{
}
void push_mem_context(void)
{
}
void pop_mem_context(void)
{
}

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