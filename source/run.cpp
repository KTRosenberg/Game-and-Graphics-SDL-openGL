#define WINDOW_HEADER ("")

#define SCREEN_WIDTH  (1280.0f)
#define SCREEN_HEIGHT (720.0f)
#define MS_PER_S (1000.0)
#define FRAMES_PER_SECOND (60.0)

#define EDITOR

//#define DEBUG_PRINT
#define FPS_COUNT

#define RELEASE_MODE (false)

#define LOG_WRITE_ENABLED (true)
#define LOG_PATH "./logs/"
#define LOG_FILE "log.txt"

#if !(RELEASE_MODE)
    #define USE_ASSERTS
    #pragma message("Using Asserts")
#endif


#define UNITY_BUILD (true)

#define NO_ARRAY_BOUNDS_CHECK

//#define METATESTING

//#define TEST_RENDERING

// audio

#define GLM_FORCE_ALIGNED_GENTYPES
#define AUDIO_SYS_IMPLEMENTATION
#include "audio_sys.hpp"

#define FILE_IO_IMPLEMENTATION
#include "file_io.hpp"

#define COMMON_UTILS_CPP_IMPLEMENTATION
#include "common_utils_cpp.hpp"

#include "types.h"
#include "config/config_state.cpp"

#define OPEN_GL_IMPLEMENTATION
#include "opengl.hpp"

#define CORE_UTILS_IMPLEMENTATION
#include "core_utils.h"

#define ENTITY_IMPLEMENTATION
#include "entity.h"

#define COLLISION_IMPLEMENTATION
#include "collision.h"



#include "sdl.hpp"

#include <string>

#define CAMERA_IMPLEMENTATION
#include "camera.hpp"



// TODO remove all references to iostream
#include <iostream>

int ignore_mouse_movement(void* unused, SDL_Event* event)
{
    return (event->type == SDL_MOUSEMOTION) ? 0 : 1;
}

SDL_Window* window = NULL;

void* GlobalArenaAlloc_vertex_attribute_data(size_t count) 
{
    return xmalloc(count * sizeof(GLfloat));
}
void* GlobalArenaAlloc_index_data(size_t count) 
{
    return xmalloc(count * sizeof(GLuint));    
}

// WORLD STATE
struct Room {
    Vertex_Buffer_Data  geometry;
    Collider* collision_data;
    Mat4 matrix;
};

struct World {
    Room* rooms;
    Mat4 m_view;
    Mat4 m_projection;
};

struct GlobalData {
    SDL_GLContext context;
    TextureData textures;
};

GlobalData program_data;

#define SHADER_IMPLEMENTATION
#define SHADER_OPEN_GL
#include "shader.hpp"

#define TEXTURE_IMPLEMENTATION
#define TEXTURE_OPEN_GL
#include "texture.hpp"

#define SD
#define SD_DEBUG_LOG_ON
#ifdef VULKAN_HPP
    #define SD_RENDERER_VULKAN
#elif defined(OPEN_GL_HPP)
    #define SD_RENDERER_OPENGL
    #if !defined TEST_RENDERING
        #define SD_RENDERER_VERSION_1
    #endif
#endif
#if !(RELEASE_MODE)
    #define SD_BOUNDS_CHECK
#endif

#define SD_IMPLEMENTATION
#include "sd.hpp"

#include "rotologic_renderer.hpp"


#define LOGIC_NODE_TYPE_LIST \
    LOGIC_NODE_ENTRY(VALUE, STRING(VALUE)) \
    LOGIC_NODE_ENTRY(NONE, STRING(NONE)) \
    LOGIC_NODE_ENTRY(AND, STRING(AND)) \
    LOGIC_NODE_ENTRY(OR, STRING(OR)) \
    LOGIC_NODE_ENTRY(XOR, STRING(XOR)) \
    LOGIC_NODE_ENTRY(NOT, STRING(NOT)) \
    LOGIC_NODE_ENTRY(LESS_THAN, STRING(LESS_THAN)) \
    LOGIC_NODE_ENTRY(LESS_EQ, STRING(LESS_EQ)) \
    LOGIC_NODE_ENTRY(GREATER_THAN, STRING(GREATER_THAN)) \
    LOGIC_NODE_ENTRY(GREATER_EQ, STRING(GREATER_EQ)) \
    LOGIC_NODE_ENTRY(EQUAL, STRING(EQUAL)) \
    LOGIC_NODE_ENTRY(WHILE, STRING(WHILE)) \
    LOGIC_NODE_ENTRY(ADD, STRING(ADD)) \
    LOGIC_NODE_ENTRY(SUBTRACT, STRING(SUBTRACT)) \
    LOGIC_NODE_ENTRY(MULTIPLY, STRING(MULTIPLY)) \
    LOGIC_NODE_ENTRY(DIVIDE, STRING(DIVIDE))

// for visual linking gameplay
enum struct LOGIC_NODE_TYPE {
    #define LOGIC_NODE_ENTRY(a, b) a,
    LOGIC_NODE_TYPE_LIST
    #undef LOGIC_NODE_ENTRY
    ENUM_COUNT
};
const char* const logic_node_type_strings[] = {
    #define LOGIC_NODE_ENTRY(a, b) b,
    LOGIC_NODE_TYPE_LIST
    #undef LOGIC_NODE_ENTRY
};

struct LogicInput {
    float64 value;

    operator float64(void)
    {
        return this->value;
    }
};

struct LogicNode {
    Vec3 position;
    LOGIC_NODE_TYPE type;
    
    LogicInput* in;
    usize in_count;
    usize in_received;

    LogicNode** out;
    usize out_count;
    
    bool is_negated;

    union {
        struct {
            LogicNode* out;
            usize out_count;
            float64* value_ptr;
        } value_n;
        struct {
            LogicNode* out;
            usize out_count;
        } none_n;

        struct {
            LogicNode** out;
            usize out_count;
            float64* in;
            usize in_received;
            usize in_count; 
            bool negated;
        } and_n;
        
        struct {
            LogicNode* out;
            usize out_count;
            bool negated;
        } or_n;
        
        struct {
            LogicNode* out;
            usize out_count;
            bool negated;
        } xor_n;

        struct {
            LogicNode* out;
            usize out_count;
            bool negated;
        } not_n;
        
        struct {
            LogicNode* out;
            usize out_count;
            bool negated;
        } less_than_n;
        struct {
            LogicNode* out;
            usize out_count;
            bool negated;
        } less_eq_n;
        struct {
            LogicNode* out;
            usize out_count;
            bool negated;
        } greater_than_n;
        struct {
            LogicNode* out;
            usize out_count;
            bool negated;
        } greater_eq_n;
        struct {
            LogicNode* out;
            usize out_count;
            bool negated;
        } equal_n;

        struct {
            LogicNode* out_true; // multiple out or use a separate node to feed multiple outputs? TODO
            LogicNode* out_false;
            usize out_count_true;
            usize out_count_false;
        } while_n;

        struct {
            LogicNode* out;
            usize out_count;
            float64 value;
        } add_n;
        struct {
            LogicNode* out;
            usize out_count;
            float64 value;
        } subtract_n;
        struct {
            LogicNode* out;
            usize out_count;
            float64 value;
        } multiply_n;
        struct {
            LogicNode* out;
            usize out_count;
            float64 value;
        } divide_n;
        
        // struct {

        // } for_n;
    };
};

// TODO set input of child node in parent node, must move data outside the union

#define LOGIC_NODE_PRINT
void LogicNode_traverse(LogicNode* v, float64 value, std::string tabs);

void LogicNode_handle_value(LogicNode* v, float64 value, std::string tabs)
{
    #ifdef LOGIC_NODE_PRINT
    std::cout << tabs << *(v->value_n.value_ptr) << std::endl;
    #endif
    // TODO multiple outs
    LogicNode_traverse(v->value_n.out, *(v->value_n.value_ptr), tabs + "  ");
}
void LogicNode_handle_none(LogicNode* v, float64 value, std::string tabs)
{
    #ifdef LOGIC_NODE_PRINT
    std::cout << tabs << logic_node_type_strings[(usize)v->type] << " : " << value << std::endl;
    #endif
}
void LogicNode_handle_and(LogicNode* v, float64 value, std::string tabs)
{
    #ifdef LOGIC_NODE_PRINT
    std::cout << tabs << logic_node_type_strings[(usize)v->type] << std::endl;    
    #endif
    
    v->and_n.in[v->and_n.in_received] = value;
    v->and_n.in_received += 1;
    if (v->and_n.in_received == v->and_n.in_count) {
        v->and_n.in_received = 0;
        bool out_val = true;
        for (usize i = 0; i < v->and_n.in_count && out_val == true; i += 1) {
            out_val &= (bool)v->and_n.in[i];
        }
        for (usize i = 0; i < v->and_n.out_count; i += 1) {
            LogicNode_traverse(v->and_n.out[i], out_val, tabs + "  ");
        }
    }
}
void LogicNode_handle_or(LogicNode* v, float64 value, std::string tabs)
{
    #ifdef LOGIC_NODE_PRINT
    std::cout << tabs << logic_node_type_strings[(usize)v->type] << std::endl;
    #endif
}
void LogicNode_handle_xor(LogicNode* v, float64 value, std::string tabs)
{
    #ifdef LOGIC_NODE_PRINT
    std::cout << tabs << logic_node_type_strings[(usize)v->type] << std::endl;
    #endif
}
void LogicNode_handle_not(LogicNode* v, float64 value, std::string tabs)
{
    #ifdef LOGIC_NODE_PRINT
    std::cout << tabs << logic_node_type_strings[(usize)v->type] << std::endl;
    #endif
}
void LogicNode_handle_less_than(LogicNode* v, float64 value, std::string tabs)
{
    #ifdef LOGIC_NODE_PRINT
    std::cout << tabs << logic_node_type_strings[(usize)v->type] << std::endl;
    #endif
}
void LogicNode_handle_less_eq(LogicNode* v, float64 value, std::string tabs)
{
    #ifdef LOGIC_NODE_PRINT
    std::cout << tabs << logic_node_type_strings[(usize)v->type] << std::endl;
    #endif
}
void LogicNode_handle_greater_than(LogicNode* v, float64 value, std::string tabs)
{
    #ifdef LOGIC_NODE_PRINT
    std::cout << tabs << logic_node_type_strings[(usize)v->type] << std::endl;
    #endif
}
void LogicNode_handle_greater_eq(LogicNode* v, float64 value, std::string tabs)
{
    #ifdef LOGIC_NODE_PRINT
    std::cout << tabs << logic_node_type_strings[(usize)v->type] << std::endl;
    #endif
}
void LogicNode_handle_equal(LogicNode* v, float64 value, std::string tabs)
{
    #ifdef LOGIC_NODE_PRINT
    std::cout << tabs << logic_node_type_strings[(usize)v->type] << std::endl;
    #endif
}
void LogicNode_handle_while(LogicNode* v, float64 value, std::string tabs)
{
    #ifdef LOGIC_NODE_PRINT
    std::cout << tabs << logic_node_type_strings[(usize)v->type] << std::endl;
    #endif
}
void LogicNode_handle_add(LogicNode* v, float64 value, std::string tabs)
{
    #ifdef LOGIC_NODE_PRINT
    std::cout << tabs << logic_node_type_strings[(usize)v->type] << std::endl;
    #endif
}
void LogicNode_handle_subtract(LogicNode* v, float64 value, std::string tabs)
{
    #ifdef LOGIC_NODE_PRINT
    std::cout << tabs << logic_node_type_strings[(usize)v->type] << std::endl;
    #endif
}
void LogicNode_handle_multiply(LogicNode* v, float64 value, std::string tabs)
{
    #ifdef LOGIC_NODE_PRINT
    std::cout << tabs << logic_node_type_strings[(usize)v->type] << std::endl;
    #endif
}
void LogicNode_handle_divide(LogicNode* v, float64 value, std::string tabs)
{
    #ifdef LOGIC_NODE_PRINT
    std::cout << tabs << logic_node_type_strings[(usize)v->type] << std::endl;
    #endif
}

