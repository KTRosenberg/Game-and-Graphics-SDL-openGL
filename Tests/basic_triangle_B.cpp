#define GL3_PROTOTYPES 1
#include <GL/glew.h>
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
//#include <OpenGL/gl3.h>
#include <SDL2/SDL.h>
#include <iostream>

// based on tutorial at 
// http://headerphile.com/sdl2/opengl-part-1-sdl-opengl-awesome/

SDL_GLContext gl_context;

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 640
#define TIME_UNIT_TO_SECONDS 1000

SDL_Window* window = nullptr;

GLuint shader_program = 0; 

// vertex shader source string
const GLchar* vertex_shader_src[] = {
    "#version 330 core\n"
    "layout (location = 0) in vec3 position;\n" 
    "uniform float u_time;\n"
    "out vec3 v_pos;\n"
    "void main() {\n"
    "   gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
    "   v_pos = position.xyz;\n"
    "}\n"
};

// fragment shader source string
const GLchar* fragment_shader_src[] = {
    "#version 330 core\n"
    "precision highp float;\n"
    "in vec3 v_pos;\n"
    "uniform float u_time;\n"
    "out vec4 color\n;"
    "void main() {\n"
    "   vec3 adjust = v_pos;\n"
    "   adjust.x = sin(adjust.x + u_time);\n"
    "   adjust.y = cos(adjust.y + u_time);\n"
    "   color = vec4(sqrt(adjust), 1.0);\n"
    "   //color = vec4(sqrt(v_pos), 1.0);\n"
    "}\n"
};

GLuint g_VAO = 0;
GLuint g_VBO = 0;
GLuint g_EBO = 0;

void gl_init(void)
{
    // for error checking:
    GLchar info_log[512];
    GLint success = GL_FALSE;
    // VERTEX SHADER ///////////////////////////////////////////////////////////
    
    // create
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    // set source
    glShaderSource(vertex_shader, 1, vertex_shader_src, NULL);
    // compile vertex source
    glCompileShader(vertex_shader);
    
    // error checking
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" 
                  << info_log << std::endl;
        exit(EXIT_FAILURE);
    } else {
        std::cout << "SHADER::VERTEX::COMPILATION_SUCCEEDED" << std::endl;
    }

    
    // FRAGMENT SHADER /////////////////////////////////////////////////////////
    
    // create 
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    // set source
    glShaderSource(fragment_shader, 1, fragment_shader_src, NULL);
    // compile fragment source
    glCompileShader(fragment_shader);
    
    // error checking
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" 
                  << info_log << std::endl;
        exit(EXIT_FAILURE);
    } else {
        std::cout << "SHADER::FRAGMENT::COMPILATION_SUCCEEDED" << std::endl;
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
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (success != GL_TRUE) {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" 
                  << info_log << std::endl;
        exit(EXIT_FAILURE);
    } else {
        std::cout << "SHADER::PROGRAM::LINK_SUCCEEDED" << std::endl;
    }
    
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    // VERTICES ////////////////////////////////////////////////////////////////

    const GLfloat FACTOR = 1.0;
        
    // VERTEX BUFFER OBJECT DATA
    GLfloat vertex_data[] = {
        -FACTOR,  FACTOR, 0.0f,  // Top Left
        -FACTOR, -FACTOR, 0.0f,  // Bottom Left 
         FACTOR, -FACTOR, 0.0f,  // Bottom Right
         FACTOR,  FACTOR, 0.0f,  // Top Right
    };
    
    GLuint index_data[] = {
        0, 1, 2,
        2, 3, 0
    };
    
    // VERTEX ARRAY OBJECT, VERTEX BUFFER OBJECT, ELEMENT BUFFER OBJECT
    
    glGenVertexArrays(1, &g_VAO);
    glGenBuffers(1, &g_VBO);
    glGenBuffers(1, &g_EBO);
    glBindVertexArray(g_VAO);
    
    // copy vertices buffer
    glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
    // copy element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_data), index_data, GL_STATIC_DRAW);
    
    // set attribute pointers for vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindVertexArray(0);
}

int ignore_mouse_movement(void* unused, SDL_Event* event)
{
	return (event->type == SDL_MOUSEMOTION) ? 0 : 1;
}

int main(/*int argc, char* argv[]*/)
{
    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "SDL could not initialize" << std::endl;
        return EXIT_FAILURE;
    }
    
    // disable the cursor
    SDL_ShowCursor(SDL_DISABLE);
    // ignore mouse movement events
    SDL_SetEventFilter(ignore_mouse_movement, nullptr);
    
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // create the window
    if (!(window = SDL_CreateWindow("GL TEST",
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SCREEN_WIDTH, SCREEN_HEIGHT,
                                    SDL_WINDOW_OPENGL /*| SDL_WINDOW_SHOWN*/)))
    {
        std::cout << "Window could not be created" << std::endl;
        return EXIT_FAILURE;
    }
    
	gl_context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
    glewInit();
	
	SDL_GL_SetSwapInterval(1);
	
	glEnable(GL_DEPTH_TEST);
    glDepthRange(0, 1);
    glDepthFunc(GL_LEQUAL);
	
	gl_init();
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    printf("USING GL VERSION: %s\n", glGetString(GL_VERSION));

    // MAIN RUN LOOP
    bool keep_running = true;
    SDL_Event event;
    Uint32 start_time = SDL_GetTicks();
    while (keep_running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                keep_running = false;
            }
        }
        
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(shader_program);
        
        GLuint time_addr = glGetUniformLocation(shader_program, "u_time");
        GLfloat elapsed = (SDL_GetTicks() - start_time) / (GLfloat)TIME_UNIT_TO_SECONDS;
        glUniform1f(time_addr, elapsed);
        
        glBindVertexArray(g_VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
                
        SDL_GL_SwapWindow(window);
    }
    
    glDeleteVertexArrays(1, &g_VAO);
    glDeleteBuffers(1, &g_VBO);
    glDeleteBuffers(1, &g_EBO);
    glDeleteProgram(shader_program);
    
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
