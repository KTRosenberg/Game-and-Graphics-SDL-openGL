#ifndef CORE_UTILS_H
#define CORE_UTILS_H

// #ifdef __cplusplus
// extern "C" 
// {
// #endif

#include <ostream>

#include "common_utils.h"
#include "common_utils_cpp.h"
#include "opengl.hpp"

enum struct MOVEMENT_DIRECTION : unsigned char 
{
    FORWARDS,
    BACKWARDS,
    LEFTWARDS,
    RIGHTWARDS,
    UPWARDS,
    DOWNWARDS
};

namespace input_sys {

enum struct CONTROL {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    EDIT_MODE,
    EDIT_VERBOSE,
    ZOOM_IN,
    ZOOM_OUT,
    RESET_POSITION,
    PHYSICS,
    FREE_CAM,

    COUNT
};

enum struct MOUSE_BUTTON {
    LEFT,
    RIGHT,

    COUNT
};

#define InputStateInfo(type, count, element_type) \
element_type type##_curr[(usize)count]; \
element_type type##_prev[(usize)count]

// struct KeyState {
//     u8 curr[(usize)CONTROL::COUNT];
//     u8 prev[(usize)CONTROL::COUNT];
//     u8 togg[(usize)CONTROL::COUNT];
// };



struct Input {
    InputStateInfo(keys, CONTROL::COUNT, u8);
    InputStateInfo(mouse, MOUSE_BUTTON::COUNT, u8);
    i32 mouse_x;
    i32 mouse_y;
};


typedef bool Toggle;


// pressed : +2
// on      : +1
// off     : +0
enum struct TOGGLE_BRANCH : u8 {
    PRESSED_ON  = 3,
    PRESSED_OFF = 2,
    ON          = 1,
    OFF         = 0,


    COUNT = 4
};


// static inline INPUT_STATE key_state(struct Input* input, CONTROL key);
// static inline INPUT_STATE mouse_state(struct Input* input, MOUSE_BUTTON mouse_button);

static inline void init(struct Input* input);
//static inline bool key_toggled(struct Input* input, CONTROL key);



static void keys_advance_history(struct Input* in);


static inline void key_set_up(struct Input* in, CONTROL key);

static inline void key_set_down(struct Input* in, CONTROL key);

static inline bool key_is_pressed(struct Input* in, CONTROL key);

static inline bool key_is_held(struct Input* in, CONTROL key);

static inline bool key_is_released(struct Input* in, CONTROL key);

static inline bool key_is_toggled(struct Input* in, CONTROL key);

static inline bool key_is_toggled_and_pressed(struct Input* in, CONTROL key);

static inline TOGGLE_BRANCH key_is_toggled_4_states(struct Input* in, CONTROL key, Toggle* t);



static void mouse_advance_history(struct Input* in);

static inline void mouse_set_up(struct Input* in, MOUSE_BUTTON mouse_button);

static inline void mouse_set_down(struct Input* in, MOUSE_BUTTON mouse_button);

static inline bool mouse_is_pressed(struct Input* in, MOUSE_BUTTON mouse_button);

static inline bool mouse_is_held(struct Input* in, MOUSE_BUTTON mouse_button);

static inline bool mouse_is_released(struct Input* in, MOUSE_BUTTON mouse_button);

static inline bool mouse_is_toggled(struct Input* in, MOUSE_BUTTON mouse_button);

static inline bool mouse_is_toggled_and_pressed(struct Input* in, MOUSE_BUTTON mouse_button, Toggle* t);


static inline TOGGLE_BRANCH mouse_is_toggled_4_states(struct Input* in, MOUSE_BUTTON mouse_button, Toggle* t);



static void keys_print(struct Input* in);

}

struct WindowState {
    bool focused;
    bool minimized;
    bool restored;
};


inline i32 snap_to_grid(i32 val_x, i32 len);


struct BoxComponent {
    glm::vec4 spatial;
    f64 width;
    f64 height;

    inline f32 position_x(void)
    {
        return this->spatial.x;
    }

    inline f32 position_y(void)
    {
        return this->spatial.y;
    }

    inline f32 position_z(void)
    {
        return this->spatial.z;
    }

    inline glm::vec3 position(void)
    {
        return glm::vec3(this->spatial);
    }

