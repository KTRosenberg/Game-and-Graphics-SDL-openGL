#ifndef ARRAY_HPP
#define ARRAY_HPP

#if !(UNITY_BUILD)
#include "common_utils.h"
#include "memory.cpp"
#endif



TYPE_T
struct Array_Slice {
    T* data;
    usize count;
};

TYPE_T_SIZE_N
struct Array {
    usize count;
    T     data[N];

    inline operator T*(void) 
    {
        return this->data;
    }
    
    inline operator Array_Slice<T>(void)
    {
        return Array_Slice<T>{this->data, this->count};
    }

    inline T& operator[](usize index)
    {
        ASSERT_MSG(index >= 0 && index < N, "Index %llu is out-of-bounds ranges 0..<%llu", index, N);
        return this->data[index];
    }

    inline const T& operator[](usize index) const 
    {
        ASSERT_MSG(index >= 0 && index < N, "Index %llu is out-of-bounds ranges 0..<%llu", index, N);
        
        return this->data[index];
    }

    typedef T* iterator;
    typedef const T* const_iterator;
    iterator begin_ptr(void) { return &this->data[0]; }
    iterator next_free_slot(void) { return &this->data[this->count]; }
    iterator end_ptr(void) { return next_free_slot(); }
    iterator bound(void) { return &this->data[N]; }
    usize cap(void) { return N; }


    // member procedure API

    inline Array_Slice<T> slice(usize i, usize j);

    inline usize byte_length(void);

    inline void append(T val);

    inline void append(T* val);

    inline void push(T val);

    inline void push(T* val);


    inline T* peek(void);


    inline T pop(void);


    inline bool is_empty(void);


    inline void Array_init(void);

    inline void init(void);

    inline void swap(usize i, usize j);
};

TYPE_T_SIZE_N
inline usize byte_length(Array<T, N>* array);

TYPE_T_SIZE_N
inline void append(Array<T, N>* array, T val);
TYPE_T_SIZE_N
inline void append(Array<T, N>* array, T* val);
TYPE_T_SIZE_N
inline void push(Array<T, N>* array, T val);
TYPE_T_SIZE_N
inline void push(Array<T, N>* array, T* val);

TYPE_T_SIZE_N
inline T* peek(Array<T, N>* array);

TYPE_T_SIZE_N
inline T pop(Array<T, N>* array);

TYPE_T_SIZE_N
inline bool is_empty(Array<T, N>* array);

TYPE_T_SIZE_N
inline void Array_init(Array<T, N>* array);
TYPE_T_SIZE_N
inline void init(Array<T, N>* array);


TYPE_T_SIZE_N
inline void swap(Array<T, N>* array, usize i, usize j);

TYPE_T_SIZE_N
void ordered_remove(Array<T, N>* array, usize index);

TYPE_T_SIZE_N
void unordered_remove(Array<T, N>* array, usize index);

namespace mem {
    inline mem::Allocator const& get_sys_context_array_allocator(void);
}

#define ARRAY_GROW_FORMULA(x) (2*(x) + 8)
static_assert(ARRAY_GROW_FORMULA(0) > 0, "ARRAY_GROW FORMULA(0) SHOULD NOT BE 0");

TYPE_T 
struct Dynamic_Array {
    usize          count;
    usize          cap;
    T*             data;
    mem::Allocator allocator;


    inline operator T*(void) 
    {
        return this->data;
    }
    
    inline operator Array_Slice<T>(void)
    {
        return Array_Slice<T>{this->data, this->count};
    }

    inline T& operator[](usize index)
    {
        ASSERT_MSG(index >= 0 && index < this->count, "Index %llu is out-of-bounds ranges 0..<%llu", index, this->count);
        return this->data[index];
    }

    inline const T& operator[](usize index) const 
    {
        ASSERT_MSG(index >= 0 && index < this->count, "Index %llu is out-of-bounds ranges 0..<%llu", index, this->count);
        
        return this->data[index];
    }

    typedef T* iterator;
    typedef const T* const_iterator;
    iterator next_free_slot(void) { return &this->data[this->count]; }
    iterator begin_ptr(void) { return &this->data[0]; }
    iterator end_ptr(void) { return next_free_slot(); }


