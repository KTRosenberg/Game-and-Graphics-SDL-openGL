#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#ifdef __cplusplus
extern "C" 
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>
#include <getopt.h>
#include <string.h>

// Credit to Handmade Network person for the following macros {
typedef int8_t   int8;
typedef int16_t  int16;
typedef int32_t  int32;
typedef int64_t  int64;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float    float32; 
typedef double   float64;

typedef int8     i8;
typedef int16    i16;
typedef int32    i32;
typedef int64    i64;

typedef i8       s8;
typedef i16      s16;
typedef i32      s32;
typedef i64      s64;

typedef uint8    u8;
typedef uint16   u16;
typedef uint32   u32;
typedef uint64   u64;

typedef float32  f32; 
typedef float64  f64;

typedef i64 isize;
typedef u64 usize;
typedef unsigned char* ucharptr;


#define bytes(n) (n * 1ull)
#define kb(n) (bytes(n) * 1024ull)
#define mb(n) (kb(n) * 1024ull)
#define gb(n) (mb(n) * 1024ull)


#define foreach(i, lim) for (u64 (i) = 0; (i) < ((u64)lim); (i += 1))
#define forrange(i, l, h) for (i64 (i) = (l); (i) < (h); (i += 1))


#define PASTE2(x, y) x##y
#define PASTE(x, y) PASTE2(x, y)
#define STR(str) #str
#define STRING(str) STR(str)

#define LOG_PARAMS const char *const file_name, const int line_number
#define LOG_ARGS __FILE__, __LINE__

#define FILE_NAME (__FILE__)
#define LINE_NUMBER (__LINE__)

// }

// taken Pervognsen bitwise utilities // https://github.com/pervognsen/bitwise
void* xmalloc(size_t num_bytes);
void* xcalloc(size_t num_elems, size_t elem_size);
void* xcalloc_1arg(size_t bytes);
void* xrealloc(void* ptr, size_t num_bytes);


void* memdup(void *src, size_t size);

char *strf(const char *fmt, ...);

// I/O
char* read_file(const char *path);
char* read_file_mode(const char *path, const char * mode);
bool write_file(const char *path, const char *buf, size_t len);
bool write_file_mode(const char *path, const char* mode, const char *buf, size_t len);


#define ARENA_DEFAULT_BLOCK_SIZE (1024 * 1024)
#define ARENA_ALIGNMENT 8


#define MIN(x, y) ((x) <= (y) ? (x) : (y))
#define MAX(x, y) ((x) >= (y) ? (x) : (y))
#define CLAMP_MAX(x, max) MIN(x, max)
#define CLAMP_MIN(x, min) MAX(x, min)
#define IS_POW2(x) (((x) != 0) && ((x) & ((x)-1)) == 0)
#define ALIGN_DOWN(n, a) ((n) & ~((a) - 1))
#define ALIGN_UP(n, a) ALIGN_DOWN((n) + (a) - 1, (a))
#define ALIGN_DOWN_PTR(p, a) ((void *)ALIGN_DOWN((uintptr_t)(p), (a)))
#define ALIGN_UP_PTR(p, a) ((void *)ALIGN_UP((uintptr_t)(p), (a)))

typedef struct {
    char* ptr;
    char* end;
    char** blocks;
    size_t block_count;
    size_t block_cap;
} ArenaAllocator;

void ArenaAllocator_init(ArenaAllocator* arena);
void* ArenaAllocator_allocate(ArenaAllocator* arena, size_t size);
void ArenaAllocator_grow(ArenaAllocator* arena, size_t min_size);
void ArenaAllocator_delete(ArenaAllocator* arena);

#define StaticArrayCount(array) (sizeof(array) / sizeof((array)[0]))
#define PointerArraySize(ptr, count) (sizeof((*ptr)) * count)

#define cast(type_, v_) ((type_)v_)
#define deref(ptr_) (*ptr_)
//#define dref_as(ptr_, type_) *((type_*)ptr_)

void debug_print(const char* const in);

