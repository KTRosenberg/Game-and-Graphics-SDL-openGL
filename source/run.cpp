#include "opengl.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/quaternion.hpp>

#ifdef _WIN32
#   define SDL_MAIN_HANDLED
#endif
#include "sdl.hpp"

#include <iostream>
#include <string>
#include <array>
#include <vector>

#include "shader.hpp"
#include "texture.hpp"
#include "camera.hpp"
#include "mesh_generator.hpp"

#define WINDOW_HEADER ("")

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720
#define TIME_UNIT_TO_SECONDS 1000
#define FRAMES_PER_SECOND 60

//#define DISPLAY_FPS

#define IMG_PATH_1 "textures/final_rush_walkway_2.png"
#define IMG_PATH_2 "textures/brick.png"
#define IMG_PATH_3 "textures/lavatile.jpg"

int ignore_mouse_movement(void* unused, SDL_Event* event)
{
    return (event->type == SDL_MOUSEMOTION) ? 0 : 1;
}

#define DEBUG_PRINT

void debug_print(const char* const in);
void debug_print(const char* const in) 
{
    #ifdef DEBUG_PRINT
    puts(in);
    #endif
}

#define FP_CAM
// #define FREE_CAM

#define MOUSE_ON

#define CUBES
//#define SPHERES
//#define TORI

SDL_Window* window = NULL;

// based on tutorial at 
// http://headerphile.com/sdl2/opengl-part-1-sdl-opengl-awesome/

typedef void* (*Fn_MemoryAllocator)(size_t bytes);


void* xmalloc(size_t num_bytes);
void* xmalloc(size_t num_bytes)
{
    void* ptr = malloc(num_bytes);
    if (ptr == NULL) {
        perror("xmalloc failed");
        exit(EXIT_FAILURE);
    }

    return ptr;
}

void* xcalloc(size_t num_elems, size_t elem_size);
void* xcalloc(size_t num_elems, size_t elem_size)
{
    void* ptr = calloc(num_elems, elem_size);
    if (ptr == NULL) {
        perror("xcalloc failed");
        exit(EXIT_FAILURE);
    }

    return ptr;
}

void* xcalloc_1arg(size_t bytes);
void* xcalloc_1arg(size_t bytes)
{
    return xcalloc(1, bytes);
}

void* xrealloc(void* ptr, size_t num_bytes);
void* xrealloc(void* ptr, size_t num_bytes)
{
    ptr = realloc(ptr, num_bytes);
    if (ptr == NULL) {
        perror("xrealloc failed");
        exit(EXIT_FAILURE);
    }

    return ptr;
}

// TEXTURES, GEOMETRY
typedef struct TextureData {
    Texture* ids;
    size_t count;
} TextureData;

void TextureData_init(TextureData* t, const size_t id_count) 
{
    t->ids = (Texture*)xmalloc(id_count * sizeof(t->ids));
    t->count = id_count;
    glGenTextures(t->count, t->ids);
}

void TextureData_init_static(TextureData* t, const size_t id_count, Texture* buffer)
{
    t->ids = buffer;
    t->count = id_count;
    glGenTextures(id_count, t->ids);
}

void TextureData_delete(TextureData* t)
{
    glDeleteTextures(t->count, t->ids);
    free(t);
}
void TextureData_delete_static(TextureData* t)
{
    glDeleteTextures(t->count, t->ids);
}

struct sceneData {
    glm::mat4 m_model;
    glm::mat4 m_view;
    glm::mat4 m_projection;
} scene;

template <typename T>
struct CappedArray {
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


    typedef CappedArray* iterator;
    typedef const CappedArray* const_iterator;
    iterator begin() { return &this->array[0]; }
    iterator end() { return &this->array[this->cap]; }
};

template <typename T>
void CappedArray_init(CappedArray<T>* arr, size_t cap, Fn_MemoryAllocator alloc) {
    arr->array = (T*)alloc(sizeof(T) * cap);
    arr->count = 0;
    arr->cap = cap;
}

template <typename T, size_t N>
struct CappedArrayStatic {
    size_t cap;
    size_t count;
    T array[N];

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
        return N;
    }

    typedef CappedArrayStatic* iterator;
    typedef const CappedArrayStatic* const_iterator;
    iterator begin() { return &this->array[0]; }
    iterator end() { return &this->array[N]; }
};

