#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#if !(UNITY_BUILD)
#include "opengl.hpp"

#include "sdl.hpp"

#include <string>

#include <stdio.h>
#endif

// must generate first, then pass id
GLboolean GL_texture_load(Texture* texture_id, const char* const path, const GLboolean alpha, const GLint param_edge_x, const GLint param_edge_y);
// generates one texture automatically
GLboolean GL_texture_gen_and_load_1(Texture* texture_id, const char* const path, const GLboolean alpha, const GLint param_edge_x, const GLint param_edge_y);

#ifdef TEXTURE_IMPLEMENTATION
#undef TEXTURE_IMPLEMENTATION
#include "texture.cpp"
#endif

#endif // TEXTURE_HPP
