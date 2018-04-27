#include "shader.hpp"

using namespace file_io;

Shader::Shader(void)
    :
        _is_valid(false)
{
}

// Shader::~Shader(void)
// {
//     glDeleteProgram(this->_program);
// }

void Shader::remove_program(void)
{
    this->_is_valid = false;
    glDeleteProgram(this->_program);    
}


bool Shader::load_from_file(const std::string& vertex_path, const std::string& fragment_path,
                        const std::string& vert_addons,
                        const std::string& frag_addons) 
{
    if (this->_is_valid) {
        return false;
    }

    bool status = false;
    std::string vs = Shader::retrieve_src_from_file(vertex_path.c_str(), &status);
    if (!status) {
        return false;
    }
    std::string fs = Shader::retrieve_src_from_file(fragment_path.c_str(), &status);
    if (!status) {
        return false;
    } 
    
    return this->load_from_src(
        vs,
        fs,
        vert_addons,
        frag_addons
    );
}

bool Shader::load_from_src(const std::string& vertex_src, const std::string& fragment_src,
                        const std::string& vert_addons,
                        const std::string& frag_addons)
{
    if (this->_is_valid) {
        std::cout << "ERROR::Shader_already_loaded" << std::endl;
        return false;
    }
    
    std::string vertex_combined;
    std::string fragment_combined;
    
    if (vert_addons.length() > 0) {
        vertex_combined = vertex_src;
        std::string::size_type main_idx = vertex_combined.find("void main(");
        if (main_idx == std::string::npos) {
            return this->_is_valid = false;
        }
        vertex_combined.insert(main_idx, vert_addons);
    }
    if (frag_addons.length() > 0) {
        fragment_combined = fragment_src;
        std::string::size_type main_idx = fragment_combined.find("void main(");
        if (main_idx == std::string::npos) {
            return this->_is_valid = false;
        }
        fragment_combined.insert(main_idx, frag_addons);
    }
        
    return this->_is_valid = this->init_program(
        (vertex_combined.length() > 0) ? vertex_combined.c_str() : vertex_src.c_str(), 
        (fragment_combined.length() > 0) ? fragment_combined.c_str() : fragment_src.c_str()
    );
}

bool Shader::init_program(const GLchar* vertex_src, const GLchar* fragment_src)
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
        fprintf(stderr, "%s %s\n", "ERROR::SHADER::VERTEX::COMPILATION_FAILED", info_log);
        return false;
    } else {
        puts("SHADER::VERTEX::COMPILATION_SUCCEEDED");
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
        fprintf(stderr, "%s %s\n", "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED", info_log);
        return false;
    } else {
        puts("SHADER::FRAGMENT::COMPILATION_SUCCEEDED");
    }
       
    // create shader program
    this->_program = glCreateProgram();
    // attach vertex shader
    glAttachShader(this->_program, vertex_shader);
    // attach fragment shader
    glAttachShader(this->_program, fragment_shader);
    // link the program
    glLinkProgram(this->_program);
    
    // error checking
    glGetProgramiv(this->_program, GL_LINK_STATUS, &success);
    if (success != GL_TRUE) {
        glGetProgramInfoLog(this->_program, 512, NULL, info_log);
        fprintf(stderr, "%s %s\n", "ERROR::SHADER::PROGRAM::LINK_FAILED", info_log);
        return false;
    } else {
        puts("SHADER::PROGRAM::LINK_SUCCEEDED");
    }
    
    // glDetachShader(this->_program, vertex_shader);
    // glDetachShader(this->_program, fragment_shader);
    
    // glDeleteShader(vertex_shader);
    // glDeleteShader(fragment_shader); // TODO clean-up later
    
    return true;  
}

std::string Shader::retrieve_src_from_file(const GLchar* path, bool* is_valid)
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

GLuint Shader::program(void) const
{
    return this->_program;
}
    
void Shader::use(void) const
{
    glUseProgram(this->_program);
}

void Shader::stop_using(void) const
{
    glUseProgram(0);
}

bool Shader::is_valid(void) const
{
    return this->_is_valid;
}

