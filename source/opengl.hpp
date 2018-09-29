#ifndef OPEN_GL_HPP

#ifdef _WIN32
#   include <GL\glew.h>
#   include <SDL_opengl.h>
#   include <gl\glu.h>
#else // __APPLE_
#   define GL3_PROTOTYPES 1
#   define GLEW_STATIC
#   include <GL/glew.h>
#   //include <OpenGL/glu.h>
#   include <OpenGL/gl.h>
//#   include <OpenGL/gl3.h>
#endif

#define OPEN_GL_HPP

#include "common_utils_cpp.hpp"

typedef GLuint UniformLocation;
typedef GLuint Texture;
typedef GLuint VertexArray;
typedef VertexArray VAO;
typedef GLuint VertexBuffer;
typedef VertexBuffer VBO; 
typedef GLuint ElementBuffer;
typedef ElementBuffer EBO;
typedef GLuint GLBuffer;

#define gl_get_errors() \
    do { \
        GLenum err = GL_NO_ERROR; \
        while ((err = glGetError()) != GL_NO_ERROR) { \
            fprintf(stderr, "ERROR: 0x%x\n", err); \
        } \
    } while (0) \
\

struct AttributeData {
    GLuint    index;
    GLint     size;
    GLenum    type;
    GLboolean normalized;
    GLsizei   stride;
    GLvoid*   pointer;
    GLchar*   name;
};

struct VertexAttributeArray {
    VertexArray vao;
    size_t stride;

    operator GLuint() { return vao; }
};
typedef VertexAttributeArray VAttribArr;

struct VertexBufferData {
    VertexBuffer vbo;
    ElementBuffer ebo;
    size_t    v_cap;
    size_t    v_count;
    size_t    i_cap;
    size_t    i_count;
    GLfloat*  vertices;
    GLuint*   indices;
};

struct GLData {
    VertexAttributeArray vao;
    VertexBufferData vbd;
};

struct TextureData {
    Texture* ids;
    usize count;
};



void TextureData_init(TextureData* t, const size_t id_count);
void TextureData_init_inplace(TextureData* t, const size_t id_count, Texture* buffer);
void TextureData_delete(TextureData* t);
void TextureData_delete_inplace(TextureData* t);

void AttributeData_init(
    AttributeData* a,
    GLuint index,
    GLint size,
    GLenum type,
    GLboolean normalized,
    GLsizei stride,
    GLvoid* pointer,
    GLchar* name    
);

void VertexBufferData_init(
    VertexBufferData* g,
    const size_t v_cap,
    const size_t i_cap,
    Fn_MemoryAllocator alloc_v,
    Fn_MemoryAllocator alloc_i
);
void VertexBufferData_init_inplace(
    VertexBufferData* g,
    const size_t v_cap,
    GLfloat* vertices,
    const size_t i_cap,
    GLuint* indices
);
void VertexBufferData_delete(VertexBufferData* g);
void VertexBufferData_delete_inplace(VertexBufferData* g);
void VertexBufferData_init_with_arenas(ArenaAllocator* v_arena, ArenaAllocator* i_arena, VertexBufferData* vbd, size_t v_count_elements, size_t i_count_elements);

void VertexAttributeArray_init(VertexAttributeArray* vao, size_t stride);
void VertexAttributeArray_delete(VertexAttributeArray* vao);

// TODO vertex package structs instead of all floats

#define attribute_offsetof(offset) (GLvoid*)(offset * sizeof(GLfloat))
#define attribute_sizeof(vao) vao->stride * sizeof(GLfloat)

void gl_set_and_enable_vertex_attrib_ptr(GLuint index, GLint size, GLenum type, GLboolean normalized, size_t offset, VertexAttributeArray* va);
void gl_bind_buffers_and_upload_data(VertexBufferData* vbd, GLenum usage, size_t v_cap, size_t i_cap, GLintptr v_begin_offset = 0, GLintptr i_begin_offset = 0);
void gl_bind_buffers_and_upload_data(VertexBufferData* vbd, GLenum usage, GLintptr v_begin_offset = 0, GLintptr i_begin_offset = 0);
void gl_bind_buffers_and_upload_sub_data(VertexBufferData* vbd);
void gl_bind_buffers_and_upload_sub_data(VertexBufferData* vbd, usize v_dest_offset, usize v_sub_count, GLintptr v_begin_offset, usize i_dest_offset, usize i_sub_count, GLintptr i_begin_offset);
void gl_bind_buffers_and_upload_sub_data(VertexBufferData* vbd, usize v_dest_offset, usize v_sub_count, GLintptr v_begin_offset, usize i_dest_offset, usize i_sub_count, GLintptr i_begin_offset, GLfloat* vertices, GLuint* indices);

