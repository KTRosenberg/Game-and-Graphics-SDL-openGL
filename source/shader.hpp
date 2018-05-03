#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>

#include <string>
#include <iostream>
#include <vector>

#include "file_io.hpp"

// #ifdef __cplusplus 
// extern "C" {
// #endif
//     bool create_shader(const GLchar* vertex_src, const GLchar* fragment_src)
//     {
//         return false;
//     }

//     bool delete_shader(void) 
//     {
//         return false;
//     }

//     std::string retrieve_src_from_file(const GLchar* path);


//     typedef struct _ShaderData {

//     } ShaderData;

// #ifdef __cplusplus 
// }
// #endif

struct Shader {
    GLuint _program;
    bool _is_valid;
    bool init_program(const GLchar* vertex_src, const GLchar* fragment_src);


    
    bool load_from_file(const std::string& vertex_path, const std::string& fragment_path,
                            const std::string& vert_addons="",
                            const std::string& frag_addons="");
    bool load_from_src(const std::string& vertex_src, const std::string& fragment_src,
                            const std::string& vert_addons="",
                            const std::string& frag_addons="");
    
    //~Shader(void);
    void remove_program(void);
    
    static std::string retrieve_src_from_file(const GLchar* path, bool* is_valid);
    
    GLuint program(void) const;
    void use(void) const;
    void stop_using(void) const;
    bool is_valid(void) const;
    
    operator GLuint() 
    {
        return this->_program;
    }
};


typedef Shader Shader;

#endif // SHADER_HPP