// ATTRIBUTES AND VERTEX ARRAYS

typedef struct AttributeData {
    GLuint    index;
    GLint     size;
    GLenum    type;
    GLboolean normalized;
    GLsizei   stride;
    GLvoid*   pointer;
    GLchar*   name;
} AttributeData;

void AttributeData_init(
    AttributeData* a,
    GLuint index,
    GLint size,
    GLenum type,
    GLboolean normalized,
    GLsizei stride,
    GLvoid* pointer,
    GLchar* name    
) {
    a->index = index;
    a->size = size;
    a->type = type;
    a->normalized = normalized;
    a->stride = stride;
    a->pointer = pointer;
    a->name = name;
}

// #define VAO_ATTRIBUTE_MAIN_TEST_COUNT 3
// typedef struct _VertexArrayData {
//     VertexArray vao;
//     size_t attribute_count;
//     AttributeData attributes[VAO_ATTRIBUTE_MAIN_TEST_COUNT];
// } VertexArrayData;

// #define VAO_ATTRIBUTE_LIGHT_DAT_COUNT 3
// typedef struct _VertexArrayData {
//     VertexArray vao;
//     size_t attribute_count;
//     AttributeData attributes[VAO_ATTRIBUTE_MAIN_TEST_COUNT];
// } VertexArrayData;

// VERTEX BUFFERS


// struct MemoryAllocator {
//     void* type;
//     Fn_MemoryAllocator fn_alloc;
    
//     void* alloc(size_t bytes)
//     {
//         return Fn_MemoryAllocator(type, bytes);
//     }
// };

// void MemoryAllocator_init(MemoryAllocator* ma, void* type, Fn_MemoryAllocator* fn_alloc, Fn_MemoryAllocatorType_init fn_init, void* args)
// {
//     ma->fn_alloc = fn_alloc;
//     ma->type = alloc;
//     fn_init(fn_alloc, args);
// }

#define ARENA_DEFAULT_BLOCK_SIZE (1024 * 1024)
#define ARENA_ALIGNMENT 8


#define MIN(x, y) ((x) <= (y) ? (x) : (y))
#define MAX(x, y) ((x) >= (y) ? (x) : (y))
#define CLAMP_MAX(x, max) MIN(x, max)
#define CLAMP_MIN(x, min) MAX(x, min)

#define ALIGN_DOWN(n, a) ((n) & ~((a) - 1))
#define ALIGN_UP(n, a) ALIGN_DOWN((n) + (a) - 1, (a))
#define ALIGN_DOWN_PTR(p, a) ((void *)ALIGN_DOWN((uintptr_t)(p), (a)))
#define ALIGN_UP_PTR(p, a) ((void *)ALIGN_UP((uintptr_t)(p), (a)))