    inline f32 angle(void)
    {
        return this->spatial.w;
    }

    inline glm::vec2 calc_position_center(void)
    {
        return glm::vec2(this->spatial.x + (this->width / 2.0f), this->spatial.y + (this->height / 2));
    }

    inline void position_set(f64 x, f64 y)
    {
        this->spatial.x = x;
        this->spatial.y = y;
    }

    inline void position_set_center(f64 x, f64 y)
    {
        const f64 half_width = this->width / 2;
        const f64 half_height = this->height / 2;
        this->spatial.x = x - half_width;
        this->spatial.y = y - half_height;
    }

    inline void position_set_center(void)
    {
        const f64 half_width = this->width / 2;
        const f64 half_height = this->height / 2;
        this->spatial.x -= half_width;
        this->spatial.y -= half_height;
    }

    inline void orientation_set(f64 angle)
    {
        this->spatial.w = angle;
    }
};

void BoxComponent_init(f64 x, f64 y, f64 z, f64 angle, f64 width, f64 height);


#define PLAYER_BASE_SPEED (0.01 * 360.0f)
#define PLAYER_MAX_SPEED (32.0)

struct Player {
    BoxComponent bound;
    bool on_ground;
    f64 state_change_time;
    glm::vec2 speed_ground;
    glm::vec2 speed_air;

    inline std::pair<glm::vec4, glm::vec4> floor_sensors(void)
    {
        return {
            // TODO each might have angle for w component
            glm::vec4(
                this->bound.spatial.x + (this->bound.width / 2) - 8.0, 
                this->bound.spatial.y + (this->bound.height / 2),
                this->bound.spatial.z, 
                0.0f
            ), 
            glm::vec4(
                this->bound.spatial.x + (this->bound.width / 2) + 8.0, 
                this->bound.spatial.y + (this->bound.height / 2),
                this->bound.spatial.z, 
                0.0f
            )
        };
    }

    inline std::pair<
        std::pair<glm::vec3, glm::vec3>, 
        std::pair<glm::vec3, glm::vec3>
    >
    floor_sensor_rays(void)
    {
        return {
            {
                glm::vec3(
                    (this->bound.spatial.x + (this->bound.width / 2)) - 9.0, 
                    this->bound.spatial.y + (this->bound.height / 2),
                    this->bound.spatial.z 
                ),
                glm::vec3(
                    (this->bound.spatial.x + (this->bound.width / 2)) - 9.0, 
                    this->bound.spatial.y + this->bound.height + (this->bound.height * 0.4),
                    this->bound.spatial.z
                )
            },
            {
                glm::vec3(
                    (this->bound.spatial.x + (this->bound.width / 2)) + 9.0, 
                    this->bound.spatial.y + (this->bound.height / 2),
                    this->bound.spatial.z 
                ),
                glm::vec3(
                    (this->bound.spatial.x + (this->bound.width / 2)) + 9.0, 
                    this->bound.spatial.y + this->bound.height + (this->bound.height * 0.4),
                    this->bound.spatial.z 
                )
            }            
        };
    }

    // inline std::pair<std::pair<glm::vec4>> floor_sensor_rays(void)
    // {
    //     return {
    //         {
    //             glm::vec4(
    //                 this->bound.spatial.x + (this->bound.width / 2) - 8.0, 
    //                 this->bound.spatial.y + (this->bound.height),
    //                 this->bound.spatial.z, 
    //                 0.0f
    //             ),
    //             glm::vec4(
    //                 this->bound.spatial.x + (this->bound.width / 2) - 8.0, 
    //                 this->bound.spatial.y + (this->bound.height + this->bound + heigh),
    //                 this->bound.spatial.z, 
    //                 0.0f                    
    //             )

    //         },
    //         {
    //             glm::vec4(
    //                 this->bound.spatial.x + (this->bound.width / 2) + 8.0, 
    //                 this->bound.spatial.y + (this->bound.height),
    //                 this->bound.spatial.z, 
    //                 0.0f
    //             )

    //         }
    //     }
    // }


};

void Player_init(Player* pl);

void Player_init(Player* pl, f64 x, f64 y, f64 z, bool position_at_center, f64 angle, f64 width, f64 height);

void Player_move_test(Player* you, MOVEMENT_DIRECTION direction, GLfloat delta_time);

