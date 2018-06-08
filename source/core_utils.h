#ifndef CORE_UTILS_H
#define CORE_UTILS_H

// #ifdef __cplusplus
// extern "C" 
// {
// #endif

#include "common_utils.h"
#include <ostream>

namespace input_sys {

enum class KEY {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    EDIT_GRID,
    ZOOM_IN,
    ZOOM_OUT,

    COUNT
};

enum class MOUSE_BUTTON {
    LEFT,
    RIGHT,

    COUNT
};

enum class INPUT_STATE {
    OFF      = 0,
    JUST_ON  = 1,
    HELD     = 2,

    COUNT
};

static std::string const input_state_map[(u8)INPUT_STATE::COUNT] = {
    "OFF",
    "JUST_ON", 
    "HELD" 
};

#define InputStateInfo(type, count, element_type) \
element_type type##_curr[(usize)count]; \
element_type type##_prev[(usize)count]; \
element_type type##_togg[(usize)count]

// struct KeyState {
//     u8 curr[(usize)KEY::COUNT];
//     u8 prev[(usize)KEY::COUNT];
//     u8 togg[(usize)KEY::COUNT];
// };



struct Input {
    InputStateInfo(keys, KEY::COUNT, u8);
    InputStateInfo(mouse_buttons, MOUSE_BUTTON::COUNT, u8);
    u32 mouse_x;
    u32 mouse_y;
};


// static inline INPUT_STATE key_state(struct Input* input, KEY key);
// static inline INPUT_STATE mouse_state(struct Input* input, MOUSE_BUTTON mouse_button);

static inline void init(struct Input* input);
//static inline bool key_toggled(struct Input* input, KEY key);



static void keys_advance_history(struct Input* in);

static inline void key_set_up(struct Input* in, KEY key);

static inline void key_set_down(struct Input* in, KEY key);

static inline bool key_is_pressed(struct Input* in, KEY key);

static inline bool key_is_held(struct Input* in, KEY key);

static inline bool key_is_toggled(struct Input* in, KEY key);
static inline bool key_is_toggled_pressed(struct Input* in, KEY key);
static inline bool key_is_toggled_held(struct Input* in, KEY key);

static inline bool key_is_released(struct Input* in, KEY key);

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

#define ADVANCE()  { prev[0] = curr[0]; }
#define RELEASED() (prev[0] > curr[0])
#define HELD()     (prev[0] && curr[0])
#define PRESSED()  (prev[0] < curr[0])
#define PRESS()    do { printf("PRESS\n"); curr[0] = 1; togg[0] = togg[0] ^ (!(prev[0] && curr[0])); } while (0)
#define RELEASE()  do { printf("RELEASE\n"); curr[0] = 0; } while (0)
#define PRINT() do { printf("{P : %u}, {C : %u}, {T : %u}\n", prev[0], curr[0], togg[0]); } while (0)

static void keys_advance_history(struct Input* in)
{
    u8* curr = in->keys_curr;
    u8* prev = in->keys_prev;
    foreach (i, (usize)KEY::COUNT) {
        prev[i] = curr[i];
    }
}

static inline void key_set_up(struct Input* in, KEY key)
{
    in->keys_curr[(u8)key] = 0;
}

static inline void key_set_down(struct Input* in, KEY key)
{
    u8* k_curr = &in->keys_curr[cast(u8, key)];
    u8* k_prev = &in->keys_prev[cast(u8, key)];
    u8* k_togg = &in->keys_togg[cast(u8, key)];

    *k_curr = 1;
    *k_togg ^= (!(k_curr[0] && k_prev[0]));
}

static inline bool key_is_pressed(struct Input* in, KEY key)
{
    return in->keys_curr[cast(u8, key)] && !in->keys_prev[cast(u8, key)];
}

static inline bool key_is_held(struct Input* in, KEY key)
{
    return in->keys_curr[cast(u8, key)];
}

static inline bool key_is_toggled(struct Input* in, KEY key)
{
    return in->keys_togg[cast(u8, key)]; 
}

static inline bool key_is_toggled_pressed(struct Input* in, KEY key)
{
    return in->keys_curr[cast(u8, key)] && (!in->keys_prev[cast(u8, key)]) && in->keys_togg[cast(u8, key)];
}

static inline bool key_is_toggled_held(struct Input* in, KEY key)
{
    return in->keys_curr[cast(u8, key)] && in->keys_togg[cast(u8, key)]; 
}

static inline bool key_is_released(struct Input* in, KEY key)
{
    return !in->keys_curr[cast(u8, key)] && in->keys_prev[cast(u8, key)];  
}

static void keys_print(struct Input* in)
{
    u8* k_curr = in->keys_curr;
    u8* k_prev = in->keys_prev;
    u8* k_togg = in->keys_togg;

    printf("{\n\tCURR: [ ");
    for (usize i = 0; i < (usize)KEY::COUNT; ++i) {
        printf("%u, ", k_curr[i]);
    }
    printf("]\n\tPREV: [ ");
    for (usize i = 0; i < (usize)KEY::COUNT; ++i) {
        printf("%u, ", k_prev[i]);        
    }
    printf("]\n\tTOGG: [ ");
    for (usize i = 0; i < (usize)KEY::COUNT; ++i) {
        printf("%u, ", k_togg[i]);        
    }
    printf("]\n}\n");
}

static inline void init(struct Input* input) 
{
    memset(input, 0x00, sizeof(struct Input));
}

// static inline bool key_down(struct Input* input, KEY key) 
// {
//     return false;
// }
// static inline bool key_on(struct Input* input, KEY key)
// {
//     return false;
// }
// static inline bool key_up(struct Input* input, KEY key)
// {
//     return false;
// }
// static inline bool key_toggled(struct Input* input, KEY key)
// {  
//     return false;
// }


}

// #ifdef __cplusplus
// }
// #endif

//#endif
#endif