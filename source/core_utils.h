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

struct Input {
    u8 keys[(Count)KEY::COUNT];

    u8 mouse[(Count)MOUSE_BUTTON::COUNT];
    u32 mouse_x;
    u32 mouse_y;
};


// static inline INPUT_STATE key_state(struct Input* input, KEY key);
// static inline INPUT_STATE mouse_state(struct Input* input, MOUSE_BUTTON mouse_button);

static inline void init(struct Input* input);
//static inline bool key_toggled(struct Input* input, KEY key);


// highest bit represents toggle state
// second-highest bit represents on state
// remaining bits represent history

static inline void keys_advance_history(struct Input* in);
static inline void key_set_up(struct Input* in, KEY key);
static inline void key_set_down(struct Input* in, KEY key);
static inline bool key_toggled(struct Input* in, KEY key);
static inline bool key_down(struct Input* in, KEY key);
static inline bool key_toggle_state_on(struct Input* in, KEY key);


// #define MOUSE_ADVANCE_HISTORY(in) do { \
//     foreach (i, (Count)MOUSE_BUTTON::COUNT) { \
//         in->mouse[i] = (in->mouse[i]) | ((in->mouse[i] & (~(1 << 7))) >> 1); \
//     } \
// } while (0)


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

static inline void keys_advance_history(struct Input* in)
{
    foreach (i, (Count)KEY::COUNT) {
        in->keys[i] = (in->keys[i]) | ((in->keys[i] & (~(1 << 7))) >> 1);
    }
}

static inline void key_set_up(struct Input* in, KEY key)
{
    u8 val = in->keys[(u8)key] & 0xBF;

    u8 fifth = (val & (1 << 5)) << 2;

    u8 comp = val ^ fifth;

    u8 negate = ~comp;

    in->keys[(u8)key] = (1 << 6) | (negate & (1 << 7));
}

static inline void key_set_down(struct Input* in, KEY key)
{
    in->keys[(u8)key] |= (1 << 6);
}

static inline bool key_toggled(struct Input* in, KEY key)
{
    return (in->keys[(u8)key] & (1 << 6)) && !(in->keys[(u8)key] & (1 << 5));
}

static inline bool key_down(struct Input* in, KEY key)
{
    return in->keys[(u8)key] & (1 << 6);
}

static inline bool key_toggle_state_on(struct Input* in, KEY key)
{
    return (in->keys[(u8)key] & (1 << 7)) && key_toggled(in, key);
}

static inline void init(struct Input* input) 
{
    memset(input, 0x80, sizeof(struct Input));
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