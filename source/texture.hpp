#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "opengl.hpp"

#include "sdl.hpp"

#include <string>
#include <iostream>

namespace GL {
    GLboolean load_texture(Texture* texture_id, std::string path, GLboolean alpha=GL_TRUE);
}

#endif // TEXTURE_HPP
