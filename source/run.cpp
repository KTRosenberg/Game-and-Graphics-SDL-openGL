#include "opengl.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

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

using namespace GL;

void* xmalloc(size_t bytes);
void* xmalloc(size_t bytes)
{
    void* mem = malloc(bytes);
    if (mem == NULL) {
        abort();
    }

    return mem;
}

// TEXTURES, GEOMETRY
typedef struct _TextureData {
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

struct _sceneData {
    glm::mat4 m_model;
    glm::mat4 m_view;
    glm::mat4 m_projection;
} scene;

// ATTRIBUTES AND VERTEX ARRAYS

typedef struct _AttributeData {
    GLuint index;
    GLint size;
    GLenum type;
    GLboolean normalized;
    GLsizei stride;
    GLvoid* pointer;
    GLchar* name;
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

void create_VertexArray(VertexArray* vao) 
{
    glGenVertexArrays(1, vao);
}
void delete_VertexArray(VertexArray* vao) 
{
    glDeleteVertexArrays(1, vao);
}

// VERTEX BUFFERS

typedef struct _VertexBufferData {
    VertexBuffer vbo;
    ElementBuffer ebo;
    size_t    v_count;
    size_t    v_size_element;
    size_t    i_count;
    size_t    i_size_element;
    GLfloat*  vertices;
    GLuint*   indices;
} VertexBufferData;

void create_VertexData(
    VertexBufferData* g,
    const size_t v_count,
    const size_t v_size_element,
    const size_t i_count,
    const size_t i_size_element
) {
    glGenBuffers(2, (GLBuffer*)&g->vbo);

    g->v_count = v_count;
    g->v_size_element  = v_size_element;
    g->i_count = i_count;
    g->i_size_element  = i_size_element;

    g->vertices = (GLfloat*)xmalloc(g->v_size_element * g->v_count);
    g->indices  = (GLuint*)xmalloc(g->v_size_element * g->i_count);
}

void create_static_VertexBufferData(
    VertexBufferData* g,
    const size_t v_count,
    const size_t v_size_element,
    GLfloat* vertices,
    const size_t i_count,
    const size_t i_size_element,
    GLuint* indices
) {
    glGenBuffers(2, (GLBuffer*)&g->vbo);

    g->v_count = v_count;
    g->v_size_element  = v_size_element;
    g->i_count = i_count;
    g->i_size_element  = i_size_element;

    g->vertices = vertices;
    g->indices  = indices;
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


// COLLISION INFO
typedef struct _CollisionStatus {
    bool      collided;
    glm::vec3 point;
} CollisionStatus;

void create_CollisionStatus(CollisionStatus* cs, const bool collided, glm::vec3 point)
{
    cs->collided = collided;
    cs->point = point;
}

typedef CollisionStatus (*Fn_CollisionHandler)(glm::vec3 incoming);

typedef struct _Collider {
    glm::vec3 a;
    glm::vec3 b;
    Fn_CollisionHandler handler;
} Collider; 

// WORLD STATE
typedef struct _Room {
    VertexBufferData  geometry;
    Collider* collision_data;
    glm::mat4 matrix;
} Room;

typedef struct {
    Room* rooms;
    glm::mat4 m_view;
    glm::mat4 m_projection;
} world;

typedef struct _GLData {
    SDL_GLContext context;
    TextureData textures;
} GLData;

GLData gl_data;



// glm::vec3 light_pos(1.2f, 1.0f, 2.0f);
// glm::light_color(1.0f, 1.0f, 1.0f);
// glm::toy_color(1.0f, 0.5f, 0.31f);
// glm::result = light_color * toy_color;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
//////////////


    ///////////////////

int main(int argc, char* argv[])
{
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









    // SHADERS
    Shader shader_2d;
    shader_2d.load_from_file(
        "shaders/default_2d/default_2d.vrts",
        "shaders/default_2d/default_2d.frgs"
    );
    if (!shader_2d.is_valid()) {
        fprintf(stderr, "ERROR: shader_2d\n");
        return EXIT_FAILURE;
    }
    ///////////////


    GLfloat vertices[] = {
         0.5f,  0.5f, 0.0f, 1.0, 0.0, 0.0, 1.0,  // top right
         0.5f, -0.5f, 0.0f, 0.0, 1.0, 0.0, 1.0, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0, 0.0, 1.0, 1.0, // bottom left
        -0.5f,  0.5f, 0.0f, 1.0, 0.0, 0.0, 1.0  // top left 
    };
    GLuint indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };


    GL::VertexArray vao_2d;
    create_VertexArray(&vao_2d);
    glBindVertexArray(vao_2d);

    VertexBufferData geo;
    create_static_VertexBufferData(
        &geo, 
        sizeof(vertices) / sizeof(GLfloat),
        sizeof(GLfloat),
        vertices,
        sizeof(indices) / sizeof(GLuint),
        sizeof(GLuint),
        indices
    );

    const size_t STRIDE = 7;

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(vao_2d);

    glBindBuffer(GL_ARRAY_BUFFER, geo.vbo);
    glBufferData(GL_ARRAY_BUFFER, geo.v_count * geo.v_size_element, geo.vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geo.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, geo.i_count * geo.i_size_element, geo.indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, STRIDE * geo.v_size_element, (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, STRIDE * geo.v_size_element, (void*)(3 * geo.v_size_element));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 


    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_MULTISAMPLE);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	

    glDepthRange(0, 1);
    glDepthFunc(GL_LEQUAL);
    
    SDL_GL_SetSwapInterval(1);
    
    printf("USING GL VERSION: %s\n", glGetString(GL_VERSION));

    bool keep_running = true;
    SDL_Event event;

    const Uint32 INTERVAL = TIME_UNIT_TO_SECONDS / FRAMES_PER_SECOND;
    Uint32 start_time = SDL_GetTicks();
    Uint32 prev_time = start_time;
    Uint32 curr_time = start_time;
    Uint64 delta_time = 0;
    Uint64 dt = 0;

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
                
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader_2d.use();
        glBindVertexArray(vao_2d);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        //////////////////////////////////////////////////////////////////
        // shader_2d.use();
        // //glClear(GL_DEPTH_BUFFER_BIT);
        // glBindVertexArray(vao_2d);

        //     UniformLocation MAT_LOC = glGetUniformLocation(prog_shader, "u_matrix");

        //     glUniformMatrix4fv(MAT_LOC, 1, GL_FALSE, glm::value_ptr(scene.m_projection * ident_mat));
        
        //     glDrawElements(GL_TRIANGLES, len_indices_draw_2d, GL_UNSIGNED_INT, 0);

        // //glBindVertexArray(0);

        // shader_2d.stop_using();
        // 2d drawing
        //
                
        SDL_GL_SwapWindow(window);
    }
    
    SDL_GL_DeleteContext(gl_data.context);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}
