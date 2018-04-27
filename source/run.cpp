#include "opengl.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

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

void debug_print(std::string in);
void debug_print(std::string in) 
{
    #ifdef DEBUG_PRINT
    puts(in.c_str());
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

typedef struct _TextureData {
    Texture* ids;
    size_t count;
} TextureData;

typedef struct _GLData {
    SDL_GLContext context;
    TextureData textures;
} GLData;

GLData gl_data; 

struct {
    glm::mat4 m_model;
    glm::mat4 m_view;
    glm::mat4 m_projection;
} scene;

// glm::vec3 light_pos(1.2f, 1.0f, 2.0f);
// glm::light_color(1.0f, 1.0f, 1.0f);
// glm::toy_color(1.0f, 0.5f, 0.31f);
// glm::result = light_color * toy_color;

int main(/*int argc, char* argv[]*/)
{
    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        puts("SDL could not initialize");
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
    
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
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
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_MULTISAMPLE);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	

    glDepthRange(0, 1);
    glDepthFunc(GL_LEQUAL);
    
    SDL_GL_SetSwapInterval(1);
	
	//gl_init();
	
    // SHAPE DATA ////////////////////////////////////////////////////////////////
   
    gl_mesh::MeshData* mesh;
#ifdef CUBES
    gl_mesh::platonic_solid::Cube shape_mesh;
#elif defined SPHERES
    gl_mesh::parametric::Sphere shape_mesh(100);
#elif defined TORI
    gl_mesh::parametric::Torus shape_mesh(100);
#else
    #error SHAPE UNDEFINED
#endif
    mesh = &shape_mesh;
    const GLuint size_vertex_data = mesh->num_vertices() * 8 * sizeof(GLfloat);
    const GLuint num_indices = mesh->num_indices(); 
    const GLuint size_index_data = num_indices * sizeof(GLuint);  
    const GLfloat* vertex_data = mesh->vertex_data();
    const GLuint* index_data = mesh->index_data();
    const GLuint num_vertices = mesh->num_vertices();
    
    std::cout << size_vertex_data << std::endl;
    
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
    VaoBuffer VAO = 0;
    VboBuffer VBO = 0;
    EboBuffer EBO = 0;
    
    VaoBuffer VAO_Light = 0;
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    
    // copy vertices buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size_vertex_data, vertex_data, GL_STATIC_DRAW);
    // copy element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_index_data, index_data, GL_STATIC_DRAW);
    
    // ATTRIBUTES //
    
    // set attribute pointers for VERTEX vectors
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
//     glVertexAttribPointer(
//         glGetAttribLocation(shader_program, "position"), // if I wanted to search for the position
//         3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0
//     );
    glEnableVertexAttribArray(0);
    
    // set attribute pointers for NORMAL vectors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);    
    // set attribute pointers for UV coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindVertexArray(0);
    
    // LAMP
    glGenVertexArrays(1, &VAO_Light);
    glBindVertexArray(VAO_Light);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // copy element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    
    bool status = false;
    std::string p_noise = Shader::retrieve_src_from_file("shaders/perlin_noise.glsl", &status);
    if (status == false) {
        fprintf(stderr, "%s\n", "shader could not be loaded from file");
    }

    Shader prog_shader;
    prog_shader.load_from_file("shaders/tests/light_a.vrts", "shaders/tests/J.frgs", p_noise, p_noise);
    
    
    if (!prog_shader.is_valid()) {
        fprintf(stderr, "ERROR: LIGHT_A\n");
        return EXIT_FAILURE;
    }
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    printf("USING GL VERSION: %s\n", glGetString(GL_VERSION));
    
    ////////////////////////////////////////////////////////////////////////////
    // TEXTURE
    ////////////////////////////////////////////////////////////////////////////

    // load and create texture array
    Texture texture_ids[2];
    gl_data.textures.ids = texture_ids;
    gl_data.textures.count = 2;
//     texture[0].load(IMG_PATH_1, GL_TRUE);
//     texture[1].load(IMG_PATH_2, GL_TRUE);
    load_texture(&gl_data.textures.ids[0], IMG_PATH_1, GL_TRUE);
    load_texture(&gl_data.textures.ids[1], IMG_PATH_2, GL_TRUE);
    
    #ifdef FP_CAM
    Camera main_cam(glm::vec3(0.0f, 0.0f, 10.0f));
    #endif
    // MAIN RUN LOOP
    
	const Uint8* key_states = SDL_GetKeyboardState(NULL);

	const Uint8& up         = key_states[SDL_SCANCODE_W];
	const Uint8& down       = key_states[SDL_SCANCODE_S];
	const Uint8& left       = key_states[SDL_SCANCODE_A];
	const Uint8& right      = key_states[SDL_SCANCODE_D];
	const Uint8& rot_r = key_states[SDL_SCANCODE_RIGHT];
	const Uint8& rot_l = key_states[SDL_SCANCODE_LEFT];
// 	const Uint8& up_right   = key_states[SDL_SCANCODE_E];
// 	const Uint8& up_left    = key_states[SDL_SCANCODE_Q];
// 	const Uint8& down_right = key_states[SDL_SCANCODE_X];
// 	const Uint8& down_left  = key_states[SDL_SCANCODE_Z];

    glm::vec3 light_pos_vec(1.2f, 1.0f, 2.0f);
    
    GLfloat prev_x = SCREEN_WIDTH / 2;
    GLfloat prev_y = SCREEN_HEIGHT / 2;

    double up_acc = 1.0;
	double down_acc = 1.0;
    bool keep_running = true;
    SDL_Event event;

    const Uint32 INTERVAL = TIME_UNIT_TO_SECONDS / FRAMES_PER_SECOND;
    Uint32 start_time = SDL_GetTicks();
    Uint32 prev_time = start_time;
    Uint32 curr_time = start_time;
    Uint64 delta_time = 0;
    Uint64 dt = 0;

    while (keep_running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                keep_running = false;
            }
        }

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



        Texture* texture = gl_data.textures.ids;
                
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
        
        // TIME INFORMATION
        GLfloat elapsed = (curr_time - start_time) / (GLfloat)TIME_UNIT_TO_SECONDS;
        UniformLocation time_addr = glGetUniformLocation(prog_shader, "u_time");
        glUniform1f(time_addr, elapsed);
        
        // COLOR and LIGHT UNIFORMS
        UniformLocation object_color_loc = glGetUniformLocation(prog_shader, "object_color");
        UniformLocation light_color_loc = glGetUniformLocation(prog_shader, "light_color");
        UniformLocation light_pos_loc = glGetUniformLocation(prog_shader, "light_pos");
        glUniform3f(object_color_loc, 1.0f, 0.5f, 0.31f);
        glUniform3f(light_color_loc, 1.0f, 1.0f, 1.0f);
        
        light_pos_vec.x = 1.0f + glm::sin(curr_time / 1000.0) * 2.0f;
        light_pos_vec.y = glm::sin(curr_time / (1000.0 * 2.0f)) * 1.0f;
