#include "playground.hpp"

void setup(SD_Data* data)
{
    //printf("%s\n", "setup()");

    sd_data = *data;
    screen_width = sd_data.screen_width;
    screen_height = sd_data.screen_height;
    pg::version_number = PLAYGROUND_VERSION;
}

/*
void draw(float32 time, float32 delta_time)
{
    // clear_color(1.0f, 1.0f, 1.0f, 1.0f);
    // color(renderer(), Vec4{0.0, 0.0, 0.0, sin01(time)});
    // quad(renderer(), 0, 
    //     Vec2{screen_width * sin01(time), 50},
    //     Vec2{screen_width * sin01(time), 100},
    //     Vec2{screen_width * sin01(time) + 50, 100},
    //     Vec2{screen_width * sin01(time) + 50, 50}
    // );
}
*/


void draw(float32 time, float32 delta_time)
{
    clear_color(Vec4{0.0f, 1.0f * (sin(time) + 1.0f) / 2.0f, 0.0f, 1.0f});




    //const float32 st = 0;
    const float32 st = sin(time) * screen_width;

    quad(renderer(), 0, Vec2{0 + st, 0}, Vec2{0 + st * .5f, screen_height}, Vec2{screen_width, screen_height}, Vec2{screen_width, 0},
        Vec4{1, 0, 0, 1}, Vec4{1, 0, 0, 1}, Vec4{0, 0, 1, 0}, Vec4{0, 0, 0, 0}
    );

    color(renderer(), Vec4{1.0f, 1.0f, 1.0f, 1.0f});
    quad(renderer(), 0, Vec2{500 - 500 * sin(time), 500}, Vec2{500, 1000}, Vec2{1000, 1000}, Vec2{1000 - 500 * sin(time), 500});

    color(renderer(), {0.1, 0.457, 62.57, 0.5f});
    triangle(renderer(), 0, {100, 100}, {100, 200}, {200, 200 + (100.0f * sin01(time))});
}





