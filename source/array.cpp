TYPE_T
inline Array_Slice<T> slice(Dynamic_Array<T>* array, usize i, usize j)
{
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
    ASSERT(array->count != 0);

    T* out = &data[array->count - 1];
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
inline Dynamic_Array<T> make(void)
{
    Dynamic_Array<T> array;
    Dynamic_Array_init(&array);
    return array;
}

TYPE_T
inline void swap(Dynamic_Array<T>* array, usize i, usize j)
{
    T val_at_i = array[i];
    array[i] = array[j];
    array[j] = val_at_i;
}
