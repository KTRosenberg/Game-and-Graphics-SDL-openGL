#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "opengl.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <string>
#include <iostream>

namespace GL {
    GLboolean texture_load(Texture* texture_id, std::string path, GLboolean alpha=GL_TRUE);
}

#endif // TEXTURE_HPP
