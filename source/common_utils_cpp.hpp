#ifndef COMMON_UTILS_HPP
#define COMMON_UTILS_HPP

#include "common_utils.h"
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/*
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#define GLM_FORCE_LEFT_HANDED
//#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_ALIGNED_GENTYPES

#define GLM_ENABLE_EXPERIMENTAL
//#define GLM_FORCE_INLINE
#define GLM_FORCE_RADIANS


#include <glm/glm.hpp>
#include <glm/gtc/type_aligned.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/norm.hpp>
*/

#define GLM_FORCE_ALIGNED_GENTYPES
#include "math.hpp"

#include <bitset>
#include <limits>
#include <string>





#define TYPE_T template <typename T>
#define TYPE_T_SIZE_N template <typename T, usize N>
#define TYPE_KV template <typename K, typename V>

#define POSITIVE_INFINITY (std::numeric_limits<f64>::infinity())
#define NEGATIVE_INFINITY (-POSITIVE_INFINITY)

#define $T template <typename T>

template <typename T>
inline T dref_as(void* ptr);

TYPE_T
T make(void);

namespace m {

TYPE_T
inline T min(T val_a, T val_b)
{
    return glm::min(val_a, val_b);
}

TYPE_T
inline T max(T val_a, T val_b)
{
    return glm::max(val_a, val_b);
}

TYPE_T
inline T abs(T val)
{
    return glm::abs(val);
}

TYPE_T
inline T cos(T val)
{
    return glm::cos(val);
}

TYPE_T
inline T sin(T val)
{
    return glm::sin(val);
}

}

#define declare_pair_type(type__, name__) typedef std::pair<type__, type__> name__##_pair
declare_pair_type(Vec3, vec3);
declare_pair_type(Vec4, vec4);

inline f64 dist2(Vec3 v, Vec3 w);
inline f64 dist_to_segment_squared(Vec3 v, Vec3 w, Vec3 p);
inline f64 dist_to_segment(Vec3 v, Vec3 w, Vec3 p);

// http://alienryderflex.com/intersect/
bool line_segment_intersection(const vec3_pair* s0, const vec3_pair* s1, Vec3* out);

inline f32 lerp(f32 a, f32 b, f32 t);
inline f64 lerp(f64 a, f64 b, f64 t);

inline float64 sin01(float64 val);
inline float32 sin01(float32 val);

template<typename T>
static std::string to_binary_string(const T& x);

constexpr bool is_powerof2(uint64 N) 
{
    return N && ((N & (N - 1)) == 0);
}

constexpr uint64 next_powerof2_ge(uint64 n)
{
    if (is_powerof2(n)) {
        return n;
    }

    n -= 1;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return n + 1;
}


#define PI32 (glm::pi<f32>())
#define TAU32 (2 * glm::pi<f32>())

#define PI64 (glm::pi<f64>())
#define TAU64 (2 * glm::pi<f64>())

#define PI (PI64)
#define TAU (2 * PI)

TYPE_T
T make(void)
{
    T structure;
    init(&structure);
    return structure;
}

#include "memory.cpp"
#include "array.cpp"

constexpr bool is_pow_2_greater_equal_4(usize N)
{
    return N >= 4 && is_powerof2(N);
}

#define REQUIRED_POW_2_CAPACITY_ERROR_MESSAGE "ERROR: MUST HAVE A POWER-OF-2 CAPACITY GREATER THAN OR EQUAL TO 4"

template <typename T, usize N>
struct RingBuffer {
    static_assert(is_pow_2_greater_equal_4(N), REQUIRED_POW_2_CAPACITY_ERROR_MESSAGE);

    T buffer[N];
    const usize cap = N;
    usize head;
    usize tail;
    usize size;

    typedef T* iterator;
    typedef const T* const_iterator;
    iterator begin() { return &this->buffer[0]; }
    iterator end() { return &this->buffer[this->cap]; }
};

template <typename T, usize N>
void RingBuffer_init(RingBuffer<T, N>* buffer);


template <typename T, usize N>
inline bool RingBuffer_is_full(RingBuffer<T, N>* buffer);

template <typename T, usize N>
inline bool RingBuffer_is_empty(RingBuffer<T, N>* buffer);

template <typename T, usize N>
void RingBuffer_enqueue(RingBuffer<T, N>* buffer, T in);

