// OPENGL-RELATED
#ifdef _WIN32
#   include <GL\glew.h>
#   include <SDL_opengl.h>
#   include <gl\glu.h>
#else // __APPLE_
#   define GL3_PROTOTYPES 1
#   define GLEW_STATIC
#   include <GL/glew.h>
#endif

// CONVENIENCE TYPEDEFS FOR GL DATA
typedef GLuint UniformLocation;
typedef GLuint Texture;
typedef GLuint VertexArray;
typedef VertexArray Vao;
typedef GLuint VertexBuffer;
typedef VertexBuffer Vbo; 
typedef GLuint ElementBuffer;
typedef ElementBuffer Ebo;
typedef GLuint GLBuffer;

// MATH LIBRARY
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/quaternion.hpp>

// SDL INCLUDES
#ifdef _WIN32
#   define SDL_MAIN_HANDLED
#   include <SDL.h>
#   include <SDL_image.h>
#else // __APPLE__
#   include <SDL2/SDL.h>
#   include <SDL2/SDL_image.h>
#endif


#include <iostream>
#include <string>

// TEXTURE LOADING ///////////////////////////////////////////////////////////////////////////////////////////////

// must generate first, then pass id
GLboolean GL_texture_load(Texture* texture_id, const char* const path, const GLboolean alpha, const GLint param_edge_x, const GLint param_edge_y);
// generates one texture automatically
GLboolean GL_texture_gen_and_load_1(Texture* texture_id, const char* const path, const GLboolean alpha, const GLint param_edge_x, const GLint param_edge_y);

GLboolean GL_texture_load(Texture* texture_id, const char* const path, const GLboolean alpha, const GLint param_edge_x, const GLint param_edge_y)
{
    // load image
    SDL_Surface* img = nullptr; 
    if (!(img = IMG_Load(path))) {
        fprintf(stderr, "SDL_image could not be loaded %s, SDL_image Error: %s\n", 
               path, IMG_GetError());
        return GL_FALSE;
    }
    

    glBindTexture(GL_TEXTURE_2D, *texture_id);
    // image assignment
    GLuint format = (alpha) ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, format, img->w, img->h, 0, format, GL_UNSIGNED_BYTE, (GLvoid*)img->pixels);

    // wrapping behavior
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param_edge_x);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param_edge_y);
    // texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    // free the surface
    SDL_FreeSurface(img);
    
    return GL_TRUE;
}

GLboolean GL_texture_gen_and_load_1(Texture* texture_id, const char* const path, const GLboolean alpha, const GLint param_edge_x, const GLint param_edge_y)
{
    glGenTextures(1, texture_id);
    return GL_texture_load(texture_id, path, alpha, param_edge_x, param_edge_y);   
}

// FOR SHADER LOADING
#include "shader.hpp"
#include "camera.hpp"

#define WINDOW_HEADER "TEST"

#define SCREEN_WIDTH 1280.0
#define SCREEN_HEIGHT 720.0
#define MS_PER_S 1000.0
#define FRAMES_PER_SECOND 60.0

// EVENT HANDLER, IGNORES ALL MOUSE INPUT
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

// WINDOW
SDL_Window* window = NULL;

//////////////

