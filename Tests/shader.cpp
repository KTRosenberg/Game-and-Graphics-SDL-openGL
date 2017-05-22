#include "shader.hpp"

Shader::Shader(const GLchar* vertex_path, const GLchar* fragment_path)
{   
    std::string vertex_src_obj;
    std::string fragment_src_obj;
    
    try {
        FileHandle file_vs(vertex_path, "r");
        FileHandle file_fs(fragment_path, "r");
        
        if (!(file_vs.is_valid() & file_fs.is_valid())) {
            throw std::exception();
        }
        
        vertex_src_obj = file_vs.read();
        fragment_src_obj = file_fs.read();        
    } catch (std::exception& ex) {
        std::cout << "ERROR::SHADER_SOURCE_CANNOT_BE_FOUND" << std::endl;
        throw std::exception();
    }
    
    const GLchar* vertex_src = vertex_src_obj.c_str();
    const GLchar* fragment_src = fragment_src_obj.c_str();
    
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
        throw std::exception();
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
        throw std::exception();
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
        throw std::exception();
    } else {
        std::cout << "SHADER::PROGRAM::LINK_SUCCEEDED" << std::endl;
    }
    
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

GLuint Shader::program(void) const
{
    return this->_program;
}
    
void Shader::use(void)
{
    glUseProgram(this->_program);
}