    // member procedure API

    inline Array_Slice<T> slice(usize i, usize j);

    inline usize byte_length(void);

    inline void append(T val);

    inline void append(T* val);

    inline void push(T val);

    inline void push(T* val);


    inline T* peek(void);


    inline T pop(void);


    inline bool is_empty(void);

    inline void init(void);

    inline void swap(usize i, usize j);

    static Dynamic_Array<T> make(mem::Allocator const& a);
    static Dynamic_Array<T> make(void);

    static Dynamic_Array<T> make(mem::Allocator const& a, usize count);
    static Dynamic_Array<T> make(usize count);

    static Dynamic_Array<T> make(mem::Allocator const& a, usize count, usize capacity);
    static Dynamic_Array<T> make(usize count, usize capacity);

    static Dynamic_Array<T> make_from_ptr(T* data, usize count, usize capacity);

    // static Dynamic_Array<T> make()
    // static Dynamic_Array<T> make()
    // static Dynamic_Array<T> make()
};

TYPE_T
inline usize byte_length(Dynamic_Array<T>* array);

TYPE_T
inline void append(Dynamic_Array<T>* array, T val);
TYPE_T
inline void append(Dynamic_Array<T>* array, T* val);
TYPE_T
inline void push(Dynamic_Array<T>* array, T val);
TYPE_T
inline void push(Dynamic_Array<T>* array, T* val);

TYPE_T
inline T* peek(Dynamic_Array<T>* array);

TYPE_T
inline T pop(Dynamic_Array<T>* array);

TYPE_T
inline void reset_back(Dynamic_Array<T>* array);

TYPE_T
inline bool is_empty(Dynamic_Array<T>* array);

TYPE_T
void init(Dynamic_Array<T>* array);
TYPE_T
void init(Dynamic_Array<T>* array, mem::Allocator const& a);

TYPE_T
void init(Dynamic_Array<T>* array, usize count);
TYPE_T
void init(Dynamic_Array<T>* array, mem::Allocator const& a, usize count);

template <typename T>
void init(Dynamic_Array<T>* array, mem::Allocator const& a, usize count, usize capacity);
TYPE_T
void init(Dynamic_Array<T>* array, usize count, usize capacity);


TYPE_T
inline void swap(Dynamic_Array<T>* array, usize i, usize j);


TYPE_T
void deallocate(Dynamic_Array<T>* array);

TYPE_T
void clear(void);

TYPE_T
void reserve(Dynamic_Array<T>* array, usize cap);

TYPE_T
void array__grow(Dynamic_Array<T>* array, usize min_cap);

TYPE_T
void resize(Dynamic_Array<T>* array, usize count);

TYPE_T
void set_capacity(Dynamic_Array<T>* array, usize capacity);

TYPE_T
void ordered_remove(Dynamic_Array<T>* array, usize index);

TYPE_T
void unordered_remove(Dynamic_Array<T>* array, usize index);

TYPE_T
void copy(Dynamic_Array<T>* array, Dynamic_Array<T> const& data, isize offset);
TYPE_T
void copy(Dynamic_Array<T>* array, Dynamic_Array<T> const& data, isize offset, usize count);


#endif // ARRAY_HPP

#ifdef ARRAY_IMPLEMENTATION
#undef ARRAY_IMPLEMENTATION

// floating procedure API ////////////////////////////////////////////

TYPE_T
inline Array_Slice<T> slice(Array_Slice<T>* array, usize i, usize j)
{
    ASSERT_MSG(i <= j && j < array->count, "Slice out-of-bounds >= %llu", array->count);

    return Array_Slice<T>{&array->data[i], j - i};
}

TYPE_T_SIZE_N
inline Array_Slice<T> slice(Array<T, N>* array, usize i, usize j)
{
    ASSERT_MSG(i <= j && j < array->count, "Slice out-of-bounds >= %llu", array->count);
    return Array_Slice<T>{&array->data[i], j - i};
}

TYPE_T_SIZE_N
inline usize byte_length(Array<T, N>* array)
{
    return sizeof(T) * N;
}

