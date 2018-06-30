#define WINDOW_HEADER ("")

#define SCREEN_WIDTH  (1280.0f)
#define SCREEN_HEIGHT (720.0f)
#define MS_PER_S (1000.0)
#define FRAMES_PER_SECOND (60.0)

#define EDITOR

//#define DEBUG_PRINT
//#define FPS_COUNT


#include "test.h"


#define COMMON_UTILS_IMPLEMENTATION
#include "common_utils.h"
#define COMMON_UTILS_CPP_IMPLEMENTATION
#include "common_utils_cpp.h"
#define CORE_UTILS_IMPLEMENTATION
#include "core_utils.h"

#define COLLISION_IMPLEMENTATION
#include "collision.h"



#include "opengl.hpp"

#include "sdl.hpp"

#include <iostream>
#include <string>
//#include <array>
#include <vector>

#include "shader.hpp"
#include "texture.hpp"
#include "camera.hpp"

#include "collision.h"

#include "file_io.hpp"


int ignore_mouse_movement(void* unused, SDL_Event* event)
{
    return (event->type == SDL_MOUSEMOTION) ? 0 : 1;
}


SDL_Window* window = NULL;

typedef void* (*Fn_MemoryAllocator)(size_t bytes);


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

void TextureData_init_inplace(TextureData* t, const size_t id_count, Texture* buffer)
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
void TextureData_delete_inplace(TextureData* t)
{
    glDeleteTextures(t->count, t->ids);
}

struct sceneData {
    glm::mat4 m_model;
    glm::mat4 m_view;
    glm::mat4 m_projection;
} scene;


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

struct VertexBufferData {
    VertexBuffer vbo;
    ElementBuffer ebo;
    size_t    v_cap;
    size_t    v_count;
    size_t    i_cap;
    size_t    i_count;
    GLfloat*  vertices;
    GLuint*   indices;
};

struct Open_GL_Data {
    VertexBuffer vbo;
    ElementBuffer ebo;
    DynamicBuffer<GLfloat> vertices;
    DynamicBuffer<GLuint> indices;
};

struct Entity {
    glm::vec3 position;
    GLfloat rotation;
};

// struct VertexBufferDataAlt {
//     VertexBuffer vbo;
//     ElementBuffer ebo;
//     Buffer<GLfloat> vertices_lines;
//     Buffer<GLuint>  indices;
// } VertexBufferDataAlt;

// typedef VertexBufferData VBData;



void VertexBufferData_init(
    VertexBufferData* g,
    const size_t v_cap,
    const size_t i_cap,
    Fn_MemoryAllocator alloc_v,
    Fn_MemoryAllocator alloc_i
) {
    glGenBuffers(1, (GLBuffer*)&g->vbo);
    glGenBuffers(1, (GLBuffer*)&g->ebo);

    g->v_cap = v_cap;
    g->i_cap = i_cap;

    g->vertices = (GLfloat*)alloc_v(sizeof(GLfloat) * g->v_cap);
    g->indices  = (GLuint*)alloc_i(sizeof(GLuint) * g->i_cap);

    g->v_count = v_cap;
    g->i_count = i_cap;
}

void VertexBufferData_init_inplace(
    VertexBufferData* g,
    const size_t v_cap,
    GLfloat* vertices,
    const size_t i_cap,
    GLuint* indices
) {
    glGenBuffers(1, (GLBuffer*)&g->vbo);
    glGenBuffers(1, (GLBuffer*)&g->ebo);

    g->v_cap = v_cap;
    g->i_cap = i_cap;

    g->vertices = vertices;
    g->indices  = indices;

    g->v_count = v_cap;
    g->i_count = i_cap;
}

void VertexBufferData_delete(VertexBufferData* g)
{
    glDeleteBuffers(1, (GLBuffer*)&g->vbo);
    glDeleteBuffers(1, (GLBuffer*)&g->ebo);
    free(g->vertices);
    free(g->indices);
}
void VertexBufferData_delete_inplace(VertexBufferData* g)
{
    glDeleteBuffers(1, (GLBuffer*)&g->vbo);
    glDeleteBuffers(1, (GLBuffer*)&g->ebo);
}

void VertexBufferData_init_with_arenas(ArenaAllocator* v_arena, ArenaAllocator* i_arena, VertexBufferData* vbd, size_t v_count_elements, size_t i_count_elements) 
{
    VertexBufferData_init_inplace(
        vbd, 
        v_count_elements,
        (GLfloat*)ArenaAllocator_allocate(v_arena, v_count_elements * sizeof(GLfloat)),
        i_count_elements,
        (GLuint*)ArenaAllocator_allocate(i_arena, i_count_elements * sizeof(GLuint))
    );
}

void* GlobalArenaAlloc_vertex_attribute_data(size_t count) 
{
    return xmalloc(count * sizeof(GLfloat));
}
void* GlobalArenaAlloc_index_data(size_t count) 
{
    return xmalloc(count * sizeof(GLuint));    
}


typedef struct VertexAttributeArray {
    VertexArray vao;
    size_t stride;

    operator GLuint() { return vao; }

} VertexAttributeArray;

typedef VertexAttributeArray VAttribArr;

void VertexAttributeArray_init(VertexAttributeArray* vao, size_t stride) 
{
    glGenVertexArrays(1, &vao->vao);
    vao->stride = stride;
}
void VertexAttributeArray_delete(VertexAttributeArray* vao) 
{
    glDeleteVertexArrays(1, &vao->vao);
}

#define attribute_offsetof(offset) (GLvoid*)(offset * sizeof(GLfloat))

#define attribute_sizeof(vao) vao->stride * sizeof(GLfloat)

void gl_set_and_enable_vertex_attrib_ptr(GLuint index, GLint size, GLenum type, GLboolean normalized, size_t offset, VertexAttributeArray* va)
{
    glVertexAttribPointer(index, size, type, normalized, attribute_sizeof(va), attribute_offsetof(offset));            
    glEnableVertexAttribArray(index);
}

void gl_bind_buffers_and_upload_data(VertexBufferData* vbd, GLenum usage, size_t v_cap, size_t i_cap, GLintptr v_begin_offset = 0, GLintptr i_begin_offset = 0)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbd->vbo);
    glBufferData(GL_ARRAY_BUFFER, v_cap * sizeof(GLfloat), vbd->vertices + v_begin_offset, usage);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbd->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, i_cap * sizeof(GLuint), vbd->indices + i_begin_offset, usage);            
}

void gl_bind_buffers_and_upload_data(VertexBufferData* vbd, GLenum usage, GLintptr v_begin_offset = 0, GLintptr i_begin_offset = 0)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbd->vbo);
    glBufferData(GL_ARRAY_BUFFER, vbd->v_cap * sizeof(GLfloat), vbd->vertices + v_begin_offset, usage);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbd->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vbd->i_cap * sizeof(GLuint), vbd->indices + i_begin_offset, usage);           
}

void gl_bind_buffers_and_upload_sub_data(VertexBufferData* vbd)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbd->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vbd->v_count * sizeof(GLfloat), vbd->vertices);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbd->ebo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vbd->i_count * sizeof(GLuint), vbd->indices);
}

void gl_bind_buffers_and_upload_sub_data(VertexBufferData* vbd, usize v_dest_offset, usize v_sub_count, GLintptr v_begin_offset, usize i_dest_offset, usize i_sub_count, GLintptr i_begin_offset)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbd->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, v_dest_offset * sizeof(GLfloat), v_sub_count * sizeof(GLfloat), vbd->vertices + v_begin_offset);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbd->ebo);
    glBufferSubData(GL_ARRAY_BUFFER, i_dest_offset * sizeof(GLuint), i_sub_count * sizeof(GLuint), vbd->indices + i_begin_offset);
}

void gl_bind_buffers_and_upload_sub_data(VertexBufferData* vbd, usize v_dest_offset, usize v_sub_count, GLintptr v_begin_offset, usize i_dest_offset, usize i_sub_count, GLintptr i_begin_offset, GLfloat* vertices, GLuint* indices)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbd->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, v_dest_offset * sizeof(GLfloat), v_sub_count * sizeof(GLfloat), vertices + v_begin_offset);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbd->ebo);
    glBufferSubData(GL_ARRAY_BUFFER, i_dest_offset * sizeof(GLuint), i_sub_count * sizeof(GLuint), indices + i_begin_offset);
}

struct GLData {
    VertexAttributeArray vao;
    VertexBufferData vbd;
};

void GLData_init(GLData* gl_data, size_t attribute_stride, const size_t v_cap, const size_t i_cap, Fn_MemoryAllocator alloc_v, Fn_MemoryAllocator alloc_i) 
{
    VertexAttributeArray_init(&gl_data->vao, attribute_stride);
    VertexBufferData_init(&gl_data->vbd, v_cap, i_cap, alloc_v, alloc_i);
}

void GLData_init_inplace(GLData* gl_data, size_t attribute_stride, const size_t v_cap, GLfloat* vertices, const size_t i_cap, GLuint* indices) 
{
    VertexAttributeArray_init(&gl_data->vao, attribute_stride);
    VertexBufferData_init_inplace(&gl_data->vbd, v_cap, vertices, i_cap, indices);    
}

