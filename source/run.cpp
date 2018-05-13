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

#define SCREEN_WIDTH  1280.0
#define SCREEN_HEIGHT 720.0
#define MS_PER_S 1000.0
#define FRAMES_PER_SECOND 60.0

//#define DISPLAY_FPS

#define IMG_PATH_1 "textures/final_rush_walkway_2.png"
#define IMG_PATH_2 "textures/brick.png"
#define IMG_PATH_3 "textures/lavatile.jpg"

int ignore_mouse_movement(void* unused, SDL_Event* event)
{
    return (event->type == SDL_MOUSEMOTION) ? 0 : 1;
}

//#define DEBUG_PRINT

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

void VertexAttributeArray_init(VertexAttributeArray* vao, size_t stride) 
{
    glGenVertexArrays(1, &vao->vao);
    vao->stride = stride;
}
inline void VertexAttributeArray_delete(VertexAttributeArray* vao) 
{
    glDeleteVertexArrays(1, &vao->vao);
}
inline GLvoid* attribute_offsetof(size_t offset)
{
    return (GLvoid*)(offset * sizeof(GLfloat));
}
inline size_t attribute_sizeof(VertexAttributeArray* vao) 
{
    return vao->stride * sizeof(GLfloat);
}

void gl_set_and_enable_vertex_attrib_ptr(GLuint index, GLint size, GLenum type, GLboolean normalized, size_t offset, VertexAttributeArray* va)
{
    glVertexAttribPointer(index, size, type, normalized, attribute_sizeof(va), attribute_offsetof(offset));            
    glEnableVertexAttribArray(index);
}

void gl_bind_buffers_and_upload_data(VertexBufferData* vbd, size_t v_cap, size_t i_cap, GLenum usage)
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


//////////////

