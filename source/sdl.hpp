#ifndef SDL_HPP
#define SDL_HPP

#ifdef _WIN32
#  define SDL_MAIN_HANDLED
#	include <SDL.h>
#	include <SDL_image.h>
#else // __APPLE__
#	include <SDL2/SDL.h>
#	include <SDL2/SDL_image.h>
#endif

#endif // SDL_HPP