inline void GLData_advance(GLData* const gl_data, const size_t i)
{
    gl_data->vbd.v_count += (i * gl_data->vao.stride);
    gl_data->vbd.i_count += i;
}

void GLData_delete(GLData* gl_data)
{
    VertexAttributeArray_delete(&gl_data->vao);
    VertexBufferData_delete(&gl_data->vbd);
}

void GLData_delete_inplace(GLData* gl_data)
{
    VertexAttributeArray_delete(&gl_data->vao);
    VertexBufferData_delete_inplace(&gl_data->vbd);    
}

// WORLD STATE
struct Room {
    VertexBufferData  geometry;
    Collider* collision_data;
    glm::mat4 matrix;
};

struct World {
    Room* rooms;
    glm::mat4 m_view;
    glm::mat4 m_projection;
};

struct GlobalData {
    SDL_GLContext context;
    TextureData textures;
};

GlobalData program_data;

#define GL_DRAW2D
#include "gl_draw2d.h"

WindowState window_state;

#define MAX_CONTROLLERS (1)
//SDL_GameController* controller_handles[MAX_CONTROLLERS];
//int controller_index = 0;
SDL_GameController* controller_handle = nullptr;
char* controller_mapping = nullptr;

bool poll_input_events(input_sys::Input* input, SDL_Event* event)
{
    using namespace input_sys;

    keys_advance_history(input);

#ifdef EDITOR
    mouse_advance_history(input);
#endif

    while (SDL_PollEvent(event)) {

        switch (event->type) {
        case SDL_QUIT:
            if (controller_handle != nullptr) {
                SDL_GameControllerClose(controller_handle);
            }
            return false;
        case SDL_WINDOWEVENT:
            switch (event->window.event) {
            // case SDL_WINDOWEVENT_SHOWN:
            //     SDL_Log("Window %d shown", event->window.windowID);
            //     break;
            // case SDL_WINDOWEVENT_HIDDEN:
            //     SDL_Log("Window %d hidden", event->window.windowID);
            //     break;
            // case SDL_WINDOWEVENT_EXPOSED:
            //     SDL_Log("Window %d exposed", event->window.windowID);
            //     break;
            // case SDL_WINDOWEVENT_MOVED:
            //     SDL_Log("Window %d moved to %d,%d",
            //             event->window.windowID, event->window.data1,
            //             event->window.data2);
            //     break;
            // case SDL_WINDOWEVENT_RESIZED:
            //     SDL_Log("Window %d resized to %dx%d",
            //             event->window.windowID, event->window.data1,
            //             event->window.data2);
            //     break;
            // case SDL_WINDOWEVENT_SIZE_t_delta_sD:
            //     SDL_Log("Window %d size changed to %dx%d",
            //             event->window.windowID, event->window.data1,
            //             event->window.data2);
            //     break;
            case SDL_WINDOWEVENT_MINIMIZED:
                window_state.minimized = true;
                // SDL_Log("Window %d minimized", event->window.windowID);
                break;
            // case SDL_WINDOWEVENT_MAXIMIZED:
            //     SDL_Log("Window %d maximized", event->window.windowID);
            //     break;
            case SDL_WINDOWEVENT_RESTORED:
                window_state.minimized = false;
                window_state.restored = true;
                // SDL_Log("Window %d restored", event->window.windowID);
                break;
            // case SDL_WINDOWEVENT_ENTER:
            //     SDL_Log("Mouse entered window %d",
            //             event->window.windowID);
            //     break;
            // case SDL_WINDOWEVENT_LEAVE:
            //     SDL_Log("Mouse left window %d", event->window.windowID);
            //     break;
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                window_state.focused = true;
                // SDL_Log("Window %d gained keyboard focus",
                //         event->window.windowID);
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                window_state.focused = false;
                // SDL_Log("Window %d lost keyboard focus",
                //         event->window.windowID);
                break;
            // case SDL_WINDOWEVENT_CLOSE:
            //     SDL_Log("Window %d closed", event->window.windowID);
            //     break;
    // #if SDL_VERSION_ATLEAST(2, 0, 5)
    //         case SDL_WINDOWEVENT_TAKE_FOCUS:
    //             SDL_Log("Window %d is offered a focus", event->window.windowID);
    //             break;
    //         case SDL_WINDOWEVENT_HIT_TEST:
    //             SDL_Log("Window %d has a special hit test", event->window.windowID);
    //             break;
    // #endif
            default:
                // SDL_Log("Window %d got unknown event %d",
                //         event->window.windowID, event->window.event);
                break;
            }
            break;
        case SDL_CONTROLLERDEVICEADDED:
            if (SDL_IsGameController(event->cdevice.which)) {
                controller_handle = SDL_GameControllerOpen(event->cdevice.which);
                fprintf(
                    stdout, 
                    "ADDING CONTROLLER (%s) TO PORT (%d)\n", 
                    SDL_GameControllerName(controller_handle), event->cdevice.which
                );

                controller_mapping = SDL_GameControllerMapping(controller_handle);
                SDL_Log("CONTROLLER IS MAPPED AS \"%s\".", controller_mapping);
                SDL_free(controller_mapping);
            } else {
                fprintf(stderr, "CONTROLLER INCOMPATIBLE");
            }
            break;
        case SDL_CONTROLLERDEVICEREMOVED:
            if (controller_handle != nullptr) {
                fprintf(
                    stdout, 
                    "REMOVING CONTROLLER (%s) FROM PORT (%d)\n",
                    SDL_GameControllerName(controller_handle), event->cdevice.which
                );
                SDL_GameControllerClose(controller_handle);
                input_sys::init(input);
            }
            break;
        case SDL_CONTROLLERBUTTONDOWN:
            switch (event->cbutton.button) {
            case SDL_CONTROLLER_BUTTON_A:
                key_set_down(input, CONTROL::JUMP);
                break;
            case SDL_CONTROLLER_BUTTON_B:
                std::cout << "DOWN_BUTTON_B" << std::endl;
                break;
            case SDL_CONTROLLER_BUTTON_X:
                std::cout << "DOWN_BUTTON_X" << std::endl;
                break;
            case SDL_CONTROLLER_BUTTON_Y:
                std::cout << "DOWN_BUTTON_Y" << std::endl;
                break;
            case SDL_CONTROLLER_BUTTON_BACK:
                std::cout << "DOWN_BUTTON_BACK" << std::endl;
#ifdef EDITOR
                key_set_down(input, CONTROL::EDIT_MODE);
#endif             
                break;
            case SDL_CONTROLLER_BUTTON_GUIDE:
                std::cout << "DOWN_BUTTON_GUIDE" << std::endl;
                key_set_down(input, CONTROL::RESET_POSITION);
                break;
            case SDL_CONTROLLER_BUTTON_START:
                std::cout << "DOWN_BUTTON_START" << std::endl;
                break;
            case SDL_CONTROLLER_BUTTON_LEFTSTICK:
                std::cout << "DOWN_BUTTON_LEFTSTICK" << std::endl;
                break;
            case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
                std::cout << "DOWN_BUTTON_RIGHTSTICK" << std::endl;
                break;
            case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
                std::cout << "DOWN_BUTTON_LEFTSHOULDER" << std::endl;
                break;
            case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
                std::cout << "DOWN_BUTTON_RIGHTSHOULDER" << std::endl;
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_UP:
                std::cout << "DOWN_BUTTON_DPAD_UP" << std::endl;
                key_set_down(input, CONTROL::UP);
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                std::cout << "DOWN_BUTTON_DPAD_DOWN" << std::endl;
                key_set_down(input, CONTROL::DOWN);
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                std::cout << "DOWN_BUTTON_DPAD_LEFT" << std::endl;
                key_set_down(input, CONTROL::LEFT);
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                std::cout << "DOWN_BUTTON_DPAD_RIGHT" << std::endl;
                key_set_down(input, CONTROL::RIGHT);
                break;
            default:
                //std::cout << "UNKNOWN" << std::endl;
                break;
            }
            break;
        case SDL_CONTROLLERBUTTONUP:
            switch (event->cbutton.button) {
            case SDL_CONTROLLER_BUTTON_A:
                std::cout << "UP_BUTTON_A" << std::endl;
                key_set_up(input, CONTROL::JUMP);
                break;
            case SDL_CONTROLLER_BUTTON_B:
                std::cout << "UP_BUTTON_B" << std::endl;
                break;
            case SDL_CONTROLLER_BUTTON_X:
                std::cout << "UP_BUTTON_X" << std::endl;
                break;
            case SDL_CONTROLLER_BUTTON_Y:
                std::cout << "UP_BUTTON_Y" << std::endl;
                break;

            case SDL_CONTROLLER_BUTTON_BACK:
                std::cout << "UP_BUTTON_BACK" << std::endl;
#ifdef EDITOR
                key_set_up(input, CONTROL::EDIT_MODE);
#endif              
                break;

            case SDL_CONTROLLER_BUTTON_GUIDE:
                std::cout << "UP_BUTTON_GUIDE" << std::endl;
                key_set_up(input, CONTROL::RESET_POSITION);
                break;
            case SDL_CONTROLLER_BUTTON_START:
                std::cout << "UP_BUTTON_START" << std::endl;
                break;
            case SDL_CONTROLLER_BUTTON_LEFTSTICK:
                std::cout << "UP_BUTTON_LEFTSTICK" << std::endl;
                break;
            case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
                std::cout << "UP_BUTTON_RIGHTSTICK" << std::endl;
                break;
            case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
                std::cout << "UP_BUTTON_LEFTSHOULDER" << std::endl;
                break;
            case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
                std::cout << "UP_BUTTON_RIGHTSHOULDER" << std::endl;
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_UP:
                std::cout << "UP_BUTTON_DPAD_UP" << std::endl;
                key_set_up(input, CONTROL::UP);
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                std::cout << "UP_BUTTON_DPAD_DOWN" << std::endl;
                key_set_up(input, CONTROL::DOWN);
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                std::cout << "UP_BUTTON_DPAD_LEFT" << std::endl;
                key_set_up(input, CONTROL::LEFT);
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                std::cout << "UP_BUTTON_DPAD_RIGHT" << std::endl;
                key_set_up(input, CONTROL::RIGHT);
                break;
            default:
                //std::cout << "UNKNOWN" << std::endl;
                break;
            }
            break;
        case SDL_CONTROLLERAXISMOTION:
            #define JOYSTICK_DEADZONE (8000)
            switch (event->caxis.axis) {
            case SDL_CONTROLLER_AXIS_MAX:
                std::cout << "AXIS_MAX " << event->caxis.value << std::endl;
                break;
            case SDL_CONTROLLER_AXIS_LEFTY:
                if (glm::abs(event->caxis.value) > JOYSTICK_DEADZONE) {
                    std::cout << "AXIS_LEFTY " << event->caxis.value << std::endl;
                }
                break;
            case SDL_CONTROLLER_AXIS_RIGHTY:
                if (glm::abs(event->caxis.value) > JOYSTICK_DEADZONE) {
                    std::cout << "AXIS_RIGHTY " << event->caxis.value << std::endl;
                }
                break;
            case SDL_CONTROLLER_AXIS_LEFTX:
                if (glm::abs(event->caxis.value) > JOYSTICK_DEADZONE) {
                    std::cout << "AXIS_LEFTX " << event->caxis.value << std::endl;
                }
                break;
            case SDL_CONTROLLER_AXIS_RIGHTX:
                if (glm::abs(event->caxis.value) > JOYSTICK_DEADZONE) {
                    std::cout << "AXIS_RIGHTX " << event->caxis.value << std::endl;
                }
                break;
            case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
                std::cout << "AXIS_TRIGGERLEFT " << event->caxis.value << std::endl;
                break;
            case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
                std::cout << "AXIS_TRIGGERRIGHT " << event->caxis.value << std::endl;
                break;
            default:
                //std::cout << "UNKNOWN" << std::endl;
                break;
            }
            break;
        case SDL_KEYDOWN:
            switch (event->key.keysym.scancode) {
            case SDL_SCANCODE_W:
                key_set_down(input, CONTROL::UP);
                break;
            case SDL_SCANCODE_S:
                key_set_down(input, CONTROL::DOWN);
                break;
            case SDL_SCANCODE_A:
                key_set_down(input, CONTROL::LEFT);
                break;
            case SDL_SCANCODE_D:
                key_set_down(input, CONTROL::RIGHT);
                break;
            case SDL_SCANCODE_J:
            case SDL_SCANCODE_K:
            case SDL_SCANCODE_L:
                key_set_down(input, CONTROL::JUMP);
                break;            
            case SDL_SCANCODE_0:
                key_set_down(input, CONTROL::RESET_POSITION);
                break;
            case SDL_SCANCODE_C:
                key_set_down(input, CONTROL::FREE_CAM);
                break;                
#ifdef EDITOR
            case SDL_SCANCODE_E:
                key_set_down(input, CONTROL::EDIT_MODE);
                break;
            case SDL_SCANCODE_P:
                key_set_down(input, CONTROL::PHYSICS);
                break;
            case SDL_SCANCODE_V:
                key_set_down(input, CONTROL::EDIT_VERBOSE);
                break;
#endif
            case SDL_SCANCODE_UP:
                key_set_down(input, CONTROL::ZOOM_IN);
                break;
            case SDL_SCANCODE_DOWN:
                key_set_down(input, CONTROL::ZOOM_OUT);
                break;
            default:
                break;
            }
            break;
        case SDL_KEYUP:
            switch (event->key.keysym.scancode) {
            case SDL_SCANCODE_W:
                key_set_up(input, CONTROL::UP);
                break;
            case SDL_SCANCODE_S:
                key_set_up(input, CONTROL::DOWN);
                break;
            case SDL_SCANCODE_A:
                key_set_up(input, CONTROL::LEFT);
                break;
            case SDL_SCANCODE_D:
                key_set_up(input, CONTROL::RIGHT);
                break;
            case SDL_SCANCODE_J:
            case SDL_SCANCODE_K:
            case SDL_SCANCODE_L:
                key_set_up(input, CONTROL::JUMP);
                break; 
            case SDL_SCANCODE_0:
                key_set_up(input, CONTROL::RESET_POSITION);
                break;
            case SDL_SCANCODE_C:
                key_set_up(input, CONTROL::FREE_CAM);
                break; 
#ifdef EDITOR
            case SDL_SCANCODE_E:
                key_set_up(input, CONTROL::EDIT_MODE);
                break;
            case SDL_SCANCODE_P:
                key_set_up(input, CONTROL::PHYSICS);
                break;
            case SDL_SCANCODE_V:
                key_set_up(input, CONTROL::EDIT_VERBOSE);
                break;
#endif
            case SDL_SCANCODE_UP:
                key_set_up(input, CONTROL::ZOOM_IN);
                break;
            case SDL_SCANCODE_DOWN:
                key_set_up(input, CONTROL::ZOOM_OUT);
                break;
            default:
                break;            
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            switch (event->button.button) {
            case SDL_BUTTON_LEFT:
                mouse_set_down(input, MOUSE_BUTTON::LEFT);
                break;
            case SDL_BUTTON_RIGHT:
                mouse_set_down(input, MOUSE_BUTTON::RIGHT);
                break;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            switch (event->button.button) {
            case SDL_BUTTON_LEFT:
                mouse_set_up(input, MOUSE_BUTTON::LEFT);
                break;
            case SDL_BUTTON_RIGHT:
                mouse_set_up(input, MOUSE_BUTTON::RIGHT);
                break;
            }
            break;
        default:
            break;
        }

    }

#ifdef EDITOR
    SDL_GetMouseState(&input->mouse_x, &input->mouse_y);
#endif

    if (window_state.minimized) {
        SDL_Delay(1000);
    } else if (!window_state.focused) {
        SDL_Delay(64);
    }


    return true;
}

template <usize N>
void draw_player_collision(Player* you, GLDraw2D<N>* ctx)
{
    const glm::vec3 off(0.5, 0.5, 0.0);
    BoxComponent* bc = &you->bound;
    glm::vec3 top_left = bc->position() + off;
    glm::vec3 top_right = top_left + glm::vec3(bc->width, 0.0, 0.0);
    glm::vec3 bottom_right = top_left + glm::vec3(bc->width, bc->height, 0.0);
    glm::vec3 bottom_left = top_left + glm::vec3(0.0, bc->height, 0.0);

    ctx->draw_type = GL_LINES;
    ctx->line(top_left, top_right);
    ctx->line(top_right, bottom_right);
    ctx->line(bottom_right, bottom_left);
    ctx->line(bottom_left, top_left);

    ctx->color = Color::RED;
    auto floor_sensor_rays = you->floor_sensor_rays();
    floor_sensor_rays.first.first += off;
    floor_sensor_rays.first.second += off;
    floor_sensor_rays.second.first += off;
    floor_sensor_rays.second.second += off;

    ctx->line(floor_sensor_rays.first.first, floor_sensor_rays.first.second);
    ctx->line(floor_sensor_rays.second.first, floor_sensor_rays.second.second);
}

template <usize N>
void BoxComponent_draw(BoxComponent* bc, GLDraw2D<N>* ctx)
{
    const glm::vec3 off(0.5, 0.5, 0.0);
    glm::vec3 top_left = bc->position() + off;
    glm::vec3 top_right = top_left + glm::vec3(bc->width, 0.0, 0.0);
    glm::vec3 bottom_right = top_left + glm::vec3(bc->width, bc->height, 0.0);
    glm::vec3 bottom_left = top_left + glm::vec3(0.0, bc->height, 0.0);

    ctx->draw_type = GL_LINES;
    ctx->line(top_left, top_right);
    ctx->line(top_right, bottom_right);
    ctx->line(bottom_right, bottom_left);
    ctx->line(bottom_left, top_left);    
}

bool temp_test_collision(Player* you, Collider* c, CollisionStatus* status)
{
    auto sensors = you->floor_sensor_rays();

    std::pair<glm::vec3, glm::vec3>* ray0 = &sensors.first;
    std::pair<glm::vec3, glm::vec3>* ray1 = &sensors.second;
    std::pair<glm::vec3, glm::vec3> collider = {
        c->a,
        c->b
    };


        // printf("COLLIDER: ");
        // Collider_print(c);
        // printf("\nagainst\n");
        // vec3_pair_print(&ray0.first, &ray0.second);
        // printf("\nand\n");
        // vec3_pair_print(&ray1.first, &ray1.second);
        // printf("\n-------------------------\n");

    glm::vec3 va(POSITIVE_INFINITY);
    glm::vec3 vb(POSITIVE_INFINITY);
    glm::vec3* choice = &va;
    bool possibly_collided = false;

    if (line_segment_intersection(ray0, &collider, &va)) {
        choice = &va;
        possibly_collided = true;
    }

    if (line_segment_intersection(ray1, &collider, &vb)) {
        choice = (va.y < vb.y) ? &va : &vb;
        possibly_collided = true;
    }

    if (!possibly_collided) {
        return false;
    }

    glm::vec3* out = &status->intersection;

    // TODO FIX BUG: HEIGHT OVERRIDDEN BY SUCCESSIVE COLLIDERS EVEN IF LOWER,
    // MUST COMPARE ALL COLLIDERS BEFORE MODIFYING VALUE
   // std::cout << "ON_GROUND: " << ((you->on_ground) ? "TRUE" : "FALSE") << std::endl;
    if (!you->on_ground && you->bound.spatial.y + you->bound.height >= choice->y) {
        f64 new_y = choice->y;
        // if (!first_check && new_y >= you->bound.spatial.y) {
        //     return false;
        // }

        //you->bound.spatial.y = new_y;

        if (new_y > out->y) {
            return false;
        }

        out->x = choice->x;
        out->y = choice->y;
        out->z = 0.0;
        status->collider = c;
        
        return true;
    } else if (you->on_ground) {
        f64 new_y = choice->y;
        // if (!first_check && new_y >= you->bound.spatial.y) {
        //     return false;
        // }

        //you->bound.spatial.y = new_y;
        if (new_y > out->y) {
            return false;
        }

        out->x = choice->x;
        out->y = choice->y;
        out->z = 0.0;
        status->collider = c;

        // glm::vec3* a = &c->a;
        // glm::vec3* b = &c->b;
        //std::cout << glm::degrees(atan2pos_64(b->y - a->y, b->x - a->x)) << std::endl;


        //vec3_pair_print(&c->a, &c->b);
        
        return true;        
    }


    return false;

}

int main(int argc, char* argv[])
{
    using namespace input_sys;

    CommandLineArgs cmd;
    if (!parse_command_line_args(&cmd, argc, argv)) {
        return EXIT_FAILURE;
    }

    //std::cout << dist_to_segment(glm::vec3(0.0, 0.0, 0.0), glm::vec3(1280.0, 0.0, 0.0), glm::vec3(0.0, 720.0, 0.0)) << std::endl;
    //return 0;
    // FILE* fp = fopen("worlds/lines_test_a.txt", "r");
    // if (fp) {
    //     fseek(fp, 0, SEEK_END);
    //     i64 file_size = ftell(fp);
    //     ftell(fp);
    //     rewind(fp);
    //     printf("%lld\n", file_size);

    //     fclose(fp);

    //     //return EXIT_SUCCESS;
    // }
    // printf("%llu\n", collision_map.element_length());

    // collision_map[0].a.x = 1.0f;
    // collision_map[0].a.y = 2.0f;
    // collision_map[0].a.z = 3.0f;
    // collision_map[0].b.x = 4.0f;
    // collision_map[0].b.y = 5.0f;
    // collision_map[0].b.z = 6.0f;

    // collision_map.elements_used += 1;

    // Collider_print(&collision_map[0]);

    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC) < 0) {
        fprintf(stderr, "%s\n", "SDL could not initialize");
        return EXIT_FAILURE;
    }
    
    // MOUSE ///////////////////////////////////////////// 
    // hide the cursor
    
    // ignore mouse movement events
    #ifndef EDITOR
    SDL_ShowCursor(SDL_DISABLE);
    SDL_SetEventFilter(ignore_mouse_movement, NULL); ///////////////////////////
    #endif

    // openGL initialization ///////////////////////////////////////////////////
    
    // if (argc >= 3) {
    //     SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, atoi(argv[1]));
    //     SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, atoi(argv[2]));
    // } else {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);        
    // }
   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    // SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
    // SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

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

    window_state.focused = true;
    window_state.minimized = false;

    //SDL_SetWindowFullscreen(window, true);
    
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if(!(IMG_Init(imgFlags) & imgFlags)) {
      fprintf(stderr, "SDL_image could not initialize, SDL_image Error: %s\n", IMG_GetError());
        SDL_DestroyWindow(window);
        return EXIT_FAILURE;
    }
    
   program_data.context = SDL_GL_CreateContext(window);

   glewExperimental = GL_TRUE;
    glewInit();


    bool status = false;
    std::string glsl_perlin_noise = Shader_retrieve_src_from_file("shaders/perlin_noise.glsl", &status);
    if (!status) {
        fprintf(stderr, "ERROR: failed to load shader addon source");
        return false;
    } 

    // SHADERS
    Shader shader_2d;
    if (false == Shader_load_from_file(
        &shader_2d,
        "shaders/parallax/parallax_v2_vrt.glsl",
        "shaders/parallax/parallax_v2_frg.glsl",
        glsl_perlin_noise,
        glsl_perlin_noise
    )) {
        fprintf(stderr, "ERROR: shader_2d\n");
        return EXIT_FAILURE;
    }


    Shader shader_grid;
    if (false == Shader_load_from_file(
        &shader_grid,
        "shaders/default_2d/grid_vrt.glsl",
        "shaders/default_2d/grid_frg.glsl"
    )) {
        fprintf(stderr, "ERROR: shader_grid\n");
        return EXIT_FAILURE;
    }
///////////////

    const GLfloat ASPECT = (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT;

    size_t STRIDE = 5;

// QUADS
    size_t POINTS_PER_QUAD = 4;
    size_t POINTS_PER_TRI = 3;
    size_t TRIS_PER_QUAD = 2;

    GLfloat wf = 1.0f;
    GLfloat hf = 1.0f * (512.0 / 360.0);
    const GLfloat OFF = 0.0f * wf * ASPECT;

    const GLfloat y_off_left = (16.0f / 45.0f);
    const GLfloat x_off_right = (512.0f / 640.0f);

    glm::vec2 tex_res(2048.0f, 1024.0f);

    glm::vec3 world_bguv_factor = glm::vec3(glm::vec2(1.0f) / tex_res, 1.0f);

    GLuint layers_per_row = (GLuint)(tex_res.x / SCREEN_WIDTH);
    // GLfloat x_off = (GLfloat)(GLdouble)(SCREEN_WIDTH / tex_res.x);
    // GLfloat y_off = (GLfloat)(GLdouble)(SCREEN_HEIGHT / tex_res.x);

    GLfloat x_ratio = SCREEN_WIDTH / tex_res.x;
    GLfloat y_ratio = SCREEN_HEIGHT / tex_res.y;

    GLfloat X_OFF = (tex_res.x - SCREEN_WIDTH) / 2.0f;
    GLfloat Y_OFF = (tex_res.y - SCREEN_HEIGHT) / 2.0f;

    GLfloat T[] = {
       0.0f - X_OFF,      0.0f - Y_OFF,      0.0f,    0.0f, 0.0f,    // top left
       0.0f - X_OFF,      tex_res.y - Y_OFF, 0.0f,    0.0f, 1.0f,    // bottom left
       tex_res.x - X_OFF, tex_res.y - Y_OFF, 0.0f,    1.0f, 1.0f,    // bottom right
       tex_res.x - X_OFF, 0.0f - Y_OFF,      0.0f,    1.0f, 0.0f,    // top right
    };

    GLuint TI[] = {
        0, 1, 2,
        2, 3, 0,
    };


    //print_array(T, 4, 6);

// TOTAL ALLOCATION
    // const size_t BATCH_COUNT = 1024;
    // const size_t GUESS_VERTS_PER_DRAW = 4;

//////////////////////////////////////////////////

    VertexAttributeArray vao_2d2;
    VertexBufferData tri_data;

    VertexAttributeArray_init(&vao_2d2, STRIDE);

    glBindVertexArray(vao_2d2.vao);

        VertexBufferData_init_inplace(
            &tri_data, 
            StaticArrayCount(T),
            T,
            StaticArrayCount(TI),
            TI
        );


        gl_bind_buffers_and_upload_data(&tri_data, GL_STATIC_DRAW);
        // POSITION
        gl_set_and_enable_vertex_attrib_ptr(0, 3, GL_FLOAT, GL_FALSE, 0, &vao_2d2);
        // UV
        gl_set_and_enable_vertex_attrib_ptr(1, 2, GL_FLOAT, GL_FALSE, 3, &vao_2d2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    // GLData grid;
    // GLData_init_inplace(&grid, STRIDE, StaticArrayCount(T) / 15, T, StaticArrayCount(TI) / 15, TI);
    // glBindVertexArray(grid.vao);

    //     gl_bind_buffers_and_upload_data(&grid.vbd, GL_STATIC_DRAW, grid.vbd.v_cap, grid.vbd.i_cap, StaticArrayCount(T) / 15, 0);

    //     // POSITION
    //     gl_set_and_enable_vertex_attrib_ptr(0, 3, GL_FLOAT, GL_FALSE, 0, &grid.vao);
    //     // UV
    //     gl_set_and_enable_vertex_attrib_ptr(1, 2, GL_FLOAT, GL_FALSE, 3, &grid.vao);

    //     glBindBuffer(GL_ARRAY_BUFFER, 0);

    // glBindVertexArray(0);

///////////////////////////////////////////////////////////////////////////////////////////////////////////

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    glEnable(GL_MULTISAMPLE);
   
   // glEnable(GL_DEPTH_TEST);
 //    glDepthRange(0, 1);
 //    glDepthFunc(GL_LEQUAL);


    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ONE);
    //glBlendEquationSeparate(GL_FUNC_ADD, GL_MAX);

    #ifdef SDL_H
    {
        SDL_version compiled;
        SDL_version linked;

        SDL_VERSION(&compiled);
        SDL_GetVersion(&linked);
        
        printf(
            "COMPILED AGAINST SDL VERSION %d.%d.%d.\n",
            compiled.major, compiled.minor, compiled.patch
        );
        printf(
            "LINKED AGAINST SDL VERSION %d.%d.%d.\n",
            linked.major, linked.minor, linked.patch
        );
    }
    #endif

    printf("USING GL VERSION: %s\n", glGetString(GL_VERSION));

    glm::mat4 mat_ident(1.0f);
    glm::mat4 mat_projection = glm::ortho(
        0.0f, 
        1.0f * SCREEN_WIDTH, 
        1.0f * SCREEN_HEIGHT,
        0.0f,
        0.0f, 
        1.0f * 10.0f
    );
    //glm::mat4 mat_projection = glm::perspective(glm::radians(45.0f), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

//////////////////
// TEST INPUT
    glm::vec3 start_pos(0.0f, 0.0f, 1.0f);
    
    FreeCamera main_cam(start_pos);
    main_cam.orientation = glm::quat();
    main_cam.speed = PLAYER_BASE_SPEED;
    main_cam.offset = glm::vec2(SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2.0);
    main_cam.target = glm::vec2(0);
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


    const f64 POS_ACC = 1.08;
    const f64 NEG_ACC = 1.0 / POS_ACC;

    // double up_acc = 110.0;
    // double down_acc = 110.0;
    // double left_acc = 110.0;
    // double right_acc = 110.0;
    // double forwards_acc = 110.0;
    // double backwards_acc = 110.0;

    double up_acc = 1.0;
    double down_acc = 1.0;
    double left_acc = 1.0;
    double right_acc = 1.0;
    double forwards_acc = 1.0;
    double backwards_acc = 1.0;

    double max_acc = 1000.0;


/////////////////
// MAIN LOOP
#ifdef DEBUG_PRINT
    glm::vec3 prev_pos(0.0);
#endif

    // Texture tex0;
    // if (GL_texture_gen_and_load_1(&tex0, "./textures/bg_test_3_3.png", GL_TRUE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE) == GL_FALSE) {
    //     return EXIT_FAILURE;
    // }


    Texture bgs[5];
    foreach(i, 5) {
        if (GL_FALSE == GL_texture_gen_and_load_1(
                &bgs[i], ("./textures/separate_test_2/" + std::to_string(i) + ".png").c_str(), 
                GL_TRUE, GL_REPEAT, GL_CLAMP_TO_EDGE
        )) {
            return EXIT_FAILURE;
        }
    }

    gl_get_errors();


    glUseProgram(shader_2d);

    //UniformLocation RES_LOC = glGetUniformLocation(shader_2d, "u_resolution");
    //UniformLocation COUNT_LAYERS_LOC = glGetUniformLocation(shader_2d, "u_count_layers");
    UniformLocation MAT_LOC = glGetUniformLocation(shader_2d, "u_matrix");
    //UniformLocation TIME_LOC = glGetUniformLocation(shader_2d, "u_time");
    UniformLocation CAM_LOC = glGetUniformLocation(shader_2d, "u_position_cam");
    //UniformLocation ASPECT_LOC = glGetUniformLocation(shader_2d, "u_aspect");

    const GLuint UVAL_COUNT_LAYERS = 5;

    //glUniform2fv(RES_LOC, 1, glm::value_ptr(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT)));
    //glUniform1i(COUNT_LAYERS_LOC, UVAL_COUNT_LAYERS);
    //glUniform1f(ASPECT_LOC, (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT);
    // TEXTURE 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bgs[0]);
    glUniform1i(glGetUniformLocation(shader_2d, "tex0"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bgs[1]);
    glUniform1i(glGetUniformLocation(shader_2d, "tex1"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, bgs[2]);
    glUniform1i(glGetUniformLocation(shader_2d, "tex2"), 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, bgs[3]);
    glUniform1i(glGetUniformLocation(shader_2d, "tex3"), 3);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, bgs[4]);
    glUniform1i(glGetUniformLocation(shader_2d, "tex4"), 4);


    gl_get_errors();


    #ifdef GL_DRAW2D
    GLDraw2D<> gl_draw2d;
    if (!gl_draw2d.init(mat_projection)) {
        return EXIT_FAILURE;
    }
    #endif

    Toggle free_cam_toggle = false;

    #ifdef EDITOR
    glUseProgram(shader_grid);

    UniformLocation COLOR_LOC_GRID = glGetUniformLocation(shader_grid, "u_color");
    glUniform4fv(COLOR_LOC_GRID, 1, glm::value_ptr(glm::vec4(0.25f, 0.25f, 0.25f, 0.5f)));

    UniformLocation SQUARE_PIXEL_LOC_GRID = glGetUniformLocation(shader_grid, "u_grid_square_pix");

    GLfloat grid_square_pixel_size = 16.0f;
    glUniform1f(SQUARE_PIXEL_LOC_GRID, tex_res.x / grid_square_pixel_size);

    UniformLocation MAT_LOC_GRID = glGetUniformLocation(shader_grid, "u_matrix");

    UniformLocation CAM_LOC_GRID = glGetUniformLocation(shader_grid, "u_position_cam");


    GLDraw2D<> existing;
    GLDraw2D<256> in_prog;
    Toggle drawing = false;
    Toggle deletion = false;

    if (!existing.init(mat_projection)) {
        fprintf(stderr, "FAILED TO INITIALIZE EDITOR DATA \"existing\"\n");
        return EXIT_FAILURE;
    }
    if (!in_prog.init(mat_projection)) {
        fprintf(stderr, "FAILED TO INITIALIZE EDITOR DATA \"in_prog\"\n");
        return EXIT_FAILURE;
    }
    #endif


    size_t display_mode_count = 0;
    SDL_DisplayMode mode;

    if (SDL_GetDisplayMode(0, 0, &mode) != 0) {
        SDL_Log("SDL_GetDisplayMode failed: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    printf("REFRESH_RATE: %d\n", mode.refresh_rate);


    SDL_GL_SetSwapInterval(1);
    const f64 INTERVAL = MS_PER_S / mode.refresh_rate;
    const f64 REFRESH_RATE = mode.refresh_rate;

    f64 frequency  = SDL_GetPerformanceFrequency();

    u64 t_now      = SDL_GetPerformanceCounter();
    u64 t_prev     = 0.0;
    u64 t_start    = t_now;
    u64 t_delta    = 0;
    
    f64 t_now_s         = (f64)t_now / frequency;
    f64 t_prev_s        = 0.0;
    f64 t_since_start_s = 0.0;
    f64 t_delta_s       = 0.0;

    #ifdef FPS_COUNT
    f64 frame_time = 0.0;
    u32 frame_count = 0;
    u32 fps = 0;
    #endif


    input_sys::Input input = {};
    input_sys::init(&input);

    #define CONTROLLER_MAPPING_FILE "./mapping/gamecontrollerdb.txt"
    if (SDL_GameControllerAddMappingsFromFile(CONTROLLER_MAPPING_FILE) < 0) {
        fprintf(stderr, "FAILED TO LOAD CONTROLLER MAPPINGS FROM %s\n", CONTROLLER_MAPPING_FILE);
    }

    // for (int joystick_index = 0; joystick_index < max_joysticks; ++joystick_index) {
    //     if (!SDL_IsGameController(joystick_index)) {
    //         std::cout << "NOT GAME CONTROLLER" << std::endl;
    //         continue;
    //     }
    //     if (controller_index >= MAX_CONTROLLERS) {
    //         break;
    //     }

    //     controller_handles[controller_index] = SDL_GameControllerOpen(joystick_index);
        
    //     fprintf(stdout, "CONTROLLER: %s\n", SDL_GameControllerName(controller_handles[controller_index]));
    // }

    bool is_running = true;
    SDL_Event event;

#ifdef EDITOR
    Toggle grid_toggle = false;
    Toggle physics_toggle = false;
    Toggle verbose_view_toggle = false;
    glm::vec3 in_progress_line[2];
    in_progress_line[0] = glm::vec3(0.0f);
    in_progress_line[1] = glm::vec3(0.0f);


////
    collision_map.first_free()->a = glm::vec3(0.0, 5 * 128, 0.0);
    collision_map.first_free()->b = glm::vec3(SCREEN_WIDTH, 5 * 128, 0.0);
    collision_map.elements_used += 1;

    collision_map.first_free()->a = glm::vec3(512.0, 3 * 128, 0.0);
    collision_map.first_free()->b = glm::vec3(768.0, 3 * 128, 0.0);
    collision_map.elements_used += 1;

    existing.update_projection_matrix = true;
    existing.projection_matrix = mat_projection;
    existing.begin();
    existing.draw_type = GL_LINES;
    existing.color = Color::BLACK;
    existing.transform_matrix = glm::mat4(1.0);
    
    foreach (i, collision_map.elements_used) {
        existing.line(collision_map[i].a, collision_map[i].b);        
    }
    existing.end_no_reset();
////
#endif

    Player you;
    Player_init(&you, SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2.0, 0.0, true, 0, 20, 40);
    you.state_change_time = t_now;


    // f64 X[8] = {
    //     glm::degrees(atan2pos_64(0.0, 1.0)),
    //     glm::degrees(atan2pos_64(0.5, 0.5)),
    //     glm::degrees(atan2pos_64(1.0, 0.0)),
    //     glm::degrees(atan2pos_64(0.5, -0.5)),
    //     glm::degrees(atan2pos_64(0.0, -1.0)),
    //     glm::degrees(atan2pos_64(-0.5, -0.5)),
    //     glm::degrees(atan2pos_64(-1.0, 0.0)),
    //     glm::degrees(atan2pos_64(-0.5, 0.5))
    // };
    // print_array(X, 8);
    // return 0;


    // glm::vec2 a(0, 0);
    // glm::vec2 b(1, 1);
    // std::cout << glm::degrees(atan2pos_64(b.y - a.y, b.x - a.x)) << std::endl;

    while (is_running) {
        t_prev = t_now;
        t_prev_s = t_now_s;

        t_now = SDL_GetPerformanceCounter();
        t_now_s = (f64)t_now / frequency;

        t_delta = (t_now - t_prev);
        t_delta_s = (f64)t_delta / frequency;

        f64 t_since_start_s = ((f64)(t_now - t_start)) / frequency;

        // INPUT /////////////////////////////////
        if (!poll_input_events(&input, &event)) {
            is_running = false;
            continue;
        } else if (window_state.minimized) {
            continue;
        } else if (window_state.restored) {
            window_state.restored = false;
            SDL_GL_SwapWindow(window);
            continue;
        }


        bool free_cam_is_on = key_is_toggled(&input, CONTROL::FREE_CAM, &free_cam_toggle);
        bool camera_locked = key_is_held(&input, CONTROL::UP);

        if (free_cam_is_on) {
            main_cam.is_catching_up = true;
        } else if (camera_locked) {
            main_cam.is_catching_up = true;
        }


        {

            main_cam.orientation = glm::quat();

            if (free_cam_is_on) {
                if (key_is_held(&input, CONTROL::UP)) {
                    FreeCamera_process_directional_movement(&main_cam, MOVEMENT_DIRECTION::UPWARDS, t_delta_s * up_acc);
                    up_acc *= POS_ACC;
                    up_acc = glm::min(max_acc, up_acc);
                } else {
                    if (up_acc > 1.0) {
                        FreeCamera_process_directional_movement(&main_cam, MOVEMENT_DIRECTION::UPWARDS, t_delta_s * up_acc);
                    }
                    up_acc = glm::max(1.0, up_acc * NEG_ACC);
                }
                if (key_is_held(&input, CONTROL::DOWN)) {
                    FreeCamera_process_directional_movement(&main_cam, MOVEMENT_DIRECTION::DOWNWARDS, t_delta_s * down_acc);
                    down_acc *= POS_ACC;
                    down_acc = glm::min(max_acc, down_acc);
                } else {
                    if (down_acc > 1.0) {
                        FreeCamera_process_directional_movement(&main_cam, MOVEMENT_DIRECTION::DOWNWARDS, t_delta_s * down_acc);
                    } 
                    down_acc = glm::max(1.0, down_acc * NEG_ACC);
                }
            }

            if (key_is_held(&input, CONTROL::LEFT)) {
                if (free_cam_is_on) {
                    FreeCamera_process_directional_movement(&main_cam, MOVEMENT_DIRECTION::LEFTWARDS, t_delta_s * left_acc);
                } else {
                // TEMP
                    Player_move_test(&you, MOVEMENT_DIRECTION::LEFTWARDS, t_delta_s * left_acc);
                }

                left_acc *= POS_ACC;
                left_acc = glm::min(max_acc, left_acc);
            
            } else {
                if (left_acc > 1.0) {
                    if (free_cam_is_on) {
                        FreeCamera_process_directional_movement(&main_cam, MOVEMENT_DIRECTION::LEFTWARDS, t_delta_s * left_acc);
                    } else {
                    // TEMP
                        Player_move_test(&you, MOVEMENT_DIRECTION::LEFTWARDS, t_delta_s * left_acc);
                    }
                }
                left_acc = glm::max(1.0, left_acc * NEG_ACC);
                left_acc = glm::min(max_acc, left_acc);
            }

            if (key_is_held(&input, CONTROL::RIGHT)) {
                if (free_cam_is_on) {
                    FreeCamera_process_directional_movement(&main_cam, MOVEMENT_DIRECTION::RIGHTWARDS, t_delta_s * right_acc);
                } else {
                    // TEMP
                    Player_move_test(&you, MOVEMENT_DIRECTION::RIGHTWARDS, t_delta_s * right_acc);
                }

                right_acc *= POS_ACC;
                right_acc = glm::min(max_acc, right_acc);

            } else {
                if (right_acc > 1.0) {
                    if (free_cam_is_on) {
                        FreeCamera_process_directional_movement(&main_cam, MOVEMENT_DIRECTION::RIGHTWARDS, t_delta_s * right_acc);
                    } else {
                        // TEMP
                        Player_move_test(&you, MOVEMENT_DIRECTION::RIGHTWARDS, t_delta_s * right_acc);
                    }
                }
                right_acc = glm::max(1.0, right_acc * NEG_ACC);
            }

            // if (*forwards) {
            //     FreeCamera_process_directional_movement(&main_cam, MOVEMENT_DIRECTION::FORWARDS, t_delta_s * forwards_acc);
            //     forwards_acc *= POS_ACC;
            //     forwards_acc = glm::min(max_acc, forwards_acc);
            // } else {
            //     if (forwards_acc > 1.0) {
            //         FreeCamera_process_directional_movement(&main_cam, MOVEMENT_DIRECTION::FORWARDS, t_delta_s * forwards_acc);
            //     }
            //     forwards_acc = glm::max(1.0, forwards_acc * NEG_ACC);
            // }
            // if (*backwards) {
            //     FreeCamera_process_directional_movement(&main_cam, MOVEMENT_DIRECTION::BACKWARDS, t_delta_s * backwards_acc);
            //     backwards_acc *= POS_ACC;
            //     backwards_acc = glm::min(max_acc, backwards_acc);
            // } else {
            //     if (backwards_acc > 1.0) {
            //         FreeCamera_process_directional_movement(&main_cam, MOVEMENT_DIRECTION::BACKWARDS, t_delta_s * backwards_acc);
            //     } 
            //     backwards_acc = glm::max(1.0, backwards_acc * NEG_ACC);  
            // }
            if (key_is_pressed(&input, CONTROL::RESET_POSITION)) {
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
                main_cam.is_catching_up = false;

                up_acc        = 1.0;
                down_acc      = 1.0;
                left_acc      = 1.0;
                right_acc     = 1.0;
                backwards_acc = 1.0;
                forwards_acc  = 1.0;

                Player_init(&you, SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2.0, 0.0, true, 0, 20, 40);
                you.state_change_time = t_now;
                you.on_ground = false;
            }

            if (!free_cam_is_on && 
                !(camera_locked)) {

                FreeCamera_target_set(&main_cam, you.bound.calc_position_center());
                FreeCamera_target_follow(&main_cam, t_delta_s);
                up_acc        = 1.0;
                down_acc      = 1.0;
            }
        }



        //main_cam.rotate((GLfloat)curr_time / MS_PER_S, 0.0f, 0.0f, 1.0f);
    //////////////////
    // DRAW

                
        glClearColor(97.0 / 255.0, 201.0 / 255.0, 255.0 / 255.0, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_2d);


        //glUniformMatrix4fv(MAT_LOC, 1, GL_FALSE, glm::value_ptr(ViewCamera_calc_view_matrix(&main_cam) * mat_ident));
        glUniformMatrix4fv(MAT_LOC, 1, GL_FALSE, glm::value_ptr(
            mat_projection
            /**FreeCamera_calc_view_matrix(&main_cam)*/
            /*glm::translate(mat_ident, glm::vec3(glm::sin(((double)t_now / frequency)), 0.0, 0.0)) * */
            /*glm::scale(mat_ident, glm::vec3(0.25, 0.25, 1.0))* */
                        )
        );

        //glUniform1f(TIME_LOC, t_since_start);

        glm::vec3 pos = main_cam.position;
        #ifdef DEBUG_PRINT

            if (pos.x != prev_pos.x || pos.y != prev_pos.y || pos.z != prev_pos.z) {
                std::cout << "VIEW_POSITION{x : " << pos.x << ", y : " << pos.y << ", z: " << pos.z << "}" << std::endl;
            }
            prev_pos.x = pos.x;
            prev_pos.y = pos.y;
            prev_pos.z = pos.z;
        #endif

        glUniform3fv(CAM_LOC, 1, glm::value_ptr(pos * world_bguv_factor));
        glEnable(GL_DEPTH_TEST);
        //glClear(GL_DEPTH_BUFFER_BIT);
        // glDepthRange(0, 1);
        
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glDisable(GL_DEPTH_TEST);
        
        glBindVertexArray(vao_2d2.vao);
        glDrawElements(GL_TRIANGLES, tri_data.i_count, GL_UNSIGNED_INT, 0);
        //glBindVertexArray(0);

        #ifdef GL_DRAW2D

        glEnable(GL_DEPTH_TEST);
        glDepthRange(0, 1);
        glClear(GL_DEPTH_BUFFER_BIT);


        glm::mat4 cam = FreeCamera_calc_view_matrix(&main_cam);

        // gl_draw2d.begin();

        //     //gl_draw2d.transform_matrix = FreeCamera_calc_view_matrix(&main_cam);
        //     gl_draw2d.transform_matrix = glm::mat4(1.0f);

        //     // gl_draw2d.draw_type = GL_LINES;
        //     // gl_draw2d.color = Color::RED;
        //     // gl_draw2d.vertex({0.5, 0.0, -1.0});
        //     // gl_draw2d.vertex({1.0, 1.0, -1.0});
            
        //     // gl_draw2d.draw_type = GL_LINES;
        //     // gl_draw2d.color = Color::GREEN;
        //     // gl_draw2d.line({0.0, 0.0, -5.0}, {1.0, 1.0, -5.0});

        //     // gl_draw2d.color = Color::GREEN;
        //     // gl_draw2d.circle(0.25, {0.0, 0.0, 0.0});

        //     gl_draw2d.draw_type = GL_TRIANGLES;

        //     GLfloat CX = (SCREEN_WIDTH / 2.0f);
        //     GLfloat CY = 384.0f;

        //     gl_draw2d.color = glm::vec4(252.0f / 255.0f, 212.0f / 255.0f, 64.0f / 255.0f, 1.0f);

        //     gl_draw2d.transform_matrix = cam;

        //     gl_draw2d.circle(90.0f, {CX, CY, -1.0});

        //     gl_draw2d.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        //     gl_draw2d.transform_matrix = glm::translate(cam, glm::vec3(CX - 27.0f, CY - 25.0f, 0.0f));
        //     gl_draw2d.circle(10.0f, {0.0f, 0.0f, 0.0f});

        //     gl_draw2d.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        //     gl_draw2d.transform_matrix = glm::translate(cam, glm::vec3(CX + 27.0f, CY - 25.0f, 0.0f));
        //     gl_draw2d.circle(10.0f, {0.0f, 0.0f, 0.0f});

            
        //     gl_draw2d.draw_type = GL_LINES;

        //     #define BASE_TILE_SIZE (128.0f)
        //     #define TILE_SCALE (2.0f)

        //     CX = 5.0f / TILE_SCALE;
        //     CY = 3.0f / TILE_SCALE;
            
        //     glm::mat4 model(1.0f);
        //     model = glm::scale(model, glm::vec3(BASE_TILE_SIZE * TILE_SCALE, BASE_TILE_SIZE * TILE_SCALE, 1.0f));
        //     model = glm::translate(model, glm::vec3({CX, CY, 0.0}));
        //     model = glm::rotate(model, (GLfloat)t_since_start, glm::vec3(0.0f, 0.0f, 1.0f));
        //     model = glm::translate(model, glm::vec3({-CX, -CY, 0.0}));
            

        //     gl_draw2d.transform_matrix = cam * model;

        //     gl_draw2d.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        //     {
        //         GLfloat off = 1.0f;
        //         // horizontal
        //         gl_draw2d.line(glm::vec3(CX - off, CY - off, 0.0f), glm::vec3(CX + off, CY - off, 0.0f));
        //         gl_draw2d.line(glm::vec3(CX - off, CY + off, 0.0f), glm::vec3(CX + off, CY + off, 0.0f));
        //         // vertical
        //         gl_draw2d.line(glm::vec3(CX - off, CY - off, 0.0f), glm::vec3(CX - off, CY + off, 0.0f));
        //         gl_draw2d.line(glm::vec3(CX + off, CY - off, 0.0f), glm::vec3(CX + off, CY + off, 0.0f));
        //     }   



        // gl_draw2d.end();

        #endif


        #ifdef EDITOR

        if (key_is_toggled(&input, CONTROL::EDIT_MODE, &grid_toggle)) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glClear(GL_DEPTH_BUFFER_BIT);


            glUseProgram(shader_grid);



            if (key_is_pressed(&input, CONTROL::ZOOM_IN)) {
                grid_square_pixel_size *= 2;
                grid_square_pixel_size = glm::clamp(grid_square_pixel_size, 4.0f, 64.0f);

                glUniform1f(SQUARE_PIXEL_LOC_GRID, tex_res.x / grid_square_pixel_size);
            } else if (key_is_pressed(&input, CONTROL::ZOOM_OUT)) {
                grid_square_pixel_size /= 2;
                grid_square_pixel_size = glm::clamp(grid_square_pixel_size, 4.0f, 64.0f);

                glUniform1f(SQUARE_PIXEL_LOC_GRID, tex_res.x / grid_square_pixel_size);
            }


            glUniformMatrix4fv(MAT_LOC_GRID, 1, GL_FALSE, glm::value_ptr(
                    mat_projection
                )
            );

            glUniform3fv(CAM_LOC_GRID, 1, glm::value_ptr(pos));


            glBindVertexArray(vao_2d2.vao);
            glDrawElements(GL_TRIANGLES, tri_data.i_count, GL_UNSIGNED_INT, 0);


            glm::mat4 rev_view = FreeCamera_calc_view_matrix_reverse(&main_cam);

            glm::vec4 mouse = glm::vec4((int)input.mouse_x, (int)input.mouse_y, 0.0f, 1.0f);

            mouse = rev_view * mouse;

            //printf("CURSOR: [x: %f, y: %f]\n", mouse.x, mouse.y);

            // if (mouse_is_toggled(&input, MOUSE_BUTTON::LEFT, &drawing)) {
            //     printf("TOGGLED DRAWING ON\n");
            // } else if (mouse_is_pressed(&input, MOUSE_BUTTON::LEFT) && !drawing) {
            //     printf("ENDING DRAWING\n");
            // }
            // if (drawing) {
            //     printf("DRAWING\n");
            // }
            glClear(GL_DEPTH_BUFFER_BIT);

            i32 grid_len = tex_res.x / grid_square_pixel_size;

            //printf("CURSOR_SNAPPED: [x: %d, y: %d]\n", snap_to_grid(mouse.x, grid_len), snap_to_grid(mouse.y, grid_len));

            if (mouse_is_toggled(&input, MOUSE_BUTTON::RIGHT, &deletion)) {
                drawing = false;
                if (mouse_is_pressed(&input, MOUSE_BUTTON::LEFT)) {
                    in_prog.begin();

                    {
                        in_prog.draw_type = GL_TRIANGLES;
                        in_prog.transform_matrix = cam;
                        in_prog.color = Color::RED;
                        in_prog.circle(
                            10.0f,
                            glm::vec3(
                                mouse.x, 
                                mouse.y,
                                1.0f
                            )
                        );
                    }

                    in_prog.end();
                } else {
                    in_prog.begin();

                    {
                        in_prog.draw_type = GL_TRIANGLES;
                        in_prog.transform_matrix = cam;
                        in_prog.color = Color::RED;
                        in_prog.circle(
                            5.0f,
                            glm::vec3(
                                mouse.x, 
                                mouse.y,
                                1.0f
                            )
                        );
                    }

                    in_prog.end();                    
                }


                existing.update_projection_matrix = true;
                existing.projection_matrix = mat_projection * cam;
                existing.begin();
                existing.draw_type = GL_LINES;
                //existing.transform_matrix = cam;
                existing.end_no_reset();

            } else {
                switch (mouse_is_toggled_4_states(&input, MOUSE_BUTTON::LEFT, &drawing)) {
                case TOGGLE_BRANCH::PRESSED_ON:
                    //printf("TOGGLED DRAWING ON\n");
                    in_progress_line[0].x = snap_to_grid(mouse.x, grid_len);
                    in_progress_line[0].y = snap_to_grid(mouse.y, grid_len);
                    in_progress_line[0].z = mouse.z;

                    collision_map.first_free()->a = in_progress_line[0];
                    collision_map.first_free()->a.z = 0;
                case TOGGLE_BRANCH::ON:
                    //printf("\tDRAWING\n");
                    in_progress_line[1].x = snap_to_grid(mouse.x, grid_len);
                    in_progress_line[1].y = snap_to_grid(mouse.y, grid_len);
                    in_progress_line[1].z = mouse.z;

                    collision_map.first_free()->b = in_progress_line[1];
                    collision_map.first_free()->b.z = 0;

                    in_prog.begin();
                    in_prog.draw_type = GL_LINES;
                    in_prog.transform_matrix = cam;
                    in_prog.color = Color::BLACK;
                    in_prog.line(in_progress_line[0], in_progress_line[1]);

                    {
                        in_prog.draw_type = GL_TRIANGLES;
                        in_prog.transform_matrix = cam;
                        in_prog.color = Color::BLUE;
                        in_prog.circle(
                            5.0f,
                            glm::vec3(
                                snap_to_grid(mouse.x, grid_len), 
                                snap_to_grid(mouse.y, grid_len),
                                1.0f
                            )
                        );
                    }

                    in_prog.end();

                    existing.update_projection_matrix = true;
                    existing.projection_matrix = mat_projection * cam;
                    existing.begin();
                    existing.draw_type = GL_LINES;
                    //existing.transform_matrix = cam;
                    existing.end_no_reset();

                    break;
                case TOGGLE_BRANCH::PRESSED_OFF:

                    //printf("ENDING DRAWING\n");

                    collision_map.elements_used += 1;

                    in_prog.begin();
                    {
                        in_prog.draw_type = GL_TRIANGLES;
                        in_prog.transform_matrix = cam;
                        in_prog.color = Color::GREEN;
                        in_prog.circle(
                            10.0f,
                            glm::vec3(
                                snap_to_grid(mouse.x, grid_len), 
                                snap_to_grid(mouse.y, grid_len),
                                1.0f
                            )
                        );
                    }
                    in_prog.end();

                    existing.update_projection_matrix = true;
                    existing.projection_matrix = mat_projection * cam;
                    existing.begin();
                    existing.draw_type = GL_LINES;
                    //existing.transform_matrix = cam;

                    //sort_segment(in_progress_line);
                    
                    existing.line(in_progress_line[0], in_progress_line[1]);
                    
                    // {
                    //     f64 dy = in_progress_line[1].y - in_progress_line[0].y;
                    //     f64 dx = in_progress_line[1].x - in_progress_line[0].x;
                    //     existing.color = Color::BLUE;

                    //     glm::vec3 na(-dy, dx, 0.0);
                    //     glm::vec3 nb(dy, -dx, 0.0);

                    //     //na = glm::normalize(na);
                    //     //nb = glm::normalize(nb);

                    //     existing.line(na, nb);

                        
                    //     existing.color = Color::BLACK;
                    //     vec3_pair_print(&na, &nb);


                    // }

                    existing.end_no_reset();
                    break;
                case TOGGLE_BRANCH::OFF:
                    in_prog.begin();
                    {
                        in_prog.draw_type = GL_TRIANGLES;
                        in_prog.transform_matrix = cam;
                        in_prog.color = Color::BLUE;
                        in_prog.circle(
                            5.0f,
                            glm::vec3(
                                snap_to_grid(mouse.x, grid_len), 
                                snap_to_grid(mouse.y, grid_len),
                                1.0f
                            )
                        );
                    }
                    in_prog.end();

                    existing.update_projection_matrix = true;
                    existing.projection_matrix = mat_projection * cam;
                    existing.begin();
                    existing.draw_type = GL_LINES;
                    //existing.transform_matrix = cam;
                    existing.end_no_reset();
                    break;
                default:
                    break;
                }
            }

            gl_draw2d.begin();

            glClear(GL_DEPTH_BUFFER_BIT);

            gl_draw2d.draw_type = GL_LINES;

            gl_draw2d.color = Color::BLUE;

            gl_draw2d.transform_matrix = cam;

            //draw_lines_from_image(&gl_draw2d, "./test_paths/C.bmp", {glm::vec3(1.0f), glm::vec3(0.0f)});


            f64 WEE = ((f64)(t_now - you.state_change_time)) / frequency;

            // if (/*key_is_toggled(&input, CONTROL::PHYSICS, &physics_toggle) && */!you.on_ground) {
            //     you.bound.spatial.y = you.bound.spatial.y + 1 * 9.81 * (WEE * WEE);
            // } else if (key_is_pressed(&input, CONTROL::JUMP)) { // TEMPORARY
            //     you.bound.spatial.y -= you.bound.height * 4;
            // }

            
            gl_draw2d.end();

            gl_draw2d.begin();

            glClear(GL_DEPTH_BUFFER_BIT);

            gl_draw2d.color = Color::GREEN;
            gl_draw2d.transform_matrix = cam;

            //std::cout << (GRAVITY_DEFAULT * t_delta_s * INTERVAL) << std::endl;
            //std::cout << t_delta_s * INTERVAL << std::endl;
            if (!you.on_ground) {

                you.velocity_air += (GRAVITY_DEFAULT * DELTA_TIME_FACTOR(t_delta_s, REFRESH_RATE));

                //std::cout << "MULTIPLIER V1 " << (INTERVAL / t_delta_s) / 1000 << std::endl;
                //std::cout << "MULTIPLIER V2 " << (1 / (t_delta_s * REFRESH_RATE)) << std::endl;
                if (!key_is_held(&input, CONTROL::JUMP)) {
                    if (you.velocity_air.y < you.initial_jump_velocity_short) {
                        you.velocity_air.y = you.initial_jump_velocity_short;
                    }
                }
                you.bound.spatial.y += you.velocity_air.y;
            }


            bool collided = false;

            CollisionStatus status;
            CollisionStatus_init(&status);
            for (auto it = collision_map.begin(); it != collision_map.first_free(); ++it)
            {
                //Collider_print(it);
                
                if (temp_test_collision(&you, it, &status)) {
                    //printf("COLLISION\n");
                    gl_draw2d.line(glm::vec3(0.0), status.intersection);
                    you.on_ground = true;
                    you.state_change_time = t_now;
                    collided = true;

                    //puts("COLLIDED");
                } else {
                    //puts("NO COLLISION");
                }
            }

            if (!collided) {
                you.on_ground = false;
            } else {

                if (you.on_ground) {
                    if (key_is_pressed(&input, CONTROL::JUMP)) {
                        you.velocity_air.y = you.initial_jump_velocity;
                        you.on_ground = false;
                    }
                }
                //you.bound.spatial.x = out.x - (1 * you.bound.width); <-- ENABLE TO MAKE THE FLOOR A TREADMILL
                you.bound.spatial.y = status.intersection.y - (1 * you.bound.height);

                Collider* col = status.collider;
                glm::vec3* a = &col->a;
                glm::vec3* b = &col->b;
                you.bound.spatial.w = glm::mod(atan2pos_64(b->y - a->y, b->x - a->x), glm::pi<f64>());

                // draw surface and normals
                if (key_is_toggled(&input, CONTROL::EDIT_VERBOSE, &verbose_view_toggle)) {
                    f64 dy = col->b.y - col->a.y;
                    f64 dx = col->b.x - col->a.x;

                    glm::vec3 na(-dy, dx, 0.0);
                    glm::vec3 nb(dy, -dx, 0.0);

                    //na = glm::normalize(na);
                    //nb = glm::normalize(nb);

                    gl_draw2d.color = Color::GREEN;
                    gl_draw2d.line(status.collider->a, status.collider->b);
                    gl_draw2d.color = Color::BLUE;
                    gl_draw2d.line(na + col->a, nb + col->a);

                    
                    //existing.color = Color::BLACK;
                    //vec3_pair_print(&na, &nb);

                    //std::cout << glm::degrees(you.bound.spatial.w) << std::endl;
                }

            }

            gl_draw2d.color = Color::BLUE;
            draw_player_collision(&you, &gl_draw2d);

            gl_draw2d.end();

            glDisable(GL_BLEND);

            // if (collision_map.elements_used > 0) {
            //     printf("{");
            //     for (auto it = collision_map.begin(); it != collision_map.first_free(); ++it)
            //     {
            //         printf("\n");
            //         Collider_print(it);
            //     }
            //     printf("\n}\n");
            // }

        }

        #endif

        SDL_GL_SwapWindow(window);

        #ifdef FPS_COUNT
        frame_count += 1;
        if (t_now_s - frame_time > 1.0) {
            fps = frame_count;
            frame_count = 0;
            frame_time = t_now_s;
            printf("%f\n", (double)fps);
        }
        #endif
    //////////////////
    }
    
    VertexAttributeArray_delete(&vao_2d2);
    VertexBufferData_delete_inplace(&tri_data);
    #ifdef GL_DRAW2D
    gl_draw2d.free();
    #endif
    #ifdef EDITOR
    in_prog.free();
    existing.free();
    glDeleteProgram(shader_grid);
    #endif
    glDeleteProgram(shader_2d);

    SDL_GL_DeleteContext(program_data.context);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}