//         glm::mat4 rot;
//         rot = glm::rotate(rot, (GLfloat)curr_time / 1000.0f, glm::vec3(0.0f, 1.0f, 0.0f));
//         light_pos_vec = glm::mat3(rot) * light_pos_vec;
        glUniform3f(light_pos_loc, light_pos_vec.x, light_pos_vec.y, light_pos_vec.z);


        #ifdef FP_CAM			
		if (up) {
			main_cam.process_directional_movement(Camera_Movement::FORWARDS, (delta_time / (GLfloat)TIME_UNIT_TO_SECONDS) * up_acc);
			debug_print("FORWARD");	
			up_acc *= 1.06;		
		} else {
		    up_acc = 1.0;
		}
		if (down) {
			main_cam.process_directional_movement(Camera_Movement::BACKWARDS, (delta_time / (GLfloat)TIME_UNIT_TO_SECONDS) * down_acc);
			debug_print("BACKWARD");	
		    down_acc *= 1.06;
		} else {
		    down_acc = 1.0;
		}
		if (left) {
			main_cam.process_directional_movement(Camera_Movement::LEFTWARDS, delta_time / (GLfloat)TIME_UNIT_TO_SECONDS);
			debug_print("LEFTWARD");	
		}
		if (right) {
			main_cam.process_directional_movement(Camera_Movement::RIGHTWARDS, delta_time / (GLfloat)TIME_UNIT_TO_SECONDS);
			debug_print("RIGHTWARD");	
		}
		
		#ifdef MOUSE_ON
		int x = 0;
		int y = 0;
		SDL_GetMouseState(&x, &y);
		if (x && y) {
		    main_cam.process_mouse_movement(x - prev_x, y - prev_y);
		    prev_x = x;
		    prev_y = y;
		}
		#endif
		#endif
        
        // TRANSFORMATION MATRICES
        
        // transformations
        glm::mat4 ident_mat;
        scene.m_model = ident_mat;
        
        #ifdef FREE_CAM
        FreeCamera main_cam(glm::vec3(0.0f, 0.0f, 0.0f));
        main_cam.rotate(elapsed, glm::vec3(0.0f, 1.0f, 0.0f));
        main_cam.translate(glm::vec3(0.0f, 0.0f, -10.0f));
        #endif
        
        scene.m_view = main_cam.get_view_matrix();

        
        //scene.m_view = glm::rotate(scene.m_view, elapsed, glm::vec3(0.0f, 1.0f, 0.0f));
        
        UniformLocation model_loc = glGetUniformLocation(prog_shader, "model");
        UniformLocation view_loc = glGetUniformLocation(prog_shader, "view");
        UniformLocation projection_loc = glGetUniformLocation(prog_shader, "projection");
        UniformLocation model_view_projection_loc = glGetUniformLocation(prog_shader, "model_view_projection");
        
        UniformLocation mat_norm_loc = glGetUniformLocation(prog_shader, "mat_norm");
        
        //scene.m_model = glm::scale(scene.m_model, glm::vec3(SCREEN_HEIGHT/4, SCREEN_HEIGHT/4, 1.0f));
        //scene.m_view = glm::translate(scene.m_view, glm::vec3(0.0f, 0.0f, -10.0f));
        
        // set uniforms
        // view
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(scene.m_view));
        
        // projection
        // left, right, bottom, top
        //scene.m_projection = glm::ortho(-(float)SCREEN_WIDTH / 2.f, (float)SCREEN_WIDTH / 2.f, -(float)SCREEN_HEIGHT / 2.f, (float)SCREEN_HEIGHT / 2, 0.1f, 100.0f);
        scene.m_projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(scene.m_projection));
        
        // NORMAL MATRIX
        glm::mat3 m_norm;
 
        glBindVertexArray(VAO);
        for (GLuint i = 0; i < 7; i++) { 
            scene.m_model = ident_mat;
            
            // 
            scene.m_model = glm::translate(scene.m_model, shape_translations[i] + 5.0f * glm::vec3(0.0f, 0.0f, glm::sin((GLfloat)curr_time / 1000.0f)));
            //scene.m_model = glm::scale(scene.m_model, glm::vec3(0.5f, 0.5f, 0.5f));
//             if (i % 2 == 0) {
//                 scene.m_model = glm::rotate(scene.m_model, elapsed + glm::radians(20.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));
//             }         
            
            // set more uniforms
            // model
            glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(scene.m_model));
            // NORMAL MATRIX (using view space)
            m_norm = glm::transpose(glm::inverse(scene.m_model * scene.m_view));
            glUniformMatrix3fv(mat_norm_loc, 1, GL_FALSE, glm::value_ptr(m_norm));
                        
            // model_view_projection
            glUniformMatrix4fv(model_view_projection_loc, 1, GL_FALSE, glm::value_ptr(scene.m_projection * scene.m_view * scene.m_model));
            //glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, 0); // cubes
            glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(0);
                
        SDL_GL_SwapWindow(window);
    }

    for (size_t i = 0; i < gl_data.textures.count; ++i) {
        glDeleteTextures(1, &gl_data.textures.ids[i]);
    }
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &VAO_Light);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    
    SDL_GL_DeleteContext(gl_data.context);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}