template <typename T, usize N>
T RingBuffer_dequeue(RingBuffer<T, N>* buffer);

template <typename T, usize N>
T* RingBuffer_dequeue_pointer(RingBuffer<T, N>* buffer);


void vec2_print(Vec2* v);
void vec2_print(Vec2 v);
void vec2_pair_print(Vec2* a, Vec2* b);
void vec2_pair_print(Vec2 a, Vec2 b);
void vec2_println(Vec2* v);
void vec2_println(Vec2 v);
void vec2_pair_println(Vec2* a, Vec2* b);
void vec2_pair_println(Vec2 a, Vec2 b);

void vec3_print(Vec3* v);
void vec3_print(Vec3 v);
void vec3_pair_print(Vec3* a, Vec3* b);
void vec3_pair_print(Vec3 a, Vec3 b);

void vec4_print(Vec4* v);
void vec4_print(Vec4 v);
void vec4_pair_print(Vec4* a, Vec4* b);
void vec4_pair_print(Vec4 a, Vec4 b);

template<typename T>
void print_array(T* array, const usize N, const usize M = 1);

#define atan2_32(y, x) -glm::atan2<f32, glm::highp>(y, x)
#define atan2_64(y, x) -glm::atan2<f64, glm::highp>(y, x)

static inline f32 atan2pos_32(f64 y, f64 x);
static inline f64 atan2pos_64(f64 y, f64 x);

inline float64 angular_velocity(float64 radians, float64 time_delta);
inline Vec2 angular_impulse(float64 angular_velocity, Vec2 center, Vec2 point);

bool check_file_status(const char* file_path, struct stat* file_stat);



#include <ck_ring.h>

template <typename T, usize N>
struct Concurrent_Ring_Buffer {
    static_assert(is_pow_2_greater_equal_4(N), REQUIRED_POW_2_CAPACITY_ERROR_MESSAGE);
    
    ck_ring_t ring;
    T buffer[N];

    inline usize cap(void)
    {
        return N;
    }
};




// }
#endif


#ifdef COMMON_UTILS_CPP_IMPLEMENTATION
#undef COMMON_UTILS_CPP_IMPLEMENTATION

template <typename T>
inline T dref_as(void* ptr)
{
    return *static_cast<T*>(ptr);
}


inline f64 dist2(Vec3 v, Vec3 w)
{
    f64 a = v.x - w.x;
    f64 b = v.y - w.y;
    return (a * a) + (b * b);
}

inline f64 dist_to_segment_squared(Vec3 v, Vec3 w, Vec3 p)
{ 
    const f64 l2 = dist2(v, w);
    if (l2 == 0.0) {
        return dist2(p, v);
    }

    const f64 t = m::max(0.0, m::min(1.0, dot(p - v, w - v) / l2));
    return dist2(p, Vec3(v.x + t * (w.x - v.x), v.y + t * (w.y - v.y), 0.0));
}

inline f64 dist_to_segment(Vec3 v, Vec3 w, Vec3 p)
{
    return glm::sqrt(dist_to_segment_squared(v, w, p));
}

// http://alienryderflex.com/intersect/
bool line_segment_intersection(const vec3_pair* s0, const vec3_pair* s1, Vec3* out)
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

inline f32 lerp(f32 a, f32 b, f32 t)
{
    return (1 - t) * a + t * b;
}

inline f64 lerp(f64 a, f64 b, f64 t)
{
    return (1 - t) * a + t * b;
}

#define SIN01_RETURN_VAL ((glm::sin(val) + 1.0) / 2.0)
inline float64 sin01(float64 val)
{
    return SIN01_RETURN_VAL;
}

inline float32 sin01(float32 val)
{
    return SIN01_RETURN_VAL;
}
#undef SIN01_RETURN_VAL

template<typename T>
static std::string to_binary_string(const T& x)
{
    return std::bitset<sizeof(T) * 8>(x).to_string();
}

#define MEMORY_IMPLEMENTATION
#include "memory.cpp"
#define ARRAY_IMPLEMENTATION
#include "array.cpp"


template <typename T, usize N>
void RingBuffer_init(RingBuffer<T, N>* buffer)
{
    buffer->head = 0;
    buffer->tail = 0;
    buffer->size = 0;
}

