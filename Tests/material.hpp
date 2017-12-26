#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <GL/glew.h>

#include <vector>

#include "shader.hpp"
//#include "texture.hpp"

class Material {
private:
    GLShader* shader;
    GLuint* textures;
    size_t _num_textures;
    Material(void) = delete;
public:
    Material(const std::string vertex_src, const std::string fragment_src);
    Material(const GLchar* vertex_path, const GLchar* fragment_path);
    ~Material(void);
    
    const GLShader* get_program(void) const;
    void init_textures(size_t num_textures);
    GLuint texture(size_t i) const;
    size_t num_textures(void) const;
};

#endif // MATERIAL_HPP

