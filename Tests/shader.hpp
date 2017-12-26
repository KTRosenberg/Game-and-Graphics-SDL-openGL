#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>

#include <string>
#include <iostream>
#include <vector>

#include "file_handle.hpp"

class GLShader {
    GLuint _program;
    bool _is_valid;
    bool init_program(const GLchar* vertex_src, const GLchar* fragment_src);
public:
    GLShader(void);
    
    bool load_from_file(const std::string& vertex_path, const std::string& fragment_path,
                            const std::string& vert_addons="",
                            const std::string& frag_addons="");
    bool load_from_src(const std::string& vertex_src, const std::string& fragment_src,
                            const std::string& vert_addons="",
                            const std::string& frag_addons="");
    
    //~Shader(void);
    void remove_program(void);
    
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

typedef GLShader Shader;

#endif // SHADER_HPP
