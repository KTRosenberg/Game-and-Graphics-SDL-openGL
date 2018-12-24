#include "texture.hpp"

GLboolean GL_texture_load(Texture* texture_id, const char* const path, const GLboolean alpha, const GLint param_edge_x, const GLint param_edge_y)
{
    // load image
    printf("loading %s\n", path);
    SDL_Surface* img = NULL; 
    if (!(img = IMG_Load(path))) {
        fprintf(stderr, "SDL_image could not be loaded %s, SDL_image Error: %s\n", 
               path, IMG_GetError());
        return GL_FALSE;
    }
    

    glBindTexture(GL_TEXTURE_2D, *texture_id);
    // image assignment
    GLuint format = (alpha) ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, format, img->w, img->h, 0, format, GL_UNSIGNED_BYTE, (GLvoid*)img->pixels);
    //glGenerateMipmap(GL_TEXTURE_2D);
    // wrapping behavior
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param_edge_x);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param_edge_y);
    // texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    /*
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    */
    glBindTexture(GL_TEXTURE_2D, 0);
    // free the surface
    SDL_FreeSurface(img);
    
    return GL_TRUE;
}

GLboolean GL_texture_gen_and_load_1(Texture* texture_id, const char* const path, const GLboolean alpha, const GLint param_edge_x, const GLint param_edge_y)
{
    glGenTextures(1, texture_id);
    return GL_texture_load(texture_id, path, alpha, param_edge_x, param_edge_y);   
}
