#ifndef SHADER_HPP
#define SHADER_HPP

#include "opengl.hpp"

#include <string>
#include <iostream>
//#include <vector>
#include <stdbool.h>

#include "file_io.hpp"

// #ifdef __cplusplus 
// extern "C" {
// #endif
//     bool init_shader(const GLchar* vertex_src, const GLchar* fragment_src)
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
    GLuint program;
    usize id;
    static usize id_pool;
        
    operator GLuint() 
    {
        return this->program;
    }
};

bool Shader_init_program(Shader* shader, const GLchar* vertex_src, const GLchar* fragment_src);

bool Shader_load_from_file(Shader* shader, const std::string& vertex_path, const std::string& fragment_path,
                        const std::string& vert_addons="",
                        const std::string& frag_addons="");

bool Shader_load_from_src(Shader* shader, const std::string& vertex_src, const std::string& fragment_src,
                        const std::string& vert_addons="",
                        const std::string& frag_addons="");
    
std::string Shader_retrieve_src_from_file(const GLchar* path, bool* is_valid);

#ifdef SHADER_IMPLEMENTATION
#undef SHADER_IMPLEMENTATION
#include "shader.cpp"
#endif

#endif // SHADER_HPP
