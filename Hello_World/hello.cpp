#include "hello.hpp"

void sdl_quit_on_error_gracefully(std::string message)
{
	printf("%s: SDL_ERROR: %s\n", message.c_str(), SDL_GetError());
	SDL_Quit();
	exit(-1);
}
