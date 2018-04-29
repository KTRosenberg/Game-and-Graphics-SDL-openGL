#include "mesh_generator.hpp"
#include <iostream>

namespace gl_mesh {

// PARAMETRIC

std::vector<GLfloat> parametric::SphereFunction::operator() (GLfloat u, GLfloat v)
{
    const double PI = glm::pi<double>();
    const double theta = 2 * PI * u;
    const double phi = PI * (v - 0.5);

    double c = glm::cos(theta) * glm::cos(phi);
    double s = glm::sin(theta) * glm::cos(phi);
    double z = glm::sin(phi);

    GLfloat pos_x = static_cast<GLfloat>(c);
    GLfloat pos_y = static_cast<GLfloat>(s);
    GLfloat pos_z = static_cast<GLfloat>(z);   

    return {
        pos_x, pos_y, pos_z, // pos
        pos_x, pos_y, pos_z, // norm
        u, v // uv
    };       
}

std::vector<GLfloat> parametric::TorusFunction::operator() (GLfloat u, GLfloat v)
{
    const double PI = glm::pi<double>();
    const double theta = 2 * PI * u;
    const double phi = 2 * PI * v;

    double r = 0.3;

    double c = glm::cos(theta) * (1 + r * glm::cos(phi));
    double s = glm::sin(theta) * (1 + r * glm::cos(phi));
    double z = r * glm::sin(phi);

    GLfloat pos_x = static_cast<GLfloat>(c);
    GLfloat pos_y = static_cast<GLfloat>(s);
    GLfloat pos_z = static_cast<GLfloat>(z);

    double nor_x_hp = r * glm::cos(phi) * glm::cos(theta);
    double nor_y_hp = r * glm::cos(phi) * glm::sin(theta);
    double nor_z_hp = r * glm::sin(phi);

    GLfloat nor_x = static_cast<GLfloat>(nor_x_hp);
    GLfloat nor_y = static_cast<GLfloat>(nor_y_hp);
    GLfloat nor_z = static_cast<GLfloat>(nor_z_hp);

    return {
        pos_x, pos_y, pos_z, // pos
        nor_x, nor_y, nor_z, // norm
        u, v // uv
    };       
}

void parametric::add_mesh_vertices(std::vector<GLfloat>& V, GLint m, GLint n, 
                                      parametric::MeshFunction& func)
{
    auto append = [&V](std::vector<GLfloat>& A) mutable
    {
        for (GLuint i = 0; i < A.size(); i++) {
            V.push_back(A[i]);
        }
    };

    for (GLint j = 0; j < n; j++) {
        for (GLint i = 0; i < m; i++) {
            std::vector<GLfloat> A = func(i / (GLfloat)m, 
                                          j / (GLfloat)n);
            std::vector<GLfloat> B = func((i + 1) / (GLfloat)m,
                                          j / (GLfloat)n);
            std::vector<GLfloat> C = func(i / (GLfloat)m, 
                                          (j + 1) / (GLfloat)n);
            std::vector<GLfloat> D = func((i + 1) / (GLfloat)m, 
                                          (j + 1) / (GLfloat)n);
            append(A);
            append(B);
            append(D);
            append(D); 
            append(C); 
            append(A);
        }
    }
}

std::vector<GLfloat> parametric::sphere(GLint m)
{
    return sphere(m, m);     
}
std::vector<GLfloat> parametric::sphere(GLint m, GLint n)
{
    std::vector<GLfloat> V;
    SphereFunction f;
    add_mesh_vertices(V, m, n, f);
    return V;     
}

///// trying objects ///////////////////////////////////////////////////////////
std::vector<GLuint> MeshData::create_linear_indices(GLuint n)
{
    std::vector<GLuint> V(n);
    std::iota(V.begin(), V.end(), 0);
    return V;
}

std::vector<GLfloat> parametric::torus(GLint m)
{
    return torus(m, m); 
}
std::vector<GLfloat> parametric::torus(GLint m, GLint n)
{
    std::vector<GLfloat> V;
    TorusFunction f;
    add_mesh_vertices(V, m, n, f);
    return V;   
}

parametric::Sphere::Sphere(GLint m) 
    : parametric::Sphere::Sphere(m, m)   
{
}
parametric::Sphere::Sphere(GLint m, GLint n)
{
    this->_vertex_data = parametric::sphere(m, n);
    this->_num_vertices = this->_vertex_data.size() / 8;
    this->_index_data = MeshData::create_linear_indices(this->_num_vertices);
    this->_num_indices = this->_index_data.size();
}

GLuint parametric::Sphere::num_vertices(void)
{
    return this->_num_vertices;
}
GLuint parametric::Sphere::num_indices(void)
{
    return this->_num_indices;
}
GLfloat* parametric::Sphere::vertex_data(void)
{
    return &(this->_vertex_data[0]);
}
GLuint* parametric::Sphere::index_data(void)
{
    return &(this->_index_data[0]);
}


parametric::Torus::Torus(GLint m)
    : parametric::Torus::Torus(m, m) 
{
}
parametric::Torus::Torus(GLint m, GLint n)
{
    this->_vertex_data = parametric::torus(m, n);
    this->_num_vertices = this->_vertex_data.size() / 8;
    this->_index_data = MeshData::create_linear_indices(this->_num_vertices);
    this->_num_indices = this->_index_data.size();
}

GLuint parametric::Torus::num_vertices(void)
{
    return this->_num_vertices;
}
GLuint parametric::Torus::num_indices(void)
{
    return this->_num_indices;
}
GLfloat* parametric::Torus::vertex_data(void)
{
    return &(this->_vertex_data[0]);
}
GLuint* parametric::Torus::index_data(void)
{
    return &(this->_index_data[0]);
}

// PLATONIC SOLIDS

GLuint platonic_solid::Cube::_num_vertices = 24;
GLuint platonic_solid::Cube::_num_indices = 36;
// front, right, back, left, top, bottom
GLfloat platonic_solid::Cube::_vertex_data[192] = {
    // pos                // normals         // uv coords                                  
    -1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f, // Top Left
    -1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // Bottom Left 
     1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, // Bottom Right
     1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, // Top Right
     
     1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, // Top Left
     1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, // Bottom Left 
     1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, // Bottom Right
     1.0f,  1.0f, -1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, // Top Right
     
     1.0f,  1.0f, -1.0f,  0.0f, 0.0f,-1.0f,  0.0f, 1.0f, // Top Left
     1.0f, -1.0f, -1.0f,  0.0f, 0.0f,-1.0f,  0.0f, 0.0f, // Bottom Left 
    -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,-1.0f,  1.0f, 0.0f, // Bottom Right
    -1.0f,  1.0f, -1.0f,  0.0f, 0.0f,-1.0f,  1.0f, 1.0f, // Top Right
    
    -1.0f,  1.0f, -1.0f, -1.0f, 0.0f, 0.0f,  0.0f, 1.0f, // Top Left
    -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, // Bottom Left 
    -1.0f, -1.0f,  1.0f, -1.0f, 0.0f, 0.0f,  1.0f, 0.0f, // Bottom Right
    -1.0f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f,  1.0f, 1.0f, // Top Right
    
    -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // Top Left
    -1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // Bottom Left 
     1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Bottom Right
     1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, // Top Right
     
    -1.0f, -1.0f,  1.0f,  0.0f,-1.0f, 0.0f,  0.0f, 1.0f, // Top Left
    -1.0f, -1.0f, -1.0f,  0.0f,-1.0f, 0.0f,  0.0f, 0.0f, // Bottom Left 
     1.0f, -1.0f, -1.0f,  0.0f,-1.0f, 0.0f,  1.0f, 0.0f, // Bottom Right
     1.0f, -1.0f,  1.0f,  0.0f,-1.0f, 0.0f,  1.0f, 1.0f  // Top Right
};
GLuint platonic_solid::Cube::_index_data[36] = {
    0, 1, 2,
    2, 3, 0,
    
    4, 5, 6,
    6, 7, 4,
    
    8, 9, 10,
    10, 11, 8,
    
    12, 13, 14,
    14, 15, 12,
    
    16, 17, 18,
    18, 19, 16,
    
    20, 21, 22,
    22, 23, 20
};

GLuint platonic_solid::Cube::num_vertices(void)
{
    return platonic_solid::Cube::_num_vertices;
}

GLuint platonic_solid::Cube::num_indices(void)
{
    return platonic_solid::Cube::_num_indices;
}

GLfloat* platonic_solid::Cube::vertex_data(void)
{
    return &(platonic_solid::Cube::_vertex_data[0]);
}

GLuint* platonic_solid::Cube::index_data(void)
{
    return &(platonic_solid::Cube::_index_data[0]);
}

}
