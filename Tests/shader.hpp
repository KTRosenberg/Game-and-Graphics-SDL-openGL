#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>

#include <string>
#include <iostream>

#include "file_handle.hpp"

class Shader {
    GLuint _program;
    Shader(void) = delete;
public:    
    Shader(const GLchar* vertex_path, const GLchar* fragment_path);
    
    GLuint program(void) const;
    void use(void);    
};

#endif // SHADER_HPP
