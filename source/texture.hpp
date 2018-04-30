#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "opengl.hpp"

#include "sdl.hpp"

#include <string>

 GLboolean GL_texture_load(Texture* texture_id, const char* const path, const GLboolean alpha);
 GLboolean GL_texture_gen_and_load_1(Texture* texture_id, const char* const path, const GLboolean alpha);

#endif // TEXTURE_HPP