typedef struct ArenaAllocator {
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

template <typename T>
struct ArenaBuffer {
    T* memory;
    size_t count;

    operator T*()
    {
        return this->memory;
    }

    inline T& operator[](size_t i)
    {
        return this->memory[i];
    }
     
    inline const T& operator[](size_t i) const 
    {
        return this->memory[i];
    }

    inline size_t size() const
    {
        return sizeof(T) * count;
    }

    inline size_t element_size() const
    {
        return sizeof(T);
    }
};


void ArenaBuffer_init_bytes(ArenaAllocator* arena, ArenaBuffer<char>* buffer, size_t count) 
{
    buffer->memory = (char*)ArenaAllocator_allocate(arena, count);
    buffer->count = count;
}

template <typename T>
void ArenaBuffer_init(ArenaAllocator* arena, ArenaBuffer<T>* buffer, size_t count) 
{
    buffer->memory = (T*)ArenaAllocator_allocate(arena, count * sizeof(T));
    buffer->count = count;
}


typedef struct VertexBufferData {
    VertexBuffer vbo;
    ElementBuffer ebo;
    size_t    v_cap;
    size_t    v_count;
    size_t    i_cap;
    size_t    i_count;
    GLfloat*  vertices;
    GLuint*   indices;
} VertexBufferData;



struct VertexBufferDataAlt {
    VertexBuffer vbo;
    ElementBuffer ebo;
    CappedArray<GLfloat> vertices;
    CappedArray<GLuint>  indices;
} VertexBufferDataAlt;

struct VertexBufferDataBatches {
    CappedArray<VertexBuffer> vbos;
    CappedArray<ElementBuffer> ebos;

    CappedArray<GLfloat> vertex_batches;
    CappedArray<GLuint> vertex_batch_offsets;
    
    CappedArray<GLuint> index_batches; 
    CappedArray<GLuint> index_batch_offsets;
} VertexBufferDataBatches;

typedef VertexBufferData VBData;



void VertexBufferData_init(
    VertexBufferData* g,
    const size_t v_cap,
    const size_t i_cap,
    Fn_MemoryAllocator alloc
) {
    glGenBuffers(2, (GLBuffer*)&g->vbo);

    g->v_cap = v_cap;
    g->i_cap = i_cap;

    g->vertices = (GLfloat*)alloc(sizeof(GLfloat) * g->v_cap);
    g->indices  = (GLuint*)alloc(sizeof(GLuint) * g->i_cap);

    g->v_count = 0;
    g->i_count = 0;
}

void VertexBufferData_init_static(
    VertexBufferData* g,
    const size_t v_cap,
    GLfloat* vertices,
    const size_t i_cap,
    GLuint* indices
) {
    glGenBuffers(2, (GLBuffer*)&g->vbo);

    g->v_cap = v_cap;
    g->i_cap = i_cap;

    g->vertices = vertices;
    g->indices  = indices;

    g->v_count = 0;
    g->i_count = 0;
}

void VertexBufferData_delete(VertexBufferData* g)
{
    glDeleteBuffers(2, (GLBuffer*)&g->vbo);
    free(g->vertices);
    free(g->indices);
}
void VertexBufferData_delete_static(VertexBufferData* g)
{
    glDeleteBuffers(2, (GLBuffer*)&g->vbo);
}


typedef struct VertexAttributeArray {
    VertexArray vao;
    size_t stride;
} VertexAttributeArray;

typedef VertexAttributeArray VAttribArr;

inline void VertexAttributeArray_init(VertexAttributeArray* vao, size_t stride) 
{
    glGenVertexArrays(1, &vao->vao);
    vao->stride = stride;
}
inline void VertexAttributeArray_delete(VertexAttributeArray* vao) 
{
    glDeleteVertexArrays(1, &vao->vao);
}
inline void* attribute_offsetof(size_t offset)
{
    return (void*)(offset * sizeof(GLfloat));
}
inline size_t attribute_sizeof(VertexAttributeArray* vao) 
{
    return vao->stride * sizeof(GLfloat);
}

inline void gl_set_and_enable_vertex_attrib_ptr(GLuint index, GLint size, GLenum type, GLboolean normalized, size_t offset, VertexAttributeArray* va)
{
    glVertexAttribPointer(index, size, type, normalized, attribute_sizeof(va), attribute_offsetof(offset));            
    glEnableVertexAttribArray(index);
}

inline void gl_bind_buffers_and_upload_data(VertexBufferData* vbd, size_t v_cap, size_t i_cap, GLenum usage)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbd->vbo);
    glBufferData(GL_ARRAY_BUFFER, v_cap * sizeof(GLfloat), vbd->vertices, usage);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbd->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, i_cap * sizeof(GLuint), vbd->indices, usage);            
}


// COLLISION INFO
typedef struct CollisionStatus {
    bool      collided;
    glm::vec3 point;
} CollisionStatus;

void CollisionStatus_init(CollisionStatus* cs, const bool collided, glm::vec3 point)
{
    cs->collided = collided;
    cs->point = point;
}

typedef CollisionStatus (*Fn_CollisionHandler)(glm::vec3 incoming);

typedef struct Collider {
    glm::vec3 a;
    glm::vec3 b;
    Fn_CollisionHandler handler;
} Collider; 

// WORLD STATE
typedef struct Room {
    VertexBufferData  geometry;
    Collider* collision_data;
    glm::mat4 matrix;
} Room;

typedef struct {
    Room* rooms;
    glm::mat4 m_view;
    glm::mat4 m_projection;
} World;

typedef struct GLData {
    SDL_GLContext context;
    TextureData textures;
} GLData;