TYPE_T_SIZE_N
inline void append(Array<T, N>* array, T val)
{
    ASSERT_MSG(array->count < N, "Array has reached maximum capacity %llu", N);

    array->data[array->count] = val;
    array->count += 1;
}

TYPE_T_SIZE_N
inline void append(Array<T, N>* array, T* val)
{
    ASSERT_MSG(array->count < N, "Array has reached maximum capacity %llu", array->count);

    array->data[array->count] = *val;
    array->count += 1;
}

TYPE_T_SIZE_N
inline void push(Array<T, N>* array, T val)
{
    ASSERT_MSG(array->count < N, "Array has reached maximum capacity %llu", array->count);

    array->data[array->count] = val;
    array->count += 1;
}

TYPE_T_SIZE_N
inline void push(Array<T, N>* array, T* val)
{
    ASSERT_MSG(array->count < N, "Array has reached maximum capacity %llu", array->count);

    array->data[array->count] = *val;
    array->count += 1;
}

TYPE_T_SIZE_N
inline T* peek(Array<T, N>* array)
{
    if (array->count == 0) {
        return nullptr;
    }
    return &array->data[array->count - 1];
}

TYPE_T_SIZE_N
inline T pop(Array<T, N>* array)
{
    ASSERT_MSG(array->count != 0, "cannot pop if empty");

    array->count -= 1;
    return array->data[array->count];
}

TYPE_T_SIZE_N
inline void reset_back(Array<T, N>* array)
{
    array->count = 0;
}

TYPE_T_SIZE_N
inline bool is_empty(Array<T, N>* array)
{
    return (array->count == 0);
}

TYPE_T_SIZE_N
inline void Array_init(Array<T, N>* array)
{
    array->count = 0;
}

TYPE_T_SIZE_N
inline void init(Array<T, N>* array)
{
    Array_init(array);
}

TYPE_T_SIZE_N
inline void swap(Array<T, N>* array, usize i, usize j)
{
    T val_at_i = array[i];
    array[i] = array[j];
    array[j] = val_at_i;
}

TYPE_T_SIZE_N
void ordered_remove(Array<T, N>* array, usize index)
{
    ASSERT(0 <= index && index < array->count);

    usize bytes = sizeof(T) * (array->count - (index + 1));
    memmove(array->data + index, array->data + index + 1, bytes);
    array->count -= 1;
}

TYPE_T_SIZE_N
void unordered_remove(Array<T, N>* array, usize index)
{
    ASSERT(0 <= index && index < array->count);

    usize n = array->count - 1;

    memmove(array->data + index, array->data + n, sizeof(T));

    pop(array);
}

// member procedure API ///////////////////////////////////////

TYPE_T_SIZE_N
inline Array_Slice<T> Array<T, N>::slice(usize i, usize j)
{
    return ::slice(this, i, j);
}

TYPE_T_SIZE_N
inline usize Array<T, N>::byte_length(void)
{
    return ::byte_length(this);
}

TYPE_T_SIZE_N
inline void Array<T, N>::append(T val)
{
    ::append(this, val);
}

TYPE_T_SIZE_N
inline void Array<T, N>::append(T* val)
{
    ::append(this, val);
}

TYPE_T_SIZE_N
inline void Array<T, N>::push(T val)
{
    ::push(this, val);
}

TYPE_T_SIZE_N
inline void Array<T, N>::push(T* val)
{
    ::push(this, val);
}

TYPE_T_SIZE_N
inline T* Array<T, N>::peek(void)
{
    return ::peek(this);
}

TYPE_T_SIZE_N
inline T Array<T, N>::pop(void)
{
    return ::pop(this);
}

TYPE_T_SIZE_N
inline bool Array<T, N>::is_empty(void)
{
    return ::is_empty(this);
}

TYPE_T_SIZE_N
inline void Array<T, N>::init(void)
{
    ::Array_init(this);
}

TYPE_T_SIZE_N
inline void Array<T, N>::swap(usize i, usize j)
{
    ::swap(this, i, j);
}


// floating procedure API ////////////////////////////////////////////

TYPE_T
inline Array_Slice<T> slice(Dynamic_Array<T>* array, usize i, usize j)
{
    ASSERT_MSG(i <= j && j < array->count, "Slice out-of-bounds >= %llu", array->count);

    return Array_Slice<T>{&array->data[i], j - i};
}

