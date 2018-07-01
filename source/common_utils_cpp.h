#ifndef COMMON_UTILS_HPP
#define COMMON_UTILS_HPP

#include <cstring>
#include <ctime>
#include <csignal>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/norm.hpp>

#include <bitset>
#include <limits>
#include <string>
#include <iostream>

#define POSITIVE_INFINITY (std::numeric_limits<f64>::infinity())
#define NEGATIVE_INFINITY (-POSITIVE_INFINITY)

#define declare_pair_type(type__, name__) typedef std::pair<type__, type__> name__##_pair
declare_pair_type(glm::vec3, vec3);
declare_pair_type(glm::vec4, vec4);

inline f64 dist2(glm::vec3 v, glm::vec3 w);
inline f64 dist_to_segment_squared(glm::vec3 v, glm::vec3 w, glm::vec3 p);
inline f64 dist_to_segment(glm::vec3 v, glm::vec3 w, glm::vec3 p);

// http://alienryderflex.com/intersect/
bool line_segment_intersection(const vec3_pair* s0, const vec3_pair* s1, glm::vec3* out);

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

void vec2_print(glm::vec2* v);

void vec2_pair_print(glm::vec2* a, glm::vec2* b);

void vec3_print(glm::vec3* v);

void vec3_pair_print(glm::vec3* a, glm::vec3*b);

void vec4_print(glm::vec4* v);

void vec4_pair_print(glm::vec4* a, glm::vec4* b);

template<typename T>
void print_array(T* array, const usize N, const usize M = 1);

#define atan2_32(y, x) -glm::atan2<f32, glm::highp>(y, x)
#define atan2_64(y, x) -glm::atan2<f64, glm::highp>(y, x)

static inline f32 atan2pos_32(f64 y, f64 x);
static inline f64 atan2pos_64(f64 y, f64 x);

bool check_file_status(const char* file_path, struct stat* file_stat);

template <typename T>
inline void zero_mem(T* ptr);

// }
#endif


#ifdef COMMON_UTILS_CPP_IMPLEMENTATION


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

// http://alienryderflex.com/intersect/
bool line_segment_intersection(const vec3_pair* s0, const vec3_pair* s1, glm::vec3* out)
{
    f64 Ax = s0->first.x;
    f64 Ay = s0->first.y;
    f64 Bx = s0->second.x;
    f64 By = s0->second.y;

    f64 Cx = s1->first.x;
    f64 Cy = s1->first.y;
    f64 Dx = s1->second.x;
    f64 Dy = s1->second.y;

    f64 distAB, theCos, theSin, newX, ABpos;
    
    // if ((Ax == Bx && Ay == By) || (Cx == Dx && Cy == Dy)) {
    //     return false;
    // }

    // //  Fail if the segments share an end-point.
    // if ((Ax==Cx && Ay==Cy) || (Bx==Cx && By==Cy) ||  
    //     (Ax==Dx && Ay==Dy) || (Bx==Dx && By==Dy)) {
    //     return false; 
    // }

    //  (1) Translate the system so that point A is on the origin.
    Bx -= Ax; 
    By -= Ay;
    
    Cx -= Ax; 
    Cy -=Ay;
    
    Dx -= Ax; 
    Dy -=Ay;

    //  Discover the length of segment A-B.
    distAB = glm::sqrt(Bx*Bx + By*By);

   //  (2) Rotate the system so that point B is on the positive X axis.
    theCos = Bx / distAB;
    theSin = By / distAB;
    newX   = Cx*theCos + Cy*theSin;
    Cy     = Cy*theCos - Cx*theSin; 
    Cx = newX;
    newX = Dx*theCos + Dy*theSin;
    Dy   = Dy*theCos - Dx*theSin; 
    Dx = newX;

    //  Fail if segment C-D doesn't cross line A-B.
    if ((Cy < 0.0 && Dy < 0.0) || (Cy >= 0.0 && Dy >= 0.0)) {
        return false;
    }

    //  (3) Discover the position of the intersection point along line A-B.
    ABpos = Dx + (Cx-Dx)*Dy / (Dy-Cy);

    //  Fail if segment C-D crosses line A-B outside of segment A-B.
    if (ABpos < 0.0 || ABpos > distAB) {
        return false;
    }

    //  (4) Apply the discovered position to line A-B in the original coordinate system.
    out->x =Ax+ABpos * theCos;
    out->y =Ay+ABpos * theSin;
    out->z = 0.0;

    //  Success.
    return true; 
}

template<typename T>
static std::string to_binary_string(const T& x)
{
    return std::bitset<sizeof(T) * 8>(x).to_string();
}

void vec2_print(glm::vec2* v)
{
    printf("[%f, %f]", v->x, v->y);      
}

void vec3_pair_print(glm::vec2* a, glm::vec2* b)
{
    printf("[[%f, %f][%f, %f]]", a->x, a->y, b->x, b->y);  
}

void vec3_print(glm::vec3* v)
{
    printf("[%f, %f, %f]", v->x, v->y, v->z);  
}

void vec3_pair_print(glm::vec3* a, glm::vec3* b)
{
    printf("[[%f, %f, %f][%f, %f, %f]]", a->x, a->y, a->z, b->x, b->y, b->z);  
}

void vec4_print(glm::vec4* v)
{
    printf("[%f, %f, %f, %f]", v->x, v->y, v->z, v->w);  
}

void vec4_pair_print(glm::vec4* a, glm::vec4*b)
{
    printf("[[%f, %f, %f, %f][%f, %f, %f, %f]]", a->x, a->y, a->z, a->w, b->x, b->y, b->z, b->w);  
}

template<typename T>
void print_array(T* const array, const usize N, const usize M)
{
    std::string s = "{\n";
    for (usize i = 0; i < N; ++i) {
        T* row = &array[i * M];
        s += "[";
        for (usize j = 0; j < M; ++j) {
            s += std::to_string(row[j]) + ", ";
        }
        s += "],\n";
    }
    s += "}";

    std::cout << s << std::endl;
}

static inline f32 atan2pos_32(f64 y, f64 x)
{
    f32 val = glm::atan2<f32, glm::highp>(-y, x);

    return (val < 0) ? val + 2 * glm::pi<f64>() : val;
}
static inline f64 atan2pos_64(f64 y, f64 x)
{
    f64 val = glm::atan2<f64, glm::highp>(-y, x);

    return (val < 0) ? val + 2 * glm::pi<f64>() : val;
}

bool check_file_status(const char* file_path, struct stat* file_stat)
{
    return stat(file_path, file_stat) == 0;
}

template <typename T>
inline void zero_mem(T* ptr)
{
    memset(ptr, 0, sizeof(T));
}


#undef COMMON_UTILS_CPP_IMPLEMENTATION

#endif
