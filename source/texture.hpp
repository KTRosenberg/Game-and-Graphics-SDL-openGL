#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "opengl.hpp"

#include "sdl.hpp"

#include <string>

namespace GL {
    GLboolean texture_load(Texture* texture_id, const char* const path, const GLboolean alpha);
    GLboolean texture_gen_and_load_1(Texture* texture_id, const char* const path, const GLboolean alpha);
}

#endif // TEXTURE_HPP
