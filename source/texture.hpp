#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "opengl.hpp"

#include "sdl.hpp"

#include <string>

// must generate first, then pass id
GLboolean GL_texture_load(Texture* texture_id, const char* const path, const GLboolean alpha, const GLint param_edge);
// generates one texture automatically
GLboolean GL_texture_gen_and_load_1(Texture* texture_id, const char* const path, const GLboolean alpha, const GLint param_edge);

#endif // TEXTURE_HPP
