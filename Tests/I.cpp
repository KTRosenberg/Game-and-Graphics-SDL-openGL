#define GL3_PROTOTYPES 1
#define GLEW_STATIC
#include <GL/glew.h>
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
//#include <OpenGL/gl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>
#include <string>
#include <vector>

#include "shader.hpp"
#include "texture.hpp"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define TIME_UNIT_TO_SECONDS 1000

#define IMG_PATH_1 "textures/final_rush_walkway_2.png"
#define IMG_PATH_2 "textures/brick.png"
#define IMG_PATH_3 "textures/lavatile.jpg"


SDL_Window* window = nullptr;

// based on tutorial at 
// http://headerphile.com/sdl2/opengl-part-1-sdl-opengl-awesome/

SDL_GLContext gl_context;

GLuint shader_program = 0;

GLuint g_VAO = 0;
GLuint g_VBO = 0;
GLuint g_EBO = 0;

int ignore_mouse_movement(void* unused, SDL_Event* event)
{
	return (event->type == SDL_MOUSEMOTION) ? 0 : 1;
}

GLuint load_texture(std::string path, GLboolean alpha=GL_TRUE);
GLuint load_texture(std::string path, GLboolean alpha)
{
    // load image
    SDL_Surface* img = nullptr; 
    if (!(img = IMG_Load(path.c_str()))) {
        printf("SDL_image could not be loaded %s, SDL_image Error: %s\n", 
               path.c_str(), IMG_GetError());
        return (GLuint)0;
    }
    
    GLuint texture_id = 0;
    glGenTextures(1, &texture_id);

    glBindTexture(GL_TEXTURE_2D, texture_id);
    // image assignment
    GLuint format = (alpha) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, img->w, img->h, 0, format, GL_UNSIGNED_BYTE, img->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    // wrapping behavior
    GLuint alpha_behavior = (alpha && false) ? GL_CLAMP_TO_EDGE : GL_REPEAT;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, alpha_behavior);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, alpha_behavior);
    // texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    // free the surface
    SDL_FreeSurface(img);
    
    return texture_id;
}

struct {
    glm::mat4 m_model;
    glm::mat4 m_view;
    glm::mat4 m_projection;
} scene;

// struct ParametricFunction {
//     virtual std::vector<GLfloat> operator() (GLfloat u, GLfloat v) = 0;
// };
// 
// struct SphereFunction : public ParametricFunction {
//     virtual std::vector<GLfloat> operator() (GLfloat u, GLfloat v);
// };
// std::vector<GLfloat> SphereFunction::operator() (GLfloat u, GLfloat v)
// {
//     double PI = glm::pi<double>();
//     double theta = 2 * PI * u;
//     double phi = PI * (v - 0.5);
//     double z = glm::sin(phi);
//     
//     double c = glm::cos(theta) * glm::cos(phi);
//     double s = glm::sin(theta) * glm::cos(phi);
//     return {static_cast<GLfloat>(c), static_cast<GLfloat>(s), static_cast<GLfloat>(z), u, v};       
// }
// 
// struct GeometryGenerator {
//     static void add_mesh_vertices(std::vector<GLfloat>& V, GLint m, GLint n, ParametricFunction* func);
//     static std::vector<GLfloat> sphere(GLint n);
// };
// 
// std::vector<GLfloat> GeometryGenerator::sphere(GLint n)
// {
//     std::vector<GLfloat> V;
//     SphereFunction f;
//     GeometryGenerator::add_mesh_vertices(V, n, n, &f);
//     return std::move(V);       
// }
// 
// void GeometryGenerator::add_mesh_vertices(std::vector<GLfloat>& V, GLint m, GLint n, ParametricFunction* func)
// {
//     auto append = [V](std::vector<GLfloat>& A) mutable
//     {
//         for (GLuint i = 0; i < A.size(); i++) {
//             V.push_back(A[i]);
//         }
//     };
//     
//     for (GLint j = 0; j < n; j++) {
//         for (GLint i = 0; i < m; i++) {
//             std::vector<GLfloat> A = (*func)(i / (GLfloat)m, j / (GLfloat)n);
//             std::vector<GLfloat> B = (*func)((i + 1) / (GLfloat)m, j / (GLfloat)n);
//             std::vector<GLfloat> C = (*func)(i / (GLfloat)m, (j + 1) / (GLfloat)n);
//             std::vector<GLfloat> D = (*func)((i + 1) / (GLfloat)m, (j + 1) / (GLfloat)n);
//             append(A); append(B); append(D);
//             append(D); append(C); append(A);
//         }
//     }
// } 

