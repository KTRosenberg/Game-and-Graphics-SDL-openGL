#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>

#include <string>
#include <iostream>

#include "file_handle.hpp"

class Shader {
    GLuint _program;
    bool _is_valid;
    Shader(void) = delete;
    bool init_program(const GLchar* vertex_path, const GLchar* fragment_path);
public:
    Shader(const std::string vertex_src, const std::string fragment_src);
    Shader(const GLchar* vertex_path, const GLchar* fragment_path);
    
    ~Shader(void);
    
    static std::string retrieve_src_from_file(const GLchar* path);
    
    GLuint program(void) const;
    void use(void) const;
    void stop_using(void) const;
    bool is_valid(void) const;
    
    
    operator GLuint() 
    {
        return this->_program;
    }
};

#endif // SHADER_HPP
