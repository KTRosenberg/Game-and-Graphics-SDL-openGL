#ifndef CORE_UTILS_H
#define CORE_UTILS_H

#ifdef __cplusplus
extern "C" 
{
#endif

enum Key {
    UP,
    DOWN,
    LEFT,
    RIGHT,

    KEY_COUNT
};

enum MouseButton {
    LEFT,
    RIGHT,

    MOUSE_BUTTON_COUNT
};

struct Input {
    u8 keys[Key.KEY_COUNT];
    u8 keys_prev[Key.KEY_COUNT];

    u8 mouse[MouseButton.MOUSE_BUTTON_COUNT];
    u8 mouse_prev[MouseButton.MOUSE_BUTTON_COUNT];
    u32 mouse_x;
    u32 mouse_y;
};


INLINE bool key_just_pressed(struct Input* input, enum Key key);
INLINE bool key_is_pressed(struct Input* input, enum Key key);
INLINE bool mouse_just_down(struct Input* input, enum MouseButton mouse_button);
INLINE bool mouse_is_down(struct Input* input, enum MouseButton mouse_button);

#ifdef __cplusplus
}
#endif

#ifdef CORE_UTILS_IMPLEMENTATION

#ifdef __cplusplus
extern "C" 
{
#endif



INLINE bool key_just_pressed(struct Input* input, enum Key key)
{
    return (input->keys[key] && !input->keys_prev[key]);
}

INLINE bool key_is_pressed(struct Input* input, enum Key key)
{
    return input->keys[key];
}

INLINE bool mouse_just_down(struct Input* input, enum MouseButton mouse_button)
{
    return (input->mouse[mouse_button] && !input->mouse_prev[mouse_button]);
}

INLINE bool mouse_is_down(struct Input* input, enum MouseButton mouse_button)
{
    return (input->mouse[mouse_button]);
}

#ifdef __cplusplus
}
#endif

//#endif
#endif