GLData gl_data;



// glm::vec3 light_pos(1.2f, 1.0f, 2.0f);
// glm::light_color(1.0f, 1.0f, 1.0f);
// glm::toy_color(1.0f, 0.5f, 0.31f);
// glm::result = light_color * toy_color;


//////////////

int main(int argc, char* argv[])
{
    std::cout << std::boolalpha << std::is_pod<ArenaAllocator>::value  << std::endl;

    ArenaAllocator arena;
    ArenaAllocator_init(&arena);

    ArenaBuffer<char> buff;
    ArenaBuffer_init<char>(&arena, &buff, 26);
    ArenaBuffer<char> buffc;
    ArenaBuffer_init<char>(&arena, &buffc, 26);

    {
        char* it = buff;
        char* end = &buffc[buffc.count];
        while (it != end) {
            *it = '0';
            ++it;
        }
    }

    for (size_t i = 0; i < buff.count; ++i) {
        buff[i] = 'a' + i;
    }
    for (size_t i = 0; i < buffc.count; ++i) {
        buffc[i] = 'a' + i;
    }

    {
        char* it = buff;
        char* end = &buffc[buffc.count];
        while (it != end) {
            //std::cout << *it << std::endl;
            ++it;
        }
    }

    ArenaAllocator_delete(&arena);

    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "%s\n", "SDL could not initialize");
        return EXIT_FAILURE;
    }
    
    // MOUSE ///////////////////////////////////////////// 
    // hide the cursor
    SDL_ShowCursor(SDL_DISABLE);
    
    // ignore mouse movement events
    #ifndef MOUSE_ON
    SDL_SetEventFilter(ignore_mouse_movement, nullptr); ///////////////////////////
    #endif
    // openGL initialization ///////////////////////////////////////////////////
    
    if (argc >= 3) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, atoi(argv[1]));
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, atoi(argv[2]));
    } else {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);        
    }
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

    // create the window
    if (NULL == (window = SDL_CreateWindow(
        WINDOW_HEADER,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI)))
    {
        fprintf(stderr, "Window could not be created\n");
        return EXIT_FAILURE;
    }
    
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if(!(IMG_Init(imgFlags) & imgFlags)) {
     	fprintf(stderr, "SDL_image could not initialize, SDL_image Error: %s\n", IMG_GetError());
        SDL_DestroyWindow(window);
        return EXIT_FAILURE;
    }
    
	gl_data.context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
    glewInit();

    bool status;
    std::string glsl_perlin_noise = Shader::retrieve_src_from_file("shaders/perlin_noise.glsl", &status);
    if (!status) {
        return false;
    } 

    // SHADERS
    Shader shader_2d;
    shader_2d.load_from_file(
        "shaders/parallax/parallax_v2.vrts",
        "shaders/parallax/parallax_v2.frgs",
        glsl_perlin_noise,
        glsl_perlin_noise
    );
    if (!shader_2d.is_valid()) {
        fprintf(stderr, "ERROR: shader_2d\n");
        return EXIT_FAILURE;
    }