#ifdef USE_ASSERTS
// https://stackoverflow.com/questions/3767869/adding-message-to-assert
#define ASSERT_MSG(condition,...) assert( \
    condition|| \
    (fprintf(stderr,__VA_ARGS__)&&fprintf(stderr," at %s:%d\n",__FILE__,__LINE__)) \
);

#define ASSERT(condition) assert( \
    condition|| \
    (fprintf(stderr," at %s:%d\n",__FILE__,__LINE__)) \
);

#else

#define ASSERT_MSG(condition,...) condition
#define ASSERT(condition) condition

#endif

typedef void* (*Fn_Memory_Allocator)(usize bytes);
typedef void (*Fn_Memory_Deallocator)(void* memory);

#define PROGRAM_ARGS_COUNT (2)
extern struct option program_args[PROGRAM_ARGS_COUNT + 1];

typedef struct {
    bool verbose;
    bool hot_config;
} CommandLineArgs;

bool parse_command_line_args(CommandLineArgs* cmd, const int argc, char* argv[]);

// Hash Map
uint64_t hash_uint64(uint64_t x);
uint64_t hash_ptr(const void* ptr);
uint64_t hash_mix(uint64_t x, uint64_t y);
uint64_t hash_bytes(const void* ptr, size_t len);

typedef struct Map {
    uint64_t* keys;
    uint64_t* vals;
    size_t len;
    size_t cap;
} Map;

uint64_t map_get_uint64_from_uint64(Map* map, uint64_t key);

void map_put_uint64_from_uint64(Map* map, uint64_t key, uint64_t val);

void map_grow(Map* map, size_t new_cap);

void map_put_uint64_from_uint64(Map* map, uint64_t key, uint64_t val);

void* map_get(Map* map, const void* key);

void map_put(Map* map, const void* key, void* val);

void* map_get_from_uint64(Map *map, uint64_t key);

void map_put_from_uint64(Map* map, uint64_t key, void* val);

uint64_t map_get_uint64(Map* map, void* key);

void map_put_uint64(Map* map, void* key, uint64_t val);

// String interning

typedef struct Intern {
    size_t len;
    struct Intern* next;
    char str[];
} Intern;

extern ArenaAllocator intern_arena;
extern Map interns;

const char *str_intern_range(const char* start, const char* end);

const char *str_intern(const char* str);

bool str_islower(const char* str);


#ifdef __cplusplus
}
#endif

// }
#endif


#ifdef COMMON_UTILS_IMPLEMENTATION

#ifdef __cplusplus
extern "C" 
{
#endif

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

void* memdup(void *src, size_t size) {
    void *dest = xmalloc(size);
    memcpy(dest, src, size);
    return dest;
}

char* strf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    size_t n = 1 + vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    char* str = xmalloc(n);
    va_start(args, fmt);
    vsnprintf(str, n, fmt, args);
    va_end(args);
    return str;
}

char* read_file(const char* path) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long len = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buf = xmalloc(len + 1);
    if (len && fread(buf, len, 1, file) != 1) {
        fclose(file);
        free(buf);
        return NULL;
    }

    fclose(file);   
    buf[len] = 0;
    return buf;
}

