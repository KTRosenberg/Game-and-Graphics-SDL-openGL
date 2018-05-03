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

#define SCREEN_WIDTH  720
#define SCREEN_HEIGHT 480
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

void create_TextureData(TextureData* t, const size_t id_count) 
{
    t->ids = (Texture*)xmalloc(id_count * sizeof(t->ids));
    t->count = id_count;
    glGenTextures(t->count, t->ids);
}

void create_static_TextureData(TextureData* t, const size_t id_count, Texture* buffer)
{
    t->ids = buffer;
    t->count = id_count;
    glGenTextures(id_count, t->ids);
}

void delete_TextureData(TextureData* t)
{
    glDeleteTextures(t->count, t->ids);
    free(t);
}
void delete_static_TextureData(TextureData* t)
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
void CappedArray_create(CappedArray<T>* arr, size_t cap, Fn_MemoryAllocator alloc) {
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

void create_AttributeData(
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


struct LinearAllocator {
    void* memory;
    unsigned char* marker;
    size_t max_size;

    Fn_MemoryAllocator alloc;

    struct LinearAllocator* successor;
};

struct MemoryAllocator {
    void* type;
    Fn_MemoryAllocator fn_alloc;
    
    // void* alloc(size_t bytes)
    // {
    //     return Fn_MemoryAllocator(bytes, type);
    // }
};

// void MemoryAllocator_create(MemoryAllocator* ma, void* type, Fn_MemoryAllocator* fn_alloc, Fn_MemoryAllocatorType_create fn_create, void* args)
// {
//     ma->fn_alloc = fn_alloc;
//     ma->type = alloc;
//     fn_create(fn_alloc, args);
// }

typedef uint8_t MemoryIndex;

#define ARENA_DEFAULT_BLOCK_SIZE (1024 * 1024)
typedef struct MemoryArena {
    size_t   size;
    uint8_t  used_idx;
    uint8_t* base;
    int32_t  _temp_count;

    void** blocks;
} MemoryArena;

void* MemoryArena_push_data(void)
{
    return (void*)0;
}


void LinearAllocator_create(LinearAllocator* allocator, size_t max_size)
{
    allocator->max_size  = max_size;
    //allocator->alloc     = alloc;
    allocator->memory    = NULL;
    allocator->marker    = NULL;
    allocator->successor = NULL;
}

// void* LinearAllocator_allocate(size_t bytes)
// {
//     void* allocator->alloc(bytes)
// }

template <typename T>
struct Buffer {
    T* memory;
    size_t cap;

    const operator T*()
    {
        return this->memory;
    }

    size_t size() const
    {
        return sizeof(T) * cap;
    }

    size_t element_size() const
    {
        return sizeof(T);
    }
};

template <typename T>
void Buffer_create(Buffer<T>* b, size_t cap, Fn_MemoryAllocator alloc) {
    b->memory = alloc(cap * sizeof(T));
    b->cap = cap;
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



void create_VertexBufferData(
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

void create_static_VertexBufferData(
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

void delete_VertexBufferData(VertexBufferData* g)
{
    glDeleteBuffers(2, (GLBuffer*)&g->vbo);
    free(g->vertices);
    free(g->indices);
}
void delete_static_VertexBufferData(VertexBufferData* g)
{
    glDeleteBuffers(2, (GLBuffer*)&g->vbo);
}


typedef struct VertexAttributeArray {
    VertexArray vao;
    size_t stride;
} VertexAttributeArray;

typedef VertexAttributeArray VAttribArr;

inline void create_VertexAttributeArray(VertexAttributeArray* vao, size_t stride) 
{
    glGenVertexArrays(1, &vao->vao);
    vao->stride = stride;
}
inline void delete_VertexAttributeArray(VertexAttributeArray* vao) 
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

void create_CollisionStatus(CollisionStatus* cs, const bool collided, glm::vec3 point)
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
    std::cout << std::boolalpha << std::is_pod<CappedArray<GLfloat>>::value  << std::endl;

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
#define TRANSITION
#ifdef TRANSITION
    Shader shader_2d;
    shader_2d.load_from_file(
        "shaders/transition_a/transition_a.vrts",
        "shaders/transition_a/transition_a.frgs",
        glsl_perlin_noise,
        glsl_perlin_noise
    );
    if (!shader_2d.is_valid()) {
        fprintf(stderr, "ERROR: shader_2d\n");
        return EXIT_FAILURE;
    }
#else
    puts("WEE");
    Shader shader_2d;
    shader_2d.load_from_file(
        "shaders/default_2d/default_2d.vrts",
        "shaders/default_2d/default_2d.frgs",
        glsl_perlin_noise,
        glsl_perlin_noise
    );
    if (!shader_2d.is_valid()) {
        fprintf(stderr, "ERROR: shader_2d\n");
        return EXIT_FAILURE;
    }
#endif
///////////////

    const GLfloat ASPECT = (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT;

    const size_t STRIDE = 7;

// LINES
    const size_t COUNT_LINES = 1;
    const size_t POINTS_PER_LINE = 2;
    const size_t len_v_lines = STRIDE * COUNT_LINES * POINTS_PER_LINE;
    const size_t len_i_lines = 2;

    GLfloat wf = 1.0f;//(GLfloat)SCREEN_WIDTH;
    GLfloat hf = 1.0f;//(GLfloat)SCREEN_HEIGHT;

    GLfloat L[len_v_lines] = {
         wf * ASPECT,  hf,   0.0f, 1.0, 0.0, 0.0, 1.0,  // top right
        -wf * ASPECT, -hf,   0.0f, 0.0, 0.0, 1.0, 1.0, // bottom left
    };

    GLuint LI[len_i_lines] = {
        0, 1
    };

// QUADS
    const size_t COUNT_QUADS = 2;
    const size_t POINTS_PER_QUAD = 4;
    const size_t POINTS_PER_TRI = 3;
    const size_t TRIS_PER_QUAD = 2;
    const size_t len_v_tris = STRIDE * COUNT_QUADS * POINTS_PER_QUAD;
    const size_t len_i_tris = COUNT_QUADS * TRIS_PER_QUAD * POINTS_PER_TRI;

    // X formation quads
#ifdef TRANSITION
        //single quad
    GLfloat T[] = {
       -wf * ASPECT,  hf,  0.0f, 0.0, 0.0, 1.0, 1.0,  // top left
       -wf * ASPECT, -hf,  0.0f, 0.0, 0.0, 1.0, 1.0,  // bottom left
        wf * ASPECT, -hf,  0.0f, 0.0, 0.0, 1.0, 1.0,  // bottom right
        wf * ASPECT,  hf,  0.0f, 0.0, 0.0, 1.0, 1.0,  // top right
    };
    GLuint TI[] = {  // note that we start from 0!
        0, 1, 2,  // first Triangle
        2, 3, 0,   // second Triangle
    };
#else
    GLfloat T[] = {
       -wf * ASPECT,  hf,  1.0f, 0.0, 0.0, 1.0, 1.0,  // top left
       -wf * ASPECT, -hf,  1.0f, 0.0, 0.0, 1.0, 1.0,  // bottom left
        wf * ASPECT, -hf, -1.0f, 0.0, 0.0, 1.0, 1.0,  // bottom right
        wf * ASPECT,  hf, -1.0f, 0.0, 0.0, 1.0, 1.0,  // top right

        -wf * ASPECT,  hf, -1.0f, 1.0, 0.0, 0.0, 1.0,  // top left
        -wf * ASPECT, -hf, -1.0f, 1.0, 0.0, 0.0, 1.0, // bottom left
         wf * ASPECT, -hf,  1.0f, 1.0, 0.0, 0.0, 1.0, // bottom right
         wf * ASPECT,  hf,  1.0f, 1.0, 0.0, 0.0, 1.0, // top right
    };
    GLuint TI[] = {  // note that we start from 0!
        0, 1, 2,  // first Triangle
        2, 3, 0,   // second Triangle

        4, 5, 6,
        6, 7, 4,
    };
#endif

// TOTAL ALLOCATION
    const size_t BATCH_COUNT = 1024;
    const size_t GUESS_VERTS_PER_DRAW = 4;
    const size_t BATCH_COUNT_EXTRA = BATCH_COUNT * GUESS_VERTS_PER_DRAW * STRIDE;

// R1 ////////////////////////////////////////////////

    VertexAttributeArray vao_2d;
    VertexBufferData lines_data;

    create_VertexAttributeArray(&vao_2d, 7);

    glBindVertexArray(vao_2d.vao);


        GLfloat lines_VBO_data[BATCH_COUNT_EXTRA * sizeof(GLfloat)];
        GLuint lines_EBO_data[BATCH_COUNT_EXTRA * sizeof(GLuint)];

        create_static_VertexBufferData(
            &lines_data, 
            BATCH_COUNT_EXTRA,
            lines_VBO_data,
            BATCH_COUNT_EXTRA,
            lines_EBO_data
        );
        lines_data.v_count = len_v_lines;
        lines_data.i_count = len_i_lines;


        for (size_t i = 0; i < len_v_lines; ++i) {
            lines_VBO_data[i] = L[i];
        }
        for (size_t i = 0; i < len_i_lines; ++i) {
            lines_EBO_data[i] = LI[i];
        }

        
        gl_bind_buffers_and_upload_data(&lines_data, BATCH_COUNT_EXTRA, BATCH_COUNT_EXTRA, GL_STATIC_DRAW);
        // POSITION
        gl_set_and_enable_vertex_attrib_ptr(0, 3, GL_FLOAT, GL_FALSE, 0, &vao_2d);
        // COLOR
        gl_set_and_enable_vertex_attrib_ptr(1, 4, GL_FLOAT, GL_FALSE, 3, &vao_2d);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

// R2 ////////////////////////////////////////////////

    VertexAttributeArray vao_2d2;
    VertexBufferData tri_data;

    create_VertexAttributeArray(&vao_2d2, 7);

    glBindVertexArray(vao_2d2.vao);

        GLfloat tris_VBO_data[BATCH_COUNT_EXTRA * sizeof(GLfloat)];
        GLuint  tris_EBO_data[BATCH_COUNT_EXTRA * sizeof(GLuint)];

        create_static_VertexBufferData(
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


        Buffer<int> b;
        b.memory = (int*)xcalloc_1arg(sizeof(int) * 10);
        free(b.memory);
        b.memory = NULL;


        //std::cout << b[0] << std::endl;

        gl_bind_buffers_and_upload_data(&tri_data, BATCH_COUNT_EXTRA, BATCH_COUNT_EXTRA, GL_STATIC_DRAW);
        // POSITION
        gl_set_and_enable_vertex_attrib_ptr(0, 3, GL_FLOAT, GL_FALSE, 0, &vao_2d2);
        // COLOR
        gl_set_and_enable_vertex_attrib_ptr(1, 4, GL_FLOAT, GL_FALSE, 3, &vao_2d2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

///////////////////////////////////////////////////////////////////////////////////////////////////////////

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_MULTISAMPLE);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	

    glDepthRange(0, 1);
    glDepthFunc(GL_LEQUAL);
    
    SDL_GL_SetSwapInterval(1);
    
    printf("USING GL VERSION: %s\n", glGetString(GL_VERSION));

    glm::mat4 mat_ident(1.0f);
    glm::mat4 mat_projection = glm::ortho(-1.0f * ASPECT, 1.0f * ASPECT, -1.0f, 1.0f, -1.0f, 1.0f);
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
    // ViewCamera_create(
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
                // FreeCamera_create(
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

        #ifdef DEBUG_PRINT
            glm::vec3* pos = &main_cam.position;
            std::cout << "VIEW_POSITION{x : " << pos->x << ", y : " << pos->y << ", z: " << pos->z << "}" << std::endl;
        #endif
        }

        //main_cam.rotate((GLfloat)curr_time / TIME_UNIT_TO_SECONDS, 0.0f, 0.0f, 1.0f);
    //////////////////
    // DRAW

                
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_2d);

        UniformLocation MAT_LOC = glGetUniformLocation(shader_2d, "u_matrix");
        UniformLocation TIME_LOC = glGetUniformLocation(shader_2d, "u_time");
        UniformLocation RES_LOC = glGetUniformLocation(shader_2d, "u_resolution");

        //glUniformMatrix4fv(MAT_LOC, 1, GL_FALSE, glm::value_ptr(ViewCamera_calc_view_matrix(&main_cam) * mat_ident));
        glUniformMatrix4fv(MAT_LOC, 1, GL_FALSE, glm::value_ptr(mat_projection * FreeCamera_calc_view_matrix(&main_cam) * mat_ident  * glm::scale(mat_ident, glm::vec3(0.5, 0.5, 0.5)) ));
        glUniform1f(TIME_LOC, ((GLdouble)curr_time / TIME_UNIT_TO_SECONDS));
        glUniform2fv(RES_LOC, 1, glm::value_ptr(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT)));

        glBindVertexArray(vao_2d.vao);
        glDrawElements(GL_LINES, lines_data.i_count, GL_UNSIGNED_INT, (void*)0);

        glBindVertexArray(vao_2d2.vao);
        glDrawElements(GL_TRIANGLES, tri_data.i_count, GL_UNSIGNED_INT, (void*)0);
                
        SDL_GL_SwapWindow(window);

    //////////////////
    }
    
    delete_static_VertexBufferData(&lines_data);
    delete_VertexAttributeArray(&vao_2d);
    SDL_GL_DeleteContext(gl_data.context);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}
