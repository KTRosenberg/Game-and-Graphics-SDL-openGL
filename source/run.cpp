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

// VERTEX BUFFERS

typedef struct _VertexBufferData {
    VertexBuffer vbo;
    ElementBuffer ebo;
    size_t    v_cap;
    size_t    v_count;
    size_t    v_size_element;
    size_t    i_cap;
    size_t    i_count;
    size_t    i_size_element;
    GLfloat*  vertices;
    GLuint*   indices;
} VertexBufferData;

struct VertexBufferDataAlt {
    VertexBuffer vbo;
    ElementBuffer ebo;
    std::vector<GLfloat>  vertices;
    std::vector<GLuint>   indices;
} VertexBufferDataAlt;

typedef VertexBufferData VBData;

void create_VertexBufferData(
    VertexBufferData* g,
    const size_t v_cap,
    const size_t v_size_element,
    const size_t i_cap,
    const size_t i_size_element
) {
    glGenBuffers(2, (GLBuffer*)&g->vbo);

    g->v_cap = v_cap;
    g->v_size_element  = v_size_element;
    g->i_cap = i_cap;
    g->i_size_element  = i_size_element;

    g->vertices = (GLfloat*)xmalloc(g->v_size_element * g->v_cap);
    g->indices  = (GLuint*)xmalloc(g->i_size_element * g->i_cap);

    g->v_count = 0;
    g->i_count = 0;
}

void create_static_VertexBufferData(
    VertexBufferData* g,
    const size_t v_cap,
    const size_t v_size_element,
    GLfloat* vertices,
    const size_t i_cap,
    const size_t i_size_element,
    GLuint* indices
) {
    glGenBuffers(2, (GLBuffer*)&g->vbo);

    g->v_cap = v_cap;
    g->v_size_element  = v_size_element;
    g->i_cap = i_cap;
    g->i_size_element  = i_size_element;

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


typedef struct _vertexAttributeArray {
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
inline void* attribute_offsetof(VertexBufferData* vb, size_t offset)
{
    return (void*)(offset * vb->v_size_element);
}
inline size_t attribute_sizeof(VertexAttributeArray* vao, VertexBufferData* vb) 
{
    return vao->stride * vb->v_size_element;
}

inline void gl_set_and_enable_vertex_attrib_ptr(GLuint index, GLint size, GLenum type, GLboolean normalized, size_t offset, VertexAttributeArray* va, VertexBufferData* vb)
{
    glVertexAttribPointer(index, size, type, normalized, attribute_sizeof(va, vb), attribute_offsetof(vb, offset));            
    glEnableVertexAttribArray(index);
}

inline void gl_bind_buffers_and_upload_data(VertexBufferData* vbd, size_t v_cap, size_t i_cap, GLenum usage)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbd->vbo);
    glBufferData(GL_ARRAY_BUFFER, v_cap * vbd->v_size_element, vbd->vertices, usage);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbd->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, i_cap * vbd->i_size_element, vbd->indices, usage);            
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

    const size_t STRIDE = 7;

// LINES
    const size_t COUNT_LINES = 1;
    const size_t POINTS_PER_LINE = 2;
    const size_t len_v_lines = STRIDE * COUNT_LINES * POINTS_PER_LINE;
    const size_t len_i_lines = 2;

    GLfloat wf = (GLfloat)SCREEN_WIDTH;
    GLfloat hf = (GLfloat)SCREEN_HEIGHT;

    GLfloat L[len_v_lines] = {
        wf,   0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0,  // top right
        0.0f, hf,   0.0f, 0.0, 0.0, 1.0, 1.0, // bottom left
    };

    GLuint LI[len_i_lines] = {
        0, 1
    };

