#ifndef COMMON_UTILS_HPP
#define COMMON_UTILS_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/norm.hpp>

#include <bitset>

#include <limits>

#define POSITIVE_INFINITY (std::numeric_limits<f64>::infinity())

inline f64 dist2(glm::vec3 v, glm::vec3 w);
inline f64 dist_to_segment_squared(glm::vec3 v, glm::vec3 w, glm::vec3 p);
inline f64 dist_to_segment(glm::vec3 v, glm::vec3 w, glm::vec3 p);

template<typename T>
static std::string to_binary_string(const T& x);

template <typename T, usize N>
struct Buffer {
    T memory[N];
    usize elements_used;

    operator T*(void)
    {
        return this->memory;
    }

    inline T& operator[](usize i)
    {
        return this->memory[i];
    }

    inline const T& operator[](usize i) const 
    {
        return this->memory[i];
    }

    inline usize byte_length(void) const
    {
        return sizeof(T) * N;
    }

    inline usize element_length(void) const
    {
        return N;
    }

    inline void push_back(T val)
    {
        assert(this->elements_used < N);
        memory[this->elements_used] = val;
        this->elements_used += 1;
    }

    inline void push_back(T* val)
    {
        assert(this->elements_used < N);
        memory[this->elements_used] = *val;
        this->elements_used += 1;
    }

    inline void reset()
    {
        this->elements_used = 0;
    }

    typedef T* iterator;
    typedef const T* const_iterator;
    iterator begin(void) { return &this->memory[0]; }
    iterator end(void) { return &this->memory[N]; }
    iterator first_free(void) { return &this->memory[this->elements_used]; }
};

template <typename T>
struct DynamicBuffer {
    size_t cap;
    size_t count;
    T*  array;

    operator T*()
    {
        return this->array;
    }

    T& operator[](size_t i)
    {
        return this->array[i];
    }
     
    const T& operator[](size_t i) const 
    {
        return this->array[i];
    }

    inline size_t element_count() const
    {
        return this->count;
    }

    inline size_t element_size() const
    {
        return sizeof(T);
    }

    inline size_t size() const
    {
        return this->cap;
    }


    typedef T* iterator;
    typedef const T* const_iterator;
    iterator begin() { return &this->array[0]; }
    iterator end() { return &this->array[this->cap]; }
};

void vec3_print(glm::vec3* v);

void vec3_pair_print(glm::vec3* a, glm::vec3*b);

// }
#endif


#ifdef COMMON_UTILS_IMPLEMENTATION_CPP


inline f64 dist2(glm::vec3 v, glm::vec3 w)
{
    f64 a = v.x - w.x;
    f64 b = v.y - w.y;
    return (a * a) + (b * b);
}

inline f64 dist_to_segment_squared(glm::vec3 v, glm::vec3 w, glm::vec3 p)
{ 
    const f64 l2 = dist2(v, w);
    if (l2 == 0.0) {
        return dist2(p, v);
    }

    const f64 t = glm::max(0.0, glm::min(1.0, dot(p - v, w - v) / l2));
    return dist2(p, glm::vec3(v.x + t * (w.x - v.x), v.y + t * (w.y - v.y), 0.0));
}

inline f64 dist_to_segment(glm::vec3 v, glm::vec3 w, glm::vec3 p)
{
    return glm::sqrt(dist_to_segment_squared(v, w, p));
}

template<typename T>
static std::string to_binary_string(const T& x)
{
    return std::bitset<sizeof(T) * 8>(x).to_string();
}

void vec3_print(glm::vec3* v)
{
    printf("[%f, %f, %f]", v->x, v->y, v->z);  
}

void vec3_pair_print(glm::vec3* a, glm::vec3*b)
{
    printf("[[%f, %f, %f][%f, %f, %f]]", a->x, a->y, a->z, b->x, b->y, b->z);  
}


#undef COMMON_UTILS_IMPLEMENTATION_CPP

#endif