///////////////

    const GLfloat ASPECT = (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT;

    const size_t STRIDE = 9;

// QUADS
    const size_t COUNT_QUADS = 4;
    const size_t POINTS_PER_QUAD = 4;
    const size_t POINTS_PER_TRI = 3;
    const size_t TRIS_PER_QUAD = 2;
    const size_t len_v_tris = STRIDE * COUNT_QUADS * POINTS_PER_QUAD;
    const size_t len_i_tris = COUNT_QUADS * TRIS_PER_QUAD * POINTS_PER_TRI * 3;

    const GLfloat wf = 1.0f;
    const GLfloat hf = 1.0f;

    const GLfloat OFF = 0.0f * ASPECT * 2.0f;

    GLfloat T[] = {
       -wf * ASPECT,  hf,  0.0f,    1.0, 0.0, 1.0, 1.0,    0.0, 0.0,  // top left
       -wf * ASPECT, -hf,  0.0f,    1.0, 0.0, 1.0, 1.0,    0.0, 1.0,  // bottom left
        wf * ASPECT, -hf,  0.0f,    1.0, 0.0, 1.0, 1.0,    1.0, 1.0, // bottom right
        wf * ASPECT,  hf,  0.0f,    1.0, 0.0, 1.0, 1.0,    1.0, 0.0, // top right

       -wf * ASPECT + (OFF),  hf,  1.0f,    1.0, 0.0, 0.0, 1.0,    0.0, 0.0,  // top left
       -wf * ASPECT + (OFF), -hf,  1.0f,    1.0, 0.0, 0.0, 1.0,    0.0, 1.0,  // bottom left
        wf * ASPECT + (OFF), -hf,  1.0f,    1.0, 0.0, 0.0, 1.0,    1.0, 1.0, // bottom right
        wf * ASPECT + (OFF),  hf,  1.0f,    1.0, 0.0, 0.0, 1.0,    1.0, 0.0, // top right

       -wf * ASPECT + (2 * OFF),  hf,  2.0f,    0.0, 1.0, 0.0, 1.0,    0.0, 0.0,  // top left
       -wf * ASPECT + (2 * OFF), -hf,  2.0f,    0.0, 1.0, 0.0, 1.0,    0.0, 1.0,  // bottom left
        wf * ASPECT + (2 * OFF), -hf,  2.0f,    0.0, 1.0, 0.0, 1.0,    1.0, 1.0, // bottom right
        wf * ASPECT + (2 * OFF),  hf,  2.0f,    0.0, 1.0, 0.0, 1.0,    1.0, 0.0, // top right


       -wf * ASPECT + (3 * OFF),  hf,  3.0f,    0.0, 0.0, 1.0, 1.0,    0.0, 0.0,  // top left
       -wf * ASPECT + (3 * OFF), -hf,  3.0f,    0.0, 0.0, 1.0, 1.0,    0.0, 1.0,  // bottom left
        wf * ASPECT + (3 * OFF), -hf,  3.0f,    0.0, 0.0, 1.0, 1.0,    1.0, 1.0, // bottom right
        wf * ASPECT + (3 * OFF),  hf,  3.0f,    0.0, 0.0, 1.0, 1.0,    1.0, 0.0, // top right


       -wf * ASPECT + (4 * OFF),  hf,  4.0f,    1.0, 1.0, 1.0, 1.0,    0.0, 0.0,  // top left
       -wf * ASPECT + (4 * OFF), -hf,  4.0f,    1.0, 1.0, 1.0, 1.0,    0.0, 1.0,  // bottom left
        wf * ASPECT + (4 * OFF), -hf,  4.0f,    1.0, 1.0, 1.0, 1.0,    1.0, 1.0, // bottom right
        wf * ASPECT + (4 * OFF),  hf,  4.0f,    1.0, 1.0, 1.0, 1.0,    1.0, 0.0 // top right
    };
    GLuint TI[] = {  // note that we start from 0!
        0, 1, 2,
        2, 3, 0,

        0, 1, 2,
        2, 3, 0,

        0, 1, 2,
        2, 3, 0,



        4, 5, 6,
        6, 7, 4,

        4, 5, 6,
        6, 7, 4,

        4, 5, 6,
        6, 7, 4,



        8, 9 ,10,
        10, 11, 8,

        8, 9 ,10,
        10, 11, 8,

        8, 9 ,10,
        10, 11, 8,



        12, 13, 14,
        14, 15, 12,

        12, 13, 14,
        14, 15, 12,

        12, 13, 14,
        14, 15, 12,



        16, 17, 18,
        18, 19, 16,

        16, 17, 18,
        18, 19, 16,

        16, 17, 18,
        18, 19, 16,
    };

// TOTAL ALLOCATION
    const size_t BATCH_COUNT = 1024;
    const size_t GUESS_VERTS_PER_DRAW = 4;
    const size_t BATCH_COUNT_EXTRA = BATCH_COUNT * GUESS_VERTS_PER_DRAW * STRIDE;

//////////////////////////////////////////////////

    VertexAttributeArray vao_2d2;
    VertexBufferData tri_data;

    VertexAttributeArray_init(&vao_2d2, STRIDE);

    glBindVertexArray(vao_2d2.vao);

        GLfloat tris_VBO_data[BATCH_COUNT_EXTRA * sizeof(GLfloat)];
        GLuint  tris_EBO_data[BATCH_COUNT_EXTRA * sizeof(GLuint)];

        VertexBufferData_init_static(
            &tri_data, 
            BATCH_COUNT_EXTRA,
            tris_VBO_data,
            BATCH_COUNT_EXTRA,
            tris_EBO_data
        );
        tri_data.i_count = len_i_tris;

        for (size_t i = 0; i < len_v_tris; ++i) {
            tris_VBO_data[i] = T[i];
        }
        for (size_t i = 0; i < len_i_tris; ++i) {
            tris_EBO_data[i] = TI[i];
        }


        //std::cout << b[0] << std::endl;

        gl_bind_buffers_and_upload_data(&tri_data, BATCH_COUNT_EXTRA, BATCH_COUNT_EXTRA, GL_STATIC_DRAW);
        // POSITION
        gl_set_and_enable_vertex_attrib_ptr(0, 3, GL_FLOAT, GL_FALSE, 0, &vao_2d2);
        // COLOR
        gl_set_and_enable_vertex_attrib_ptr(1, 4, GL_FLOAT, GL_FALSE, 3, &vao_2d2);
        // UV
        gl_set_and_enable_vertex_attrib_ptr(2, 2, GL_FLOAT, GL_FALSE, 7, &vao_2d2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

///////////////////////////////////////////////////////////////////////////////////////////////////////////

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	//glEnable(GL_MULTISAMPLE);
	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA);
	

    glDepthRange(0, 1);
    glDepthFunc(GL_LEQUAL);
    
    SDL_GL_SetSwapInterval(1);
    
    printf("USING GL VERSION: %s\n", glGetString(GL_VERSION));

    glm::mat4 mat_ident(1.0f);
    glm::mat4 mat_projection = glm::ortho(-1.0f * ASPECT, 1.0f * ASPECT, -1.0f, 1.0f, -1.0f * 100, 1.0f * 100);
    //glm::mat4 mat_projection = glm::perspective(glm::radians(45.0f), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

    bool keep_running = true;
    SDL_Event event;

    const Uint32 INTERVAL = TIME_UNIT_TO_SECONDS / FRAMES_PER_SECOND;
    Uint32 start_time = SDL_GetTicks();
    Uint32 prev_time = start_time;
    Uint32 curr_time = start_time;
    Uint64 delta_time = 0;
    Uint64 dt = 0;

//////////////////
// TEST INPUT
    glm::vec3 start_pos(0.0f, 0.0f, 1.0f);
    
    FreeCamera main_cam(start_pos);
    main_cam.orientation = glm::quat();
    main_cam.speed = 0.01;
    // ViewCamera_init(
    //     &main_cam,
    //     start_pos,
    //     ViewCamera_default_speed,
    //     -1000.0f,
    //     1000.0f,
    //     0.0f,
    //     0.0f,
    //     0.0f,
    //     0.0f
    // );
    
    const Uint8* key_states = SDL_GetKeyboardState(NULL);

    const Uint8* up         = &key_states[SDL_SCANCODE_W];
    const Uint8* down       = &key_states[SDL_SCANCODE_S];
    const Uint8* left       = &key_states[SDL_SCANCODE_A];
    const Uint8* right      = &key_states[SDL_SCANCODE_D];
    const Uint8* rot_r      = &key_states[SDL_SCANCODE_RIGHT];
    const Uint8* rot_l      = &key_states[SDL_SCANCODE_LEFT];
//  const Uint8& up_right   = key_states[SDL_SCANCODE_E];
//  const Uint8& up_left    = key_states[SDL_SCANCODE_Q];
//  const Uint8& down_right = key_states[SDL_SCANCODE_X];
//  const Uint8& down_left  = key_states[SDL_SCANCODE_Z];
    const Uint8* forwards = &key_states[SDL_SCANCODE_UP];
    const Uint8* backwards = &key_states[SDL_SCANCODE_DOWN];

    const Uint8* reset = &key_states[SDL_SCANCODE_0];
    const Uint8* toggle_projection = &key_states[SDL_SCANCODE_P];

    bool projection_is_ortho = true;

    const double POS_ACC = 1.06;
    const double NEG_ACC = 1.0 / POS_ACC;

    double up_acc = 1.0;
    double down_acc = 1.0;
    double left_acc = 1.0;
    double right_acc = 1.0;
    double forwards_acc = 1.0;
    double backwards_acc = 1.0;


/////////////////
// MAIN LOOP
#ifdef DEBUG_PRINT
    glm::vec3 prev_pos(0.0);
#endif


    // Texture tex0;
    // GL_texture_gen_and_load_1(&tex0, "./textures/bg_test_2_w2.png", GL_TRUE, GL_REPEAT, GL_CLAMP_TO_EDGE);
    // Texture tex1;
    // GL_texture_gen_and_load_1(&tex1, "./textures/bg_test_2_w2.png", GL_TRUE, GL_REPEAT, GL_CLAMP_TO_EDGE);
    Texture tex0;
    GL_texture_gen_and_load_1(&tex0, "./textures/bg_test_3.png", GL_TRUE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    Texture tex1;
    GL_texture_gen_and_load_1(&tex1, "./textures/bg_test_3.png", GL_TRUE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    // TEXTURE 0
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, texture[0]);
    // glUniform1i(glGetUniformLocation(prog_shader, "tex0"), 0);
    // //
    // glUniform1i(glGetUniformLocation(prog_shader, "tex0"), 0);


    glUseProgram(shader_2d);

    UniformLocation RES_LOC = glGetUniformLocation(shader_2d, "u_resolution");
    UniformLocation COUNT_LAYERS = glGetUniformLocation(shader_2d, "u_count_layers");
    UniformLocation OFFSET_LAYERS_X = glGetUniformLocation(shader_2d, "u_offset_layers_x");

    const GLint UVAL_COUNT_LAYERS = COUNT_QUADS;
    const GLfloat UVAL_OFFSET_LAYERS = (1.0f / (GLfloat)UVAL_COUNT_LAYERS); 

    glUniform2fv(RES_LOC, 1, glm::value_ptr(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT)));
    glUniform1i(COUNT_LAYERS, UVAL_COUNT_LAYERS);
    glUniform1f(OFFSET_LAYERS_X, UVAL_OFFSET_LAYERS);

    // TEXTURE 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex0);
    glUniform1i(glGetUniformLocation(shader_2d, "tex0"), 0);
    // TEXTURE 0
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex1);
    glUniform1i(glGetUniformLocation(shader_2d, "tex1"), 1);


    while (keep_running) {
        curr_time = SDL_GetTicks();
        // frame-rate independence?
        dt = (Uint64)(curr_time - prev_time);
        delta_time = dt;

        if (dt < INTERVAL) {
            continue;
        }

        do {
            dt -= INTERVAL;
        } while (dt >= INTERVAL);
        prev_time = curr_time;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                keep_running = false;
            }
        }
        main_cam.orientation = glm::quat();

    //////////////////
    // TEST INPUT
        {

            const double CHANGE = (delta_time / (GLfloat)TIME_UNIT_TO_SECONDS);

            if (*up) {
                FreeCamera_process_directional_movement(&main_cam, Movement_Direction::UPWARDS, CHANGE * up_acc);
                up_acc *= POS_ACC;
            } else {
                if (up_acc > 1.0) {
                    FreeCamera_process_directional_movement(&main_cam, Movement_Direction::UPWARDS, CHANGE * up_acc);
                }
                up_acc = glm::max(1.0, up_acc * NEG_ACC);
            }
            if (*down) {
                FreeCamera_process_directional_movement(&main_cam, Movement_Direction::DOWNWARDS, CHANGE * down_acc);
                down_acc *= POS_ACC;
            } else {
                if (down_acc > 1.0) {
                    FreeCamera_process_directional_movement(&main_cam, Movement_Direction::DOWNWARDS, CHANGE * down_acc);
                } 
                down_acc = glm::max(1.0, down_acc * NEG_ACC);  
            }

            if (*left) {
                FreeCamera_process_directional_movement(&main_cam, Movement_Direction::LEFTWARDS, CHANGE * left_acc);
                left_acc *= POS_ACC;
            } else {
                if (left_acc > 1.0) {
                    FreeCamera_process_directional_movement(&main_cam, Movement_Direction::LEFTWARDS, CHANGE * left_acc);
                }
                left_acc = glm::max(1.0, left_acc * NEG_ACC);
            }

            if (*right) {
                FreeCamera_process_directional_movement(&main_cam, Movement_Direction::RIGHTWARDS, CHANGE * right_acc);
                right_acc *= POS_ACC;
            } else {
                if (right_acc > 1.0) {
                    FreeCamera_process_directional_movement(&main_cam, Movement_Direction::RIGHTWARDS, CHANGE * right_acc);
                }
                right_acc = glm::max(1.0, right_acc * NEG_ACC);
            }

            if (*forwards) {
                FreeCamera_process_directional_movement(&main_cam, Movement_Direction::FORWARDS, CHANGE * forwards_acc);
                forwards_acc *= POS_ACC;
            } else {
                if (forwards_acc > 1.0) {
                    FreeCamera_process_directional_movement(&main_cam, Movement_Direction::FORWARDS, CHANGE * forwards_acc);
                }
                forwards_acc = glm::max(1.0, forwards_acc * NEG_ACC);
            }
            if (*backwards) {
                FreeCamera_process_directional_movement(&main_cam, Movement_Direction::BACKWARDS, CHANGE * backwards_acc);
                backwards_acc *= POS_ACC;
            } else {
                if (backwards_acc > 1.0) {
                    FreeCamera_process_directional_movement(&main_cam, Movement_Direction::BACKWARDS, CHANGE * backwards_acc);
                } 
                backwards_acc = glm::max(1.0, backwards_acc * NEG_ACC);  
            }

            if (*reset) {
                // FreeCamera_init(
                //     &main_cam,
                //     start_pos,
                //     ViewCamera_default_speed,
                //     -1000.0f,
                //     1000.0f,
                //     0.0f,
                //     0.0f,
                //     0.0f,
                //     0.0f
                // );
                main_cam.position = start_pos;
                main_cam.orientation = glm::quat();

                up_acc        = 1.0;
                down_acc      = 1.0;
                left_acc      = 1.0;
                right_acc     = 1.0;
                backwards_acc = 1.0;
                forwards_acc  = 1.0;
            }
        }

        //main_cam.rotate((GLfloat)curr_time / TIME_UNIT_TO_SECONDS, 0.0f, 0.0f, 1.0f);
    //////////////////
    // DRAW

                
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //glUseProgram(shader_2d);

        UniformLocation MAT_LOC = glGetUniformLocation(shader_2d, "u_matrix");
        UniformLocation TIME_LOC = glGetUniformLocation(shader_2d, "u_time");
        UniformLocation CAM_LOC = glGetUniformLocation(shader_2d, "u_position_cam");

        //glUniformMatrix4fv(MAT_LOC, 1, GL_FALSE, glm::value_ptr(ViewCamera_calc_view_matrix(&main_cam) * mat_ident));
        glUniformMatrix4fv(MAT_LOC, 1, GL_FALSE, glm::value_ptr(
            mat_projection * 
            //FreeCamera_calc_view_matrix(&main_cam) * 
            mat_ident /** 
            glm::translate(mat_ident, glm::vec3(glm::sin((GLfloat)curr_time / TIME_UNIT_TO_SECONDS), 0.0, 0.0))*/
            /** glm::scale(mat_ident, glm::vec3(0.5, 0.5, 0.5))*/ )
        );

        glUniform1f(TIME_LOC, ((GLdouble)curr_time / TIME_UNIT_TO_SECONDS));

        glm::vec3* pos = &main_cam.position;
        #ifdef DEBUG_PRINT

            if (pos->x != prev_pos.x || pos->y != prev_pos.y || pos->z != prev_pos.z) {
                std::cout << "VIEW_POSITION{x : " << pos->x << ", y : " << pos->y << ", z: " << pos->z << "}" << std::endl;
            }
            prev_pos.x = pos->x;
            prev_pos.y = pos->y;
            prev_pos.z = pos->z;
        #endif

        glUniform3fv(CAM_LOC, 1, glm::value_ptr(*pos));


        glBindVertexArray(vao_2d2.vao);
        glDrawElements(GL_TRIANGLES, tri_data.i_count, GL_UNSIGNED_INT, (void*)0);
                
        SDL_GL_SwapWindow(window);

    //////////////////
    }
    
    VertexAttributeArray_delete(&vao_2d2);
    SDL_GL_DeleteContext(gl_data.context);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}