TYPE_T
inline usize byte_length(Dynamic_Array<T>* array)
{
    return sizeof(T) * array->count;
}

TYPE_T
inline void append(Dynamic_Array<T>* array, T val)
{
    array->data[array->count] = val;
    array->count += 1;
}

TYPE_T
inline void append(Dynamic_Array<T>* array, T* val)
{
    array->data[array->count] = *val;
    array->count += 1;
}

TYPE_T
inline void push(Dynamic_Array<T>* array, T val)
{
    if (array->cap < array->count + 1) {
        array__grow(array, 0);
    }

    array->data[array->count] = val;
    array->count += 1;
}

TYPE_T
inline void push(Dynamic_Array<T>* array, T* val)
{
    if (array->cap < array->count + 1) {
        array__grow(array, 0);
    }

    array->data[array->count] = *val;
    array->count += 1;
}

TYPE_T
inline T* peek(Dynamic_Array<T>* array)
{
    if (array->count == 0) {
        return nullptr;
    }
    return &array->data[array->count - 1];
}

TYPE_T
inline T pop(Dynamic_Array<T>* array)
{
    ASSERT_MSG(array->count != 0, "cannot pop if empty");

    array->count -= 1;
    return array->data[array->count];
}

TYPE_T
inline bool is_empty(Dynamic_Array<T>* array)
{
    return (array->count == 0);
}


TYPE_T
inline void swap(Dynamic_Array<T>* array, usize i, usize j)
{
    T val_at_i = array[i];
    array[i] = array[j];
    array[j] = val_at_i;
}

TYPE_T
void deallocate(Dynamic_Array<T>* array)
{
    if (array->allocator.deallocate != nullptr) {
        array->allocator.deallocate((void*)&array->allocator, array->data);
    }
    array->data = nullptr;
    array->count = 0;
    array->cap = 0;

}

TYPE_T
inline void clear(Dynamic_Array<T>* array)
{
    array->count = 0;
}

TYPE_T
void reserve(Dynamic_Array<T>* array, usize cap)
{
    if (array->capacity < cap) {
        set_capacity(array, cap);
    }
}

TYPE_T
void array__grow(Dynamic_Array<T>* array, usize min_cap)
{
    usize new_cap = ARRAY_GROW_FORMULA(array->cap);
    if (new_cap < min_cap) {
        new_cap = min_cap;
    }
    set_capacity(array, new_cap);
}

TYPE_T
void resize(Dynamic_Array<T>* array, usize count)
{
    if (array->cap < count) {
        array__grow(array, count);
    }
    array->count = count;
}

TYPE_T
void set_capacity(Dynamic_Array<T>* array, usize capacity)
{
    if (capacity == array->cap) {
        return;
    }

    if (capacity < array->count) {
        resize(array, capacity);
    }

    T* new_data = nullptr;
    if (capacity > 0) {
        new_data = (T*)array->allocator.allocate((void*)&array->allocator, sizeof(T) * capacity);
        memmove(new_data, array->data, sizeof(T) * array->cap);
    }

    array->allocator.deallocate((void*)&array->allocator, (void*)array->data);
    array->data = new_data;
    array->cap = capacity;
}

TYPE_T
void ordered_remove(Dynamic_Array<T>* array, usize index)
{
    ASSERT(0 <= index && index < array->count);

    usize bytes = sizeof(T) * (array->count - (index + 1));
    memmove(array->data + index, array->data + index + 1, bytes);
    array->count -= 1;
}

TYPE_T
void unordered_remove(Dynamic_Array<T>* array, usize index)
{
    ASSERT(0 <= index && index < array->count);

    usize n = array->count - 1;

    memmove(array->data + index, array->data + n, sizeof(T));

    pop(array);
}


TYPE_T
void init(Dynamic_Array<T>* array, mem::Allocator const& a)
{
    usize cap = ARRAY_GROW_FORMULA(0);
    init(array, a, 0, cap);
}
TYPE_T
void init(Dynamic_Array<T>* array)
{
    usize cap = ARRAY_GROW_FORMULA(0); 
    init(array, mem::get_sys_context_array_allocator(), 0, cap);
}


