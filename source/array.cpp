#ifndef ARRAY_HPP
#define ARRAY_HPP

#if !(UNITY_BUILD)
#include "common_utils.h"
#include "memory.cpp"
#endif



TYPE_T
struct Array_Slice {
    const T* data;
    const usize count;
};

#define ARRAY_GROW_FORMULA(x) (2*(x) + 8)
static_assert(ARRAY_GROW_FORMULA(0) > 0);

TYPE_T_SIZE_N
struct Array {
    T     data[N];
    usize count;

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
    iterator begin(void) { return &this->data[0]; }
    iterator end(void) { return next_free_slot(); }
    iterator bound(void) { return &this->data[N]; }
    usize cap(void) { return N; }
    iterator next_free_slot(void) { return &this->data[this->count]; }


    // member procedure API

    inline Array_Slice<T> slice(usize i, usize j);

    inline usize byte_length(void);

    inline void append(T val);

    inline void append(T* val);

    inline void push(T val);

    inline void push(T* val);


    inline T* peek(void);


    inline T pop(void);


    inline void reset_back(void);


    inline bool is_empty(void);


    inline void Dynamic_Array_init(void);

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
inline void reset_back(Array<T, N>* array);

TYPE_T_SIZE_N
inline bool is_empty(Array<T, N>* array);

TYPE_T_SIZE_N
inline void Dynamic_Array_init(Array<T, N>* array);
TYPE_T_SIZE_N
inline void init(Array<T, N>* array);


TYPE_T_SIZE_N
inline void swap(Array<T, N>* array, usize i, usize j);

TYPE_T 
struct Dynamic_Array {
    T*             data;
    usize          count;
    usize          cap;
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
    iterator begin(void) { return &this->data[0]; }
    iterator end(void) { return next_free_slot(); }
    iterator next_free_slot(void) { return &this->data[this->count]; }


    // member procedure API

    inline Array_Slice<T> slice(usize i, usize j);

    inline usize byte_length(void);

    inline void append(T val);

    inline void append(T* val);

    inline void push(T val);

    inline void push(T* val);


    inline T* peek(void);


    inline T pop(void);


    inline void reset_back(void);


    inline bool is_empty(void);


    inline void Dynamic_Array_init(void);

    inline void init(void);

    inline void swap(usize i, usize j);
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
inline void Dynamic_Array_init(Dynamic_Array<T>* array);
TYPE_T
inline void init(Dynamic_Array<T>* array);


TYPE_T
inline void swap(Dynamic_Array<T>* array, usize i, usize j);


#endif // ARRAY_HPP

#ifdef ARRAY_IMPLEMENTATION
#undef ARRAY_IMPLEMENTATION

// floating procedure API ////////////////////////////////////////////

TYPE_T_SIZE_N
inline Array_Slice<T> slice(Array<T, N>* array, usize i, usize j)
{
    ASSERT_MSG(j < array->count, "Slice out-of-bounds >= %llu", N);
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

    T* out = &array->data[array->count - 1];
    array->count -= 1;
    return *out;
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
inline void Array<T, N>::reset_back(void)
{   
    ::reset_back(this);
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
    ASSERT_MSG(j < array->count, "Slice out-of-bounds >= %llu", array->count);

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
    array->data[array->count] = val;
    array->count += 1;
}

TYPE_T
inline void push(Dynamic_Array<T>* array, T* val)
{
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

    T* out = &array->data[array->count - 1];
    array->count -= 1;
    return *out;
}

TYPE_T
inline void reset_back(Dynamic_Array<T>* array)
{
    array->count = 0;
}

TYPE_T
inline bool is_empty(Dynamic_Array<T>* array)
{
    return (array->count == 0);
}

TYPE_T
inline void Dynamic_Array_init(Dynamic_Array<T>* array)
{
    // TODO
    array->count = 0;
    array->cap = 0;
    //array->cap = N;
}

TYPE_T
inline void init(Dynamic_Array<T>* array)
{
    Dynamic_Array_init(array);
}

TYPE_T
inline void swap(Dynamic_Array<T>* array, usize i, usize j)
{
    T val_at_i = array[i];
    array[i] = array[j];
    array[j] = val_at_i;
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
inline void Dynamic_Array<T>::reset_back(void)
{   
    ::reset_back(this);
}

TYPE_T
inline bool Dynamic_Array<T>::is_empty(void)
{
    return ::is_empty(this);
}

TYPE_T
inline void Dynamic_Array<T>::init(void)
{
    ::Dynamic_Array_init(this);
}

TYPE_T
inline void Dynamic_Array<T>::swap(usize i, usize j)
{
    ::swap(this, i, j);
}



#endif // ARRAY_CPP
