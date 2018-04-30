#ifndef OPEN_GL_HPP

#ifdef _WIN32
#   include <GL\glew.h>
#   include <SDL_opengl.h>
#   include <gl\glu.h>
#else // __APPLE_
#   define GL3_PROTOTYPES 1
#   define GLEW_STATIC
#   include <GL/glew.h>
#   include <OpenGL/glu.h>
#   include <OpenGL/gl.h>
//#   include <OpenGL/gl3.h>
#endif

#define OPEN_GL_HPP


typedef GLuint UniformLocation;
typedef GLuint Texture;
typedef GLuint VertexArray;
typedef VertexArray Vao;
typedef GLuint VertexBuffer;
typedef VertexBuffer Vbo; 
typedef GLuint ElementBuffer;
typedef ElementBuffer Ebo;
typedef GLuint GLBuffer;

#endif // OPEN_GL_HPP