// #ifdef __cplusplus
// }
// #endif
#endif


#ifdef CORE_UTILS_IMPLEMENTATION

// #ifdef __cplusplus
// extern "C" 
// {
// #endif

namespace input_sys {

static void keys_advance_history(struct Input* in)
{
    u8* curr = in->keys_curr;
    u8* prev = in->keys_prev;
    foreach (i, CONTROL::COUNT) {
        prev[i] = curr[i];
    }
}

static inline void key_set_up(struct Input* in, CONTROL key)
{
    in->keys_curr[cast(u8, key)] = 0x00;
}

static inline void key_set_down(struct Input* in, CONTROL key)
{
    in->keys_curr[cast(u8, key)] = 0x01;
}

static inline bool key_is_pressed(struct Input* in, CONTROL key)
{
    return in->keys_curr[cast(u8, key)] && !in->keys_prev[cast(u8, key)];
}

static inline bool key_is_held(struct Input* in, CONTROL key)
{
    return in->keys_curr[cast(u8, key)];
}

static inline bool key_is_released(struct Input* in, CONTROL key)
{
    return !in->keys_curr[cast(u8, key)] && in->keys_prev[cast(u8, key)];  
}

static inline bool key_is_toggled(struct Input* in, CONTROL key, Toggle* t)
{
    if (key_is_pressed(in, key)) {
        *t = !*t;
    }
    return *t; 
}

static inline bool key_is_toggled_and_pressed(struct Input* in, CONTROL key, Toggle* t)
{
    if (key_is_pressed(in, key)) {
        *t = !*t;
        return *t;
    }
    return false; 
}

// pressed : +2
// on      : +1
// off     : +0

// pressed + on      : 3
// pressed + off     : 2
// not-pressed + on  : 1
// not-pressed + off : 0
static inline TOGGLE_BRANCH key_is_toggled_4_states(struct Input* in, CONTROL key, Toggle* t)
{
    u8 state = 0;
    
    if (key_is_pressed(in, key)) {

        state = 2;

        *t = !*t;
    }

    state += (*t);

    return (TOGGLE_BRANCH)state; 
}


static void mouse_advance_history(struct Input* in)
{
    u8* curr = in->mouse_curr;
    u8* prev = in->mouse_prev;
    foreach (i, MOUSE_BUTTON::COUNT) {
        prev[i] = curr[i];
    }
}

static inline void mouse_set_up(struct Input* in, MOUSE_BUTTON mouse_button)
{
    in->mouse_curr[cast(u8, mouse_button)] = 0x00;
}

static inline void mouse_set_down(struct Input* in, MOUSE_BUTTON mouse_button)
{
    in->mouse_curr[cast(u8, mouse_button)] = 0x01;
}

static inline bool mouse_is_pressed(struct Input* in, MOUSE_BUTTON mouse_button)
{
    return in->mouse_curr[cast(u8, mouse_button)] && !in->mouse_prev[cast(u8, mouse_button)];
}

static inline bool mouse_is_held(struct Input* in, MOUSE_BUTTON mouse_button)
{
    return in->mouse_curr[cast(u8, mouse_button)];
}

static inline bool mouse_is_released(struct Input* in, MOUSE_BUTTON mouse_button)
{
    return !in->mouse_curr[cast(u8, mouse_button)] && in->mouse_prev[cast(u8, mouse_button)];  
}

static inline bool mouse_is_toggled(struct Input* in, MOUSE_BUTTON mouse_button, Toggle* t)
{
    if (mouse_is_pressed(in, mouse_button)) {
        *t = !*t;
    }
    return *t; 
}

static inline bool mouse_is_toggled_and_pressed(struct Input* in, MOUSE_BUTTON mouse_button, Toggle* t)
{
    if (mouse_is_pressed(in, mouse_button)) {
        *t = !*t;
        return *t;
    }
    return false; 
}


// pressed : +2
// on      : +1
// off     : +0

// pressed + on      : 3
// pressed + off     : 2
// not-pressed + on  : 1
// not-pressed + off : 0
static inline TOGGLE_BRANCH mouse_is_toggled_4_states(struct Input* in, MOUSE_BUTTON mouse_button, Toggle* t)
{
    u8 state = 0;
    
    if (mouse_is_pressed(in, mouse_button)) {

        state = 2;

        *t = !*t;
    }

    state += (*t);

    return (TOGGLE_BRANCH)state; 
}

static void keys_print(struct Input* in)
{
    u8* k_curr = in->keys_curr;
    u8* k_prev = in->keys_prev;
    printf("{\n\tCURR: [ ");
    for (usize i = 0; i < (usize)CONTROL::COUNT; ++i) {
        printf("%u, ", k_curr[i]);
    }
    printf("]\n\tPREV: [ ");
    for (usize i = 0; i < (usize)CONTROL::COUNT; ++i) {
        printf("%u, ", k_prev[i]);        
    }
    printf("]\n}\n");
}

static inline void init(struct Input* input) 
{
    memset(input, 0x00, sizeof(struct Input));
}


}

