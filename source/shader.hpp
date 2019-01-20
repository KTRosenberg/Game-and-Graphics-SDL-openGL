#ifndef SHADER_HPP
#define SHADER_HPP

#if !(UNITY_BUILD)
#include "opengl.hpp"

#include <string.h>
#include <string>
//#include <vector>
#include <stdbool.h>

#include "file_io.hpp"
#endif

#ifdef SHADER_OPEN_GL

struct Shader {
    Shader_Program program;
    Dynamic_Array<Uniform_Location> uniform_locations;
    
    operator GLuint()
    {
        return this->program;
    }
};

struct Shader_Catalogue {
    Dynamic_Array<Shader> shaders;
};

bool Shader_init_program(Shader* shader, const GLchar* vertex_src, const GLchar* fragment_src);
void Shader_deinit_program(Shader& shader);
void Shader_Catalogue_deinit_programs(Shader_Catalogue& catalogue);

bool Shader_load_from_file(Shader* shader, const std::string& vertex_path, const std::string& fragment_path,
                        const std::string& vert_addons="",
                        const std::string& frag_addons="");

bool Shader_load_from_src_txt(Shader* shader, const std::string& vertex_src, const std::string& fragment_src,
                        const std::string& vert_addons="",
                        const std::string& frag_addons="");
    
std::string Shader_retrieve_src_txt_from_file(const GLchar* path, bool* is_valid);

bool set_uniform(Shader_Program* shader);
Uniform_Location get_uniform_location(Shader& program, std::string const& name); 

#endif

#ifdef SHADER_IMPLEMENTATION
#undef SHADER_IMPLEMENTATION
#include "shader_opengl.cpp"
#endif

#else
// TODO
#endif
