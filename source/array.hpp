#ifndef ARRAY_HPP
#define ARRAY_HPP

#if !(UNITY_BUILD)
#include "common_utils.h"
#endif

template <typename T>
struct Array_Slice {
    const T* data;
    const usize count;
};

template <typename T, usize N>
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
    iterator first_free(void) { return &this->data[this->count]; }


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

}; 

template <typename T>
struct DynamicArray {
    usize cap;
    usize count;
    T*    data;

    operator T*(void) 
    {
        return this->data;
    }
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

    inline usize byte_length(DynamicArray<T>* arr) const
    {
        return sizeof(T) * arr->count;
    }

    inline void push_back(T val)
    {
        //ASSERT(count < N);
        data[count] = val;
        count += 1;
    }

    inline void push_back(T* val)
    {
        //ASSERT(count < N);
        data[count] = *val;
        count += 1;
    }

    inline void push(T val)
    {
        //ASSERT(count < N);
        data[count] = val;
        count += 1;
    }

    inline void push(T* val)
    {
        //ASSERT(count < N);
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
        //ASSERT(this->count != 0);

        T* out = &data[count - 1];
        count -= 1;
        return *out;
    }

    inline void reset(void)
    {
        count = 0;
    }

    inline bool is_empty(void)
    {
        return count == 0;
    }

    typedef T* iterator;
    typedef const T* const_iterator;
    iterator begin(void) { return &this->data[0]; }
    iterator end(void) { return first_free(); }
    iterator first_free(void) { return &this->data[this->count]; }


    static void init(DynamicArray<T>* array)
    {
        // TODO
        array->count = 0;
        //array->cap = N;
    }
    static DynamicArray<T> make(void)
    {
        DynamicArray<T> array;
        array.init(&array);
        return array;
    }
};

#endif // ARRAY_HPP

#ifdef ARRAY_IMPLEMENTATION
#undef ARRAY_IMPLEMENTATION

#endif // ARRAY_CPP