int main(/*int argc, char* argv[]*/)
{
    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialize" << std::endl;
        return EXIT_FAILURE;
    }
    
    // disable the cursor
    SDL_ShowCursor(SDL_DISABLE);
    // ignore mouse movement events
    SDL_SetEventFilter(ignore_mouse_movement, nullptr);
    
    // openGL initialization ///////////////////////////////////////////////////
    
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

    // create the window
    if (!(window = SDL_CreateWindow("openGL TEST",
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SCREEN_WIDTH, SCREEN_HEIGHT,
                                    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN /*| SDL_WINDOW_ALLOW_HIGHDPI*/)))
    {
        std::cout << "Window could not be created" << std::endl;
        return EXIT_FAILURE;
    }
    
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if(!(IMG_Init(imgFlags) & imgFlags))
    {
     	printf("SDL_image could not initialize, SDL_image Error: %s\n", IMG_GetError());
        SDL_DestroyWindow(window);
        return EXIT_FAILURE;
    }
    
	gl_context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
    glewInit();
	
	SDL_GL_SetSwapInterval(1);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_MULTISAMPLE);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	

    glDepthRange(0, 1);
    glDepthFunc(GL_LEQUAL);
	
	//gl_init();
	
    // VERTICES ////////////////////////////////////////////////////////////////

    const GLfloat FACTOR = 0.5;
        
    // VERTEX BUFFER OBJECT DATA

//     GLfloat vertex_data[] = {
//         // pos                   // uv coords                                  
//         -FACTOR,  FACTOR, 0.0f,  0.0f, 1.0f, // Top Left
//         -FACTOR, -FACTOR, 0.0f,  0.0f, 0.0f, // Bottom Left 
//          FACTOR, -FACTOR, 0.0f,  1.0f, 0.0f, // Bottom Right
//          FACTOR,  FACTOR, 0.0f,  1.0f, 1.0f, // Top Right
//     };
//     
//     GLuint index_data[] = {
//         0, 1, 2,
//         2, 3, 0,
//     };

    // front, right, back, left, top, bottom
    GLfloat vertex_data[] = {
        // pos                   // uv coords                                  
        -FACTOR,  FACTOR,  FACTOR,  0.0f, 1.0f, // Top Left
        -FACTOR, -FACTOR,  FACTOR,  0.0f, 0.0f, // Bottom Left 
         FACTOR, -FACTOR,  FACTOR,  1.0f, 0.0f, // Bottom Right
         FACTOR,  FACTOR,  FACTOR,  1.0f, 1.0f, // Top Right
         
         FACTOR,  FACTOR,  FACTOR,  0.0f, 1.0f, // Top Left
         FACTOR, -FACTOR,  FACTOR,  0.0f, 0.0f, // Bottom Left 
         FACTOR, -FACTOR, -FACTOR,  1.0f, 0.0f, // Bottom Right
         FACTOR,  FACTOR, -FACTOR,  1.0f, 1.0f, // Top Right
         
         FACTOR,  FACTOR, -FACTOR,  0.0f, 1.0f, // Top Left
         FACTOR, -FACTOR, -FACTOR,  0.0f, 0.0f, // Bottom Left 
        -FACTOR, -FACTOR, -FACTOR,  1.0f, 0.0f, // Bottom Right
        -FACTOR,  FACTOR, -FACTOR,  1.0f, 1.0f, // Top Right
        
        -FACTOR,  FACTOR, -FACTOR,  0.0f, 1.0f, // Top Left
        -FACTOR, -FACTOR, -FACTOR,  0.0f, 0.0f, // Bottom Left 
        -FACTOR, -FACTOR,  FACTOR,  1.0f, 0.0f, // Bottom Right
        -FACTOR,  FACTOR,  FACTOR,  1.0f, 1.0f, // Top Right
        
        -FACTOR,  FACTOR, -FACTOR,  0.0f, 1.0f, // Top Left
        -FACTOR,  FACTOR,  FACTOR,  0.0f, 0.0f, // Bottom Left 
         FACTOR,  FACTOR,  FACTOR,  1.0f, 0.0f, // Bottom Right
         FACTOR,  FACTOR, -FACTOR,  1.0f, 1.0f, // Top Right
         
        -FACTOR, -FACTOR,  FACTOR,  0.0f, 1.0f, // Top Left
        -FACTOR, -FACTOR, -FACTOR,  0.0f, 0.0f, // Bottom Left 
         FACTOR, -FACTOR, -FACTOR,  1.0f, 0.0f, // Bottom Right
         FACTOR, -FACTOR,  FACTOR,  1.0f, 1.0f, // Top Right
    };
    
    GLuint index_data[] = {
        0, 1, 2,
        2, 3, 0,
                0, 1, 2,
                2, 3, 0,
                
                0, 1, 2,
                2, 3, 0,
                
                0, 1, 2,
                2, 3, 0,
                
                0, 1, 2,
                2, 3, 0,
                
                0, 1, 2,
                2, 3, 0,
    };
    for (GLuint i = 0; i < 6; i++) {
        for (GLuint j = 0; j < 6; j++) {
            index_data[(i * 6) + j] += (4 * i);
        }
    }
    
    glm::vec3 shape_translations[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-3.8f, -2.0f, -12.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.0f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f)  
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
    
    // ATTRIBUTES //
    
    // set attribute pointers for vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
