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