void LogicNode_traverse(LogicNode* v, float64 value, std::string tabs)
{
    using T = LOGIC_NODE_TYPE;
    switch (v->type) {
    case T::VALUE:
        LogicNode_handle_value(v, value, tabs);
        break;
    case T::NONE:
        LogicNode_handle_none(v, value, tabs);
        break;
    case T::AND:
        LogicNode_handle_and(v, value, tabs);
        break;
    case T::OR:
        LogicNode_handle_or(v, value, tabs);
        break;
    case T::XOR:
        LogicNode_handle_xor(v, value, tabs);
        break;
    case T::NOT:
        LogicNode_handle_not(v, value, tabs);
        break;
    case T::LESS_THAN:
        LogicNode_handle_less_than(v, value, tabs);
        break;
    case T::LESS_EQ:
        LogicNode_handle_less_eq(v, value, tabs);
        break;
    case T::GREATER_THAN:
        LogicNode_handle_greater_than(v, value, tabs);
        break;
    case T::GREATER_EQ:
        LogicNode_handle_greater_eq(v, value, tabs);
        break;
    case T::EQUAL:
        LogicNode_handle_equal(v, value, tabs);
        break;
    case T::WHILE:
        LogicNode_handle_while(v, value, tabs);
        break;
    case T::ADD:
        LogicNode_handle_add(v, value, tabs);
        break;
    case T::SUBTRACT:
        LogicNode_handle_subtract(v, value, tabs);
        break;
    case T::MULTIPLY:
        LogicNode_handle_multiply(v, value, tabs);
        break;
    case T::DIVIDE:
        LogicNode_handle_divide(v, value, tabs);
        break;
    default:
        break;
    }
}

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
                std::cout << "DOWN_BUTTON_A" << std::endl;
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
                std::cout << "UNKNOWN" << std::endl;
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
                std::cout << "UNKNOWN" << std::endl;
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
                if (m::abs(event->caxis.value) > JOYSTICK_DEADZONE) {
                    std::cout << "AXIS_LEFTY " << event->caxis.value << std::endl;
                }
                break;
            case SDL_CONTROLLER_AXIS_RIGHTY:
                if (m::abs(event->caxis.value) > JOYSTICK_DEADZONE) {
                    std::cout << "AXIS_RIGHTY " << event->caxis.value << std::endl;
                }
                break;
            case SDL_CONTROLLER_AXIS_LEFTX:
                if (m::abs(event->caxis.value) > JOYSTICK_DEADZONE) {
                    std::cout << "AXIS_LEFTX " << event->caxis.value << std::endl;
                }
                break;
            case SDL_CONTROLLER_AXIS_RIGHTX:
                if (m::abs(event->caxis.value) > JOYSTICK_DEADZONE) {
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
            case SDL_SCANCODE_GRAVE:
                key_set_down(input, CONTROL::LOAD_CONFIG);
                break;
            case SDL_SCANCODE_T:
                key_set_down(input, CONTROL::TEMP);
                break;
            case SDL_SCANCODE_N:
                key_set_down(input, CONTROL::ROTATE_ANTICLOCKWISE);
                break;
            case SDL_SCANCODE_M:
                key_set_down(input, CONTROL::ROTATE_CLOCKWISE);
                break;
            case SDL_SCANCODE_LSHIFT:
                key_set_down(input, CONTROL::SHIFT);
                break;

            case SDL_SCANCODE_1:
                key_set_down(input, CONTROL::AUDIO_TRACK_1);
                break;
            case SDL_SCANCODE_2:
                key_set_down(input, CONTROL::AUDIO_TRACK_2);
                break;
            case SDL_SCANCODE_3:
                key_set_down(input, CONTROL::AUDIO_TRACK_3);
                break;
            case SDL_SCANCODE_4:
                key_set_down(input, CONTROL::AUDIO_TRACK_4);
                break;
            case SDL_SCANCODE_5:
                key_set_down(input, CONTROL::AUDIO_TRACK_5);
                break;
            case SDL_SCANCODE_6:
                key_set_down(input, CONTROL::AUDIO_TRACK_6);
                break;
            case SDL_SCANCODE_7:
                key_set_down(input, CONTROL::AUDIO_TRACK_7);
                break;
            case SDL_SCANCODE_8:
                key_set_down(input, CONTROL::AUDIO_TRACK_8);
                break;
            case SDL_SCANCODE_9:
                key_set_down(input, CONTROL::AUDIO_TRACK_9);
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
            case SDL_SCANCODE_GRAVE:
                key_set_up(input, CONTROL::LOAD_CONFIG);
                break;
            case SDL_SCANCODE_T:
                key_set_up(input, CONTROL::TEMP);
                break;
            case SDL_SCANCODE_N:
                key_set_up(input, CONTROL::ROTATE_ANTICLOCKWISE);
                break;
            case SDL_SCANCODE_M:
                key_set_up(input, CONTROL::ROTATE_CLOCKWISE);
                break;
            case SDL_SCANCODE_LSHIFT:
                key_set_up(input, CONTROL::SHIFT);
                break;

            case SDL_SCANCODE_1:
                key_set_up(input, CONTROL::AUDIO_TRACK_1);
                break;
            case SDL_SCANCODE_2:
                key_set_up(input, CONTROL::AUDIO_TRACK_2);
                break;
            case SDL_SCANCODE_3:
                key_set_up(input, CONTROL::AUDIO_TRACK_3);
                break;
            case SDL_SCANCODE_4:
                key_set_up(input, CONTROL::AUDIO_TRACK_4);
                break;
            case SDL_SCANCODE_5:
                key_set_up(input, CONTROL::AUDIO_TRACK_5);
                break;
            case SDL_SCANCODE_6:
                key_set_up(input, CONTROL::AUDIO_TRACK_6);
                break;
            case SDL_SCANCODE_7:
                key_set_up(input, CONTROL::AUDIO_TRACK_7);
                break;
            case SDL_SCANCODE_8:
                key_set_up(input, CONTROL::AUDIO_TRACK_8);
                break;
            case SDL_SCANCODE_9:
                key_set_up(input, CONTROL::AUDIO_TRACK_9);
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

#ifndef TEST_RENDERING
template <usize N>
void draw_player_collision(Player* you, sd::Render_Batch<N>* ctx)
{
    const Vec3 off(0.5, 0.5, 0.0);
    BoxComponent* bc = &you->bound;
    Vec3 top_left = bc->position() + off;
    Vec3 top_right = top_left + Vec3(bc->width, 0.0, 0.0);
    Vec3 bottom_right = top_left + Vec3(bc->width, bc->height, 0.0);
    Vec3 bottom_left = top_left + Vec3(0.0, bc->height, 0.0);

    { // bound
        ctx->draw_type = sd::LINES;
        sd::line(ctx, top_left, top_right);
        sd::line(ctx, top_right, bottom_right);
        sd::line(ctx, bottom_right, bottom_left);
        sd::line(ctx, bottom_left, top_left);
    }

    { // floor sensors
        ctx->color = Color::RED;
        auto floor_sensor_rays = you->floor_sensor_rays();
        floor_sensor_rays.first.first   += off;
        floor_sensor_rays.first.second  += off;
        floor_sensor_rays.second.first  += off;
        floor_sensor_rays.second.second += off;

        sd::line(ctx, floor_sensor_rays.first.first, floor_sensor_rays.first.second);
        sd::line(ctx, floor_sensor_rays.second.first, floor_sensor_rays.second.second);
    }

    { // side sensors
        auto side_sensor_rays = you->side_sensor_rays();
        side_sensor_rays.first.first   += off;
        side_sensor_rays.first.second  += off;
        side_sensor_rays.second.first  += off;
        side_sensor_rays.second.second += off;

        ctx->color = Vec4(1.0, 165.0 / 255, 0.0, 1.0);
        sd::line(ctx, side_sensor_rays.first.first, side_sensor_rays.first.second);
        ctx->color = Vec4(148.0 / 255, 0.0, 211.0 / 255, 1.0);
        sd::line(ctx, side_sensor_rays.second.first, side_sensor_rays.second.second);
    }
}


template <usize N>
void BoxComponent_draw(BoxComponent* bc, sd::Render_Batch<N>* ctx)
{
    const Vec3 off(0.5, 0.5, 0.0);
    Vec3 top_left = bc->position() + off;
    Vec3 top_right = top_left + Vec3(bc->width, 0.0, 0.0);
    Vec3 bottom_right = top_left + Vec3(bc->width, bc->height, 0.0);
    Vec3 bottom_left = top_left + Vec3(0.0, bc->height, 0.0);

    ctx->draw_type = sd::LINES;
    sd::line(ctx, top_left, top_right);
    sd::line(ctx, top_right, bottom_right);
    sd::line(ctx, bottom_right, bottom_left);
    sd::line(ctx, bottom_left, top_left);
}
#endif

bool temp_test_collision(Player* you, Collider* c, CollisionStatus* status)
{
    auto sensors = you->floor_sensor_rays();

    vec3_pair* ray0 = &sensors.first;
    vec3_pair* ray1 = &sensors.second;
    std::pair<Vec3, Vec3> collider = {
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

    Vec3 va(POSITIVE_INFINITY);
    Vec3 vb(POSITIVE_INFINITY);
    Vec3* choice = &va;
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

    Vec3* out = &status->intersection;

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

        // Vec3* a = &c->a;
        // Vec3* b = &c->b;
        //std::cout << glm::degrees(atan2pos_64(b->y - a->y, b->x - a->x)) << std::endl;


        //vec3_pair_print(&c->a, &c->b);
        
        return true;        
    }


    return false;

}

char temp_test_collision_sides(Player* you, Collider* c, CollisionStatus* l, CollisionStatus* r)
{
    auto sensors = you->side_sensor_rays();

    std::pair<Vec3, Vec3>* ray0 = &sensors.first;
    std::pair<Vec3, Vec3>* ray1 = &sensors.second;
    std::pair<Vec3, Vec3> collider = {
        c->a,
        c->b
    };

    Vec3 vl(NEGATIVE_INFINITY);
    Vec3 vr(POSITIVE_INFINITY);
    
    bool collision_l = false;
    bool collision_r = false;

    if (line_segment_intersection(ray0, &collider, &vl)) {
        collision_l = true;
    }
    if (line_segment_intersection(ray1, &collider, &vr)) {
        collision_r = true;
    }

    if (!(collision_l || collision_r)) {
        return 0;
    }

    Vec3* out_l = &l->intersection;
    Vec3* out_r = &r->intersection;

    if (collision_l && collision_r) {
        if (vl.x > out_l->x) {
            out_l->x = vl.x;
            out_l->y = vl.y;
            out_l->z = 0.0;

            l->collider = c;
        }
        if (vr.x < out_r->x) {
            out_r->x = vr.x;
            out_r->y = vr.y;
            out_r->z = 0.0;

            r->collider = c;
        }

        return 'b';
    } else if (collision_l) {
        if (vl.x > out_l->x) {
            out_l->x = vl.x;
            out_l->y = vl.y;
            out_l->z = 0.0;

            l->collider = c;
        }
        return 'l';
    } else { // if (collision_r)
        if (vr.x < out_r->x) {
            out_r->x = vr.x;
            out_r->y = vr.y;
            out_r->z = 0.0;

            r->collider = c;
        }
        return 'r';
    }
}

struct AirPhysicsConfig {
    std::string path;
    FILE* fd;
    struct stat stat;
    time_t t_prev_mod;
    f64 gravity;
    f64 player_initial_velocity;
    f64 player_initial_velocity_short;
};

bool load_config(AirPhysicsConfig* conf)
{
    #ifdef EDITOR
    check_file_status(conf->path.c_str(), &conf->stat);
    if (conf->stat.st_mtime != conf->t_prev_mod) {

        conf->t_prev_mod = conf->stat.st_mtime;
        
        char buff[512];

        std::string conf_string = file_io::read_file(conf->fd);

        if (conf_string.length() == 0 || conf_string.find("DEFAULT") == 0) {
            puts("USING DEFAULT PARAMETERS");
            conf->gravity = physics::GRAVITY_DEFAULT;
            conf->player_initial_velocity = Player::JUMP_VELOCITY_DEFAULT;
            conf->player_initial_velocity_short = Player::JUMP_VELOCITY_SHORT_DEFAULT;
            rewind(conf->fd);
            return true;
        }


        puts("MODIFYING PARAMETERS");
        printf("%s\n", conf_string.c_str());

        char seps[] = " ,:;\t\n";

        char* token = strtok((char*)conf_string.c_str(), seps);

        sscanf(token, "%lf", &conf->gravity);
        token = strtok(NULL, seps);
        sscanf(token, "%lf", &conf->player_initial_velocity);
        token = strtok(NULL, seps);
        sscanf(token, "%lf", &conf->player_initial_velocity_short);

        rewind(conf->fd);

        printf("%lf : %lf : %lf\n", conf->gravity, conf->player_initial_velocity, conf->player_initial_velocity_short);

        return true;
    }
    #endif

    return false;
}

#ifdef METATESTING
#include "metatesting.cpp"
#endif


struct Type_Info;

TYPE_T
struct Hee {
    T t;
};

TYPE_T
struct Wee {
    Hee<T> hee;
};

struct Type_Info_Named {
    char* name;
    Type_Info* type_info;
};

struct Type_Info_Struct {
    Array_Slice<Type_Info> types;
    Array_Slice<char*>     names;
    Array_Slice<usize>     offsets;
};

enum struct TYPE_INFO_TYPE {
    NAMED,
    STRUCT
};

struct Type_Info {
    usize size;

    TYPE_INFO_TYPE type_info_type;
    union {
        Type_Info_Named ti_named;
        Type_Info_Struct ti_struct;
    };
};

static int assign_type_id(void)
{
    static int id = 0;
    id += 1;
    return id - 1;
}
struct _wee {
    static int id;
};
int _wee::id = assign_type_id();

struct _hee {
    static int id;
};
int _hee::id = assign_type_id();

/*

struct Entity {
    Transform t;

    type_id id;
    Fn_entity_handler (void* args) entity_handler;
};

*/

enum struct ENTITY_KIND {
    FISH,
    ENUM_COUNT
};

struct Fish {

};

struct Entity {
    Vec4 transform;

    ENTITY_KIND kind;
    union {
        Fish k_fish;
    };
};

#define ENTITY_TYPE_SYSTEM_IMPLEMENTATION
#include "entity_type_system.cpp"



//////////////////////////////////////////////////////////////////////////////////////////////
#ifdef TEST_RENDERING
#include "test_rendering.cpp"
#endif
//////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
    //logging_init(LOG_PATH, LOG_FILE, LOG_WRITE_ENABLED);
//////////////////////////////////////////////////////////////////////////////////////////////
    #ifdef TEST_RENDERING
    return test_rendering(argc, argv);
    #else
//////////////////////////////////////////////////////////////////////////////////////////////


    // Type_Info ti_wee;
    // ti_wee.type = TYPE_INFO_TYPE::NAMED;

    // Type_Info_Named ti_named_wee;

    // Type_Info ti_named_wee;
    // Type_Info_Struct ti_struct_wee;

    // Type_Info ti_hee;
    // Type_Info_Named ti_named_hee
    // Type_Info_Struct ti_struct_hee;

    // Type_Info ti_t;


    // Type_Info __[3];
    // tis.types.data = __;
    // tis.types.count = 3;
/*
        struct {
            LogicNode* out;
            usize count;
            float64* value_ptr;
        } value_n;
        struct {
            LogicNode* out;
            usize count;
        } none_n;

        struct {
            LogicNode** out;
            usize count;
            float64* in;
            usize input_received;
            usize input_count; 
            bool negated;
        } and_n;
        */

    srand(time(NULL));

    float64 avals[] = {(float64)(rand() % 2), (float64)(rand() % 2), (float64)(rand() % 2)};
    float64 bvals[] = {(float64)(rand() % 2), (float64)(rand() % 2), (float64)(rand() % 2)};

    float64 root_a_val = 0.0;
    float64 root_b_val = 0.0;

    LogicNode root_a;
    root_a.type = LOGIC_NODE_TYPE::VALUE;
    root_a.value_n.out_count = 1;
    root_a.value_n.value_ptr = &root_a_val;
    LogicNode root_b;
    root_b.type = LOGIC_NODE_TYPE::VALUE;
    root_b.value_n.out_count = 1;
    root_b.value_n.value_ptr = &root_b_val;


    LogicNode and_gate;
    and_gate.type = LOGIC_NODE_TYPE::AND;
    and_gate.and_n.in = new float64[2];
    and_gate.and_n.in_received = 0;
    and_gate.and_n.in_count = 2;
    and_gate.and_n.out = new LogicNode*[1];
    and_gate.and_n.out_count = 1;
    
    LogicNode leaf;
    leaf.type = LOGIC_NODE_TYPE::NONE;

    root_a.value_n.out = &and_gate;
    root_b.value_n.out = &and_gate;
    and_gate.and_n.out[0] = &leaf;

    foreach (i, 3) {
        root_a_val = avals[i];
        root_b_val = bvals[i];
        LogicNode_traverse(&root_a, 0.0, "");
        LogicNode_traverse(&root_b, 0.0, "");
        std::cout << "------------------------" << std::endl;

    }

    delete[] and_gate.and_n.in;
    delete[] and_gate.and_n.out;
    //return 0;
    // auto b = Buffer<usize, 10>::Buffer_make();
    // b.count = 0;
    // for (usize i = 0; i < 10; i += 1) {
    //     b.push_back(i);
    // }
    // WEE(b);
    // WEE(b.slice(1, 3));

    // return 0; 
    #ifdef METATESTING
    puts("metatesting, main program disabled");
    metatesting();
    return EXIT_SUCCESS;
    #endif
    using namespace input_sys;
    int control_lock_time = 0;
    bool control_lock = false;
    //RingBuffer_init(&buff);

    // Thing_array[0].speed = 1.0f;
    
    // TEST change Thing[4].speed to 2.0f
    // ucharptr ptr = (ucharptr)&Thing_array[0];
    // ptr += (4 * meta_arrays[0].element_size); 
    // ptr += Thing_meta_data[1].offset;
    // *((f32*)ptr) = 2.0f;

    // std::cout << Thing_array[4].speed << std::endl;

    std::cout << StaticArrayCount(config_state) << std::endl;
    // modify
    for (usize k = 0; k < 2; k += 1) {
        for (usize i = 0; i < StaticArrayCount(config_state); i += 1) {
            using pt = PROPERTY_TYPE;
            switch (config_state[i].type) {
            case pt::PROP_f64:
                std::cout << *cast(f64*, config_state[i].ptr) << std::endl;
                *cast(f64*, config_state[i].ptr) *= 2.0;
            }
        }
    }
    // reset to defaults
    for (usize i = 0; i < StaticArrayCount(config_state); i += 1) {
        using pt = PROPERTY_TYPE;
        switch (config_state[i].type) {
        case pt::PROP_f64:
            *cast(f64*, config_state[i].ptr) = config_state[i].f_f64;
        }
    }

    CommandLineArgs cmd;
    if (!parse_command_line_args(&cmd, argc, argv)) {
        return EXIT_FAILURE;
    }

    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC) < 0) {
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

    // OpenGL initialization ///////////////////////////////////////////////////
    
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
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN)))
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

    ConfigState_load_runtime();

    bool status = false;
    std::string glsl_perlin_noise = Shader_retrieve_src_txt_from_file("shaders/perlin_noise.glsl", &status);
    if (!status) {
        fprintf(stderr, "ERROR: failed to load shader addon source");
        return EXIT_FAILURE;
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

    Vec2 tex_res(2048.0f, 1024.0f);

    Vec3 world_bguv_factor = Vec3(Vec2(1.0f) / tex_res, 1.0f);

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

    Vertex_Attribute_Array vao_2d2;
    Vertex_Buffer_Data tri_data;

    Vertex_Attribute_Array_init(&vao_2d2, STRIDE);

    glBindVertexArray(vao_2d2.vao);

        Vertex_Buffer_Data_init_inplace(
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

    mat4 mat_ident(1.0f);
    mat4 mat_projection = glm::ortho(
        0.0f, 
        1.0f * SCREEN_WIDTH, 
        1.0f * SCREEN_HEIGHT,
        0.0f,
        0.0f, 
        1.0f * 10.0f
    );

    //Mat4 mat_projection = glm::perspective(glm::radians(45.0f), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

//////////////////
// TEST INPUT
    Vec3 start_pos(0.0f, 0.0f, 1.0f);
    
    FreeCamera main_cam;
    FreeCamera_init(&main_cam, start_pos);
    main_cam.orientation = Quat();
    main_cam.speed = PLAYER_BASE_SPEED;
    main_cam.offset = Vec2(SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2.0);
    main_cam.target = Vec2(0);
    main_cam.scale = 1.0;
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
    Vec3 prev_pos(0.0);
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

    GL_GET_ERRORS();


    glUseProgram(shader_2d);

    //Uniform_Location RES_LOC = glGetUniformLocation(shader_2d, "u_resolution");
    //Uniform_Location COUNT_LAYERS_LOC = glGetUniformLocation(shader_2d, "u_count_layers");
    Uniform_Location MAT_LOC = glGetUniformLocation(shader_2d, "u_matrix");
    Uniform_Location TIME_LOC = glGetUniformLocation(shader_2d, "u_time");
    Uniform_Location CAM_LOC = glGetUniformLocation(shader_2d, "u_position_cam");
    Uniform_Location SCALE_LOC = glGetUniformLocation(shader_2d, "u_scale");
    glUniform1f(SCALE_LOC, (GLfloat)1.0);
    //Uniform_Location ASPECT_LOC = glGetUniformLocation(shader_2d, "u_aspect");

    const GLuint UVAL_COUNT_LAYERS = 5;

    //glUniform2fv(RES_LOC, 1, glm::value_ptr(Vec2(SCREEN_WIDTH, SCREEN_HEIGHT)));
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


    GL_GET_ERRORS();


    #ifdef SD
    auto drawctx = sd::Render_Batch_make(mat_projection);
    #endif

    Toggle free_cam_toggle = false;

    #ifdef EDITOR
    glUseProgram(shader_grid);

    Uniform_Location COLOR_LOC_GRID = glGetUniformLocation(shader_grid, "u_color");
    glUniform4fv(COLOR_LOC_GRID, 1, glm::value_ptr(Vec4(0.25f, 0.25f, 0.25f, 0.5f)));

    Uniform_Location SQUARE_PIXEL_LOC_GRID = glGetUniformLocation(shader_grid, "u_grid_square_pix");
    Uniform_Location RESOLUTION_LOC_GRID = glGetUniformLocation(shader_grid, "u_resolution");


    glUniform2fv(RESOLUTION_LOC_GRID, 1, glm::value_ptr(Vec2(SCREEN_WIDTH, SCREEN_HEIGHT)));

    GLfloat grid_square_pixel_size = 16.0f;
    glUniform1f(SQUARE_PIXEL_LOC_GRID, tex_res.x / grid_square_pixel_size);

    Uniform_Location MAT_LOC_GRID = glGetUniformLocation(shader_grid, "u_matrix");

    Uniform_Location CAM_LOC_GRID = glGetUniformLocation(shader_grid, "u_position_cam");

    Uniform_Location SCALE_LOC_GRID = glGetUniformLocation(shader_grid, "u_scale");
    glUniform1f(SCALE_LOC_GRID, (GLfloat)1.0);

    sd::Render_Batch<> existing;
    sd::Render_Batch<256> in_prog;
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
    const f64 REFRESH_RATE = mode.refresh_rate * (60.0 / mode.refresh_rate);

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
    Vec3 in_progress_line[2];
    in_progress_line[0] = Vec3(0.0f);
    in_progress_line[1] = Vec3(0.0f);


////
    collision_map.next_free_slot_ptr()->a = Vec3(0.0, 5 * 128, 0.0);
    collision_map.next_free_slot_ptr()->b = Vec3(1280.0f, 5 * 128, 0.0);
    collision_map.count += 1;

//
    collision_map.next_free_slot_ptr()->a = Vec3(512.0, 3 * 128, 0.0);
    collision_map.next_free_slot_ptr()->b = Vec3(768.0, 3 * 128, 0.0);
    collision_map.count += 1;

    existing.begin();
    existing.draw_type = sd::LINES;
    existing.color = Color::BLACK;
    
    foreach (i, collision_map.count) {
        existing.line(collision_map[i].a, collision_map[i].b);
    }

    existing.end_no_reset();


    Toggle temp = false;
////
#endif

    Player you;
    Player_init(&you, 1280.0 / 2.0, 720.0 / 2.0, 0.0, true, 0, 20, 40);
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


    // Vec2 a(0, 0);
    // Vec2 b(1, 1);
    // std::cout << glm::degrees(atan2pos_64(b.y - a.y, b.x - a.x)) << std::endl;

    #define AIR_CONFIG_PATH "./config/air.txt"

    AirPhysicsConfig air_physics_conf;

    air_physics_conf.path = AIR_CONFIG_PATH;
    air_physics_conf.fd = fopen(air_physics_conf.path.c_str(), "r");
    if (air_physics_conf.fd == nullptr) {
        fprintf(stderr, "ERROR: CANNOT OPEN AIR CONFIG FILE");
    }
    air_physics_conf.gravity                    = physics::gravity;
    air_physics_conf.player_initial_velocity       = Player::JUMP_VELOCITY_DEFAULT;
    air_physics_conf.player_initial_velocity_short = Player::JUMP_VELOCITY_SHORT_DEFAULT;
    air_physics_conf.t_prev_mod = -1;

    if (cmd.hot_config) {
        if (load_config(&air_physics_conf)) {
            physics::gravity = air_physics_conf.gravity;
            you.initial_jump_velocity = air_physics_conf.player_initial_velocity;
            you.initial_jump_velocity_short = air_physics_conf.player_initial_velocity_short;
        }
    }


    //fclose(jump_conf_fd);

    // audio

    AudioSystem_init();

    // Audio_Args audio_args;
    // Audio_Args_init(&audio_args, 1);

    // mal_decoder_config decoder_conf;
    // decoder_conf.format = mal_format_f32;
    // decoder_conf.channels = 2;
    // decoder_conf.sampleRate = 44100;

    // mal_result result = mal_decoder_init_file_wav(
    //     "audio/time_rush_v_2_0_1_export_16_bit.wav", 
    //     &decoder_conf, 
    //     &audio_args.decoders[0]
    // );

    Audio_Args audio_args;
    Audio_Args_init(&audio_args, 7);

    mal_decoder_config decoder_conf;
    decoder_conf.format = mal_format_f32;
    decoder_conf.channels = 2;
    decoder_conf.sampleRate = 44100;



    for (usize i = 0; i < 7; i += 1) {
        mal_result result = mal_decoder_init_file_wav(
            ("audio/separate_instruments_test/time_rush_v_2_0_1_export_0" + std::to_string(i) +  ".wav").c_str(), 
            &decoder_conf, 
            &audio_args.decoders[i]
        );


        if (result != MAL_SUCCESS) {
            fprintf(stderr, "ERROR: FAILED TO DECODE AUDIO\n");
            return -2;
        }
    }

    mal_device_config config = mal_device_config_init_playback(
        audio_args.decoders[0].outputFormat,
        audio_args.decoders[0].outputChannels,
        audio_args.decoders[0].outputSampleRate,
        on_send_frames_to_device
    );

    
    if (mal_device_init(
            NULL,
            mal_device_type_playback,
            NULL,
            &config,
            &audio_args.decoders[0],
            &audio_system.device
        ) != MAL_SUCCESS) {
        fprintf(stderr, "ERROR: FAILED TO OPEN PLAYBACK DEVICE\n");
        mal_decoder_uninit(&audio_args.decoders[0]);
        return -3;     
    }

    if (mal_device_start(&audio_system.device) != MAL_SUCCESS) {
        fprintf(stderr, "ERROR: FAILED TO START PLAYBACK DEVICE\n");
        mal_device_uninit(&audio_system.device);
        mal_decoder_uninit(&audio_args.decoders[0]);
        return -4; 
    }

    // printf("press enter to quit...");
    // getchar();

    // mal_device_uninit(&device);
    // mal_decoder_uninit(&audio_args.decoders[0]);

    // return EXIT_SUCCESS;

    {
        int w = 0;
        int h = 0;
        SDL_GetWindowSize(window, &w, &h);
        std::cout << "WIDTH: " << w << " HEIGHT: " << h << std::endl;
    }
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

        bool left_held = false;
        bool right_held = false;

        {

            //main_cam.orientation = Quat();

            if (free_cam_is_on) {
                if (key_is_held(&input, CONTROL::SHIFT)) {
                    if (key_is_held(&input, CONTROL::UP)) {
                        main_cam.scale += (t_delta_s * 4.0);
                    } else if (key_is_held(&input, CONTROL::DOWN)) {
                        main_cam.scale -= (t_delta_s * 4.0);
                    }

                    main_cam.scale = glm::clamp(main_cam.scale, 0.0625f, 4.0f);

                    // if (key_is_pressed(&input, CONTROL::UP)) {
                    //     main_cam.scale = m::min(4.0, main_cam.scale * 2.0);
                    // } else if (key_is_pressed(&input, CONTROL::DOWN)) {
                    //     main_cam.scale = m::max(0.015625, main_cam.scale / 2.0);
                    // }
                }
                else { 
                    if (key_is_held(&input, CONTROL::UP)) {
                        FreeCamera_process_directional_movement(&main_cam, MOVEMENT_DIRECTION::UPWARDS, t_delta_s * up_acc);
                        up_acc *= POS_ACC;
                        up_acc = m::min(max_acc, up_acc);
                    } else {
                        if (up_acc > 1.0) {
                            FreeCamera_process_directional_movement(&main_cam, MOVEMENT_DIRECTION::UPWARDS, t_delta_s * up_acc);
                        }
                        up_acc = m::max(1.0, up_acc * NEG_ACC);
                    }
                    if (key_is_held(&input, CONTROL::DOWN)) {
                        FreeCamera_process_directional_movement(&main_cam, MOVEMENT_DIRECTION::DOWNWARDS, t_delta_s * down_acc);
                        down_acc *= POS_ACC;
                        down_acc = m::min(max_acc, down_acc);
                    } else {
                        if (down_acc > 1.0) {
                            FreeCamera_process_directional_movement(&main_cam, MOVEMENT_DIRECTION::DOWNWARDS, t_delta_s * down_acc);
                        } 
                        down_acc = m::max(1.0, down_acc * NEG_ACC);
                    }
                }
            }



            if (key_is_held(&input, CONTROL::LEFT)) {
                if (free_cam_is_on) {
                    FreeCamera_process_directional_movement(&main_cam, MOVEMENT_DIRECTION::LEFTWARDS, t_delta_s * left_acc);
                } else {
                // TEMP
                    left_held = true;
                    //Player_move_test(&you, MOVEMENT_DIRECTION::LEFTWARDS, t_delta_s * left_acc);
                }

                left_acc *= POS_ACC;
                left_acc = m::min(max_acc, left_acc);
            
            } else {
                if (left_acc > 1.0) {
                    if (free_cam_is_on) {
                        FreeCamera_process_directional_movement(&main_cam, MOVEMENT_DIRECTION::LEFTWARDS, t_delta_s * left_acc);
                    } else {
                    // TEMP
                        //Player_move_test(&you, MOVEMENT_DIRECTION::LEFTWARDS, t_delta_s * left_acc);
                    }
                }
                left_acc = m::max(1.0, left_acc * NEG_ACC);
                left_acc = m::min(max_acc, left_acc);
            }

            if (key_is_held(&input, CONTROL::RIGHT)) {
                if (free_cam_is_on) {
                    FreeCamera_process_directional_movement(&main_cam, MOVEMENT_DIRECTION::RIGHTWARDS, t_delta_s * right_acc);
                } else {
                    // TEMP
                    right_held = true;
                    //Player_move_test(&you, MOVEMENT_DIRECTION::RIGHTWARDS, t_delta_s * right_acc);
                }

                right_acc *= POS_ACC;
                right_acc = m::min(max_acc, right_acc);

            } else {
                if (right_acc > 1.0) {
                    if (free_cam_is_on) {
                        FreeCamera_process_directional_movement(&main_cam, MOVEMENT_DIRECTION::RIGHTWARDS, t_delta_s * right_acc);
                    } else {
                        // TEMP
                        //Player_move_test(&you, MOVEMENT_DIRECTION::RIGHTWARDS, t_delta_s * right_acc);
                    }
                }
                right_acc = m::max(1.0, right_acc * NEG_ACC);
            }

            const f64 dt_factor = DELTA_TIME_FACTOR(t_delta_s, REFRESH_RATE);
            const f64 friction = Player::GROUND_ACCELERATION_DEFAULT;

            // TODO ground to air, air to ground angles, probably keep a single variable to share between ground and air instead (rewrite)
            
            //std::cout << you.bound.spatial.w << std::endl;

            float64 angle = you.bound.spatial.w;

            // TODO RE-ADD STATEMENT std::cout << "ANGLE: " << angle << std::endl;

            if (you.on_ground) {

                //std::cout << "BEFORE" << you.velocity_ground.x << std::endl;
                you.velocity_ground.x -= (.125 * 4) * glm::sin(angle) * dt_factor;

                //std::cout << "SUBTRACTING " << (.125 * 4) * glm::sin(angle) * dt_factor << std::endl;
                //std::cout << "AFTER " << you.velocity_ground.x << std::endl;

                #define ANGLE_TOO_STEEP ((PI / 8) * 3)

                // if (((angle <= -(glm::pi<f64>() / 8) * 3) && you.velocity_ground.x < 0.0) || 
                //     ((angle >=  (glm::pi<f64>() / 8) * 3) && you.velocity_ground.x > 0.0)) {
                if (left_held && -angle < ANGLE_TOO_STEEP) {
                    if (you.velocity_ground.x > 0.0) {
                        you.velocity_ground.x -= Player::GROUND_NEGATIVE_ACCELERATIION_DEFAULT * dt_factor;
                    } else {                     
                        you.velocity_ground.x -= you.acceleration_ground * dt_factor;
                    }
                } else if (right_held && angle < ANGLE_TOO_STEEP) {
                    if (you.velocity_ground.x < 0.0) {
                        you.velocity_ground.x += Player::GROUND_NEGATIVE_ACCELERATIION_DEFAULT * dt_factor;
                        //std::cout << "SUBTRACTING SLOPE FACTOR: " << (.125 * 4) * glm::sin(angle) << std::endl;
                        //std::cout << "ADDING DECCELERATION: " << Player::GROUND_NEGATIVE_ACCELERATIION_DEFAULT << std::endl;
                        //std::cout << "DIFF DEC - SLOPE: " << Player::GROUND_NEGATIVE_ACCELERATIION_DEFAULT - (.125 * 4) * glm::sin(angle) << std::endl;

                    } else {
                        you.velocity_ground.x += you.acceleration_ground * dt_factor;
                    }
                } else if (you.velocity_ground.x != 0.0) {
                    // TODO improve friction
                    if (m::abs(you.velocity_ground.x) < friction * dt_factor) {
                        you.velocity_ground.x = 0.0;
                    } else {
                        you.velocity_ground.x -= friction * glm::sign(you.velocity_ground.x) * dt_factor;   
                    } 
                }
            } else {
                // TODO switch between velocity_ground and velocity_air or just use one velocity for both
                if (left_held) {
                    you.velocity_ground.x -= you.acceleration_air * dt_factor;
                } else if (right_held) {
                    you.velocity_ground.x += you.acceleration_air * dt_factor;
                }

                if (you.velocity_air.y < 0 && you.velocity_air.y > -4.0) {
                    if (m::abs(you.velocity_ground.x) >= 16.0) {
                        you.velocity_ground.x *= 0.90;
                    }
                }
            }

            if (you.velocity_ground.x < -you.max_speed) {
                you.velocity_ground.x = -you.max_speed;
            } else if (you.velocity_ground.x > you.max_speed) {
                you.velocity_ground.x = you.max_speed;
            }

                float64 y_comp = -glm::sin(angle);
                float64 x_comp = glm::cos(angle);


                // TODO RE-ADD STATEMENT std::cout << "V: " << you.velocity_ground.x << ":" << x_comp << ":" << y_comp << " SLOPE FACTOR: " << (.125 * 4) * glm::sin(angle) * dt_factor << std::endl;
            if (you.on_ground) {


                // if (((angle <= -(glm::pi<f64>() / 8) * 3) && you.velocity_ground.x < 0.0) || 
                //     ((angle >=  (glm::pi<f64>() / 8) * 3) && you.velocity_ground.x > 0.0)) {
                //     you.velocity_ground.x = -you.velocity_ground.x * x_comp;
                //     you.velocity_ground.y = -you.velocity_ground.y * y_comp;
                // }

                you.bound.spatial.x += you.velocity_ground.x * x_comp;
                you.bound.spatial.y += you.velocity_ground.x * y_comp;

                //draw_player_collision(&you, &drawctx);

            } else {
                you.bound.spatial.x += you.velocity_ground.x;
            }



            //printf("%f %f\n", you.velocity_ground.x, you.velocity_air.y);

            {
                bool collided_l = false;
                bool collided_r = false;

                CollisionStatus status_l;
                CollisionStatus_init(&status_l, Vec3(NEGATIVE_INFINITY, NEGATIVE_INFINITY, 0.0));
                CollisionStatus status_r;
                CollisionStatus_init(&status_r);

                // this will be off by one movement, need to reorganize so camera updated after play is updated,
                // also cannot draw bg yet... will need to sequence things differently

                drawctx.begin();
                drawctx.transform_matrix = FreeCamera_calc_view_matrix(&main_cam);

                for (auto it = collision_map.begin_ptr(); it != collision_map.next_free_slot_ptr(); it += 1)
                {
                    //Collider_print(it);
                    
                    switch (temp_test_collision_sides(&you, it, &status_l, &status_r)) {
                    case 'l': { // left
                        drawctx.line(Vec3(0.0), status_l.intersection);
                        collided_l = true;
                        break;
                    }
                    case 'r': { // right
                        drawctx.line(Vec3(0.0), status_r.intersection);
                        collided_r = true;
                        break;
                    }
                    case 'b': { // both
                        collided_l = true;
                        collided_r = true;
                        break;
                    }
                    default: { // none
                        break;
                    }

                    }
                }
                drawctx.end_no_reset();

                // TODO slopes

                if (collided_l) {
                    f64 angle = atan2_64(status_l.collider->b.y - status_l.collider->a.y, status_l.collider->b.x - status_l.collider->a.x);
                    angle = m::abs(angle);

                    if (angle > ((PI / 8) * 3)) {
                        //std::cout << "COLLIDED L" << std::endl;
                        you.bound.spatial.x = status_l.intersection.x;
                        you.velocity_ground.x = 0.0;
                    }
                }
                if (collided_r) {
                    f64 angle = atan2_64(status_r.collider->b.y - status_r.collider->a.y, status_r.collider->b.x - status_r.collider->a.x);
                    angle = m::abs(angle);

                    //std::cout << "COLLIDED R" << std::endl;
                    if (angle > ((PI / 8) * 3)) {
                        you.bound.spatial.x = status_r.intersection.x - you.bound.width;
                        you.velocity_ground.x = 0.0;
                    }
                }
            }




            // if (*forwards) {
            //     FreeCamera_process_directional_movement(&main_cam, MOVEMENT_DIRECTION::FORWARDS, t_delta_s * forwards_acc);
            //     forwards_acc *= POS_ACC;
            //     forwards_acc = m::min(max_acc, forwards_acc);
            // } else {
            //     if (forwards_acc > 1.0) {
            //         FreeCamera_process_directional_movement(&main_cam, MOVEMENT_DIRECTION::FORWARDS, t_delta_s * forwards_acc);
            //     }
            //     forwards_acc = m::max(1.0, forwards_acc * NEG_ACC);
            // }
            // if (*backwards) {
            //     FreeCamera_process_directional_movement(&main_cam, MOVEMENT_DIRECTION::BACKWARDS, t_delta_s * backwards_acc);
            //     backwards_acc *= POS_ACC;
            //     backwards_acc = m::min(max_acc, backwards_acc);
            // } else {
            //     if (backwards_acc > 1.0) {
            //         FreeCamera_process_directional_movement(&main_cam, MOVEMENT_DIRECTION::BACKWARDS, t_delta_s * backwards_acc);
            //     } 
            //     backwards_acc = m::max(1.0, backwards_acc * NEG_ACC);  
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
                main_cam.orientation = Quat();
                main_cam.is_catching_up = false;
                main_cam.scale = 1.0;

                up_acc        = 1.0;
                down_acc      = 1.0;
                left_acc      = 1.0;
                right_acc     = 1.0;
                backwards_acc = 1.0;
                forwards_acc  = 1.0;

                Player_init(&you, 1280.0 / 2.0, 720.0 / 2.0, 0.0, true, 0, 20, 40);
                you.state_change_time = t_now;
                you.on_ground = false;
            }

            if (!free_cam_is_on && 
                !(camera_locked)) {

                FreeCamera_target_set(&main_cam, you.bound.calc_position_center());
                FreeCamera_target_follow(&main_cam, t_delta_s);
                //up_acc        = 1.0;
                //down_acc      = 1.0;
            }
        }



        // AUDIO TEST

            usize audio_tracks = 0x0000000000000000; 
            if (key_is_pressed(&input, CONTROL::AUDIO_TRACK_1)) {
                BSET_ADD(audio_tracks, 0);
            }
            if (key_is_pressed(&input, CONTROL::AUDIO_TRACK_2)) {
                BSET_ADD(audio_tracks, 1);
            }
            if (key_is_pressed(&input, CONTROL::AUDIO_TRACK_3)) {
                BSET_ADD(audio_tracks, 2);
            }
            if (key_is_pressed(&input, CONTROL::AUDIO_TRACK_4)) {
                BSET_ADD(audio_tracks, 3);
            }
            if (key_is_pressed(&input, CONTROL::AUDIO_TRACK_5)) {
                BSET_ADD(audio_tracks, 4);
            }
            if (key_is_pressed(&input, CONTROL::AUDIO_TRACK_6)) {
                BSET_ADD(audio_tracks, 5);
            }
            if (key_is_pressed(&input, CONTROL::AUDIO_TRACK_7)) {
                BSET_ADD(audio_tracks, 6);
            }
            if (key_is_pressed(&input, CONTROL::AUDIO_TRACK_8)) {
                BSET_ADD(audio_tracks, 7);
            }
            if (key_is_pressed(&input, CONTROL::AUDIO_TRACK_9)) {
                BSET_ADD(audio_tracks, 8);
            }

            if (audio_tracks != 0) {

                Audio_Command cmd = {
                    .type = AUDIO_COMMAND_TYPE::TRACK_SELECTION,
                    .track_selection = {
                        .modified_tracks_bitmap = audio_tracks
                    }
                };

                printf("Sending track change: %llx\n", audio_tracks);

                if (ck_ring_enqueue_spsc_Audio_Command(&audio_args.fifo.ring, audio_args.fifo.buffer, &cmd) == false) {
                    fprintf(stderr, "ERROR: OUT OF AUDIO QUEUE SPACE\n");
                }
            }

            switch (key_is_toggled_4_states(&input, CONTROL::TEMP, &temp)) {
            case TOGGLE_BRANCH::PRESSED_ON: {
                Audio_Command cmd = {
                    .type = AUDIO_COMMAND_TYPE::ADJUST_MASTER_VOLUME,
                    .adjust_master_volume = {
                        .duration  = 5.0f,
                        .from      = 1.0f,
                        .to        = 0.0f,
                        .t_delta   = 0.0f,
                        .t_prev    = 0.0f,
                    }
                };

                if (ck_ring_enqueue_spsc_Audio_Command(&audio_args.fifo.ring, audio_args.fifo.buffer, &cmd) == false) {
                    fprintf(stderr, "ERROR: OUT OF AUDIO QUEUE SPACE\n");
                }
                break;
            }
            case TOGGLE_BRANCH::ON: {
                break;
            }
            case TOGGLE_BRANCH::PRESSED_OFF: {
                Audio_Command cmd = {
                    .type = AUDIO_COMMAND_TYPE::ADJUST_MASTER_VOLUME,
                    .adjust_master_volume = {
                        .duration  = 5.0f,
                        .from      = 0.0f,
                        .to        = 1.0f,
                        .t_delta   = 0.0f,
                        .t_prev    = 0.0f,
                    }
                };

                if (ck_ring_enqueue_spsc_Audio_Command(&audio_args.fifo.ring, audio_args.fifo.buffer, &cmd) == false) {
                    fprintf(stderr, "ERROR: OUT OF AUDIO QUEUE SPACE\n");
                }
                break;
            }
            case TOGGLE_BRANCH::OFF: {
                break;
            }
            default: {
                break;
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
            /*glm::translate(mat_ident, Vec3(glm::sin(((double)t_now / frequency)), 0.0, 0.0)) * */
            /*glm::scale(mat_ident, Vec3(0.25, 0.25, 1.0))* */
                        )
        );

        glUniform1f(TIME_LOC, t_since_start_s);

        Vec3 pos = main_cam.position;
        #ifdef DEBUG_PRINT

            if (pos.x != prev_pos.x || pos.y != prev_pos.y || pos.z != prev_pos.z) {
                std::cout << "VIEW_POSITION{x : " << pos.x << ", y : " << pos.y << ", z: " << pos.z << "}" << std::endl;
            }
            prev_pos.x = pos.x;
            prev_pos.y = pos.y;
            prev_pos.z = pos.z;
        #endif
        // Vec3 VV = pos * world_bguv_factor;
        // vec3_print(&VV);
        // std::cout << std::endl;
        glUniform1f(SCALE_LOC, main_cam.scale);

        glUniform3fv(CAM_LOC, 1, glm::value_ptr(pos * world_bguv_factor));
        //glEnable(GL_DEPTH_TEST);
        //glClear(GL_DEPTH_BUFFER_BIT);
        // glDepthRange(0, 1);

{
        //Vec2 P = Vec2(pos * world_bguv_factor);
        //P.y = glm::clamp(P.y, -1.45f, 1.45f);
        //f64 x_off = P.x;
        //f64 y_off = P.y;


        //Vec2 t4c = Vec2(0, 0);
        //t4c.x += (x_off / 1.0);
        //t4c.y += (y_off / 1.0);
                //vec2_println(t4c);
        //t4c = SCALED(t4c, vec2(x_off / 1.0, y_off / 1.0), scaler);
        //t4c = fract(t4c);

        //vec2_println(t4c);
}       
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glDisable(GL_DEPTH_TEST);

        
        glBindVertexArray(vao_2d2.vao);
        glDrawElements(GL_TRIANGLES, tri_data.i_count, GL_UNSIGNED_INT, 0);
        //glBindVertexArray(0);

        #ifdef SD

        glEnable(GL_DEPTH_TEST);
        glDepthRange(0, 1);
        glClear(GL_DEPTH_BUFFER_BIT);


        Mat4 cam = FreeCamera_calc_view_matrix(&main_cam);

        // drawctx.begin();

        //     //drawctx.transform_matrix = FreeCamera_calc_view_matrix(&main_cam);
        //     drawctx.transform_matrix = Mat4(1.0f);

        //     // drawctx.draw_type = GL_LINES;
        //     // drawctx.color = Color::RED;
        //     // drawctx.vertex({0.5, 0.0, -1.0});
        //     // drawctx.vertex({1.0, 1.0, -1.0});
            
        //     // drawctx.draw_type = GL_LINES;
        //     // drawctx.color = Color::GREEN;
        //     // drawctx.line({0.0, 0.0, -5.0}, {1.0, 1.0, -5.0});

        //     // drawctx.color = Color::GREEN;
        //     // drawctx.circle(0.25, {0.0, 0.0, 0.0});

        //     drawctx.draw_type = GL_TRIANGLES;

        //     GLfloat CX = (SCREEN_WIDTH / 2.0f);
        //     GLfloat CY = 384.0f;

        //     drawctx.color = Vec4(252.0f / 255.0f, 212.0f / 255.0f, 64.0f / 255.0f, 1.0f);

        //     drawctx.transform_matrix = cam;

        //     drawctx.circle(90.0f, {CX, CY, -1.0});

        //     drawctx.color = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
        //     drawctx.transform_matrix = glm::translate(cam, Vec3(CX - 27.0f, CY - 25.0f, 0.0f));
        //     drawctx.circle(10.0f, {0.0f, 0.0f, 0.0f});

        //     drawctx.color = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
        //     drawctx.transform_matrix = glm::translate(cam, Vec3(CX + 27.0f, CY - 25.0f, 0.0f));
        //     drawctx.circle(10.0f, {0.0f, 0.0f, 0.0f});

            
        //     drawctx.draw_type = GL_LINES;

        //     #define BASE_TILE_SIZE (128.0f)
        //     #define TILE_SCALE (2.0f)

        //     CX = 5.0f / TILE_SCALE;
        //     CY = 3.0f / TILE_SCALE;
            
        //     Mat4 model(1.0f);
        //     model = glm::scale(model, Vec3(BASE_TILE_SIZE * TILE_SCALE, BASE_TILE_SIZE * TILE_SCALE, 1.0f));
        //     model = glm::translate(model, Vec3({CX, CY, 0.0}));
        //     model = glm::rotate(model, (GLfloat)t_since_start, Vec3(0.0f, 0.0f, 1.0f));
        //     model = glm::translate(model, Vec3({-CX, -CY, 0.0}));
            

        //     drawctx.transform_matrix = cam * model;

        //     drawctx.color = Vec4(1.0f, 0.0f, 0.0f, 1.0f);
        //     {
        //         GLfloat off = 1.0f;
        //         // horizontal
        //         drawctx.line(Vec3(CX - off, CY - off, 0.0f), Vec3(CX + off, CY - off, 0.0f));
        //         drawctx.line(Vec3(CX - off, CY + off, 0.0f), Vec3(CX + off, CY + off, 0.0f));
        //         // vertical
        //         drawctx.line(Vec3(CX - off, CY - off, 0.0f), Vec3(CX - off, CY + off, 0.0f));
        //         drawctx.line(Vec3(CX + off, CY - off, 0.0f), Vec3(CX + off, CY + off, 0.0f));
        //     }   



        // drawctx.end();

        #endif


        #ifdef EDITOR

        if (key_is_toggled(&input, CONTROL::EDIT_MODE, &grid_toggle)) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glClear(GL_DEPTH_BUFFER_BIT);


            glUseProgram(shader_grid);

            glUniform1f(glGetUniformLocation(shader_grid, "u_time"), t_since_start_s);
            glUniform1f(SCALE_LOC_GRID, main_cam.scale);

            if (key_is_pressed(&input, CONTROL::ZOOM_IN)) {
                grid_square_pixel_size *= 2;
                grid_square_pixel_size = glm::clamp(grid_square_pixel_size, 4.0f, 128.0f);

                glUniform1f(SQUARE_PIXEL_LOC_GRID, tex_res.x / grid_square_pixel_size);
            } else if (key_is_pressed(&input, CONTROL::ZOOM_OUT)) {
                grid_square_pixel_size /= 2;
                grid_square_pixel_size = glm::clamp(grid_square_pixel_size, 4.0f, 128.0f);

                glUniform1f(SQUARE_PIXEL_LOC_GRID, tex_res.x / grid_square_pixel_size);
            }


            glUniformMatrix4fv(MAT_LOC_GRID, 1, GL_FALSE, glm::value_ptr(
                    mat_projection
                )
            );

            glUniform3fv(CAM_LOC_GRID, 1, glm::value_ptr(pos));


            glBindVertexArray(vao_2d2.vao);

            glDrawElements(GL_TRIANGLES, tri_data.i_count, GL_UNSIGNED_INT, 0);

            //Mat4 mat_screen_to_world = FreeCamera_calc_view_matrix_reverse(&main_cam);

            Vec4 mouse = Vec4((int)input.mouse_x, (int)input.mouse_y, 0.0f, 1.0f);
                      mouse = glm::inverse(cam) * mouse;

            i32 grid_len = (tex_res.x / grid_square_pixel_size);


            // puts("{");
            // puts("BEFORE SNAP: ");
            //     vec2_println(Vec2(mouse));
            // puts("AFTER SNAP: ");
            //     vec2_println(Vec2(snap_to_grid(mouse.x, grid_len), snap_to_grid(mouse.y, grid_len)));
            // puts("}\n");

            // {// SCALING WITH MOUSE INPUT IS NOT CORRECT, WILL LIKELY NEED TO CHANGE A LOT
            //     Vector2 o_mouse = vec2(mouse);

            //     {
            //         // manual
            //         Vector2 m2 = o_mouse;
            //         Vector2 c1 = vec2(main_cam.position);

            //         m2 -= (c1 + ((vec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2))));
            //         m2 /= main_cam.scale;
            //         m2 += (c1 + ((vec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2))));

            //         mouse = vec4(m2, 1.0, 1.0);
            //         std::cout << "{ORIGINAL:" << std::endl;
            //         vec2_print(&o_mouse);
            //         std::cout << std::endl;
            //     }
            //     // matrix

            //     // snap the mouse to the default grid
            //     vec2 m = vec2(
            //         o_mouse.x, //snap_to_grid(o_mouse.x, grid_len),
            //         o_mouse.y //snap_to_grid(o_mouse.y, grid_len)
            //     );

            //     std::cout << "PRE TRANSFORM:" << std::endl;
            //     vec2_print(&m);
            //     std::cout << "}" << std::endl;

            //     vec2 cam = vec2(main_cam.position);

            //     std::cout << "CAM: " << std::endl;
            //     vec2_println(cam);

            //     m -= (cam + main_cam.offset);
            //     m /= main_cam.scale;
            //     m += (cam + main_cam.offset);

            //     std::cout << "POST TRANSFORM:" << std::endl;
            //     vec2_print(&m);
            //     std::cout << "}" << std::endl;


            //     {
            //         std::cout << "QUICK: " << std::endl;

            //         // vec2 m_raw = vec2((int)input.mouse_x, (int)input.mouse_y);

            //         #define LEN_TEST (8)
            //         vec2 m_raw = Vec2(
            //             ((float)input.mouse_x / SCREEN_WIDTH) * LEN_TEST, 
            //             ((float)input.mouse_y / SCREEN_HEIGHT) * LEN_TEST);

            //         mat4 mat = mat4(1.0f);

            //         #define POS 0, 0
            //         mat = glm::translate(mat, Vec3(Vec2(POS) + Vec2(LEN_TEST / 2), 0));
            //         mat = glm::scale(mat, Vec3(1.0 / main_cam.scale, 1.0 / main_cam.scale, 1));
            //         mat = glm::translate(mat, -Vec3(Vec2(POS) + Vec2(LEN_TEST / 2), 0));
            //         mat = glm::translate(mat, Vec3(Vec2(POS), 0));


            //         mat4 mat2 = glm::translate(Mat4(1.0f), Vec3(Vec2(POS) + Vec2(LEN_TEST / 2), 0)) *
            //                     glm::scale(Vec3(1.0 / main_cam.scale, 1.0 / main_cam.scale, 1)) *
            //                     glm::translate(-Vec3(Vec2(POS) + Vec2(LEN_TEST / 2), 0)) *
            //                     glm::translate(Vec3(Vec2(POS), 0));


            //         if (mat == mat2) {
            //             puts("WEE");
            //             mat = mat2;
            //         } else {
            //             puts("NO");
            //         }

            //         #undef POS
            //         #undef LEN_TEST

            //         vec2 out = Vec2(mat * Vec4(m_raw, 0, 1));

            //         std::cout << "BEFORE SNAP: " << std::endl;

            //         vec2_println(out);

            //         out.x = snap_to_grid_f(out.x, 1);
            //         out.y = snap_to_grid_f(out.y, 1);

            //         vec2_println(out);
            //         std::cout << std::endl << std::endl;
            //     }
            // }



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



            // vec4_print(&mouse);
            // std::cout << std::endl;
            // vec3_print(&main_cam.position);
            // std::cout << std::endl;
            //printf("CURSOR_SNAPPED: [x: %d, y: %d]\n", snap_to_grid(mouse.x, grid_len / main_cam.scale), snap_to_grid(mouse.y, grid_len / main_cam.scale));

            if (mouse_is_toggled(&input, MOUSE_BUTTON::RIGHT, &deletion)) {
                drawing = false;

                existing.begin();
                existing.transform_matrix = cam;

                existing.draw_type = sd::LINES;
                //existing.transform_matrix = cam;
                existing.end_no_reset();

                glClear(GL_DEPTH_BUFFER_BIT);

                if (mouse_is_pressed(&input, MOUSE_BUTTON::LEFT)) {
                    in_prog.begin();

                    {
                        in_prog.draw_type = sd::TRIANGLES;
                        in_prog.transform_matrix = cam;
                        in_prog.color = Color::RED;
                        sd::circle(
                            &in_prog,
                            10.0f * (1.0 / main_cam.scale),
                            Vec3(
                                mouse.x, 
                                mouse.y,
                                1.0f
                            ),
                            32
                        );
                    }

                    in_prog.end();



                    // Vec3 M = Vec3(mouse);
                    // vec3_print(&M);
                    // std::cout << std::endl;


                    auto it = collision_map.begin_ptr();
                    f64 min_dist = dist_to_segment_squared(it->a, it->b, mouse);
                    Collider* nearest_seg = it;
                    usize selection = 0;
                    it += 1;
                    usize idx = 1;
                    for (; it != collision_map.next_free_slot_ptr(); it += 1) {
                        f64 d2 = dist_to_segment_squared(it->a, it->b, mouse);
                        if (d2 < min_dist) {
                            min_dist = d2;
                            nearest_seg = it;
                            selection = idx;
                        }
                        idx += 1;
                    }
                    if (min_dist <= COLLIDER_MAX_SELECTION_DISTANCE * (1.0 / main_cam.scale)) {
                        in_prog.begin();
                        in_prog.draw_type = sd::LINES;
                        in_prog.transform_matrix = cam;
                        in_prog.color = Color::RED;
                        in_prog.line(nearest_seg->a, nearest_seg->b);
                        in_prog.end();

                        collision_map[selection] = collision_map[collision_map.count - 1];
                        collision_map.count -= 1;

                        sd::remove_line_swap_end(&existing, selection);
                    }
                } else {
                    in_prog.begin();

                    {
                        in_prog.draw_type = sd::TRIANGLES;
                        in_prog.transform_matrix = cam;
                        in_prog.color = Color::RED;
                        sd::circle(
                            &in_prog,
                            5.0f * (1.0 / main_cam.scale),
                            Vec3(
                                mouse.x, 
                                mouse.y,
                                1.0f
                            ),
                            32
                        );
                    }

                    in_prog.end();                    
                }

            } else {
                switch (mouse_is_toggled_4_states(&input, MOUSE_BUTTON::LEFT, &drawing)) {
                case TOGGLE_BRANCH::PRESSED_ON:
                    //printf("TOGGLED DRAWING ON\n");
                    in_progress_line[0].x = snap_to_grid(mouse.x, grid_len);
                    in_progress_line[0].y = snap_to_grid(mouse.y, grid_len);
                    in_progress_line[0].z = mouse.z;

                    collision_map.next_free_slot_ptr()->a = in_progress_line[0];
                    collision_map.next_free_slot_ptr()->a.z = 0;
                case TOGGLE_BRANCH::ON:
                    //printf("\tDRAWING\n");
                    in_progress_line[1].x = snap_to_grid(mouse.x, grid_len);
                    in_progress_line[1].y = snap_to_grid(mouse.y, grid_len);
                    in_progress_line[1].z = mouse.z;

                    collision_map.next_free_slot_ptr()->b = in_progress_line[1];
                    collision_map.next_free_slot_ptr()->b.z = 0;

                    in_prog.begin();
                    in_prog.draw_type = sd::LINES;
                    in_prog.transform_matrix = cam;
                    in_prog.color = Color::BLACK;
                    in_prog.line(in_progress_line[0], in_progress_line[1]);

                    {
                        in_prog.draw_type = sd::TRIANGLES;
                        in_prog.transform_matrix = cam;
                        in_prog.color = Color::BLUE;
                        sd::circle(
                            &in_prog,
                            5.0f * (1.0 / main_cam.scale),
                            Vec3(
                                snap_to_grid(mouse.x, grid_len), 
                                snap_to_grid(mouse.y, grid_len),
                                1.0f
                            ),
                            32
                        );
                    }

                    in_prog.end();


                    existing.begin();
                    existing.transform_matrix = cam;

                    existing.draw_type = sd::LINES;
                    //existing.transform_matrix = cam;
                    existing.end_no_reset();

                    break;
                case TOGGLE_BRANCH::PRESSED_OFF:

                    //printf("ENDING DRAWING\n");

                    collision_map.count += 1;

                    in_prog.begin();
                    {
                        in_prog.draw_type = sd::TRIANGLES;
                        in_prog.transform_matrix = cam;
                        in_prog.color = Color::GREEN;
                        sd::circle(
                            &in_prog,
                            10.0f * (1.0 / main_cam.scale),
                            Vec3(
                                snap_to_grid(mouse.x, grid_len), 
                                snap_to_grid(mouse.y, grid_len),
                                1.0f
                            ),
                            32
                        );
                    }
                    in_prog.end();

                    existing.begin();
                    existing.transform_matrix = cam;

                    existing.draw_type = sd::LINES;
                    //existing.transform_matrix = cam;

                    //sort_segment(in_progress_line);
                    
                    existing.line(in_progress_line[0], in_progress_line[1]);

                    // {
                    //     f64 dy = in_progress_line[1].y - in_progress_line[0].y;
                    //     f64 dx = in_progress_line[1].x - in_progress_line[0].x;
                    //     existing.color = Color::BLUE;

                    //     Vec3 na(-dy, dx, 0.0);
                    //     Vec3 nb(dy, -dx, 0.0);

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
                        in_prog.draw_type = sd::TRIANGLES;
                        in_prog.transform_matrix = cam;
                        in_prog.color = Color::BLUE;
                        sd::circle(
                            &in_prog,
                            5.0f * (1.0 / main_cam.scale),
                            Vec3(
                                snap_to_grid(mouse.x, grid_len), 
                                snap_to_grid(mouse.y, grid_len),
                                1.0f
                            ),
                            32
                        );
                    }
                    in_prog.end();

                    existing.begin();
                    existing.transform_matrix = cam;
                    existing.draw_type = sd::LINES;
                    //existing.transform_matrix = cam;
                    existing.end_no_reset();
                    break;
                default:
                    break;
                }
            }

            drawctx.begin();

            glClear(GL_DEPTH_BUFFER_BIT);

            drawctx.draw_type = sd::LINES;

            drawctx.color = Color::BLUE;

            drawctx.transform_matrix = cam;

            //draw_lines_from_image(&drawctx, "./test_paths/C.bmp", {Vec3(1.0f), Vec3(0.0f)});


            // f64 WEE = ((f64)(t_now - you.state_change_time)) / frequency;

            // if (/*key_is_toggled(&input, CONTROL::PHYSICS, &physics_toggle) && */!you.on_ground) {
            //     you.bound.spatial.y = you.bound.spatial.y + 1 * 9.81 * (WEE * WEE);
            // } else if (key_is_pressed(&input, CONTROL::JUMP)) { // TEMPORARY
            //     you.bound.spatial.y -= you.bound.height * 4;
            // }

            
            drawctx.end();

            drawctx.begin();

            glClear(GL_DEPTH_BUFFER_BIT);

            drawctx.color = Color::GREEN;
            drawctx.transform_matrix = cam;


            if (cmd.hot_config && air_physics_conf.fd != nullptr && key_is_pressed(&input, CONTROL::LOAD_CONFIG)) {
                if (load_config(&air_physics_conf)) {
                    physics::gravity = air_physics_conf.gravity;
                    you.initial_jump_velocity= air_physics_conf.player_initial_velocity;
                    you.initial_jump_velocity_short = air_physics_conf.player_initial_velocity_short;
                }
                //fseek(air_physics_conf.fd, 0L, SEEK_SET);
            }

            //std::cout << (GRAVITY_DEFAULT * t_delta_s * INTERVAL) << std::endl;
            //std::cout << t_delta_s * INTERVAL << std::endl;
            if (!you.on_ground) {

                //std::cout << "MULTIPLIER V1 " << (INTERVAL / t_delta_s) / 1000 << std::endl;
                //std::cout << "MULTIPLIER V2 " << (1 / (t_delta_s * REFRESH_RATE)) << std::endl;

                // TODO JUMP needs to take angles into consideration when dealing with the impulse
                if (!key_is_held(&input, CONTROL::JUMP)) {
                    if (you.velocity_air.y < you.initial_jump_velocity_short) {
                        you.velocity_air.y = you.initial_jump_velocity_short;
                    }

                }

                you.velocity_air.y += (physics::gravity * DELTA_TIME_FACTOR(t_delta_s, REFRESH_RATE));
                if (you.velocity_air.y > 16) {
                    you.velocity_air.y = 16;
                }
                you.bound.spatial.x += you.velocity_air.x;
                you.bound.spatial.y += you.velocity_air.y;

            }


            bool collided = false;

            CollisionStatus status;
            CollisionStatus_init(&status);
            for (auto it = collision_map.begin_ptr(); it != collision_map.next_free_slot_ptr(); it += 1)
            {
                //Collider_print(it);
                
                if (temp_test_collision(&you, it, &status)) {
                    //printf("COLLISION\n");
                    drawctx.line(Vec3(0.0), status.intersection);

                    you.on_ground = true;
                    // you.state_change_time = t_now;
                    collided = true;

                    //puts("COLLIDED");
                } else {
                    //puts("NO COLLISION");
                }
            }


            // Vec2 tang = .1 * angular_impulse(glm::pi<double>() / 30.0, Vec2(SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.5), Vec2(you.bound.spatial.x, you.bound.spatial.y));

            // drawctx.circle(glm::distance(Vec2(SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.5), Vec2(you.bound.calc_position_center())), Vec3(SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.5, 1.0));
            
            // drawctx.color = Vec4(1.0, 1.0, 0.0, 1.0);

            // drawctx.line(Vec2(you.bound.calc_position_center()), tang + Vec2(you.bound.calc_position_center()));

            if (!collided) {
                you.on_ground = false;
            } else {
                if (you.on_ground) {
                    if (key_is_pressed(&input, CONTROL::JUMP)) {
                        you.velocity_air.y = you.initial_jump_velocity;
                        
                        //you.velocity_air.x += tang.x;
                        //you.velocity_air.y += tang.y;


                        you.on_ground = false;

                        Audio_Command cmd = {
                            .type = AUDIO_COMMAND_TYPE::DELAY,
                            .delay = {
                                .decay = 0.4,
                                .channel_a_offset_percent = 0.0,
                                .channel_b_offset_percent = 0.05
                            }
                        };

                        if (ck_ring_enqueue_spsc_Audio_Command(&audio_args.fifo.ring, audio_args.fifo.buffer, &cmd) == false) {
                            fprintf(stderr, "ERROR: OUT OF AUDIO QUEUE SPACE\n");
                        }
                    } else {
                        you.velocity_air = Vec3(0.0);
                    }

                    if (key_is_pressed(&input, CONTROL::TEMP)) {
                        Audio_Command cmd = {
                            .type = AUDIO_COMMAND_TYPE::REVERB,
                            .reverb = {
                                0.0f
                            }
                        };

                        if (ck_ring_enqueue_spsc_Audio_Command(&audio_args.fifo.ring, audio_args.fifo.buffer, &cmd) == false) {
                            fprintf(stderr, "ERROR: OUT OF AUDIO QUEUE SPACE\n");
                        }                        
                    }
                }
                //you.bound.spatial.x = out.x - (1 * you.bound.width); <-- ENABLE TO MAKE THE FLOOR A TREADMILL
                you.bound.spatial.y = status.intersection.y - (1 * you.bound.height);

                Collider* col = status.collider;
                Vec3* a = &col->a;
                Vec3* b = &col->b;
                you.bound.spatial.w = atan2_64(b->y - a->y, b->x - a->x);

                // draw surface and normals
                if (key_is_toggled(&input, CONTROL::EDIT_VERBOSE, &verbose_view_toggle)) {
                    f64 dy = col->b.y - col->a.y;
                    f64 dx = col->b.x - col->a.x;

                    auto na = Vec3{-dy, dx, 0.0};
                    auto nb = Vec3{dy, -dx, 0.0};

                    //na = glm::normalize(na);
                    //nb = glm::normalize(nb);

                    drawctx.color = Color::CYAN;
                    sd::line(&drawctx, status.collider->a, status.collider->b);

                    drawctx.color = Color::BLUE;
                    sd::line(&drawctx,/* na + */col->a, nb + col->a);

                    
                    //existing.color = Color::BLACK;
                    //vec3_pair_print(&na, &nb);

                    //std::cout << glm::degrees(you.bound.spatial.w) << std::endl;
                }

            }

            drawctx.color = Color::BLUE;
            draw_player_collision(&you, &drawctx);

            drawctx.end();

            glDisable(GL_BLEND);

            // if (collision_map.count > 0) {
            //     printf("{");
            //     for (auto it = collision_map.begin(); it != collision_map.next_free_slot_ptr(); ++it)
            //     {
            //         printf("\n");
            //         Collider_print(it);
            //     }
            //     printf("\n}\n");
            // }

            //existing.projection_matrix = mat_projection * FreeCamera_calc_view_matrix(&main_cam);
            //existing.render(&existing);
            //in_prog.transform_matrix = FreeCamera_calc_view_matrix(&main_cam);
            //in_prog.render(&in_prog);
            sd::batch_render(&in_prog);
        }

        //drawctx.transform_matrix = FreeCamera_calc_view_matrix(&main_cam);
        //drawctx.render(&drawctx);
        sd::batch_render(&drawctx);

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

    mal_device_uninit(&audio_system.device);
    mal_decoder_uninit(&audio_args.decoders[0]);

    if (air_physics_conf.fd != nullptr) {
        fclose(air_physics_conf.fd);
    }
    
    Vertex_Attribute_Array_delete(&vao_2d2);
    Vertex_Buffer_Data_delete_inplace(&tri_data);
    #ifdef SD
    sd::free(&drawctx);
    #endif
    #ifdef EDITOR
    sd::free(&in_prog);
    sd::free(&existing);
    Shader_deinit_program(shader_grid);
    #endif
    Shader_deinit_program(shader_2d);

    SDL_GL_DeleteContext(program_data.context);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;

    #endif
}
