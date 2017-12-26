#include "material.hpp"

Material::Material(const std::string vertex_src, const std::string fragment_src)
    :
        shader(nullptr),
        _num_textures(0)
{
}

Material::Material(const GLchar* vertex_path, const GLchar* fragment_path)
    :
        shader(nullptr),
        _num_textures(0)
{
}

Material::~Material(void)
{
    delete[] this->textures;
}

const GLShader* Material::get_program(void) const
{
    return this->shader;
}

void Material::init_textures(size_t num_textures)
{
    this->_num_textures = num_textures;
    this->textures = new GLuint[num_textures];
    glGenTextures(num_textures, this->textures);
}

GLuint Material::texture(size_t i) const
{
    return this->textures[i];
}

size_t Material::num_textures(void) const
{
    return this->_num_textures;
}

// void Material::set_program(const Shader* shader)
// {
//     this->shader = *shader;
// }

// void Material::set_texture(Texture texture)
// {
//     this->texture = texture;
//     this->_has_texture = true;
// }

// bool Material::has_texture(void) const;
// {
//     return this->_has_texture;
// }

