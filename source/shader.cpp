#include "shader.hpp"

using namespace file_io;

usize Shader::id_pool = 0;

bool Shader_load_from_file(Shader* shader, const std::string& vertex_path, const std::string& fragment_path,
                        const std::string& vert_addons,
                        const std::string& frag_addons) 
{

    bool status = false;
    std::string vs = Shader_retrieve_src_from_file(vertex_path.c_str(), &status);
    if (!status) {
        return false;
    }
    std::string fs = Shader_retrieve_src_from_file(fragment_path.c_str(), &status);
    if (!status) {
        return false;
    } 
    
    return Shader_load_from_src(
        shader,
        vs,
        fs,
        vert_addons,
        frag_addons
    );
}

bool Shader_load_from_src(Shader* shader, const std::string& vertex_src, const std::string& fragment_src,
                        const std::string& vert_addons,
                        const std::string& frag_addons)
{    
    std::string vertex_combined;
    std::string fragment_combined;
    
    if (vert_addons.length() > 0) {
        vertex_combined = vertex_src;
        std::string::size_type main_idx = vertex_combined.find("void main(");
        if (main_idx == std::string::npos) {
            return false;
        }
        vertex_combined.insert(main_idx, vert_addons);
    }
    if (frag_addons.length() > 0) {
        fragment_combined = fragment_src;
        std::string::size_type main_idx = fragment_combined.find("void main(");
        if (main_idx == std::string::npos) {
            return false;
        }
        fragment_combined.insert(main_idx, frag_addons);
    }
        
    return Shader_init_program(
        shader,
        (vertex_combined.length() > 0) ? vertex_combined.c_str() : vertex_src.c_str(), 
        (fragment_combined.length() > 0) ? fragment_combined.c_str() : fragment_src.c_str()
    );
}

bool Shader_init_program(Shader* shader, const GLchar* vertex_src, const GLchar* fragment_src)
{	
    // for error checking:
    GLchar info_log[512];
    GLint success = GL_FALSE;
    // VERTEX SHADER ///////////////////////////////////////////////////////////
    
    // create
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    // set source
    glShaderSource(vertex_shader, 1, &vertex_src, NULL);
    // compile vertex source
    glCompileShader(vertex_shader);
    
    // error checking
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        fprintf(stderr, "%s %s\n", "ERROR: VERTEX SHADER COMPILATION FAILED", info_log);
        return false;
    }
    
    // FRAGMENT SHADER /////////////////////////////////////////////////////////
    
    // create 
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    // set source
    glShaderSource(fragment_shader, 1, &fragment_src, NULL);
    // compile fragment source
    glCompileShader(fragment_shader);
    
    // error checking
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        fprintf(stderr, "%s %s\n", "ERROR: FRAGMENT SHADER COMPILATION FAILED", info_log);
        return false;
    }
       
    // create shader program
    shader->program = glCreateProgram();
    // attach vertex shader
    glAttachShader(shader->program, vertex_shader);
    // attach fragment shader
    glAttachShader(shader->program, fragment_shader);
    // link the program
    glLinkProgram(shader->program);
    
    // error checking
    glGetProgramiv(shader->program, GL_LINK_STATUS, &success);
    if (success != GL_TRUE) {
        glGetProgramInfoLog(shader->program, 512, NULL, info_log);
        fprintf(stderr, "%s %s\n", "ERROR: SHADER PROGRAM LINK FAILED", info_log);
        return false;
    }
    
    glDetachShader(shader->program, vertex_shader);
    glDetachShader(shader->program, fragment_shader);
    
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    return true;  
}

std::string Shader_retrieve_src_from_file(const GLchar* path, bool* is_valid)
{
    FILE* shader_fd = fopen(path, "r");
    if (shader_fd == NULL) {
        return "";
    }

    std::string out = read_file(shader_fd);

    if (ferror(shader_fd) == 0 || errno != 0) {
        *is_valid = true;
        flush_and_close_file(shader_fd);
    } else {
        *is_valid = false;
    }

    return out;
}
