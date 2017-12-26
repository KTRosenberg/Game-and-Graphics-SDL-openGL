#include "shader.hpp"

GLShader::GLShader(void)
    :
        _is_valid(false)
{
}

// Shader::~Shader(void)
// {
//     glDeleteProgram(this->_program);
// }

void GLShader::remove_program(void)
{
    this->_is_valid = false;
    glDeleteProgram(this->_program);    
}


bool GLShader::load_from_file(const std::string& vertex_path, const std::string& fragment_path,
                        const std::string& vert_addons,
                        const std::string& frag_addons) 
{
    if (this->_is_valid) {
        return false;
    }
    
    return this->load_from_src(
        GLShader::retrieve_src_from_file(vertex_path.c_str()),
        GLShader::retrieve_src_from_file(fragment_path.c_str()),
        vert_addons,
        frag_addons
    );
}

bool GLShader::load_from_src(const std::string& vertex_src, const std::string& fragment_src,
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

bool GLShader::init_program(const GLchar* vertex_src, const GLchar* fragment_src)
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
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" 
                  << info_log << std::endl;
        return false;
    } else {
        std::cout << "SHADER::VERTEX::COMPILATION_SUCCEEDED" << std::endl;
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
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" 
                  << info_log << std::endl;
        return false;
    } else {
        std::cout << "SHADER::FRAGMENT::COMPILATION_SUCCEEDED" << std::endl;
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
        std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" 
                  << info_log << std::endl;
        return false;
    } else {
        std::cout << "SHADER::PROGRAM::LINK_SUCCEEDED" << std::endl;
    }
    
    glDetachShader(this->_program, vertex_shader);
    glDetachShader(this->_program, fragment_shader);
    
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    return true;  
}

std::string GLShader::retrieve_src_from_file(const GLchar* path)
{
    FileHandle shader_file(path, "r");
    if (!shader_file.is_valid()) {
        return "";
    }
    return std::move(shader_file.read());        
}

GLuint GLShader::program(void) const
{
    return this->_program;
}
    
void GLShader::use(void) const
{
    glUseProgram(this->_program);
}

void GLShader::stop_using(void) const
{
    glUseProgram(0);
}

bool GLShader::is_valid(void) const
{
    return this->_is_valid;
}