int main(int argc, char* argv[])
{
    // INITIALIZE SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "%s\n", "SDL could not initialize");
        return EXIT_FAILURE;
    }
    
    // HIDE THE MOUSE CURSOR
    SDL_ShowCursor(SDL_DISABLE);
    // IGNORE MOUSE MOVEMENT EVENTS
    SDL_SetEventFilter(ignore_mouse_movement, nullptr);
    
    // SET SDL GL HINTS
    if (argc >= 3) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, atoi(argv[1]));
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, atoi(argv[2]));
    } else {
        // GL 3.3 BY DEFAULT
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);        
    }
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    // CREATE THE WINDOW
    if (nullptr == (window = SDL_CreateWindow(
        WINDOW_HEADER,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI)))
    {
        fprintf(stderr, "Window could not be created\n");
        return EXIT_FAILURE;
    }

    // INITIALIZE SDL2_Image    
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if(!(IMG_Init(imgFlags) & imgFlags)) {
     	fprintf(stderr, "SDL_image could not initialize, SDL_image Error: %s\n", IMG_GetError());
        SDL_DestroyWindow(window);
        return EXIT_FAILURE;
    }
    
    // INITIALIZE OPENGL CONTEXT
	SDL_GLContext program_data = SDL_GL_CreateContext(window);

    // INITIALIZE GL EXTENSION WRANGLING
	glewExperimental = GL_TRUE;
    glewInit();

    // INITIALIZE SHADERS
    Shader shader_bg;
    shader_bg.load_from_file(
        "shaders/parallax/parallax_v2.vrts",
        "shaders/parallax/parallax_v2.frgs"
    );
    if (!shader_bg.is_valid()) {
        fprintf(stderr, "ERROR: shader_bg\n");
        return EXIT_FAILURE;
    }

    Shader shader_test_shape;
    shader_test_shape.load_from_file(
        "shaders/default_2d/default_2d.vrts",
        "shaders/default_2d/default_2d.frgs"
    );
    if (!shader_test_shape.is_valid()) {
        fprintf(stderr, "ERROR: shader_test_shape\n");
        return EXIT_FAILURE;
    }

    const GLfloat ASPECT = (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT;

// QUADS
    size_t COUNT_QUADS = 15;

    GLfloat wf = 1.0f;
    GLfloat hf = 1.0f;
    // IN CASE I WANT TO TEST NON-OVERLAPPING, I REPLACE 0.0f with 1.0f
    const GLfloat OFF = 0.0f * wf * ASPECT;

    GLdouble tex_res = 4096.0;
    GLfloat x_off = (GLfloat)(GLdouble)(SCREEN_WIDTH / tex_res);
    GLfloat y_off = (GLfloat)(GLdouble)(SCREEN_HEIGHT / tex_res);
    GLfloat vx_off = 2 * wf * ASPECT;

    // BG DATA
    // FIVE LAYERS OF THREE QUADS MOVING (CAN MOVE ACCORDING TO CAMERA MOVEMENTS AS PARALLAX BGs SHOULD)
    // L0 is farthest away from camera

    // BG DATA STRIDE
    size_t STRIDE = 5; // 3 FOR POSITION, 2 FOR UV
    // VERTICES
    GLfloat V[] = {
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
    // INDICES
    GLuint VI[] = {
        0, 1, 2,
        2, 3, 0,

        4, 5, 6,
        6, 7, 4,

        8,  9,  10,
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
    GLuint indices_count = sizeof(VI) / sizeof(GLuint);

//////////////////////////////////////////////////

    // INIT GL DATA AND STATE
    VertexArray   bg_vao;
    VertexBuffer  bg_vbo;
    ElementBuffer bg_ebo;

    glGenVertexArrays(1, &bg_vao);
    glGenBuffers(1, (GLBuffer*)&bg_vbo);
    glGenBuffers(1, (GLBuffer*)&bg_ebo);

    glBindVertexArray(bg_vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, bg_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(V), V, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bg_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(VI), VI, GL_STATIC_DRAW);

    // POSITION
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, STRIDE * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    // UV
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, STRIDE * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);


// MOVING SHAPE DATA
    size_t STRIDE_s = 7;

    GLfloat V2[] = {
        -0.25f,  0.25f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -0.25f, -0.25f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f,
         0.25f, -0.25f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f,
         0.25f,  0.25f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f
    };

    GLuint VI2[] = {
        0, 1, 2,
        2, 3, 0
    };
    GLuint indices_count_s = sizeof(VI2) / sizeof(GLuint);


    VertexArray   s_vao;
    VertexBuffer  s_vbo;
    ElementBuffer s_ebo;

    glGenVertexArrays(1, &s_vao);
    glGenBuffers(1, (GLBuffer*)&s_vbo);
    glGenBuffers(1, (GLBuffer*)&s_ebo);

    glBindVertexArray(s_vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(V2), V2, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(VI2), VI2, GL_STATIC_DRAW);

    // POSITION
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, STRIDE_s * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    // COLOR
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, STRIDE_s * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
///////////////////////////////////////////////////////////////////////////////////////////////////////////

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	

    printf("USING GL VERSION: %s\n", glGetString(GL_VERSION));

    glm::mat4 mat_ident(1.0f);
    // PROJECTION, IS COORDINATE SYSTEM TOO IMPRECISE? (-1 * ASPECT, 1 * ASPEC in x to avoid stretching?)
    glm::mat4 mat_projection = glm::ortho(-wf * ASPECT, wf * ASPECT, -wf, wf, -wf * 100.0f, wf * 100.0f);

//////////////////
// TEST INPUT


    //#define CONTROLS
    // CONTROLS INACTIVE
    #ifdef CONTROLS
//     glm::vec3 start_pos(0.0f, 0.0f, 1.0f);
    
//     FreeCamera main_cam(start_pos);
//     main_cam.orientation = glm::quat();
//     main_cam.speed = 0.01;
    
//     const Uint8* key_states = SDL_GetKeyboardState(NULL);

//     const Uint8* up         = &key_states[SDL_SCANCODE_W];
//     const Uint8* down       = &key_states[SDL_SCANCODE_S];
//     const Uint8* left       = &key_states[SDL_SCANCODE_A];
//     const Uint8* right      = &key_states[SDL_SCANCODE_D];
//     const Uint8* rot_r      = &key_states[SDL_SCANCODE_RIGHT];
//     const Uint8* rot_l      = &key_states[SDL_SCANCODE_LEFT];
// //  const Uint8& up_right   = key_states[SDL_SCANCODE_E];
// //  const Uint8& up_left    = key_states[SDL_SCANCODE_Q];
// //  const Uint8& down_right = key_states[SDL_SCANCODE_X];
// //  const Uint8& down_left  = key_states[SDL_SCANCODE_Z];
//     const Uint8* forwards = &key_states[SDL_SCANCODE_UP];
//     const Uint8* backwards = &key_states[SDL_SCANCODE_DOWN];

//     const Uint8* reset = &key_states[SDL_SCANCODE_0];
//     const Uint8* toggle_projection = &key_states[SDL_SCANCODE_P];

//     const double POS_ACC = 1.06;
//     const double NEG_ACC = 1.0 / POS_ACC;

//     // double up_acc = 110.0;
//     // double down_acc = 110.0;
//     // double left_acc = 110.0;
//     // double right_acc = 110.0;
//     // double forwards_acc = 110.0;
//     // double backwards_acc = 110.0;

//     double up_acc = 1.0;
//     double down_acc = 1.0;
//     double left_acc = 1.0;
//     double right_acc = 1.0;
//     double forwards_acc = 1.0;
//     double backwards_acc = 1.0;

//     double max_acc = 2000.0;

    #endif


    glUseProgram(shader_bg);

    // SAVE UNIFORM LOCATIONS
    UniformLocation RES_LOC = glGetUniformLocation(shader_bg, "u_resolution");
    UniformLocation COUNT_LAYERS_LOC = glGetUniformLocation(shader_bg, "u_count_layers");
    UniformLocation MAT_LOC = glGetUniformLocation(shader_bg, "u_matrix");
    UniformLocation TIME_LOC = glGetUniformLocation(shader_bg, "u_time");
    UniformLocation CAM_LOC = glGetUniformLocation(shader_bg, "u_position_cam");
    UniformLocation ASPECT_LOC = glGetUniformLocation(shader_bg, "u_aspect");

    glUseProgram(shader_test_shape);

    UniformLocation MAT_LOC_s = glGetUniformLocation(shader_test_shape, "u_matrix");


    const GLint UVAL_COUNT_LAYERS = COUNT_QUADS / 3;

    glUniform2fv(RES_LOC, 1, glm::value_ptr(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT)));
    glUniform1i(COUNT_LAYERS_LOC, UVAL_COUNT_LAYERS);
    glUniform1f(ASPECT_LOC, (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT);
    
    // BG TEXTURE
    Texture tex0;
    if (GL_FALSE == GL_texture_gen_and_load_1(&tex0, "./textures/bg_test_3_3.png", GL_TRUE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE)) {
        return EXIT_FAILURE;
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex0);
    glUniform1i(glGetUniformLocation(shader_bg, "tex0"), 0);



    // SET V-SYNC ON
    SDL_GL_SetSwapInterval(1);

    bool keep_running = true;
    SDL_Event event;

    Uint64 t_now      = SDL_GetPerformanceCounter();
    Uint64 t_prev     = 0;
    Uint64 t_start    = t_now;
    double t_delta    = 0.0;
    double t_delta_s  = 0.0;
    double t_delta_ms = 0.0;
    double frequency = 0.0;

    while (keep_running) {
        t_prev = t_now;

        t_now = SDL_GetPerformanceCounter();
        frequency = SDL_GetPerformanceFrequency();
        Uint64 diff = (t_now - t_prev);
        
        t_delta    = (double)diff / (double)frequency;
        t_delta_s  = (double)diff * 100000000 / (double)frequency;
        t_delta_ms = (double)diff * 10000 / (double)frequency;
        double t_since_start = (double)(t_now - t_start) / (double)frequency;

        // std::cout << t_since_start << std::endl;

        // std::cout << "T_NOW: " << t_now << std::endl <<
        //              " T_DELTA: " << t_delta << std::endl <<
        //              " T_ELAPSED: " << (t_now - t_start) << std::endl <<
        //              " T_ELAPSED: " << ((double)(t_now - t_start) / 1000000000) << std::endl;

        // INPUT /////////////////////////////////
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                keep_running = false;
            }
        }

        // CONTROLS INACTIVE, SAME RESULT
        // #ifdef CONTROLS
        // {

        //     double CHANGE = t_delta;
        //     main_cam.orientation = glm::quat();

        //     if (*up) {
        //         FreeCamera_process_directional_movement(&main_cam, Movement_Direction::UPWARDS, CHANGE * up_acc);
        //         up_acc *= POS_ACC;
        //         up_acc = glm::min(max_acc, up_acc);
        //     } else {
        //         if (up_acc > 1.0) {
        //             FreeCamera_process_directional_movement(&main_cam, Movement_Direction::UPWARDS, CHANGE * up_acc);
        //         }
        //         up_acc = glm::max(1.0, up_acc * NEG_ACC);
        //     }
        //     if (*down) {
        //         FreeCamera_process_directional_movement(&main_cam, Movement_Direction::DOWNWARDS, CHANGE * down_acc);
        //         down_acc *= POS_ACC;
        //         down_acc = glm::min(max_acc, down_acc);
        //     } else {
        //         if (down_acc > 1.0) {
        //             FreeCamera_process_directional_movement(&main_cam, Movement_Direction::DOWNWARDS, CHANGE * down_acc);
        //         } 
        //         down_acc = glm::max(1.0, down_acc * NEG_ACC);
        //     }

        //     if (*left) {
        //         FreeCamera_process_directional_movement(&main_cam, Movement_Direction::LEFTWARDS, CHANGE * left_acc);
        //         left_acc *= POS_ACC;
        //         left_acc = glm::min(max_acc, left_acc);
        //     } else {
        //         if (left_acc > 1.0) {
        //             FreeCamera_process_directional_movement(&main_cam, Movement_Direction::LEFTWARDS, CHANGE * left_acc);
        //         }
        //         left_acc = glm::max(1.0, left_acc * NEG_ACC);
        //         left_acc = glm::min(max_acc, left_acc);
        //     }

        //     if (*right) {
        //         FreeCamera_process_directional_movement(&main_cam, Movement_Direction::RIGHTWARDS, CHANGE * right_acc);
        //         right_acc *= POS_ACC;
        //         right_acc = glm::min(max_acc, right_acc);
        //     } else {
        //         if (right_acc > 1.0) {
        //             FreeCamera_process_directional_movement(&main_cam, Movement_Direction::RIGHTWARDS, CHANGE * right_acc);
        //         }
        //         right_acc = glm::max(1.0, right_acc * NEG_ACC);
        //     }

        //     if (*forwards) {
        //         FreeCamera_process_directional_movement(&main_cam, Movement_Direction::FORWARDS, CHANGE * forwards_acc);
        //         forwards_acc *= POS_ACC;
        //         forwards_acc = glm::min(max_acc, forwards_acc);
        //     } else {
        //         if (forwards_acc > 1.0) {
        //             FreeCamera_process_directional_movement(&main_cam, Movement_Direction::FORWARDS, CHANGE * forwards_acc);
        //         }
        //         forwards_acc = glm::max(1.0, forwards_acc * NEG_ACC);
        //     }
        //     if (*backwards) {
        //         FreeCamera_process_directional_movement(&main_cam, Movement_Direction::BACKWARDS, CHANGE * backwards_acc);
        //         backwards_acc *= POS_ACC;
        //         backwards_acc = glm::min(max_acc, backwards_acc);
        //     } else {
        //         if (backwards_acc > 1.0) {
        //             FreeCamera_process_directional_movement(&main_cam, Movement_Direction::BACKWARDS, CHANGE * backwards_acc);
        //         } 
        //         backwards_acc = glm::max(1.0, backwards_acc * NEG_ACC);  
        //     }

        //     if (*reset) {
        //         main_cam.position = start_pos;
        //         main_cam.orientation = glm::quat();

        //         up_acc        = 1.0;
        //         down_acc      = 1.0;
        //         left_acc      = 1.0;
        //         right_acc     = 1.0;
        //         backwards_acc = 1.0;
        //         forwards_acc  = 1.0;
        //     }
        // }
        // #endif



    //////////////////
    // DRAW


        glUseProgram(shader_bg);

        glClearColor(97.0 / 255.0, 201.0 / 255.0, 255.0 / 255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniformMatrix4fv(MAT_LOC, 1, GL_FALSE, glm::value_ptr(mat_projection));

        // IDENTITY
        //glm::vec3 pos = main_cam.position;
        glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
        glUniform3fv(CAM_LOC, 1, glm::value_ptr(pos));

        glUniform1f(TIME_LOC, t_since_start);

        glEnable(GL_DEPTH_TEST);
        glDepthRange(0, 1);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_BLEND); 
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBindVertexArray(bg_vao);
        glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        ////


        glUseProgram(shader_test_shape);

        glClear(GL_DEPTH_BUFFER_BIT);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        glUniformMatrix4fv(MAT_LOC_s, 1, GL_FALSE, 
            glm::value_ptr(
                mat_projection * glm::translate(glm::mat4(1.0f), glm::vec3(glm::sin(t_since_start), 0.0f, 0.0f))
            )
        );

        glBindVertexArray(s_vao);
        glDrawElements(GL_TRIANGLES, indices_count_s, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);


        // SWAP BUFFERS
        SDL_GL_SwapWindow(window);
    //////////////////
    }
    

    glDeleteVertexArrays(1, &bg_vao);
    glDeleteBuffers(1, (GLBuffer*)&bg_vbo);
    glDeleteBuffers(1, (GLBuffer*)&bg_ebo);    
    glDeleteVertexArrays(1, &s_vao);
    glDeleteBuffers(1, (GLBuffer*)&s_vbo);
    glDeleteBuffers(1, (GLBuffer*)&s_ebo);

    SDL_DestroyWindow(window);
    SDL_GL_DeleteContext(program_data);
    IMG_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}