void GLData_init(GLData* gl_data, size_t attribute_stride, const size_t v_cap, const size_t i_cap, Fn_MemoryAllocator alloc_v, Fn_MemoryAllocator alloc_i);
void GLData_init_inplace(GLData* gl_data, size_t attribute_stride, const size_t v_cap, GLfloat* vertices, const size_t i_cap, GLuint* indices);
inline void GLData_advance(GLData* const gl_data, const size_t i); // probably not useful TODO replace when using different vertex formats
void GLData_delete(GLData* gl_data);
void GLData_delete_inplace(GLData* gl_data);

#endif // OPEN_GL_HPP

#ifdef OPEN_GL_IMPLEMENTATION
#undef OPEN_GL_IMPLEMENTATION

// TEXTURES
void TextureData_init(TextureData* t, const size_t id_count) 
{
    t->ids = (Texture*)xmalloc(id_count * sizeof(t->ids));
    t->count = id_count;
    glGenTextures(t->count, t->ids);
}

void TextureData_init_inplace(TextureData* t, const size_t id_count, Texture* buffer)
{
    t->ids = buffer;
    t->count = id_count;
    glGenTextures(id_count, t->ids);
}

void TextureData_delete(TextureData* t)
{
    glDeleteTextures(t->count, t->ids);
    free(t);
}
void TextureData_delete_inplace(TextureData* t)
{
    glDeleteTextures(t->count, t->ids);
}




// ATTRIBUTES AND VERTEX ARRAYS

void AttributeData_init(
    AttributeData* a,
    GLuint index,
    GLint size,
    GLenum type,
    GLboolean normalized,
    GLsizei stride,
    GLvoid* pointer,
    GLchar* name    
) {
    a->index = index;
    a->size = size;
    a->type = type;
    a->normalized = normalized;
    a->stride = stride;
    a->pointer = pointer;
    a->name = name;
}

// #define VAO_ATTRIBUTE_MAIN_TEST_COUNT 3
// typedef struct _VertexArrayData {
//     VertexArray vao;
//     size_t attribute_count;
//     AttributeData attributes[VAO_ATTRIBUTE_MAIN_TEST_COUNT];
// } VertexArrayData;

// #define VAO_ATTRIBUTE_LIGHT_DAT_COUNT 3
// typedef struct _VertexArrayData {
//     VertexArray vao;
//     size_t attribute_count;
//     AttributeData attributes[VAO_ATTRIBUTE_MAIN_TEST_COUNT];
// } VertexArrayData;

// VERTEX BUFFERS


// struct MemoryAllocator {
//     void* type;
//     Fn_MemoryAllocator fn_alloc;
    
//     void* alloc(size_t bytes)
//     {
//         return Fn_MemoryAllocator(type, bytes);
//     }
// };

// void MemoryAllocator_init(MemoryAllocator* ma, void* type, Fn_MemoryAllocator* fn_alloc, Fn_MemoryAllocatorType_init fn_init, void* args)
// {
//     ma->fn_alloc = fn_alloc;
//     ma->type = alloc;
//     fn_init(fn_alloc, args);
// }



// struct Open_GL_Data {
//     VertexBuffer vbo;
//     ElementBuffer ebo;
//     DynamicBuffer<GLfloat> vertices;
//     DynamicBuffer<GLuint> indices;
// };

// struct VertexBufferDataAlt {
//     VertexBuffer vbo;
//     ElementBuffer ebo;
//     Buffer<GLfloat> vertices_lines;
//     Buffer<GLuint>  indices;
// } VertexBufferDataAlt;

// typedef VertexBufferData VBData;



void VertexBufferData_init(
    VertexBufferData* g,
    const size_t v_cap,
    const size_t i_cap,
    Fn_MemoryAllocator alloc_v,
    Fn_MemoryAllocator alloc_i
) {
    glGenBuffers(1, (GLBuffer*)&g->vbo);
    glGenBuffers(1, (GLBuffer*)&g->ebo);

    g->v_cap = v_cap;
    g->i_cap = i_cap;

    g->vertices = (GLfloat*)alloc_v(sizeof(GLfloat) * g->v_cap);
    g->indices  = (GLuint*)alloc_i(sizeof(GLuint) * g->i_cap);

    g->v_count = v_cap;
    g->i_count = i_cap;
}

void VertexBufferData_init_inplace(
    VertexBufferData* g,
    const size_t v_cap,
    GLfloat* vertices,
    const size_t i_cap,
    GLuint* indices
) {
    glGenBuffers(1, (GLBuffer*)&g->vbo);
    glGenBuffers(1, (GLBuffer*)&g->ebo);

    g->v_cap = v_cap;
    g->i_cap = i_cap;

    g->vertices = vertices;
    g->indices  = indices;

    g->v_count = v_cap;
    g->i_count = i_cap;
}

void VertexBufferData_delete(VertexBufferData* g)
{
    glDeleteBuffers(1, (GLBuffer*)&g->vbo);
    glDeleteBuffers(1, (GLBuffer*)&g->ebo);
    free(g->vertices);
    free(g->indices);
}
void VertexBufferData_delete_inplace(VertexBufferData* g)
{
    glDeleteBuffers(1, (GLBuffer*)&g->vbo);
    glDeleteBuffers(1, (GLBuffer*)&g->ebo);
}