char* read_file_mode(const char* path, const char* mode) {
    FILE* file = fopen(path, mode);
    if (!file) {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long len = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buf = xmalloc(len + 1);
    if (len && fread(buf, len, 1, file) != 1) {
        fclose(file);
        free(buf);
        return NULL;
    }

    fclose(file);   
    buf[len] = 0;
    return buf;
}

bool write_file(const char* path, const char* buf, size_t len) {
    FILE* file = fopen(path, "w");
    if (!file) {
        return false;
    }

    size_t n = fwrite(buf, len, 1, file);
    fclose(file);
    return n == 1;
}

bool write_file_mode(const char* path, const char* mode, const char* buf, size_t len) {
    FILE* file = fopen(path, mode);
    if (!file) {
        return false;
    }

    size_t n = fwrite(buf, len, 1, file);
    fclose(file);
    return n == 1;
}

void ArenaAllocator_init(ArenaAllocator* arena)
{
    arena->ptr         = NULL;
    arena->end         = NULL;
    arena->blocks      = NULL;
    arena->block_count = 0;
    arena->block_cap   = 0;
}
void* ArenaAllocator_allocate(ArenaAllocator* arena, size_t size)
{
    if (size > (size_t)(arena->end - arena->ptr)) {
        ArenaAllocator_grow(arena, size);

        assert(size <= (size_t)(arena->end - arena->ptr));
    }
    void* ptr = (void*)arena->ptr;
    arena->ptr = (char*)ALIGN_UP_PTR(arena->ptr + size, ARENA_ALIGNMENT);
    
    assert(arena->ptr <= arena->end);
    assert(ptr == ALIGN_DOWN_PTR(ptr, ARENA_ALIGNMENT));
    
    return ptr;
}

void ArenaAllocator_grow(ArenaAllocator* arena, size_t min_size)
{
    size_t size = ALIGN_UP(CLAMP_MIN(min_size, ARENA_DEFAULT_BLOCK_SIZE), ARENA_ALIGNMENT);
    arena->ptr = (char*)xmalloc(size);

    assert(arena->ptr == ALIGN_DOWN_PTR(arena->ptr, ARENA_ALIGNMENT));

    arena->end = arena->ptr + size;

    if (arena->block_count == arena->block_cap) {
        const size_t new_cap = (arena->block_cap + 1) * 2;
        arena->blocks = (char**)xrealloc(arena->blocks, new_cap * sizeof(*(arena->blocks)));
        arena->block_cap = new_cap;
    }

    arena->blocks[arena->block_count] = arena->ptr;
    
    ++arena->block_count;
}

void ArenaAllocator_delete(ArenaAllocator* arena)
{
    for (size_t i = 0; i < arena->block_count; ++i) {
        free(arena->blocks[i]);
    }
    free(arena->blocks);
}

void debug_print(const char* const in) 
{
    #ifdef DEBUG_PRINT
    puts(in);
    #endif
}

struct option program_args[PROGRAM_ARGS_COUNT + 1] = {
    {"verbose", no_argument, NULL, 'v'},
    {"hotconfig", no_argument, NULL, 'c'},
    {0, 0, 0, 0}
};


bool parse_command_line_args(CommandLineArgs* cmd, const int argc, char* argv[])
{
    memset(cmd, 0x00, sizeof(CommandLineArgs));
    // later
    char c = '\0';

    while ((c = getopt_long(argc, argv, "vc", program_args, NULL)) != -1) {
        switch (c) {
        // number of additional threads
        case 'v':
            cmd->verbose = true;
            break;
        case 'c':
            cmd->hot_config = true;
            break;
        // missing arg
        case ':':
            fprintf(stderr, "%s: option '-%c' requires an argument\n",
                    argv[0], optopt);
            return false;
        // help
        case '?':

            return false;
        // invalid
        case 0:
            break;            
        // invalid
        default:
            fprintf(stderr, "%s: option `-%c' is invalid\n",
                    argv[0], optopt);
            return false;
        }
    }


    return true;

}

uint64_t hash_uint64(uint64_t x) {
    x *= 0xff51afd7ed558ccd;
    x ^= x >> 32;
    return x;
}

uint64_t hash_ptr(const void* ptr) {
    return hash_uint64((uintptr_t)ptr);
}

uint64_t hash_mix(uint64_t x, uint64_t y) {
    x ^= y;
    x *= 0xff51afd7ed558ccd;
    x ^= x >> 32;
    return x;
}

uint64_t hash_bytes(const void* ptr, size_t len) {
    uint64_t x = 0xcbf29ce484222325;
    const char* buf = (const char*)ptr;
    for (size_t i = 0; i < len; i++) {
        x ^= buf[i];
        x *= 0x100000001b3;
        x ^= x >> 32;
    }
    return x;
}

uint64_t map_get_uint64_from_uint64(Map *map, uint64_t key) {
    if (map->len == 0) {
        return 0;
    }
    assert(IS_POW2(map->cap));
    size_t i = (size_t)hash_uint64(key);
    assert(map->len < map->cap);
    for (;;) {
        i &= map->cap - 1;
        if (map->keys[i] == key) {
            return map->vals[i];
        } else if (!map->keys[i]) {
            return 0;
        }
        i++;
    }
    return 0;
}

void map_grow(Map* map, size_t new_cap) {
    new_cap = CLAMP_MIN(new_cap, 16);
    Map new_map = {
        .keys = xcalloc(new_cap, sizeof(uint64_t)),
        .vals = xmalloc(new_cap * sizeof(uint64_t)),
        .cap = new_cap,
    };
    for (size_t i = 0; i < map->cap; i++) {
        if (map->keys[i]) {
            map_put_uint64_from_uint64(&new_map, map->keys[i], map->vals[i]);
        }
    }
    free((void *)map->keys);
    free(map->vals);
    *map = new_map;
}

void map_put_uint64_from_uint64(Map* map, uint64_t key, uint64_t val) {
    assert(key);
    if (!val) {
        return;
    }
    if (2*map->len >= map->cap) {
        map_grow(map, 2*map->cap);
    }
    assert(2*map->len < map->cap);
    assert(IS_POW2(map->cap));
    size_t i = (size_t)hash_uint64(key);
    for (;;) {
        i &= map->cap - 1;
        if (!map->keys[i]) {
            map->len++;
            map->keys[i] = key;
            map->vals[i] = val;
            return;
        } else if (map->keys[i] == key) {
            map->vals[i] = val;
            return;
        }
        i++;
    }
}

void *map_get(Map* map, const void* key) {
    return (void *)(uintptr_t)map_get_uint64_from_uint64(map, (uint64_t)(uintptr_t)key);
}

void map_put(Map* map, const void* key, void* val) {
    map_put_uint64_from_uint64(map, (uint64_t)(uintptr_t)key, (uint64_t)(uintptr_t)val);
}

void* map_get_from_uint64(Map* map, uint64_t key) {
    return (void*)(uintptr_t)map_get_uint64_from_uint64(map, key);
}

void map_put_from_uint64(Map* map, uint64_t key, void* val) {
    map_put_uint64_from_uint64(map, key, (uint64_t)(uintptr_t)val);
}

uint64_t map_get_uint64(Map* map, void* key) {
    return map_get_uint64_from_uint64(map, (uint64_t)(uintptr_t)key);
}

void map_put_uint64(Map* map, void* key, uint64_t val) {
    map_put_uint64_from_uint64(map, (uint64_t)(uintptr_t)key, val);
}

void map_test(void) {
    Map map = {0};
    enum { N = 1024 };
    for (size_t i = 1; i < N; i++) {
        map_put(&map, (void *)i, (void *)(i+1));
    }
    for (size_t i = 1; i < N; i++) {
        void *val = map_get(&map, (void *)i);
        assert(val == (void *)(i+1));
    }
}

// String interning

ArenaAllocator intern_arena;
Map interns;

const char* str_intern_range(const char* start, const char* end) {
    size_t len = end - start;
    uint64_t hash = hash_bytes(start, len);
    uint64_t key = hash ? hash : 1;
    Intern* intern = map_get_from_uint64(&interns, key);
    for (Intern *it = intern; it; it = it->next) {
        if (it->len == len && strncmp(it->str, start, len) == 0) {
            return it->str;
        }
    }

    Intern* new_intern = ArenaAllocator_allocate(&intern_arena, offsetof(Intern, str) + len + 1);
    new_intern->len = len;
    new_intern->next = intern;
    memcpy(new_intern->str, start, len);
    new_intern->str[len] = 0;
    map_put_from_uint64(&interns, key, new_intern);
    return new_intern->str;
}

const char* str_intern(const char* str) {
    return str_intern_range(str, str + strlen(str));
}

bool str_islower(const char* str) {
    while (*str) {
        if (isalpha(*str) && !islower(*str)) {
            return false;
        }
        str++;
    }
    return true;
}


#ifdef __cplusplus
}
#endif

#undef COMMON_UTILS_IMPLEMENTATION

#endif