int main(int argc, char* argv[])
{
    // std::cout << std::boolalpha << std::is_pod<ArenaAllocator>::value  << std::endl;

    // ArenaAllocator arena;
    // ArenaAllocator_init(&arena);

    // ArenaBuffer<char> buff;
    // ArenaBuffer_init<char>(&arena, &buff, 26);
    // ArenaBuffer<char> buffc;
    // ArenaBuffer_init<char>(&arena, &buffc, 26);

    // {
    //     char* it = buff;
    //     char* end = &buffc[buffc.count];
    //     while (it != end) {
    //         *it = '0';
    //         ++it;
    //     }
    // }

    // for (size_t i = 0; i < buff.count; ++i) {
    //     buff[i] = 'a' + i;
    // }
    // for (size_t i = 0; i < buffc.count; ++i) {
    //     buffc[i] = 'a' + i;
    // }

    // {
    //     char* it = buff;
    //     char* end = &buffc[buffc.count];
    //     while (it != end) {
    //         //std::cout << *it << std::endl;
    //         ++it;
    //     }
    // }

    // ArenaAllocator_delete(&arena);

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
    // SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
    // SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);

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

    size_t STRIDE = 5;

// QUADS
    size_t COUNT_QUADS = 15;
    size_t POINTS_PER_QUAD = 4;
    size_t POINTS_PER_TRI = 3;
    size_t TRIS_PER_QUAD = 2;
    size_t len_v_tris = STRIDE * COUNT_QUADS * POINTS_PER_QUAD;
    size_t len_i_tris = COUNT_QUADS * TRIS_PER_QUAD * POINTS_PER_TRI;

    const GLfloat wf = 1.0f;
    const GLfloat hf = 1.0f;
    const GLfloat OFF = 0.0f * wf * ASPECT;

#define FOUR

#ifdef FOUR
GLdouble tex_res = 4096.0;
    GLuint layers_per_row = (GLuint)(tex_res / SCREEN_WIDTH);
    GLfloat x_off = (GLfloat)(GLdouble)(SCREEN_WIDTH / tex_res);
    GLfloat y_off = (GLfloat)(GLdouble)(SCREEN_HEIGHT / tex_res);
    GLfloat vx_off = 2 * wf * ASPECT;

    GLfloat T[] = {
        // L 0
       (-vx_off + (-wf * ASPECT)),  hf,  0.0f,    (GLfloat)x_off * 0.f, (GLfloat)y_off * 0.f,  // top left
       (-vx_off + (-wf * ASPECT)), -hf,  0.0f,    (GLfloat)x_off * 0.f, (GLfloat)y_off * 1.f,  // bottom left
       (-vx_off + (wf * ASPECT)), -hf,  0.0f,    (GLfloat)x_off * 1.f, (GLfloat)y_off * 1.f, // bottom right
       (-vx_off + (wf * ASPECT)),  hf,  0.0f,    (GLfloat)x_off * 1.f, (GLfloat)y_off * 0.f, // top right

       (-wf * ASPECT),  hf,  0.0f,    (GLfloat)x_off * 0.f, (GLfloat)y_off * 0.f,  // top left
       (-wf * ASPECT), -hf,  0.0f,    (GLfloat)x_off * 0.f, (GLfloat)y_off * 1.f,  // bottom left
       (wf * ASPECT), -hf,  0.0f,    (GLfloat)x_off * 1.f, (GLfloat)y_off * 1.f, // bottom right
       (wf * ASPECT),  hf,  0.0f,    (GLfloat)x_off * 1.f, (GLfloat)y_off * 0.f, // top right

       (vx_off + (-wf * ASPECT)),  hf,  0.0f,    (GLfloat)x_off * 0.f, (GLfloat)y_off * 0.f,  // top left
       (vx_off + (-wf * ASPECT)), -hf,  0.0f,    (GLfloat)x_off * 0.f, (GLfloat)y_off * 1.f,  // bottom left
       (vx_off + (wf * ASPECT)), -hf,  0.0f,    (GLfloat)x_off * 1.f, (GLfloat)y_off * 1.f, // bottom right
       (vx_off + (wf * ASPECT)),  hf,  0.0f,    (GLfloat)x_off * 1.f, (GLfloat)y_off * 0.f, // top right


       // L 1
       (-vx_off + (-wf * ASPECT)),  hf,  1.0f,    (GLfloat)x_off * 1.f, (GLfloat)y_off * 0.f,  // top left
       (-vx_off + (-wf * ASPECT)), -hf,  1.0f,    (GLfloat)x_off * 1.f, (GLfloat)y_off * 1.f,  // bottom left
       (-vx_off + (wf * ASPECT)), -hf,  1.0f,    (GLfloat)x_off * 2.f, (GLfloat)y_off * 1.f, // bottom right
       (-vx_off + (wf * ASPECT)),  hf,  1.0f,    (GLfloat)x_off * 2.f, (GLfloat)y_off * 0.f, // top right

       (-wf * ASPECT),  hf,  1.0f,    (GLfloat)x_off * 1.f, (GLfloat)y_off * 0.f,  // top left
       (-wf * ASPECT), -hf,  1.0f,    (GLfloat)x_off * 1.f, (GLfloat)y_off * 1.f,  // bottom left
       (wf * ASPECT), -hf,  1.0f,    (GLfloat)x_off * 2.f, (GLfloat)y_off * 1.f, // bottom right
       (wf * ASPECT),  hf,  1.0f,    (GLfloat)x_off * 2.f, (GLfloat)y_off * 0.f, // top right

       (vx_off + (-wf * ASPECT)),  hf,  1.0f,    (GLfloat)x_off * 1.f, (GLfloat)y_off * 0.f,  // top left
       (vx_off + (-wf * ASPECT)), -hf,  1.0f,    (GLfloat)x_off * 1.f, (GLfloat)y_off * 1.f,  // bottom left
       (vx_off + (wf * ASPECT)), -hf,  1.0f,    (GLfloat)x_off * 2.f, (GLfloat)y_off * 1.f, // bottom right
       (vx_off + (wf * ASPECT)),  hf,  1.0f,    (GLfloat)x_off * 2.f, (GLfloat)y_off * 0.f, // top right


        // L 2
       -vx_off + -wf * ASPECT + (2 * OFF),  hf,  2.0f,    (GLfloat)x_off * 2.f, (GLfloat)y_off * 0.f,  // top left
       -vx_off + -wf * ASPECT + (2 * OFF), -hf,  2.0f,    (GLfloat)x_off * 2.f, (GLfloat)y_off * 1.f,  // bottom left
       -vx_off +  wf * ASPECT + (2 * OFF), -hf,  2.0f,    (GLfloat)x_off * 3.f, (GLfloat)y_off * 1.f, // bottom right
       -vx_off +  wf * ASPECT + (2 * OFF),  hf,  2.0f,    (GLfloat)x_off * 3.f, (GLfloat)y_off * 0.f, // top right

       -wf * ASPECT + (2 * OFF),  hf,  2.0f,    (GLfloat)x_off * 2.f, (GLfloat)y_off * 0.f,  // top left
       -wf * ASPECT + (2 * OFF), -hf,  2.0f,    (GLfloat)x_off * 2.f, (GLfloat)y_off * 1.f,  // bottom left
        wf * ASPECT + (2 * OFF), -hf,  2.0f,    (GLfloat)x_off * 3.f, (GLfloat)y_off * 1.f, // bottom right
        wf * ASPECT + (2 * OFF),  hf,  2.0f,    (GLfloat)x_off * 3.f, (GLfloat)y_off * 0.f, // top right

       vx_off + -wf * ASPECT + (2 * OFF),  hf,  2.0f,    (GLfloat)x_off * 2.f, (GLfloat)y_off * 0.f,  // top left
       vx_off + -wf * ASPECT + (2 * OFF), -hf,  2.0f,    (GLfloat)x_off * 2.f, (GLfloat)y_off * 1.f,  // bottom left
       vx_off +  wf * ASPECT + (2 * OFF), -hf,  2.0f,    (GLfloat)x_off * 3.f, (GLfloat)y_off * 1.f, // bottom right
       vx_off +  wf * ASPECT + (2 * OFF),  hf,  2.0f,    (GLfloat)x_off * 3.f, (GLfloat)y_off * 0.f, // top right


        // L 3
       -vx_off + -wf * ASPECT + (3 * OFF),  hf,  3.0f,    (GLfloat)x_off * 0.f, (GLfloat)y_off * 1.f,  // top left
       -vx_off + -wf * ASPECT + (3 * OFF), -hf,  3.0f,    (GLfloat)x_off * 0.f, (GLfloat)y_off * 2.f,  // bottom left
       -vx_off +  wf * ASPECT + (3 * OFF), -hf,  3.0f,    (GLfloat)x_off * 1.f, (GLfloat)y_off * 2.f, // bottom right
       -vx_off +  wf * ASPECT + (3 * OFF),  hf,  3.0f,    (GLfloat)x_off * 1.f, (GLfloat)y_off * 1.f, // top right

       -wf * ASPECT + (3 * OFF),  hf,  3.0f,    (GLfloat)x_off * 0.f, (GLfloat)y_off * 1.f,  // top left
       -wf * ASPECT + (3 * OFF), -hf,  3.0f,    (GLfloat)x_off * 0.f, (GLfloat)y_off * 2.f,  // bottom left
        wf * ASPECT + (3 * OFF), -hf,  3.0f,    (GLfloat)x_off * 1.f, (GLfloat)y_off * 2.f, // bottom right
        wf * ASPECT + (3 * OFF),  hf,  3.0f,    (GLfloat)x_off * 1.f, (GLfloat)y_off * 1.f, // top right

       vx_off + -wf * ASPECT + (3 * OFF),  hf,  3.0f,    (GLfloat)x_off * 0.f, (GLfloat)y_off * 1.f,  // top left
       vx_off + -wf * ASPECT + (3 * OFF), -hf,  3.0f,    (GLfloat)x_off * 0.f, (GLfloat)y_off * 2.f,  // bottom left
       vx_off +  wf * ASPECT + (3 * OFF), -hf,  3.0f,    (GLfloat)x_off * 1.f, (GLfloat)y_off * 2.f, // bottom right
       vx_off +  wf * ASPECT + (3 * OFF),  hf,  3.0f,    (GLfloat)x_off * 1.f, (GLfloat)y_off * 1.f, // top right


        // L 4
       -vx_off + -wf * ASPECT + (4 * OFF),  hf,  4.0f,    (GLfloat)x_off * 1.f, (GLfloat)y_off * 1.f,  // top left
       -vx_off + -wf * ASPECT + (4 * OFF), -hf,  4.0f,    (GLfloat)x_off * 1.f, (GLfloat)y_off * 2.f,  // bottom left
       -vx_off +  wf * ASPECT + (4 * OFF), -hf,  4.0f,    (GLfloat)x_off * 2.f, (GLfloat)y_off * 2.f, // bottom right
       -vx_off +  wf * ASPECT + (4 * OFF),  hf,  4.0f,    (GLfloat)x_off * 2.f, (GLfloat)y_off * 1.f, // top right

       -wf * ASPECT + (4 * OFF),  hf,  4.0f,    (GLfloat)x_off * 1.f, (GLfloat)y_off * 1.f,  // top left
       -wf * ASPECT + (4 * OFF), -hf,  4.0f,    (GLfloat)x_off * 1.f, (GLfloat)y_off * 2.f,  // bottom left
        wf * ASPECT + (4 * OFF), -hf,  4.0f,    (GLfloat)x_off * 2.f, (GLfloat)y_off * 2.f, // bottom right
        wf * ASPECT + (4 * OFF),  hf,  4.0f,    (GLfloat)x_off * 2.f, (GLfloat)y_off * 1.f, // top right

       vx_off + -wf * ASPECT + (4 * OFF),  hf,  4.0f,    (GLfloat)x_off * 1.f, (GLfloat)y_off * 1.f,  // top left
       vx_off + -wf * ASPECT + (4 * OFF), -hf,  4.0f,    (GLfloat)x_off * 1.f, (GLfloat)y_off * 2.f,  // bottom left
       vx_off +  wf * ASPECT + (4 * OFF), -hf,  4.0f,    (GLfloat)x_off * 2.f, (GLfloat)y_off * 2.f, // bottom right
       vx_off +  wf * ASPECT + (4 * OFF),  hf,  4.0f,    (GLfloat)x_off * 2.f, (GLfloat)y_off * 1.f, // top right
    };
#else
    GLfloat T[] = {
       -wf * ASPECT,  hf,  0.0f,    0.0, 0.0,  // top left
       -wf * ASPECT, -hf,  0.0f,    0.0, 1.0,  // bottom left
        wf * ASPECT, -hf,  0.0f,    1.0, 1.0, // bottom right
        wf * ASPECT,  hf,  0.0f,    1.0, 0.0, // top right

       -wf * ASPECT + (OFF),  hf,  1.0f,    0.0, 0.0,  // top left
       -wf * ASPECT + (OFF), -hf,  1.0f,    0.0, 1.0,  // bottom left
        wf * ASPECT + (OFF), -hf,  1.0f,    1.0, 1.0, // bottom right
        wf * ASPECT + (OFF),  hf,  1.0f,    1.0, 0.0, // top right

       -wf * ASPECT + (2 * OFF),  hf,  2.0f,    0.0, 0.0,  // top left
       -wf * ASPECT + (2 * OFF), -hf,  2.0f,    0.0, 1.0,  // bottom left
        wf * ASPECT + (2 * OFF), -hf,  2.0f,    1.0, 1.0, // bottom right
        wf * ASPECT + (2 * OFF),  hf,  2.0f,    1.0, 0.0, // top right


       -wf * ASPECT + (3 * OFF),  hf,  3.0f,    0.0, 0.0,  // top left
       -wf * ASPECT + (3 * OFF), -hf,  3.0f,    0.0, 1.0,  // bottom left
        wf * ASPECT + (3 * OFF), -hf,  3.0f,    1.0, 1.0, // bottom right
        wf * ASPECT + (3 * OFF),  hf,  3.0f,    1.0, 0.0, // top right


       -wf * ASPECT + (4 * OFF),  hf,  4.0f,    0.0, 0.0,  // top left
       -wf * ASPECT + (4 * OFF), -hf,  4.0f,    0.0, 1.0,  // bottom left
        wf * ASPECT + (4 * OFF), -hf,  4.0f,    1.0, 1.0, // bottom right
        wf * ASPECT + (4 * OFF),  hf,  4.0f,    1.0, 0.0 // top right
    };
#endif
    GLuint TI[] = {
        0, 1, 2,
        2, 3, 0,

        4, 5, 6,
        6, 7, 4,

        8, 9 ,10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20,

        24, 25, 26,
        26, 27, 24,

        28, 29, 30,
        30, 31, 28,

        32, 33, 34,
        34, 35, 32,

        36, 37, 38,
        38, 39, 36,

        40, 41, 42,
        42, 43, 40,

        44, 45, 46,
        46, 47, 44,

        48, 49, 50,
        50, 51, 48,

        52, 53, 54,
        54, 55, 52,

        56, 57, 58,
        58, 59, 56,
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

        GLfloat tris_VBO_data[sizeof(T)];
        GLuint  tris_EBO_data[sizeof(TI) / sizeof(TI[0])];

        VertexBufferData_init_static(
            &tri_data, 
            BATCH_COUNT_EXTRA,
            tris_VBO_data,
            BATCH_COUNT_EXTRA,
            tris_EBO_data
        );
        tri_data.i_count = len_i_tris;

        memcpy(tris_VBO_data, T, sizeof(T));
        memcpy(tris_EBO_data, TI, sizeof(TI));


        //std::cout << b[0] << std::endl;

        gl_bind_buffers_and_upload_data(&tri_data, sizeof(T) / sizeof(T[0]), sizeof(TI) / sizeof(TI[0]), GL_STATIC_DRAW);
        // POSITION
        gl_set_and_enable_vertex_attrib_ptr(0, 3, GL_FLOAT, GL_FALSE, 0, &vao_2d2);
        // UV
        gl_set_and_enable_vertex_attrib_ptr(1, 2, GL_FLOAT, GL_FALSE, 3, &vao_2d2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

///////////////////////////////////////////////////////////////////////////////////////////////////////////

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	glEnable(GL_DEPTH_TEST);
    glDepthRange(0, 1);
    glDepthFunc(GL_LEQUAL);

	// glEnable(GL_MULTISAMPLE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);	

    
    printf("USING GL VERSION: %s\n", glGetString(GL_VERSION));

    glm::mat4 mat_ident(1.0f);
    glm::mat4 mat_projection = glm::ortho(-1.0f * ASPECT, 1.0f * ASPECT, -1.0f, 1.0f, -1.0f * 100.0f, 1.0f * 100.0f);
    //glm::mat4 mat_projection = glm::perspective(glm::radians(45.0f), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

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

    Texture tex0;
    if (GL_texture_gen_and_load_1(&tex0, "./textures/bg_test_3_3.png", GL_TRUE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE) == GL_FALSE) {
        return EXIT_FAILURE;
    }

    // Texture tex1;
    // if (GL_texture_gen_and_load_1(&tex1, "./textures/bla.png", GL_TRUE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE) == GL_FALSE) {
    //     return EXIT_FAILURE;
    // }
    // TEXTURE 0
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, texture[0]);
    // glUniform1i(glGetUniformLocation(prog_shader, "tex0"), 0);
    // //
    // glUniform1i(glGetUniformLocation(prog_shader, "tex0"), 0);


    glUseProgram(shader_2d);

    UniformLocation RES_LOC = glGetUniformLocation(shader_2d, "u_resolution");
    UniformLocation COUNT_LAYERS_LOC = glGetUniformLocation(shader_2d, "u_count_layers");
    UniformLocation MAT_LOC = glGetUniformLocation(shader_2d, "u_matrix");
    UniformLocation TIME_LOC = glGetUniformLocation(shader_2d, "u_time");
    UniformLocation CAM_LOC = glGetUniformLocation(shader_2d, "u_position_cam");
    UniformLocation ASPECT_LOC = glGetUniformLocation(shader_2d, "u_aspect");

    const GLint UVAL_COUNT_LAYERS = COUNT_QUADS / 3;

    glUniform2fv(RES_LOC, 1, glm::value_ptr(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT)));
    glUniform1i(COUNT_LAYERS_LOC, UVAL_COUNT_LAYERS);
    glUniform1f(ASPECT_LOC, (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT);
    // TEXTURE 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex0);
    glUniform1i(glGetUniformLocation(shader_2d, "tex0"), 0);
    // // TEXTURE 0
    // glActiveTexture(GL_TEXTURE1);
    // glBindTexture(GL_TEXTURE_2D, tex1);
    // glUniform1i(glGetUniformLocation(shader_2d, "tex1"), 1);


    size_t display_mode_count = 0;
    SDL_DisplayMode mode;

    if (SDL_GetDisplayMode(0, 0, &mode) != 0) {
        SDL_Log("SDL_GetDisplayMode failed: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    printf("REFRESH_RATE: %d\n", mode.refresh_rate);


    SDL_GL_SetSwapInterval(1);
    const double INTERVAL = MS_PER_S / mode.refresh_rate;

    bool keep_running = true;
    SDL_Event event;

    Uint64 t_now      = SDL_GetPerformanceCounter();
    Uint64 t_start    = t_now;
    Uint64 t_prev     = 0;
    double t_delta_ms = 0.0;
    double t_delta_s  = 0.0;
    double t_elapsed_s = 0.0;
    double t_accumulator_ms = 0.0;
    double t_accumulator_ms_overall = 0.0;
    Uint64 t_accumulator_count = 0;


    while (keep_running) {
        t_prev = t_now;
        t_now = SDL_GetPerformanceCounter();

        t_delta_s       = (double)((t_now - t_prev) / (double)SDL_GetPerformanceFrequency());
        t_delta_ms      = t_delta_s * MS_PER_S;
        t_elapsed_s     += t_delta_s;
        t_accumulator_ms += t_delta_ms;
        t_accumulator_ms_overall += t_delta_ms;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                keep_running = false;
            }
        }

        while (t_accumulator_ms > INTERVAL) {
            t_accumulator_ms -= INTERVAL;

            ++t_accumulator_count;

            double CHANGE = INTERVAL / MS_PER_S;
            // TODO update world state

            ////////////////
            //TEST INPUT
            {
                main_cam.orientation = glm::quat();

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
        }



        //main_cam.rotate((GLfloat)curr_time / MS_PER_S, 0.0f, 0.0f, 1.0f);
    //////////////////
    // DRAW

                
        glClearColor(97.0 / 255.0, 201.0 / 255.0, 255.0 / 255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //glUseProgram(shader_2d);

        //double t_overall = t_accumulator_ms_overall / MS_PER_S;

        //const double t_left_over = glm::lerp(, ,  t_accumulator_ms / INTERVAL);


        //std::cout << t << std::endl;

        double T = (((t_accumulator_count * INTERVAL)) + t_accumulator_ms) / MS_PER_S;

        //glUniformMatrix4fv(MAT_LOC, 1, GL_FALSE, glm::value_ptr(ViewCamera_calc_view_matrix(&main_cam) * mat_ident));
        glUniformMatrix4fv(MAT_LOC, 1, GL_FALSE, glm::value_ptr(
            mat_projection * 
            /*FreeCamera_calc_view_matrix(&main_cam) * */ 
            /*glm::translate(mat_ident, glm::vec3(glm::sin((GLfloat)T), 0.0, 0.0)) * */
            /*glm::scale(mat_ident, glm::vec3(0.25, 0.25, 1.0))* */
                        mat_ident)
        );

        glUniform1f(TIME_LOC, (double)t_accumulator_ms_overall / MS_PER_S);

        glm::vec3 pos = main_cam.position;
        #ifdef DEBUG_PRINT

            if (pos.x != prev_pos.x || pos.y != prev_pos.y || pos.z != prev_pos.z) {
                std::cout << "VIEW_POSITION{x : " << pos.x << ", y : " << pos.y << ", z: " << pos.z << "}" << std::endl;
            }
            prev_pos.x = pos.x;
            prev_pos.y = pos.y;
            prev_pos.z = pos.z;
        #endif

        glUniform3fv(CAM_LOC, 1, glm::value_ptr(pos));

        glBindVertexArray(vao_2d2.vao);
        glDrawElements(GL_TRIANGLES, tri_data.i_count, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        //std::cout << (GLdouble) curr_time / MS_PER_S << std::endl;

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