void VertexBufferData_init_with_arenas(ArenaAllocator* v_arena, ArenaAllocator* i_arena, VertexBufferData* vbd, size_t v_count_elements, size_t i_count_elements) 
{
    VertexBufferData_init_inplace(
        vbd, 
        v_count_elements,
        (GLfloat*)ArenaAllocator_allocate(v_arena, v_count_elements * sizeof(GLfloat)),
        i_count_elements,
        (GLuint*)ArenaAllocator_allocate(i_arena, i_count_elements * sizeof(GLuint))
    );
}


void VertexAttributeArray_init(VertexAttributeArray* vao, size_t stride) 
{
    glGenVertexArrays(1, &vao->vao);
    vao->stride = stride;
}
void VertexAttributeArray_delete(VertexAttributeArray* vao) 
{
    glDeleteVertexArrays(1, &vao->vao);
}

void gl_set_and_enable_vertex_attrib_ptr(GLuint index, GLint size, GLenum type, GLboolean normalized, size_t offset, VertexAttributeArray* va)
{
    glVertexAttribPointer(index, size, type, normalized, attribute_sizeof(va), attribute_offsetof(offset));            
    glEnableVertexAttribArray(index);
}

void gl_bind_buffers_and_upload_data(VertexBufferData* vbd, GLenum usage, size_t v_cap, size_t i_cap, GLintptr v_begin_offset, GLintptr i_begin_offset)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbd->vbo);
    glBufferData(GL_ARRAY_BUFFER, v_cap * sizeof(GLfloat), vbd->vertices + v_begin_offset, usage);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbd->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, i_cap * sizeof(GLuint), vbd->indices + i_begin_offset, usage);            
}

void gl_bind_buffers_and_upload_data(VertexBufferData* vbd, GLenum usage, GLintptr v_begin_offset, GLintptr i_begin_offset)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbd->vbo);
    glBufferData(GL_ARRAY_BUFFER, vbd->v_cap * sizeof(GLfloat), vbd->vertices + v_begin_offset, usage);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbd->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vbd->i_cap * sizeof(GLuint), vbd->indices + i_begin_offset, usage);           
}

void gl_bind_buffers_and_upload_sub_data(VertexBufferData* vbd)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbd->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vbd->v_count * sizeof(GLfloat), vbd->vertices);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbd->ebo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vbd->i_count * sizeof(GLuint), vbd->indices);
}

void gl_bind_buffers_and_upload_sub_data(VertexBufferData* vbd, usize v_dest_offset, usize v_sub_count, GLintptr v_begin_offset, usize i_dest_offset, usize i_sub_count, GLintptr i_begin_offset)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbd->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, v_dest_offset * sizeof(GLfloat), v_sub_count * sizeof(GLfloat), vbd->vertices + v_begin_offset);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbd->ebo);
    glBufferSubData(GL_ARRAY_BUFFER, i_dest_offset * sizeof(GLuint), i_sub_count * sizeof(GLuint), vbd->indices + i_begin_offset);
}

void gl_bind_buffers_and_upload_sub_data(VertexBufferData* vbd, usize v_dest_offset, usize v_sub_count, GLintptr v_begin_offset, usize i_dest_offset, usize i_sub_count, GLintptr i_begin_offset, GLfloat* vertices, GLuint* indices)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbd->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, v_dest_offset * sizeof(GLfloat), v_sub_count * sizeof(GLfloat), vertices + v_begin_offset);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbd->ebo);
    glBufferSubData(GL_ARRAY_BUFFER, i_dest_offset * sizeof(GLuint), i_sub_count * sizeof(GLuint), indices + i_begin_offset);
}



void GLData_init(GLData* gl_data, size_t attribute_stride, const size_t v_cap, const size_t i_cap, Fn_MemoryAllocator alloc_v, Fn_MemoryAllocator alloc_i) 
{
    VertexAttributeArray_init(&gl_data->vao, attribute_stride);
    VertexBufferData_init(&gl_data->vbd, v_cap, i_cap, alloc_v, alloc_i);
}

void GLData_init_inplace(GLData* gl_data, size_t attribute_stride, const size_t v_cap, GLfloat* vertices, const size_t i_cap, GLuint* indices) 
{
    VertexAttributeArray_init(&gl_data->vao, attribute_stride);
    VertexBufferData_init_inplace(&gl_data->vbd, v_cap, vertices, i_cap, indices);    
}

inline void GLData_advance(GLData* const gl_data, const size_t i)
{
    gl_data->vbd.v_count += (i * gl_data->vao.stride);
    gl_data->vbd.i_count += i;
}

void GLData_delete(GLData* gl_data)
{
    VertexAttributeArray_delete(&gl_data->vao);
    VertexBufferData_delete(&gl_data->vbd);
}

void GLData_delete_inplace(GLData* gl_data)
{
    VertexAttributeArray_delete(&gl_data->vao);
    VertexBufferData_delete_inplace(&gl_data->vbd);    
}

#endif