// QUADS
    const size_t COUNT_QUADS = 1;
    const size_t POINTS_PER_QUAD = 4;
    const size_t POINTS_PER_TRI = 3;
    const size_t TRIS_PER_QUAD = 2;
    const size_t len_v_tris = STRIDE * COUNT_QUADS * POINTS_PER_QUAD;
    const size_t len_i_tris = TRIS_PER_QUAD * POINTS_PER_TRI;

    GLfloat T[] = {
         wf,    0.0,  0.0f, 0.0, 0.0, 1.0, 0.5,  // top right
         wf,    hf,   0.0f, 0.0, 0.0, 1.0, 0.5, // bottom right
         0.0f,  hf,   0.0f, 0.0, 0.0, 1.0, 0.5, // bottom left
         0.0f,  0.0f, 0.0f, 1.0, 0.0, 0.0, 0.5  // top left 
    };
    GLuint TI[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

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
            sizeof(GLfloat),
            lines_VBO_data,
            BATCH_COUNT_EXTRA,
            sizeof(GLuint), 
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
        gl_set_and_enable_vertex_attrib_ptr(0, 3, GL_FLOAT, GL_FALSE, 0, &vao_2d, &lines_data);
        // COLOR
        gl_set_and_enable_vertex_attrib_ptr(1, 4, GL_FLOAT, GL_FALSE, 3, &vao_2d, &lines_data);

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
            sizeof(GLfloat),
            tris_VBO_data,
            BATCH_COUNT_EXTRA,
            sizeof(GLuint), 
            tris_EBO_data
        );
        tri_data.i_count = len_i_tris;

        for (size_t i = 0; i < len_v_tris; ++i) {
            tris_VBO_data[i] = T[i];
        }
        for (size_t i = 0; i < len_i_tris; ++i) {
            tris_EBO_data[i] = TI[i];
        }

        
        gl_bind_buffers_and_upload_data(&tri_data, BATCH_COUNT_EXTRA, BATCH_COUNT_EXTRA, GL_STATIC_DRAW);
        // POSITION
        gl_set_and_enable_vertex_attrib_ptr(0, 3, GL_FLOAT, GL_FALSE, 0, &vao_2d2, &tri_data);
        // COLOR
        gl_set_and_enable_vertex_attrib_ptr(1, 4, GL_FLOAT, GL_FALSE, 3, &vao_2d2, &tri_data);

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


    glm::mat4 mat_ident(1.0);
    glm::mat4 mat_view(1.0);
    glm::mat4 mat_projection = glm::ortho(0.0f, (GLfloat)SCREEN_WIDTH, (GLfloat)SCREEN_HEIGHT, 0.0f, -1000.0f, 1000.0f);
    glm::mat4 mat_projection_perspective = glm::perspective(glm::radians(45.0f), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

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
    Camera main_cam(glm::vec3(0.0, 0.0, 0.0f));
    
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
    const Uint8* reset = &key_states[SDL_SCANCODE_0];
    const Uint8* toggle_projection = &key_states[SDL_SCANCODE_P];

    bool projection_is_ortho = true;

    const double POS_ACC = 1.075;
    const double NEG_ACC = 1.0 / POS_ACC;

    double up_acc = 1.0;
    double down_acc = 1.0;
    double left_acc = 1.0;
    double right_acc = 1.0;


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

    //////////////////
    // TEST INPUT
        {

            const double CHANGE = (delta_time / (GLfloat)TIME_UNIT_TO_SECONDS);

            if (*up) {
                main_cam.process_directional_movement(Camera_Movement::DOWNWARDS, CHANGE * up_acc);
                up_acc *= POS_ACC;
            } else {
                if (up_acc > 1.0) {
                    main_cam.process_directional_movement(Camera_Movement::DOWNWARDS, CHANGE * up_acc);
                }
                up_acc = glm::max(1.0, up_acc * NEG_ACC);
            }
            if (*down) {
                main_cam.process_directional_movement(Camera_Movement::UPWARDS, CHANGE * down_acc);
                down_acc *= POS_ACC;
            } else {
                if (down_acc > 1.0) {
                    main_cam.process_directional_movement(Camera_Movement::UPWARDS, CHANGE * down_acc);
                } 
                down_acc = glm::max(1.0, down_acc * NEG_ACC);  
            }

            if (*left) {
                main_cam.process_directional_movement(Camera_Movement::LEFTWARDS, CHANGE * left_acc);
                left_acc *= POS_ACC;
            } else {
                if (left_acc > 1.0) {
                    main_cam.process_directional_movement(Camera_Movement::LEFTWARDS, CHANGE * left_acc);
                }
                left_acc = glm::max(1.0, left_acc * NEG_ACC);
            }

            if (*right) {
                main_cam.process_directional_movement(Camera_Movement::RIGHTWARDS, CHANGE * right_acc);
                right_acc *= POS_ACC;
            } else {
                if (right_acc > 1.0) {
                    main_cam.process_directional_movement(Camera_Movement::RIGHTWARDS, CHANGE * right_acc);
                }
                right_acc = glm::max(1.0, right_acc * NEG_ACC);
            }

            if (*reset) {
                main_cam.pos = glm::vec3(0.0, 0.0, 10.0);
                up_acc = 1.0;
                down_acc = 1.0;
                left_acc = 1.0;
                right_acc = 1.0;
            }

            if (*toggle_projection) {
                projection_is_ortho = !projection_is_ortho;
            }

        }
        //std::cout << "x:" << main_cam.pos.x << ":y" << main_cam.pos.y << ":z" << main_cam.pos.z << std::endl;
    //////////////////
    // DRAW

                
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_2d);

        UniformLocation MAT_LOC = glGetUniformLocation(shader_2d, "u_matrix");
        glUniformMatrix4fv(MAT_LOC, 1, GL_FALSE, glm::value_ptr(
            ((projection_is_ortho) ? mat_projection : mat_projection_perspective) * main_cam.get_view_matrix() * mat_ident));

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
