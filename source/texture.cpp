#include "texture.hpp"

namespace GL {

    GLboolean load_texture(Texture* texture_id, std::string path, GLboolean alpha)
    {
        // load image
        SDL_Surface* img = nullptr; 
        if (!(img = IMG_Load(path.c_str()))) {
            fprintf(stderr, "SDL_image could not be loaded %s, SDL_image Error: %s\n", 
                   path.c_str(), IMG_GetError());
            return GL_FALSE;
        }
        
        glGenTextures(1, texture_id);

        glBindTexture(GL_TEXTURE_2D, *texture_id);
        // image assignment
        GLuint format = (alpha) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, img->w, img->h, 0, format, GL_UNSIGNED_BYTE, img->pixels);
        glGenerateMipmap(GL_TEXTURE_2D);
        // wrapping behavior
        GLuint alpha_behavior = (alpha && false) ? GL_CLAMP_TO_EDGE : GL_REPEAT;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, alpha_behavior);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, alpha_behavior);
        // texture filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        // free the surface
        SDL_FreeSurface(img);
        
        return GL_TRUE;
    }

}