TYPE_T
void init(Dynamic_Array<T>* array, mem::Allocator const& a, usize count)
{
    init(array, a, count, count);
}
TYPE_T
void init(Dynamic_Array<T>* array, usize count)
{
    init(array, mem::get_sys_context_array_allocator(), count, count);
}

TYPE_T
void init(Dynamic_Array<T>* array, usize count, usize capacity)
{
    init(array, mem::get_sys_context_array_allocator(), count, capacity);
}
TYPE_T
void init(Dynamic_Array<T>* array, mem::Allocator const& a, usize count, usize capacity)
{
    array->allocator = a;
    array->data = nullptr;
    if (capacity > 0) {
        ASSERT(array->allocator.allocate != nullptr);
        array->data = (T*)array->allocator.allocate((void*)&a, sizeof(T) * capacity);
    }
    array->count = count;
    array->cap   = capacity;
}



TYPE_T
Dynamic_Array<T> Dynamic_Array<T>::make(mem::Allocator const& a)
{
    Dynamic_Array<T> array;
    ::init(&array, a);

    return array;
}
TYPE_T
Dynamic_Array<T> Dynamic_Array<T>::make(void)
{
    Dynamic_Array<T> array;
    ::init(&array);

    return array; 
}

TYPE_T
Dynamic_Array<T> Dynamic_Array<T>::make(mem::Allocator const& a, usize count)
{
    Dynamic_Array<T> array;
    ::init(&array, a, count, count);

    return array;
}
TYPE_T
Dynamic_Array<T> Dynamic_Array<T>::make(usize count)
{
    Dynamic_Array<T> array;
    ::init(&array, count, count);

    return array;
}

TYPE_T
Dynamic_Array<T> Dynamic_Array<T>::make(mem::Allocator const& a, usize count, usize capacity)
{
    Dynamic_Array<T> array;
    ::init(&array, a, count, capacity);

    return array;
}
TYPE_T
Dynamic_Array<T> Dynamic_Array<T>::make(usize count, usize capacity)
{
    Dynamic_Array<T> array;
    ::init(&array, count, capacity);

    return array;
}

TYPE_T
Dynamic_Array<T> Dynamic_Array<T>::make_from_ptr(T* data, usize count, usize capacity)
{
    Dynamic_Array<T> array = {};
    array.data  = data;
    array.count = count;
    array.cap   = capacity;

    return array;
}

TYPE_T
void copy(Dynamic_Array<T>* array, Dynamic_Array<T> const& data, isize offset)
{
    memmove(array->data + offset, data.data, sizeof(T) * data.count);
}
TYPE_T
void copy(Dynamic_Array<T>* array, Dynamic_Array<T> const& data, isize offset, usize count)
{
    memmove(array->data + offset, data.data, sizeof(T) * MIN(data.count, count));
}

// member procedure API ///////////////////////////////////////

TYPE_T
inline Array_Slice<T> Dynamic_Array<T>::slice(usize i, usize j)
{
    return ::slice(this, i, j);
}

TYPE_T
inline usize Dynamic_Array<T>::byte_length(void)
{
    return ::byte_length(this);
}

TYPE_T
inline void Dynamic_Array<T>::append(T val)
{
    ::append(this, val);
}

TYPE_T
inline void Dynamic_Array<T>::append(T* val)
{
    ::append(this, val);
}

TYPE_T
inline void Dynamic_Array<T>::push(T val)
{
    ::push(this, val);
}

TYPE_T
inline void Dynamic_Array<T>::push(T* val)
{
    ::push(this, val);
}

TYPE_T
inline T* Dynamic_Array<T>::peek(void)
{
    return ::peek(this);
}

TYPE_T
inline T Dynamic_Array<T>::pop(void)
{
    return ::pop(this);
}

TYPE_T
inline bool Dynamic_Array<T>::is_empty(void)
{
    return ::is_empty(this);
}

TYPE_T
inline void Dynamic_Array<T>::init(void)
{
    ::init(this);
}

TYPE_T
inline void Dynamic_Array<T>::swap(usize i, usize j)
{
    ::swap(this, i, j);
}



#endif // ARRAY_CPP
