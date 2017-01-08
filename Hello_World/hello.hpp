#ifndef HELLO_HPP
#define HELLO_HPP

#include <SDL2/SDL.h>
#include <cstdio>
#include <cstdlib>
#include <string>

#define WAIT_MILLI 7000

#define PRINTOUT 0

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int IMG_PROPORTION = 4;

void sdl_quit_on_error_gracefully(std::string message);

#endif // HELLO_HPP
