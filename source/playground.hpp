#include "playground_version.hpp"

#include "common_utils.h"
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

//#include "math.hpp"

#define TYPE_T template <typename T>
#define TYPE_T_SIZE_N template <typename T, usize N>
#define TYPE_KV template <typename K, typename V>

#define POSITIVE_INFINITY (std::numeric_limits<f64>::infinity())
#define NEGATIVE_INFINITY (-POSITIVE_INFINITY)

inline float64 sin01(float64 val)
{
    return ((sin(val) + 1.0) / 2.0);
}

inline float32 sin01(float32 val)
{
    return ((sin(val) + 1.0) / 2.0);    
}

// temp
void* xmalloc(size_t num_bytes)
{
    void* ptr = malloc(num_bytes);
    if (ptr == NULL) {
        perror("xmalloc failed");
        exit(EXIT_FAILURE);
    }

    return ptr;
}

void* xcalloc(size_t num_elems, size_t elem_size)
{
    void* ptr = calloc(num_elems, elem_size);
    if (ptr == NULL) {
        perror("xcalloc failed");
        exit(EXIT_FAILURE);
    }

    return ptr;
}

void* xcalloc_1arg(size_t bytes)
{
    return xcalloc(1, bytes);
}

void* xrealloc(void* ptr, size_t num_bytes)
{
    ptr = realloc(ptr, num_bytes);
    if (ptr == NULL) {
        perror("xrealloc failed");
        exit(EXIT_FAILURE);
    }

    return ptr;
}

void* mem_alloc(void* data, usize num_bytes)
{
    (void)data;
    return (void*)xmalloc(num_bytes);
}

void mem_free(void* data, void* bytes)
{
    (void)data;
    free(bytes);
}

void mem_resize(void* data, usize bytes)
{
    // TODO
}
void mem_free_all(void* data, usize bytes)
{
    // TODO
}

#define MEMORY_IMPLEMENTATION
#include "memory.cpp"
#define ARRAY_IMPLEMENTATION
#include "array.cpp"

#define VERSIONED_PROC(name) PASTE(name, PLAYGROUND_VERSION)

#define UNITY_BUILD (true)




namespace sd {
    struct Renderer;
    typedef u16 layer_index;
}
using sd::Renderer;
using sd::layer_index;

#include "playground_common.cpp"

using namespace pg;

float32 screen_width;
float32 screen_height;

bool quad(sd::Renderer& ctx, layer_index L, Vec2 tl, Vec2 bl, Vec2 br, Vec2 tr)
{
    return sd_data.quad_no_angle(&ctx, L, tl, bl, br, tr);
}
bool quad(sd::Renderer& ctx, layer_index L, Vec2 tl, Vec2 bl, Vec2 br, Vec2 tr, float32 angle)
{
    return sd_data.quad_angle(&ctx, L, tl, bl, br, tr, angle);
}
bool quad(sd::Renderer& ctx, layer_index L, Vec2 tl, Vec2 bl, Vec2 br, Vec2 tr, Vec4 color_tl, Vec4 color_bl, Vec4 color_br, Vec4 color_tr)
{
    return sd_data.quad_color_no_angle(&ctx, L, tl, bl, br, tr, color_tl, color_bl, color_br, color_tr);
}
bool quad(sd::Renderer& ctx, layer_index L, Vec2 tl, Vec2 bl, Vec2 br, Vec2 tr, Vec4 color_tl, Vec4 color_bl, Vec4 color_br, Vec4 color_tr, float32 angle)
{
    return sd_data.quad_color_angle(&ctx, L, tl, bl, br, tr, color_tl, color_bl, color_br, color_tr, angle);
}


bool triangle(sd::Renderer& ctx, layer_index L, Vec2 a, Vec2 b, Vec2 c)
{
    return sd_data.triangle_no_angle(&ctx, L, a, b, c);
}
// TODO more triangle functions

void clear(u32 mask)
{
    sd_data.clear(mask);
}
void clear_color(float32 r, float32 g, float32 b, float32 a)
{
    sd_data.clear_color_rgba(r, g, b, a);
}
void clear_color(Vec4 c)
{
    sd_data.clear_color_vec4(c);
}
void clear_color(Vec3 c)
{
    sd_data.clear_color_vec3(c);
}
void color(sd::Renderer& ctx, Vec4 c)
{
    sd_data.color(&ctx, c);
}

#define TYPE_T template <typename T>
#define TYPE_T_SIZE_N template <typename T, usize N>
#define TYPE_KV template <typename K, typename V>

#define POSITIVE_INFINITY (std::numeric_limits<f64>::infinity())
#define NEGATIVE_INFINITY (-POSITIVE_INFINITY)

using namespace sd;


inline Renderer& renderer(void)
{
    return *(sd_data.renderer);
}

void setup(SD_Data* data);
void draw(float32 time, float32 delta_time);

extern "C" {

static u64 call_count = 0;


void* VERSIONED_PROC(get_proc_address)(const char* name)
{
    printf("version: %d\n", PLAYGROUND_VERSION);

    if (0 == strcmp(name, "setup")) {
        printf("%s\n", "getting setup()");
        return (void*)setup;
    }
    if (0 == strcmp(name, "draw")) {
        printf("%s\n", "getting draw()");
        return (void*)draw;
    }

    printf("%s\n", "proc not found");
    return nullptr;
}

}
