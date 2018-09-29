#ifndef COMMON_UTILS_HPP
#define COMMON_UTILS_HPP

#include "common_utils.h"
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

typedef glm::vec2  Vector2;
typedef glm::vec3  Vector3;
typedef glm::vec4  Vector4;
typedef glm::ivec2 IntVector2;
typedef glm::ivec3 IntVector3;
typedef glm::ivec4 IntVector4;
typedef glm::mat3  Matrix3;
typedef glm::mat4  Matrix4;
typedef glm::quat  Quaternion;

typedef Vector2    Vec2;
typedef Vector3    Vec3;
typedef Vector4    Vec4;
typedef IntVector2 iVec2;
typedef IntVector3 iVec3;
typedef IntVector4 iVec4;
typedef Matrix3    Mat3;
typedef Matrix4    Mat4;
typedef Quaternion Quat;

template <typename T>
inline T dref_as(void* ptr);

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

inline f32 lerp(f32 a, f32 b, f32 t);
inline f64 lerp(f64 a, f64 b, f64 t);

inline float64 sin01(float64 val);
inline float32 sin01(float32 val);

template<typename T>
static std::string to_binary_string(const T& x);

constexpr bool is_powerof2(usize N) 
{
    return N && ((N & (N - 1)) == 0);
}


#define PI32 (glm::pi<f32>())
#define TAU32 (2 * glm::pi<f32>())

#define PI64 (glm::pi<f64>())
#define TAU64 (2 * glm::pi<f64>())

#define PI (PI64)
#define TAU (2 * PI)

template <typename T>
struct ArraySlice {
    const T* data;
    const usize count;
};

template <typename T, usize N>
struct Buffer {
    T memory[N];
    usize elements_used;

    operator ArraySlice<T>(void)
    {
        return ArraySlice<T>{this->memory, this->elements_used};
    }

    inline ArraySlice<T> slice(usize i, usize j)
    {
        return ArraySlice<T>{&this->memory[i], j - i};
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
        //std::cout << this->elements_used << "/" << N << std::endl;
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


    static void init(Buffer<T, N>* buffer)
    {
        buffer->elements_used = 0;
    }
    static Buffer<T, N> Buffer_make(void)
    {
        Buffer<T, N> buff;
        buff.init(&buff);
        return buff;
    }
};

template <typename T>
struct DynamicBuffer {
    usize cap;
    usize count;
    T*    data;

    operator T*(void)
    {
        return this->data;
    }

    T& operator[](usize i)
    {
        return this->data[i];
    }
     
    const T& operator[](usize i) const 
    {
        return this->data[i];
    }

    inline usize element_count(void) const
    {
        return this->count;
    }

    inline usize element_size(void) const
    {
        return sizeof(T);
    }

    inline usize size(void) const
    {
        return this->cap;
    }


    typedef T* iterator;
    typedef const T* const_iterator;
    iterator begin() { return &this->array[0]; }
    iterator end() { return &this->array[this->cap]; }
};


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


void vec2_print(glm::vec2* v);
void vec2_print(glm::vec2 v);
void vec2_pair_print(glm::vec2* a, glm::vec2* b);
void vec2_pair_print(glm::vec2 a, glm::vec2 b);
void vec2_println(glm::vec2* v);
void vec2_println(glm::vec2 v);
void vec2_pair_println(glm::vec2* a, glm::vec2* b);
void vec2_pair_println(glm::vec2 a, glm::vec2 b);

void vec3_print(glm::vec3* v);
void vec3_print(glm::vec3 v);
void vec3_pair_print(glm::vec3* a, glm::vec3* b);
void vec3_pair_print(glm::vec3 a, glm::vec3 b);

void vec4_print(glm::vec4* v);
void vec4_print(glm::vec4 v);
void vec4_pair_print(glm::vec4* a, glm::vec4* b);
void vec4_pair_print(glm::vec4 a, glm::vec4 b);

template<typename T>
void print_array(T* array, const usize N, const usize M = 1);

#define atan2_32(y, x) -glm::atan2<f32, glm::highp>(y, x)
#define atan2_64(y, x) -glm::atan2<f64, glm::highp>(y, x)

static inline f32 atan2pos_32(f64 y, f64 x);
static inline f64 atan2pos_64(f64 y, f64 x);

inline float64 angular_velocity(float64 radians, float64 time_delta);
inline glm::vec2 angular_impulse(float64 angular_velocity, glm::vec2 center, glm::vec2 point);

bool check_file_status(const char* file_path, struct stat* file_stat);

template <typename T>
inline void zero_mem(T* ptr);

#include <ck_ring.h>

template <usize N>
struct ConcurrentFIFO_SingleProducerSingleConsumer {
    static_assert(is_pow_2_greater_equal_4(N), REQUIRED_POW_2_CAPACITY_ERROR_MESSAGE);
    