//     glVertexAttribPointer(
//         glGetAttribLocation(shader_program, "position"), // if I wanted to search for the position
//         3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0
//     );
    glEnableVertexAttribArray(0);
    // set attribute pointers for uv coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindVertexArray(0);
    
    Shader prog_shader;
    std::string p_noise = Shader::retrieve_src_from_file("shaders/perlin_noise.shader");
    prog_shader.load_from_file("shaders/basic_i.vrts", "shaders/basic_i.frgs", p_noise, p_noise);
    
    if (!prog_shader.is_valid()) {
        return EXIT_FAILURE;
    }
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    printf("USING GL VERSION: %s\n", glGetString(GL_VERSION));
    
    ////////////////////////////////////////////////////////////////////////////
    // TEXTURE
    ////////////////////////////////////////////////////////////////////////////

    // load and create texture array
    Texture texture[2];
//     texture[0].load(IMG_PATH_1, GL_TRUE);
//     texture[1].load(IMG_PATH_2, GL_TRUE);
    texture[0].load(IMG_PATH_3, GL_FALSE);
    texture[1].load(IMG_PATH_3, GL_FALSE);
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
        // TEXTURE 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture[0]);
        glUniform1i(glGetUniformLocation(prog_shader, "tex0"), 0);
        // TEXTURE 1
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture[1]);
        glUniform1i(glGetUniformLocation(prog_shader, "tex1"), 1);
        
        prog_shader.use();
        
        // TIME UNIFORM
        GLfloat elapsed = (SDL_GetTicks() - start_time) / (GLfloat)TIME_UNIT_TO_SECONDS;
        GLuint time_addr = glGetUniformLocation(prog_shader, "u_time");
        glUniform1f(time_addr, elapsed);
        
        // TRANSFORMATION MATRICES
        
        // transformations
        glm::mat4 ident_mat;
        scene.m_model = ident_mat;
        scene.m_view = ident_mat;
        scene.m_view = glm::translate(scene.m_view, glm::vec3(0.0f, 0.0f, -3.0f));
        scene.m_view = glm::rotate(scene.m_view, elapsed, glm::vec3(0.0f, 1.0f, 0.0f));
        
        GLuint model_loc = glGetUniformLocation(prog_shader, "model");
        GLuint view_loc = glGetUniformLocation(prog_shader, "view");
        GLuint projection_loc = glGetUniformLocation(prog_shader, "projection");
        GLuint model_view_projection_loc = glGetUniformLocation(prog_shader, "model_view_projection");
        
        //scene.m_model = glm::scale(scene.m_model, glm::vec3(SCREEN_HEIGHT/4, SCREEN_HEIGHT/4, 1.0f));
        //scene.m_view = glm::translate(scene.m_view, glm::vec3(0.0f, 0.0f, -10.0f));
        
        // left, right, bottom, top
        //scene.m_projection = glm::ortho(-(float)SCREEN_WIDTH / 2.f, (float)SCREEN_WIDTH / 2.f, -(float)SCREEN_HEIGHT / 2.f, (float)SCREEN_HEIGHT / 2, 0.1f, 100.0f);
        scene.m_projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
        
        // set uniforms
        // view
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(scene.m_view));
        // projection
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(scene.m_projection));
 
        glBindVertexArray(g_VAO);
        for (GLuint i = 0; i < 10; i++) { 
            scene.m_model = ident_mat;
            scene.m_model = glm::translate(scene.m_model, shape_translations[i]);
            if (i % 2 == 0) {
                scene.m_model = glm::rotate(scene.m_model, elapsed + glm::radians(20.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));
            }         
            
            // set more uniforms
            // model
            glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(scene.m_model));            
            // model_view_projection
            glUniformMatrix4fv(model_view_projection_loc, 1, GL_FALSE, glm::value_ptr(scene.m_projection * scene.m_view * scene.m_model));
            glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, 0); // cubes
        }
        glBindVertexArray(0);
                
        SDL_GL_SwapWindow(window);
    }
    
    glDeleteVertexArrays(1, &g_VAO);
    glDeleteBuffers(1, &g_VBO);
    glDeleteBuffers(1, &g_EBO);
    //glDeleteTextures(2, texture);
    
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}
