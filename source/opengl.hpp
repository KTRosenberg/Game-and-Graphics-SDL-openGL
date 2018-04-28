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

namespace GL {
   typedef GLuint UniformLocation;
   typedef GLuint Texture;
   typedef GLuint VaoBuffer;
   typedef GLuint VboBuffer;
   typedef GLuint EboBuffer;
   typedef GLuint GLSLShader;
}

#endif // OPEN_GL_HPP

