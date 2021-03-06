#ifndef MESH_GENERATOR_H
#define MESH_GENERATOR_H

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <vector>
#include <numeric>

namespace gl_mesh {

struct MeshData {
    virtual GLuint num_vertices(void) = 0;
    virtual GLuint num_indices(void) = 0;
    virtual GLfloat* vertex_data(void) = 0;
    virtual GLuint* index_data(void) = 0;
    
    static std::vector<GLuint> create_linear_indices(GLuint n);
};


struct parametric {
    struct MeshFunction {
        virtual std::vector<GLfloat> operator() (GLfloat u, GLfloat v) = 0;
    };

    struct SphereFunction : public MeshFunction {
        virtual std::vector<GLfloat> operator() (GLfloat u, GLfloat v);
    };

    struct TorusFunction : public MeshFunction {
        virtual std::vector<GLfloat> operator() (GLfloat u, GLfloat v);
    };
    
    static void add_mesh_vertices(std::vector<GLfloat>& V, GLint m, GLint n,
                                  MeshFunction& func);

    static std::vector<GLfloat> sphere(GLint m);
    static std::vector<GLfloat> sphere(GLint m, GLint n);
    
    static std::vector<GLfloat> torus(GLint m);
    static std::vector<GLfloat> torus(GLint m, GLint n);

    struct Sphere : public MeshData {
    private:
        GLuint _num_vertices;
        GLuint _num_indices;
        std::vector<GLfloat> _vertex_data;
        std::vector<GLuint> _index_data;
        
        Sphere(void) = delete;
    public:
        Sphere(GLint m, GLint n);
        Sphere(GLint m);
        
        virtual GLuint num_vertices(void);
        virtual GLuint num_indices(void);
        virtual GLfloat* vertex_data(void);
        virtual GLuint* index_data(void);
    };
    
    struct Torus : public MeshData {
    private:
        GLuint _num_vertices;
        GLuint _num_indices;
        std::vector<GLfloat> _vertex_data;
        std::vector<GLuint> _index_data;
        
        Torus(void) = delete;
    public:
        Torus(GLint m, GLint n);
        Torus(GLint m);
        
        virtual GLuint num_vertices(void);
        virtual GLuint num_indices(void);
        virtual GLfloat* vertex_data(void);
        virtual GLuint* index_data(void);
    }; 
};

struct platonic_solid {
    struct Cube : public MeshData {
    private:
        static GLuint _num_vertices;
        static GLuint _num_indices;
        static GLfloat _vertex_data[192];
        static GLuint _index_data[36];
    public:
        virtual GLuint num_vertices(void);
        virtual GLuint num_indices(void);
        virtual GLfloat* vertex_data(void);
        virtual GLuint* index_data(void);
    };
};

}

#endif // MESH_GENERATOR_H
