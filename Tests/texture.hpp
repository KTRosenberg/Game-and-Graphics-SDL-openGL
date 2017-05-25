#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <string>
#include <iostream>

class Texture {
    GLuint id;
public:
    ~Texture(void);
    
    GLboolean load(std::string path, GLboolean alpha=GL_TRUE);
    
    operator GLuint() 
    {
        return this->id;
    }
};

#endif // TEXTURE_HPP