inline i32 snap_to_grid(i32 val_x, i32 len)
{
    return glm::round((f64)val_x / (f64)len) * len;
}


void BoxComponent_init(BoxComponent* bc, f64 x, f64 y, f64 z, f64 angle, f64 width, f64 height)
{
    bc->spatial.x = x;
    bc->spatial.y = y;
    bc->spatial.z = z;
    bc->spatial.w = angle;
    bc->width = width;
    bc->height = height;
}

void Player_init(Player* pl /*, f64 x, f64 y, f64 z, f64 angle, f64 width, f64 height*/)
{
    // BoxComponent_init(&pl->bound, x, y, z, angle, width, height);
    // pl->on_ground = false;
    // pl->state_change_time = 0.0;
    // pl->speed_ground = glm::vec3(0.0);
    // pl->speed_air = glm::vec3(0.0);

    memset(pl, 0x00, sizeof(Player));
}

void Player_init(Player* pl, f64 x, f64 y, f64 z, bool position_at_center, f64 angle, f64 width, f64 height)
{
    BoxComponent_init(&pl->bound, x, y, z, angle, width, height);
    if (position_at_center) {
        pl->bound.position_set_center();
    }

    pl->on_ground = false;
    pl->state_change_time = 0.0;
    pl->speed_ground = glm::vec2(0.0);
    pl->speed_air = glm::vec2(0.0);
}

void Player_move_test(Player* you, MOVEMENT_DIRECTION direction, GLfloat delta_time)
{
    const GLfloat velocity = glm::min(PLAYER_BASE_SPEED * delta_time, PLAYER_MAX_SPEED);
    glm::vec4* p = &you->bound.spatial;

    //printf("%f\n", velocity);

    //#define DB
    switch (direction) {
    case MOVEMENT_DIRECTION::FORWARDS:
        #ifdef DB 
        std::cout << "FORWARDS" << std::endl; 
        #endif
        p->z -= velocity;
        break;
    case MOVEMENT_DIRECTION::BACKWARDS:
        #ifdef DB 
        std::cout << "BACKWARDS" << std::endl;
        #endif
        p->z += velocity;
        break;
    case MOVEMENT_DIRECTION::LEFTWARDS:
        #ifdef DB 
        std::cout << "LEFTWARDS" << std::endl; 
        #endif
        p->x -= velocity;
        break;
    case MOVEMENT_DIRECTION::RIGHTWARDS:
        #ifdef DB 
        std::cout << "RIGHTWARDS" << std::endl; 
        #endif
        p->x += velocity;
        break;
    case MOVEMENT_DIRECTION::UPWARDS:
        #ifdef DB 
        std::cout << "UPWARDS" << std::endl; 
        #endif
        p->y -= velocity;
        break;
    case MOVEMENT_DIRECTION::DOWNWARDS:
        #ifdef DB 
        std::cout << "DOWNWARDS" << std::endl; 
        #endif
        p->y += velocity;
        break;
    }


    // p->x = glm::clamp(p->x, view->min_x, view->max_x);  
    // p->y = glm::clamp(p->y, view->min_y, view->max_y);  
    // p->z = glm::clamp(p->z, view->min_z, view->max_z);
    //p->x = glm::clamp(p->x, 0.0f, 720.0f);  
    //p->y = glm::clamp(p->y, 0.0f, 480.0f);  
    p->z = glm::clamp(p->z, -1.0f, 1.0f);
}


#undef CORE_UTILS_IMPLEMENTATION

// #ifdef __cplusplus
// }
// #endif

//#endif
#endif