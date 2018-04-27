#ifndef OPEN_GL_HPP
#define GL3_PROTOTYPES 1
#define GLEW_STATIC

#define OPEN_GL_HPP

#include <GL/glew.h>
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>

//#include <OpenGL/gl3.h>

namespace GL {
   typedef GLuint UniformLocation;
   typedef GLuint Texture;
   typedef GLuint VaoBuffer;
   typedef GLuint VboBuffer;
   typedef GLuint EboBuffer;
}


#endif // OPEN_GL_HPP

