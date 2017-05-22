#define GL3_PROTOTYPES 1
#include <GL/glew.h>
#include <OpenGL/glu.h>
#include <OpenGL/gl3.h>

#include <SDL2/SDL.h>

#include <iostream>

SDL_GLContext gl_context;

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

SDL_Window* window = nullptr;

GLuint shader_program = 0; 

// vertex shader source string
const GLchar* vertex_shader_src[] = {
    "#version 330 core\n"
    "layout (location = 0) in vec3 position;\n" 
    "out vec3 vPos;\n"
    "void main() {\n"
    "   gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
    "   vPos = gl_Position.xyz * 2.;\n"
    "}\n"
};

// fragment shader source string
const GLchar* fragment_shader_src[] = {
    "#version 330 core\n"
    "precision highp float;\n"
    "in vec3 vPos;\n"
    "out vec4 color\n;"
    "void main() {\n"
    "   color = vec4(vPos, 1.0);\n"
    "}\n"
};

GLuint g_VAO = 0;
GLuint g_VBO = 0;
GLuint g_IBO = 0;

void init_GL(void)
{    
    // VERTEX SHADER ///////////////////////////////////////////////////////////
    
    // create
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    // set source
    glShaderSource(vertex_shader, 1, vertex_shader_src, NULL);
    // compile vertex source
    glCompileShader(vertex_shader);
    
    // error checking
    {
        GLchar info_log[512];
        GLint success = GL_FALSE;
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
        if (success != GL_TRUE) {
            glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" 
                      << info_log << std::endl;
            exit(EXIT_FAILURE);
        } else {
            std::cout << "SHADER::VERTEX::COMPILATION_SUCCEEDED" << std::endl;
        }
    }

    
    // FRAGMENT SHADER /////////////////////////////////////////////////////////
    
    // create 
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    // set source
    glShaderSource(fragment_shader, 1, fragment_shader_src, NULL);
    // compile fragment source
    glCompileShader(fragment_shader);
    
    // error checking
    {
        GLchar info_log[512];
        GLint success = GL_FALSE;
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
        if (success != GL_TRUE) {
            glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" 
                      << info_log << std::endl;
            exit(EXIT_FAILURE);
        } else {
            std::cout << "SHADER::FRAGMENT::COMPILATION_SUCCEEDED" << std::endl;
        }
    }
    
    // create shader program
    shader_program = glCreateProgram();
    // attach vertex shader
    glAttachShader(shader_program, vertex_shader);
    // attach fragment shader
    glAttachShader(shader_program, fragment_shader);
    // link the program
    glLinkProgram(shader_program);
    
    // error checking
    {
        GLchar info_log[512];
        GLint success = GL_FALSE;
        glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
        if (success != GL_TRUE) {
            glGetProgramInfoLog(shader_program, 512, NULL, info_log);
            std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" 
                      << info_log << std::endl;
            exit(EXIT_FAILURE);
        } else {
            std::cout << "SHADER::PROGRAM::LINK_SUCCEEDED" << std::endl;
        }
    }
    
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    // VERTICES ////////////////////////////////////////////////////////////////
    
    // VERTEX BUFFER OBJECT DATA
    GLfloat vertex_data[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };
    
    // CREATE VERTEX ARRAY OBJECT
    glGenVertexArrays(1, &g_VAO);
    glBindVertexArray(g_VAO);
    
    // CREATE VERTEX BUFFER OBJECT
    glGenBuffers(1, &g_VBO);
    
    // copy vertices array
    glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
    glBufferData(GL_ARRAY_BUFFER, 3 * 3 * sizeof(GLfloat), vertex_data, GL_STATIC_DRAW);
    // set attribute pointers for vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
}

// based on http://headerphile.com/sdl2/opengl-part-1-sdl-opengl-awesome/
int main(int argc, char* argv[])
{
    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "SDL could not initialize" << std::endl;
        return EXIT_FAILURE;
    }
    
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	//SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // create the window
    if (!(window = SDL_CreateWindow("GL TEST",
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SCREEN_WIDTH, SCREEN_HEIGHT,
                                    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN )))
    {
        std::cout << "Window could not be created" << std::endl;
        return EXIT_FAILURE;
    }
    
	gl_context = SDL_GL_CreateContext(window);

	//#ifndef __APPLE__
	// glewExperimental = GL_TRUE;
    glewInit();
    //#endif
	
	SDL_GL_SetSwapInterval(1);
	
	init_GL();

    printf("%s\n", glGetString(GL_VERSION));
    
    //glClearColor(0.0, 0.0, 0.0, 1.0);
    //SDL_GL_SwapWindow(window);
    
//     bool loop = true;
//     while (loop) {
//         SDL_Event event;
//         while (SDL_PollEvent(&event)) {
//             if (event.type == SDL_QUIT) {
//                 loop = false;
//             }
// 
//             if (event.type == SDL_KEYDOWN) {
//                 switch (event.key.keysym.sym) {
//                 case SDLK_ESCAPE:
//                     loop = false;
//                     break;
//                 case SDLK_r:
//                     // Cover with red and update
//                     glClearColor(1.0, 0.0, 0.0, 1.0);
//                     glClear(GL_COLOR_BUFFER_BIT);
//                     SDL_GL_SwapWindow(window);
//                     break;
//                 case SDLK_g:
//                     // Cover with green and update
//                     glClearColor(0.0, 1.0, 0.0, 1.0);
//                     glClear(GL_COLOR_BUFFER_BIT);
//                     SDL_GL_SwapWindow(window);
//                     break;
//                 case SDLK_b:
//                     // Cover with blue and update
//                     glClearColor(0.0, 0.0, 1.0, 1.0);
//                     glClear(GL_COLOR_BUFFER_BIT);
//                     SDL_GL_SwapWindow(window);
//                     break;
//                 default:
//                     break;
//                 }
//             }
//         }
//     }

    glClearColor(0.0, 0.0, 0.0, 1.0);
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader_program);
        glBindVertexArray(g_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
        SDL_GL_SwapWindow(window);
    }
    
    glDeleteProgram(shader_program);
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