template <typename T, usize N>
inline bool RingBuffer_is_full(RingBuffer<T, N>* buffer)
{    
    return (buffer->head == ((buffer->tail + 1) & (N - 1)));
}

template <typename T, usize N>
inline bool RingBuffer_is_empty(RingBuffer<T, N>* buffer)
{
    return (buffer->head == buffer->tail);
}

// check whether full using separate is_full() call
template <typename T, usize N>
void RingBuffer_enqueue(RingBuffer<T, N>* buffer, T in)
{
    buffer->buffer[buffer->tail] = in;
    buffer->tail = ((buffer->tail + 1) & (N - 1));

    buffer->size += 1;
}

// check whether empty using separate is_full() call
template <typename T, usize N>
T RingBuffer_dequeue(RingBuffer<T, N>* buffer)
{
    T* out = &buffer->buffer[buffer->head];
    buffer->head = ((buffer->head + 1) & (N - 1));

    buffer->size -= 1;

    return *out;
}

// check whether empty using separate is_full() call
template <typename T, usize N>
T* RingBuffer_dequeue_pointer(RingBuffer<T, N>* buffer)
{
    T* out = &buffer->buffer[buffer->head];
    buffer->head = ((buffer->head + 1) & (N - 1));

    buffer->size -= 1;

    return out;
}


void vec2_print(Vec2* v)
{
    printf("[%f, %f]", v->x, v->y);      
}
void vec2_print(Vec2 v)
{
    printf("[%f, %f]", v.x, v.y);      
}
void vec2_pair_print(Vec2* a, Vec2* b)
{
    printf("[[%f, %f][%f, %f]]", a->x, a->y, b->x, b->y);  
}
void vec2_pair_print(Vec2 a, Vec2 b)
{
    printf("[[%f, %f][%f, %f]]", a.x, a.y, b.x, b.y);  
}
void vec2_println(Vec2* v)
{
    printf("[%f, %f]\n", v->x, v->y);      
}
void vec2_println(Vec2 v)
{
    printf("[%f, %f]\n", v.x, v.y);      
}
void vec2_pair_println(Vec2* a, Vec2* b)
{
    printf("[[%f, %f][%f, %f]]\n", a->x, a->y, b->x, b->y);  
}
void vec2_pair_println(Vec2 a, Vec2 b)
{
    printf("[[%f, %f][%f, %f]]\n", a.x, a.y, b.x, b.y);  
}

void vec3_print(Vec3* v)
{
    printf("[%f, %f, %f]", v->x, v->y, v->z);  
}
void vec3_print(Vec3 v)
{
    printf("[%f, %f, %f]", v.x, v.y, v.z);  
}
void vec3_pair_print(Vec3* a, Vec3* b)
{
    printf("[[%f, %f, %f][%f, %f, %f]]", a->x, a->y, a->z, b->x, b->y, b->z);  
}
void vec3_pair_print(Vec3 a, Vec3 b)
{
    printf("[[%f, %f, %f][%f, %f, %f]]", a.x, a.y, a.z, b.x, b.y, b.z);  
}

void vec4_print(Vec4* v)
{
    printf("[%f, %f, %f, %f]", v->x, v->y, v->z, v->w);  
}
void vec4_print(Vec4 v)
{
    printf("[%f, %f, %f, %f]", v.x, v.y, v.z, v.w);  
}
void vec4_pair_print(Vec4* a, Vec4* b)
{
    printf("[[%f, %f, %f, %f][%f, %f, %f, %f]]", a->x, a->y, a->z, a->w, b->x, b->y, b->z, b->w);  
}
void vec4_pair_print(Vec4 a, Vec4 b)
{
    printf("[[%f, %f, %f, %f][%f, %f, %f, %f]]", a.x, a.y, a.z, a.w, b.x, b.y, b.z, b.w);  
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

   printf("%s\n", s.c_str());
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

inline float64 angular_velocity(float64 radians, float64 time_delta)
{
    return radians / time_delta;
}

inline Vec2 angular_impulse(float64 angular_velocity, Vec2 center, Vec2 point)
{
    return -angular_velocity * Vec2(-(point.y - center.y), (point.x - center.x));
}

bool check_file_status(const char* file_path, struct stat* file_stat)
{
    return stat(file_path, file_stat) == 0;
}




#endif
