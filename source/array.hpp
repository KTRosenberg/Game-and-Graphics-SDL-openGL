#ifndef ARRAY_HPP
#define ARRAY_HPP

#if !(UNITY_BUILD)
#include "common_utils.h"
#endif

#include "memory.hpp"

TYPE_T
struct Array_Slice {
    const T* data;
    const usize count;
};

TYPE_T_SIZE_N 
struct Array {
    T data[N];
    usize count;
    usize cap;

    operator Array_Slice<T>(void)
    {
        return Array_Slice<T>{this->data, this->count};
    }

    inline Array_Slice<T> slice(usize i, usize j)
    {
        return Array_Slice<T>{&this->data[i], j - i};
    }

    inline T& operator[](usize i)
    {
        return this->data[i];
    }

    inline const T& operator[](usize i) const 
    {
        return this->data[i];
    }

    inline usize byte_length(Array<T, N>* arr) const
    {
        return sizeof(T) * N;
    }

    inline usize element_length(void) const
    {
        return N;
    }

    inline void push_back(T val)
    {
        ASSERT(count < N);
        data[count] = val;
        count += 1;
    }

    inline void push_back(T* val)
    {
        ASSERT(count < N);
        data[count] = *val;
        count += 1;
    }

    inline void push(T val)
    {
        ASSERT(count < N);
        data[count] = val;
        count += 1;
    }

    inline void push(T* val)
    {
        ASSERT(count < N);
        data[count] = *val;
        count += 1;
    }

    inline T* peek(void)
    {
        if (this->count == 0) {
            return nullptr;
        }
        return &data[count - 1];
    }

    inline T pop(void)
    {
        ASSERT(this->count != 0);

        T* out = &data[count - 1];
        count -= 1;
        return *out;
    }

    inline void reset(void)
    {
        this->count = 0;
    }

    inline bool is_empty(void)
    {
        return count == 0;
    }

    typedef T* iterator;
    typedef const T* const_iterator;
    iterator begin(void) { return &this->data[0]; }
    iterator end(void) { return &this->data[N]; }
    iterator next_free_slot(void) { return &this->data[this->count]; }


    static void init(Array<T, N>* array)
    {
        array->count = 0;
        array->cap = N;
    }
    static Array<T, N> make(void)
    {
        Array<T, N> array;
        array.init(&array);
        return array;
    }

    static inline void swap(Array<T, N>* array, usize i, usize j)
    {
        T val_at_i = array[i];
        array[i] = array[j];
        array[j] = val_at_i;
    }

}; 

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

    inline T& operator[](usize i)
    {
        return this->data[i];
    }

    inline const T& operator[](usize i) const 
    {
        return this->data[i];
    }

    typedef T* iterator;
    typedef const T* const_iterator;
    iterator begin(void) { return &this->data[0]; }
    iterator end(void) { return next_free_slot(); }
    iterator next_free_slot(void) { return &this->data[this->count]; }
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
inline bool reset_back(Dynamic_Array<T>* array);

TYPE_T
inline bool is_empty(Dynamic_Array<T>* array);

TYPE_T
inline void Dynamic_Array_init(Dynamic_Array<T>* array);
TYPE_T
inline void init(Dynamic_Array<T>* array);

TYPE_T
inline Dynamic_Array<T> make(void);


TYPE_T
inline void swap(Dynamic_Array<T>* array, usize i, usize j);


#endif // ARRAY_HPP

#ifdef ARRAY_IMPLEMENTATION
#undef ARRAY_IMPLEMENTATION

#include "array.cpp"

#endif // ARRAY_CPP
