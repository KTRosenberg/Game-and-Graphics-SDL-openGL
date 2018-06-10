#ifndef CORE_UTILS_H
#define CORE_UTILS_H

// #ifdef __cplusplus
// extern "C" 
// {
// #endif

#include "common_utils.h"
#include <ostream>

namespace input_sys {

enum struct CONTROL {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    EDIT_GRID,
    ZOOM_IN,
    ZOOM_OUT,
    RESET_POSITION,

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
    u32 mouse_x;
    u32 mouse_y;
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

// #ifdef __cplusplus
// }
// #endif

//#endif
#endif