    ck_ring_t ring;
    ck_ring_buffer_t buffer[N];
    const usize capacity = N;
};

// }
#endif


#ifdef COMMON_UTILS_CPP_IMPLEMENTATION

template <typename T>
inline T dref_as(void* ptr)
{
    return *static_cast<T*>(ptr);
}

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


void vec2_print(glm::vec2* v)
{
    printf("[%f, %f]", v->x, v->y);      
}
void vec2_print(glm::vec2 v)
{
    printf("[%f, %f]", v.x, v.y);      
}
void vec2_pair_print(glm::vec2* a, glm::vec2* b)
{
    printf("[[%f, %f][%f, %f]]", a->x, a->y, b->x, b->y);  
}
void vec2_pair_print(glm::vec2 a, glm::vec2 b)
{
    printf("[[%f, %f][%f, %f]]", a.x, a.y, b.x, b.y);  
}
void vec2_println(glm::vec2* v)
{
    printf("[%f, %f]\n", v->x, v->y);      
}
void vec2_println(glm::vec2 v)
{
    printf("[%f, %f]\n", v.x, v.y);      
}
void vec2_pair_println(glm::vec2* a, glm::vec2* b)
{
    printf("[[%f, %f][%f, %f]]\n", a->x, a->y, b->x, b->y);  
}
void vec2_pair_println(glm::vec2 a, glm::vec2 b)
{
    printf("[[%f, %f][%f, %f]]\n", a.x, a.y, b.x, b.y);  
}

void vec3_print(glm::vec3* v)
{
    printf("[%f, %f, %f]", v->x, v->y, v->z);  
}
void vec3_print(glm::vec3 v)
{
    printf("[%f, %f, %f]", v.x, v.y, v.z);  
}
void vec3_pair_print(glm::vec3* a, glm::vec3* b)
{
    printf("[[%f, %f, %f][%f, %f, %f]]", a->x, a->y, a->z, b->x, b->y, b->z);  
}
void vec3_pair_print(glm::vec3 a, glm::vec3 b)
{
    printf("[[%f, %f, %f][%f, %f, %f]]", a.x, a.y, a.z, b.x, b.y, b.z);  
}

void vec4_print(glm::vec4* v)
{
    printf("[%f, %f, %f, %f]", v->x, v->y, v->z, v->w);  
}
void vec4_print(glm::vec4 v)
{
    printf("[%f, %f, %f, %f]", v.x, v.y, v.z, v.w);  
}
void vec4_pair_print(glm::vec4* a, glm::vec4* b)
{
    printf("[[%f, %f, %f, %f][%f, %f, %f, %f]]", a->x, a->y, a->z, a->w, b->x, b->y, b->z, b->w);  
}
void vec4_pair_print(glm::vec4 a, glm::vec4 b)
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

inline float64 angular_velocity(float64 radians, float64 time_delta)
{
    return radians / time_delta;
}

inline glm::vec2 angular_impulse(float64 angular_velocity, glm::vec2 center, glm::vec2 point)
{
    return -angular_velocity * glm::vec2(-(point.y - center.y), (point.x - center.x));
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
