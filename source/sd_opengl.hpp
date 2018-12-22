#ifndef SD_OPENGL_HPP
#define SD_OPENGL_HPP

#if !(UNITY_BUILD)
#define COMMON_UTILS_CPP_IMPLEMENTATION
#include "common_utils_cpp.hpp"

#include "opengl.hpp"
#include "shader.hpp"
#include "sdl.hpp"
#endif

// strato-draw
namespace sd {
// inline void log(const bool log_if_false, FILE * stream, const char * format, ...)
// {

// #ifdef DRAW2D_DEBUG_LOG_ON
//     va_list argptr;
//     va_start(argptr, format);
//         vfprintf(stream, format, argptr);
//     va_end(argptr);    
// #else
//     (void)log_if_false;
//     (void)stream;
//     (void)format;
// #endif
// }

// inline void log(const bool log_if_false, const char * format, ...)
// {

// #ifdef DRAW2D_DEBUG_LOG_ON
//     va_list argptr;
//     va_start(argptr, format);
//         vfprintf(stderr, format, argptr);
//     va_end(argptr);    
// #else
//     (void)log_if_false;
//     (void)format;
// #endif
// }

// inline void log(FILE * stream, const char * format, ...)
// {

// #ifdef DRAW2D_DEBUG_LOG_ON
//     va_list argptr;
//     va_start(argptr, format);
//         vfprintf(stream, format, argptr);
//     va_end(argptr);    
// #else
//     (void)stream;
//     (void)format;
// #endif
// }

// inline void log(const char * format, ...)
// {

// #ifdef DRAW2D_DEBUG_LOG_ON
//     va_list argptr;
//     va_start(argptr, format);
//         vfprintf(stderr, format, argptr);
//     va_end(argptr);    
// #else
//     (void)format;
// #endif
// }
// }

struct Vertex_Default {
    Vec3 position;
    Vec4 color;
};
typedef Vertex_Default Vertex;

struct Vertex_Textured {
    Vec3 position;
    Vec4 color;
    Vec2 uv;
};
typedef Vertex_Textured Vertex_Tex;

struct Vertex_LineSegment {
    Vec4 segment;
    Vec4 color;
    float32 z_layer;
};
typedef Vertex_LineSegment Vertex_LS;

static constexpr GLenum TRIANGLES = GL_TRIANGLES;
static constexpr GLenum LINES     = GL_LINES;


struct Render_Batch_Base {

};

template <usize SD_RENDER_BATCH_SIZE = 2048>
struct Render_Batch {
    static constexpr GLuint DEFAULT_ATTRIBUTE_STRIDE = 7;

    GLfloat vertices_triangles[SD_RENDER_BATCH_SIZE * DEFAULT_ATTRIBUTE_STRIDE]; 
    GLuint indices_triangles[SD_RENDER_BATCH_SIZE * 2];

    GLfloat vertices_lines[SD_RENDER_BATCH_SIZE * DEFAULT_ATTRIBUTE_STRIDE]; 
    GLuint indices_lines[SD_RENDER_BATCH_SIZE * 2];

    VertexAttributeArray vao_triangles;
    VertexBufferData triangle_buffer;
    VertexAttributeArray vao_lines;
    VertexBufferData line_buffer;
    Shader shader;

    UniformLocation MAT_LOC;

    Mat4 projection_matrix;
    Mat4 transform_matrix;

    Vec4 color;

    bool update_projection_matrix;

    GLenum draw_type;


    GLuint index_triangles;
    GLuint index_lines;

    bool begun;

    // TODO
    static usize next_id;
    static Dynamic_Array<usize> ids;
    usize id;

    void begin(void) 
    {
        //return;
        ASSERT(begun == false);
        
        transform_matrix = Mat4(1.0f);

        begun = true;
    }

    //template<usize SD_RENDER_BATCH_SIZE>
    void render(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx)
    {
        glUseProgram(ctx->shader);

        glUniformMatrix4fv(ctx->MAT_LOC, 1, GL_FALSE, glm::value_ptr(ctx->projection_matrix * ctx->transform_matrix));

        glBindVertexArray(ctx->vao_triangles);
        gl_bind_buffers_and_upload_sub_data(&ctx->triangle_buffer);
        if (ctx->triangle_buffer.i_count > 0) { 
            glDrawElements(GL_TRIANGLES, ctx->triangle_buffer.i_count, GL_UNSIGNED_INT, 0);
        }

        glBindVertexArray(ctx->vao_lines);
        gl_bind_buffers_and_upload_sub_data(&ctx->line_buffer);
        if (ctx->line_buffer.i_count > 0) { 
            glDrawElements(GL_LINES, ctx->line_buffer.i_count, GL_UNSIGNED_INT, 0);
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void end(void) 
    {
        //return;
        assert(begun == true);

        glUseProgram(shader);

        if (update_projection_matrix || true) {
            update_projection_matrix = false;
            glUniformMatrix4fv(MAT_LOC, 1, GL_FALSE, glm::value_ptr(projection_matrix * transform_matrix));
        }

        glBindVertexArray(vao_triangles);
        gl_bind_buffers_and_upload_sub_data(&triangle_buffer);
        if (triangle_buffer.i_count > 0) { 
            glDrawElements(GL_TRIANGLES, triangle_buffer.i_count, GL_UNSIGNED_INT, 0);
        }

        glBindVertexArray(vao_lines);
        gl_bind_buffers_and_upload_sub_data(&line_buffer);
        if (line_buffer.i_count > 0) { 
            glDrawElements(GL_LINES, line_buffer.i_count, GL_UNSIGNED_INT, 0);
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glUseProgram(0);


        triangle_buffer.v_count = 0;
        triangle_buffer.i_count = 0;

        line_buffer.v_count = 0;
        line_buffer.i_count = 0;

        index_triangles = 0;
        index_lines = 0;

        begun = false;
    }

    void end_no_reset(void) 
    {
        //return;
        assert(begun == true);


        glUseProgram(shader);

        if (update_projection_matrix || true) {
            update_projection_matrix = false;
            glUniformMatrix4fv(MAT_LOC, 1, GL_FALSE, glm::value_ptr(projection_matrix * transform_matrix));
        }

        glBindVertexArray(vao_triangles);
        gl_bind_buffers_and_upload_sub_data(&triangle_buffer);
        if (triangle_buffer.i_count > 0) { 
            glDrawElements(GL_TRIANGLES, triangle_buffer.i_count, GL_UNSIGNED_INT, 0);
        }

        glBindVertexArray(vao_lines);
        gl_bind_buffers_and_upload_sub_data(&line_buffer);
        if (line_buffer.i_count > 0) { 
            glDrawElements(GL_LINES, line_buffer.i_count, GL_UNSIGNED_INT, 0);
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glUseProgram(0);

        begun = false;        
    }

    void reset(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx)
    {
        ctx->triangle_buffer.v_count = 0;
        ctx->triangle_buffer.i_count = 0;

        ctx->line_buffer.v_count = 0;
        ctx->line_buffer.i_count = 0;

        ctx->index_triangles = 0;
        ctx->index_lines     = 0;        
    }

    static constexpr const char* const SHADER_VERTEX_PATH = "shaders/default_2d/default_2d.vrts";
    static constexpr const char* const SHADER_FRAGMENT_PATH = "shaders/default_2d/default_2d.frgs";


    bool init(Mat4 projection_matrix)
    {
        this->projection_matrix = projection_matrix;
        update_projection_matrix = false;
        begun = false;

        index_triangles = 0;
        index_lines = 0;

        draw_type = GL_TRIANGLES;

        color = Vec4(0.0f, 0.0f, 0.0f, 1.0f);

        if (false == Shader_load_from_file(
            &shader,
            SHADER_VERTEX_PATH,
            SHADER_FRAGMENT_PATH
        )) {
            SD_LOG_ERR("%s\n", "ERROR: sd::Render_Batch initialization failed");
            return false;
        }

        const usize attribute_stride = sd::Render_Batch<SD_RENDER_BATCH_SIZE>::DEFAULT_ATTRIBUTE_STRIDE;

        glUseProgram(shader);
        MAT_LOC = glGetUniformLocation(shader, "u_matrix");
        glUniformMatrix4fv(MAT_LOC, 1, GL_FALSE, glm::value_ptr(projection_matrix));
        glUseProgram(0);

        VertexAttributeArray_init(&vao_triangles, attribute_stride);
        glBindVertexArray(vao_triangles);

            VertexBufferData_init_inplace(
                &triangle_buffer, 
                SD_RENDER_BATCH_SIZE * attribute_stride,
                vertices_triangles,
                SD_RENDER_BATCH_SIZE,
                indices_triangles
            );
            triangle_buffer.v_count = 0;
            triangle_buffer.i_count = 0;

            gl_bind_buffers_and_upload_data(&triangle_buffer, GL_STREAM_DRAW);
            // POSITION
            gl_set_and_enable_vertex_attrib_ptr(0, 3, GL_FLOAT, GL_FALSE, 0, &vao_triangles);
            // COLOR
            gl_set_and_enable_vertex_attrib_ptr(1, 4, GL_FLOAT, GL_FALSE, 3, &vao_triangles);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        VertexAttributeArray_init(&vao_lines, attribute_stride);
        glBindVertexArray(vao_lines);
            VertexBufferData_init_inplace(
                &line_buffer, 
                SD_RENDER_BATCH_SIZE * attribute_stride,
                vertices_lines,
                SD_RENDER_BATCH_SIZE,
                indices_lines
            );
            line_buffer.v_count = 0;
            line_buffer.i_count = 0;

            gl_bind_buffers_and_upload_data(&line_buffer, GL_STREAM_DRAW);
            // POSITION
            gl_set_and_enable_vertex_attrib_ptr(0, 3, GL_FLOAT, GL_FALSE, 0, &vao_lines);
            // COLOR
            gl_set_and_enable_vertex_attrib_ptr(1, 4, GL_FLOAT, GL_FALSE, 3, &vao_lines);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        index_triangles = 0;
        index_lines = 0;

        return true;
    }

    void free(void)
    {
        VertexAttributeArray_delete(&vao_triangles);
        VertexAttributeArray_delete(&vao_lines);
        VertexBufferData_delete_inplace(&triangle_buffer);
        VertexBufferData_delete_inplace(&line_buffer);
        glDeleteProgram(shader);
    }

    void remove_triangle()
    {

    }

    bool line(Vec3 a, Vec3 b)
    {
        const usize v_count = line_buffer.v_count;
        const usize i_count = line_buffer.i_count;

        const usize attribute_stride = this->vao_lines.stride;

        if (v_count + (2 * attribute_stride) > (SD_RENDER_BATCH_SIZE * attribute_stride) || (i_count + 2 > SD_RENDER_BATCH_SIZE * 2)) {
            SD_LOG_ERR("%s\n", "ERROR: add_line_segment MAX LINES EXCEEDED");
            return false;
        }

        //a = Vec3(transform_matrix * Vec4(a, 1.0f));
        //b = Vec3(transform_matrix * Vec4(b, 1.0f));

        const usize v_idx = v_count;

        memcpy(&vertices_lines[v_idx], &a[0], sizeof(a[0]) * 3);
        memcpy(&vertices_lines[v_idx + 3], &color[0], sizeof(color[0]) * 4);


        memcpy(&vertices_lines[v_idx + attribute_stride], &b[0], sizeof(b[0]) * 3);
        memcpy(&vertices_lines[v_idx + attribute_stride + 3], &color[0], sizeof(color[0]) * 4);

        indices_lines[i_count] = index_lines;
        indices_lines[i_count + 1] = index_lines + 1;
        index_lines += 2;

        line_buffer.v_count += (2 * attribute_stride);
        line_buffer.i_count += 2;

        // std::cout << "BEGIN" << std::endl;
        // std::cout << "{" << std::endl;
        // for (size_t i = 0; i < line_buffer.v_count; ++i) {
        //     std::cout << line_buffer.vertices[i] << ", ";
        // }
        // std::cout << std::endl << "}" << std::endl;

        // std::cout << "{" << std::endl;
        // for (size_t i = 0; i < line_buffer.i_count; ++i) {
        //     std::cout << line_buffer.indices[i] << ", ";
        // }
        // std::cout << std::endl << "}" << std::endl;
        // std::cout << "END" << std::endl;

        return true;
    }

    bool line(Vec2 a, Vec2 b)
    {
        const usize v_count = line_buffer.v_count;
        const usize i_count = line_buffer.i_count;

        const usize attribute_stride = this->vao_lines.stride;


        if (v_count + (2 * attribute_stride) > SD_RENDER_BATCH_SIZE * attribute_stride || i_count + 2 > SD_RENDER_BATCH_SIZE * 2) {
            SD_LOG_ERR("%s\n", "ERROR: add_line_segment MAX LINES EXCEEDED");
            return false;
        }

        //a = Vec2(transform_matrix * Vec4(a, 0.0f, 1.0f));
        //b = Vec2(transform_matrix * Vec4(b, 0.0f, 1.0f));

        const usize v_idx = v_count;

        memcpy(&vertices_lines[v_idx], &a[0], sizeof(a[0]) * 2);
        vertices_lines[v_idx + 2] = 0.0f;
        memcpy(&vertices_lines[v_idx + 3], &color[0], sizeof(color[0]) * 4);


        memcpy(&vertices_lines[v_idx + attribute_stride], &b[0], sizeof(b[0]) * 2);
        vertices_lines[v_idx + attribute_stride + 2] = 0.0f;
        memcpy(&vertices_lines[v_idx + attribute_stride + 3], &color[0], sizeof(color[0]) * 4);

        indices_lines[i_count] = index_lines;
        indices_lines[i_count + 1] = index_lines + 1;
        index_lines += 2;

        line_buffer.v_count += (2 * attribute_stride);
        line_buffer.i_count += 2;
    
        return true;
    }

    void polygon_convex_regular(GLfloat radius, Vec3 center, const usize count_sides)
    {
        usize count_tris = count_sides - 2;

        usize attribute_stride;
        usize inc;

        usize v_count = 0;
        usize i_count = 0;
        usize v_idx = 0;
        GLdouble angle_turn = -(2 * PI) / count_sides;

        switch (draw_type) {
        case GL_TRIANGLES:
            attribute_stride = vao_triangles.stride;
            inc = attribute_stride;

            v_count = triangle_buffer.v_count;
            i_count = triangle_buffer.i_count;
            v_idx = v_count;

            if (v_count + (attribute_stride * count_sides) > SD_RENDER_BATCH_SIZE * attribute_stride || i_count + (3 * count_tris) > SD_RENDER_BATCH_SIZE * 2) {
                SD_LOG_ERR("%s\n", "ERROR: polygon_convex_regular MAX TRIANGLES EXCEEDED");
                return;
            }

            for (usize p = 0, idx_off = 0; p < count_tris; ++p, idx_off += 3) {
                indices_triangles[i_count + idx_off]     = index_triangles + 0;
                indices_triangles[i_count + idx_off + 1] = index_triangles + p + 1;
                indices_triangles[i_count + idx_off + 2] = index_triangles + p + 2;
            }
            triangle_buffer.i_count += (3 * count_tris);


            for (usize p = 0, off = 0; p < count_sides; ++p, off += inc) {
                Vec3 point = Vec3(Mat4(1.0f) * 
                    Vec4(
                        (radius * glm::cos(p * angle_turn)) + center.x,
                        (radius * glm::sin(p * angle_turn)) + center.y,
                        center.z,
                        1.0f
                    )
                );

                vertices_triangles[v_idx + off]     = point.x;
                vertices_triangles[v_idx + off + 1] = point.y;
                vertices_triangles[v_idx + off + 2] = point.z;

                memcpy(&vertices_triangles[v_idx + off + 3], &color[0], sizeof(color[0]) * 4);
            }

            triangle_buffer.v_count += (attribute_stride * count_sides);

            index_triangles += count_sides;

            break;
        case GL_LINES:
            attribute_stride = vao_lines.stride;
            inc = attribute_stride;

            v_count = line_buffer.v_count;
            i_count = line_buffer.i_count;
            v_idx = v_count;

            if (v_count + (attribute_stride * count_sides) > SD_RENDER_BATCH_SIZE * attribute_stride || i_count + (2 * count_sides) > SD_RENDER_BATCH_SIZE * 2) {
                SD_LOG_ERR("%s\n", "ERROR: polygon_convex_regular MAX LINES EXCEEDED");
                return;
            }

            for (usize p = 0, off = 0; p < count_sides; ++p, off += 2) {
                indices_lines[i_count + off]     = index_lines + p;
                indices_lines[i_count + off + 1] = index_lines + p + 1;
            }
            indices_lines[i_count + (count_sides * 2) - 1] = index_lines;

            line_buffer.i_count += (2 * count_sides);

            for (usize p = 0, off = 0; p < count_sides; ++p, off += inc) {
                Vec3 point = Vec3(Mat4(1.0f) * 
                    Vec4(
                        (radius * glm::cos(p * angle_turn)) + center.x,
                        (radius * glm::sin(p * angle_turn)) + center.y,
                        center.z,
                        1.0f
                    )
                );
                vertices_lines[v_idx + off]     = point.x;
                vertices_lines[v_idx + off + 1] = point.y;
                vertices_lines[v_idx + off + 2] = point.z;

                memcpy(&vertices_lines[v_idx + off + 3], &color[0], sizeof(color[0]) * 4);
            }

            line_buffer.v_count += (attribute_stride * count_sides);          

            index_lines += count_sides; 

            break;
        }
    }

    void circle(GLfloat radius, Vec3 center, usize detail = 37)
    {
        polygon_convex_regular(radius, center, detail);
    }

    void vertex(Vec3 v)
    {
        usize v_count = 0;
        usize i_count = 0;
        usize v_idx = 0;

        usize attribute_stride;

        switch (draw_type) {
        case GL_TRIANGLES:
            attribute_stride = vao_triangles.stride;

            v_count = triangle_buffer.v_count;
            i_count = triangle_buffer.i_count;
            v_idx = v_count;

            if (v_count + attribute_stride > SD_RENDER_BATCH_SIZE * attribute_stride || i_count + 1 > SD_RENDER_BATCH_SIZE * 2) {
                SD_LOG_ERR("%s\n", "ERROR: vertex MAX TRIANGLES EXCEEDED");
                return;
            }

            //v = Vec3(transform_matrix * Vec4(v, 1.0f));


            memcpy(&vertices_triangles[v_idx], &v[0], sizeof(v[0]) * 3);
            memcpy(&vertices_triangles[v_idx + 3], &color[0], sizeof(color[0]) * 4);

            indices_triangles[i_count] = index_triangles;
            ++index_triangles;

            triangle_buffer.v_count += attribute_stride;
            ++triangle_buffer.i_count;

            // std::cout << "BEGIN" << std::endl;
            // std::cout << "{" << std::endl;
            // for (size_t i = 0; i < triangle_buffer.v_count; ++i) {
            //     std::cout << triangle_buffer.vertices[i] << ", ";
            // }
            // std::cout << std::endl << "}" << std::endl;

            // std::cout << "{" << std::endl;
            // for (size_t i = 0; i < triangle_buffer.i_count; ++i) {
            //     std::cout << triangle_buffer.indices[i] << ", ";
            // }
            // std::cout << std::endl << "}" << std::endl;
            // std::cout << "END" << std::endl;

            break;
        case GL_LINES:
            attribute_stride = vao_lines.stride;

            v_count = line_buffer.v_count;
            i_count = line_buffer.i_count;
            v_idx = v_count;

            if (v_count + attribute_stride > SD_RENDER_BATCH_SIZE * attribute_stride || i_count + 1 > SD_RENDER_BATCH_SIZE * 2) {
                SD_LOG_ERR("%s\n", "ERROR: vertex MAX LINES EXCEEDED");
                return;
            }

            //v = Vec3(transform_matrix * Vec4(v, 1.0f));


            memcpy(&vertices_lines[v_idx], &v[0], sizeof(v[0]) * 3);
            memcpy(&vertices_lines[v_idx + 3], &color[0], sizeof(color[0]) * 4);

            indices_lines[i_count] = index_lines;
            ++index_lines;

            line_buffer.v_count += attribute_stride;
            ++line_buffer.i_count;

            break;
        }      
    }
};

template<usize SD_RENDER_BATCH_SIZE> void begin(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx);
template<usize SD_RENDER_BATCH_SIZE> void render(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx);
template<usize SD_RENDER_BATCH_SIZE> void end(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx);
template<usize SD_RENDER_BATCH_SIZE> void end_no_reset(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx);
template<usize SD_RENDER_BATCH_SIZE> void layer_reset(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx);

template<usize SD_RENDER_BATCH_SIZE> inline bool layer_init(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, Mat4 projection_matrix);
template<usize SD_RENDER_BATCH_SIZE> sd::Render_Batch<SD_RENDER_BATCH_SIZE> Render_Batch_make(Mat4 projection_matrix);
template<usize SD_RENDER_BATCH_SIZE> void free(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx);

template<usize SD_RENDER_BATCH_SIZE> bool line(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, Vec3 a, Vec3 b);
template<usize SD_RENDER_BATCH_SIZE> bool line(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, Vec2 a, Vec2 b);
template<usize SD_RENDER_BATCH_SIZE> bool remove_line_swap_end(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, usize idx);
template<usize SD_RENDER_BATCH_SIZE> bool quad(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, Vec3, Vec3, Vec3, Vec3);
template<usize SD_RENDER_BATCH_SIZE> bool quad(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, Vec2, Vec2, Vec2, Vec2);
template<usize SD_RENDER_BATCH_SIZE> bool polygon_convex_regular(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, GLfloat radius, Vec3 center, const usize count_sides);
template<usize SD_RENDER_BATCH_SIZE> bool polygon_convex_regular(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, GLfloat radius, Vec2 center, const usize count_sides);
template<usize SD_RENDER_BATCH_SIZE> bool circle(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, GLfloat radius, Vec3 center, usize detail = 37);
template<usize SD_RENDER_BATCH_SIZE> bool circle(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, GLfloat radius, Vec2 center, usize detail = 37);
template<usize SD_RENDER_BATCH_SIZE> bool vertex(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, Vec3 v);
template<usize SD_RENDER_BATCH_SIZE> bool vertex(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, Vec2 v);
// template<usize SD_RENDER_BATCH_SIZE, usize N> bool polygon(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, Buffer<Vec3, N>* vs);
// template<usize SD_RENDER_BATCH_SIZE, usize N> bool polygon(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, Buffer<Vec2, N>* vs);
// template<usize SD_RENDER_BATCH_SIZE> bool polygon(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, DynamicBuffer<Vec3>* vs);
// template<usize SD_RENDER_BATCH_SIZE> bool polygon(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, DynamicBuffer<Vec2>* vs);
template<usize SD_RENDER_BATCH_SIZE> bool polygon(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, Vec3* vs, const usize count);
template<usize SD_RENDER_BATCH_SIZE> bool polygon(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, Vec2* vs, const usize count);
template<usize SD_RENDER_BATCH_SIZE> bool polygon(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, Array_Slice<Vec3> slice);
template<usize SD_RENDER_BATCH_SIZE> bool polygon(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, Array_Slice<Vec2>* slice);

template<usize SD_RENDER_BATCH_SIZE> inline void color(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, Vec4 color);
template<usize SD_RENDER_BATCH_SIZE> inline void push_context(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx);

}

#endif

#ifdef SD_IMPLEMENTATION
#undef SD_IMPLEMENTATION

namespace sd {


template<usize SD_RENDER_BATCH_SIZE> inline bool layer_init(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, Mat4 projection_matrix)
{
    ctx->projection_matrix = projection_matrix;
    ctx->update_projection_matrix = false;
    ctx->begun = false;

    ctx->index_triangles = 0;
    ctx->index_lines = 0;

    ctx->draw_type = GL_TRIANGLES;

    ctx->color = Vec4{0.0f, 0.0f, 0.0f, 1.0f};

    const char *const SHADER_VERTEX_PATH = sd::Render_Batch<SD_RENDER_BATCH_SIZE>::SHADER_VERTEX_PATH;
    const char *const SHADER_FRAGMENT_PATH = sd::Render_Batch<SD_RENDER_BATCH_SIZE>::SHADER_FRAGMENT_PATH;

    if (false == Shader_load_from_file(
        &ctx->shader,
        SHADER_VERTEX_PATH,
        SHADER_FRAGMENT_PATH
    )) {
        SD_LOG_ERR("%s\n", "ERROR: sd::Render_Batch initialization failed");
        return false;
    }

    const usize attribute_stride = sd::Render_Batch<SD_RENDER_BATCH_SIZE>::DEFAULT_ATTRIBUTE_STRIDE;

    glUseProgram(ctx->shader);
    ctx->MAT_LOC = glGetUniformLocation(ctx->shader, "u_matrix");
    glUniformMatrix4fv(ctx->MAT_LOC, 1, GL_FALSE, glm::value_ptr(ctx->projection_matrix));
    glUseProgram(0);

    VertexAttributeArray_init(&ctx->vao_triangles, attribute_stride);
    glBindVertexArray(ctx->vao_triangles);

        VertexBufferData_init_inplace(
            &ctx->triangle_buffer, 
            SD_RENDER_BATCH_SIZE * attribute_stride,
            ctx->vertices_triangles,
            SD_RENDER_BATCH_SIZE,
            ctx->indices_triangles
        );
        ctx->triangle_buffer.v_count = 0;
        ctx->triangle_buffer.i_count = 0;

        gl_bind_buffers_and_upload_data(&ctx->triangle_buffer, GL_STREAM_DRAW);
        // POSITION
        gl_set_and_enable_vertex_attrib_ptr(0, 3, GL_FLOAT, GL_FALSE, 0, &ctx->vao_triangles);
        // COLOR
        gl_set_and_enable_vertex_attrib_ptr(1, 4, GL_FLOAT, GL_FALSE, 3, &ctx->vao_triangles);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    VertexAttributeArray_init(&ctx->vao_lines, attribute_stride);
    glBindVertexArray(ctx->vao_lines);
        VertexBufferData_init_inplace(
            &ctx->line_buffer, 
            SD_RENDER_BATCH_SIZE * attribute_stride,
            ctx->vertices_lines,
            SD_RENDER_BATCH_SIZE,
            ctx->indices_lines
        );
        ctx->line_buffer.v_count = 0;
        ctx->line_buffer.i_count = 0;

        gl_bind_buffers_and_upload_data(&ctx->line_buffer, GL_STREAM_DRAW);
        // POSITION
        gl_set_and_enable_vertex_attrib_ptr(0, 3, GL_FLOAT, GL_FALSE, 0, &ctx->vao_lines);
        // COLOR
        gl_set_and_enable_vertex_attrib_ptr(1, 4, GL_FLOAT, GL_FALSE, 3, &ctx->vao_lines);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    ctx->index_triangles = 0;
    ctx->index_lines = 0;

    return true;
}

template<usize SD_RENDER_BATCH_SIZE = 2048> sd::Render_Batch<SD_RENDER_BATCH_SIZE> Render_Batch_make(Mat4 projection_matrix)
{
    sd::Render_Batch<SD_RENDER_BATCH_SIZE> ctx;
    if (sd::layer_init(&ctx, projection_matrix) == false) {
        SD_LOG_ERR("%s\n", "ERROR: Context creation failed");
    }
    return ctx;
}

template<usize N> void render(sd::Render_Batch<N>* ctx)
{
    glUseProgram(ctx->shader);

    glUniformMatrix4fv(ctx->MAT_LOC, 1, GL_FALSE, glm::value_ptr(ctx->projection_matrix * ctx->transform_matrix));

    glBindVertexArray(ctx->vao_triangles);
    gl_bind_buffers_and_upload_sub_data(&ctx->triangle_buffer);
    if (ctx->triangle_buffer.i_count > 0) { 
        glDrawElements(GL_TRIANGLES, ctx->triangle_buffer.i_count, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(ctx->vao_lines);
    gl_bind_buffers_and_upload_sub_data(&ctx->line_buffer);
    if (ctx->line_buffer.i_count > 0) { 
        glDrawElements(GL_LINES, ctx->line_buffer.i_count, GL_UNSIGNED_INT, 0);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

template<usize N> void free(sd::Render_Batch<N>* ctx)
{
    VertexAttributeArray_delete(&ctx->vao_triangles);
    VertexAttributeArray_delete(&ctx->vao_lines);
    VertexBufferData_delete_inplace(&ctx->triangle_buffer);
    VertexBufferData_delete_inplace(&ctx->line_buffer);
    glDeleteProgram(ctx->shader);
}

template<usize SD_RENDER_BATCH_SIZE> void layer_reset(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx)
{
    ctx->triangle_buffer.v_count = 0;
    ctx->triangle_buffer.i_count = 0;

    ctx->line_buffer.v_count = 0;
    ctx->line_buffer.i_count = 0;

    ctx->index_triangles = 0;
    ctx->index_lines     = 0;        
}


template<usize SD_RENDER_BATCH_SIZE> bool line(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, Vec3 a, Vec3 b)
{
    const usize v_count = ctx->line_buffer.v_count;
    const usize i_count = ctx->line_buffer.i_count;

    const usize attribute_stride = ctx->vao_lines.stride;

    if (v_count + (2 * attribute_stride) > (SD_RENDER_BATCH_SIZE * attribute_stride) || (i_count + 2 > SD_RENDER_BATCH_SIZE * 2)) {
        SD_LOG_ERR("%s\n", "ERROR: add_line_segment MAX LINES EXCEEDED");
        return false;
    }

    const usize v_idx = v_count;

    // memcpy(&ctx->vertices_lines[v_idx], &a[0], sizeof(a[0]) * 3);
    // memcpy(&ctx->vertices_lines[v_idx + 3], &ctx->color[0], sizeof(ctx->color[0]) * 4);

    ctx->vertices_lines[v_idx]     = a.x;
    ctx->vertices_lines[v_idx + 1] = a.y;
    ctx->vertices_lines[v_idx + 2] = a.z;
    ctx->vertices_lines[v_idx + 3] = ctx->color[0];
    ctx->vertices_lines[v_idx + 4] = ctx->color[1];
    ctx->vertices_lines[v_idx + 5] = ctx->color[2];
    ctx->vertices_lines[v_idx + 6] = ctx->color[3];



    // memcpy(&ctx->vertices_lines[v_idx + attribute_stride], &b[0], sizeof(b[0]) * 3);
    // memcpy(&ctx->vertices_lines[v_idx + attribute_stride + 3], &ctx->color[0], sizeof(ctx->color[0]) * 4);


    ctx->vertices_lines[v_idx + attribute_stride]     = b.x;
    ctx->vertices_lines[v_idx + 1 + attribute_stride] = b.y;
    ctx->vertices_lines[v_idx + 2 + attribute_stride] = b.z;
    ctx->vertices_lines[v_idx + 3 + attribute_stride] = ctx->color[0];
    ctx->vertices_lines[v_idx + 4 + attribute_stride] = ctx->color[1];
    ctx->vertices_lines[v_idx + 5 + attribute_stride] = ctx->color[2];
    ctx->vertices_lines[v_idx + 6 + attribute_stride] = ctx->color[3];

    ctx->indices_lines[i_count]     = ctx->index_lines;
    ctx->indices_lines[i_count + 1] = ctx->index_lines + 1;
    ctx->index_lines += 2;

    ctx->line_buffer.v_count += (2 * attribute_stride);
    ctx->line_buffer.i_count += 2;

    return true;
}

template<usize SD_RENDER_BATCH_SIZE> bool line(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, Vec2 a, Vec2 b)
{
    return sd::line(ctx, Vec3(a, 1), Vec3(b, 1));
}

template<usize SD_RENDER_BATCH_SIZE> bool remove_line_swap_end(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, usize idx)
{
    const usize attribute_stride = ctx->vao_lines.stride;

#ifdef SD_BOUNDS_CHECK
    if ((idx * (2 * attribute_stride) > (SD_RENDER_BATCH_SIZE * attribute_stride)) || (idx * 2 > SD_RENDER_BATCH_SIZE * 2)) {
        SD_LOG_ERR("%s\n", "ERROR: remove_line INDEX OUT-OF-BOUNDS");
        return false;           
    }
#endif

    // reduce counts
    ctx->line_buffer.v_count -= (2 * attribute_stride);
    ctx->line_buffer.i_count -= 2;

    // overwrite the element-to-delete with the last element
    memcpy(&ctx->vertices_lines[(2 * attribute_stride) * idx], &ctx->vertices_lines[ctx->line_buffer.v_count], sizeof(GLfloat) * 2 * attribute_stride);

    // move the line index back by 2 (for each point in the segment)
    ctx->index_lines -= 2;

    return true;
}

template<usize SD_RENDER_BATCH_SIZE> bool polygon_convex_regular(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, GLfloat radius, Vec3 center, const usize count_sides)
{
    usize count_tris = count_sides - 2;

    usize attribute_stride;
    usize inc;

    usize v_count = 0;
    usize i_count = 0;
    usize v_idx   = 0;
    GLdouble angle_turn = -(TAU) / count_sides;

    switch (ctx->draw_type) {
    case GL_TRIANGLES:
        attribute_stride = ctx->vao_triangles.stride;
        inc = attribute_stride;

        v_count = ctx->triangle_buffer.v_count;
        i_count = ctx->triangle_buffer.i_count;
        v_idx = v_count;
#ifdef SD_BOUNDS_CHECK
        if (v_count + (attribute_stride * count_sides) > SD_RENDER_BATCH_SIZE * attribute_stride || i_count + (3 * count_tris) > SD_RENDER_BATCH_SIZE * 2) {
            SD_LOG_ERR("%s\n", "ERROR: polygon_convex_regular MAX TRIANGLES EXCEEDED");
            return false;
        }
#endif

        for (usize p = 0, idx_off = 0; p < count_tris; ++p, idx_off += 3) {
            ctx->indices_triangles[i_count + idx_off]     = ctx->index_triangles + 0;
            ctx->indices_triangles[i_count + idx_off + 1] = ctx->index_triangles + p + 1;
            ctx->indices_triangles[i_count + idx_off + 2] = ctx->index_triangles + p + 2;
        }
        ctx->triangle_buffer.i_count += (3 * count_tris);


        for (usize p = 0, off = 0; p < count_sides; ++p, off += inc) {
            Vec3 point = Vec3(Mat4(1.0f) * 
                Vec4(
                    (radius * glm::cos(p * angle_turn)) + center.x,
                    (radius * glm::sin(p * angle_turn)) + center.y,
                    center.z,
                    1.0f
                )
            );

            ctx->vertices_triangles[v_idx + off]     = point.x;
            ctx->vertices_triangles[v_idx + off + 1] = point.y;
            ctx->vertices_triangles[v_idx + off + 2] = point.z;

            memcpy(&ctx->vertices_triangles[v_idx + off + 3], &ctx->color[0], sizeof(ctx->color[0]) * 4);
        }

        ctx->triangle_buffer.v_count += (attribute_stride * count_sides);

        ctx->index_triangles += count_sides;

        break;
    case GL_LINES:
        attribute_stride = ctx->vao_lines.stride;
        inc = attribute_stride;

        v_count = ctx->line_buffer.v_count;
        i_count = ctx->line_buffer.i_count;
        v_idx = v_count;
#ifdef SD_BOUNDS_CHECK
        if (v_count + (attribute_stride * count_sides) > SD_RENDER_BATCH_SIZE * attribute_stride || i_count + (2 * count_sides) > SD_RENDER_BATCH_SIZE * 2) {
            SD_LOG_ERR("%s\n", "ERROR: polygon_convex_regular MAX LINES EXCEEDED");
            return false;
        }
#endif

        for (usize p = 0, off = 0; p < count_sides; ++p, off += 2) {
            ctx->indices_lines[i_count + off]     = ctx->index_lines + p;
            ctx->indices_lines[i_count + off + 1] = ctx->index_lines + p + 1;
        }
        ctx->indices_lines[i_count + (count_sides * 2) - 1] = ctx->index_lines;

        ctx->line_buffer.i_count += (2 * count_sides);

        for (usize p = 0, off = 0; p < count_sides; ++p, off += inc) {
            Vec3 point = Vec3(Mat4(1.0f) * 
                Vec4(
                    (radius * glm::cos(p * angle_turn)) + center.x,
                    (radius * glm::sin(p * angle_turn)) + center.y,
                    center.z,
                    1.0f
                )
            );
            ctx->vertices_lines[v_idx + off]     = point.x;
            ctx->vertices_lines[v_idx + off + 1] = point.y;
            ctx->vertices_lines[v_idx + off + 2] = point.z;

            memcpy(&ctx->vertices_lines[v_idx + off + 3], &ctx->color[0], sizeof(ctx->color[0]) * 4);
        }

        ctx->line_buffer.v_count += (attribute_stride * count_sides);          

        ctx->index_lines += count_sides; 

        break;
    }

    return true;
}

template<usize SD_RENDER_BATCH_SIZE> bool polygon_convex_regular(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, GLfloat radius, Vec2 center, const usize count_sides)
{
    return sd::polygon_convex_regular(ctx, radius, Vec3(center, 1.0), count_sides);
}

template<usize SD_RENDER_BATCH_SIZE> bool circle(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, GLfloat radius, Vec3 center, usize detail)
{
    return sd::polygon_convex_regular(ctx, radius, center, detail);
}

template<usize SD_RENDER_BATCH_SIZE> bool circle(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, GLfloat radius, Vec2 center, usize detail)
{
    return sd::polygon_convex_regular(ctx, radius, center, detail);
}

template<usize SD_RENDER_BATCH_SIZE> bool vertex(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, Vec3 v)
{
    usize v_count = 0;
    usize i_count = 0;
    usize v_idx = 0;

    usize attribute_stride;

    switch (ctx->draw_type) {
    case GL_TRIANGLES:
        attribute_stride = ctx->vao_triangles.stride;

        v_count = ctx->triangle_buffer.v_count;
        i_count = ctx->triangle_buffer.i_count;
        v_idx = v_count;
#ifdef SD_BOUNDS_CHECK
        if (v_count + attribute_stride > SD_RENDER_BATCH_SIZE * attribute_stride || i_count + 1 > SD_RENDER_BATCH_SIZE * 2) {
            SD_LOG_ERR("%s\n", "ERROR: vertex MAX TRIANGLES EXCEEDED");
            return false;
        }
#endif

        memcpy(&ctx->vertices_triangles[v_idx], &v[0], sizeof(v[0]) * 3);
        memcpy(&ctx->vertices_triangles[v_idx + 3], &ctx->color[0], sizeof(ctx->color[0]) * 4);

        ctx->indices_triangles[i_count] = ctx->index_triangles;
        ctx->index_triangles += 1;

        ctx->triangle_buffer.v_count += attribute_stride;
        ctx->triangle_buffer.i_count += 1;

        break;
    case GL_LINES:
        attribute_stride = ctx->vao_lines.stride;

        v_count = ctx->line_buffer.v_count;
        i_count = ctx->line_buffer.i_count;
        v_idx = v_count;
#ifdef SD_BOUNDS_CHECK
        if (v_count + attribute_stride > SD_RENDER_BATCH_SIZE * attribute_stride || i_count + 1 > SD_RENDER_BATCH_SIZE * 2) {
            SD_LOG_ERR("%s\n", "ERROR: vertex MAX LINES EXCEEDED");
            return false;
        }
#endif

        memcpy(&ctx->vertices_lines[v_idx], &v[0], sizeof(v[0]) * 3);
        memcpy(&ctx->vertices_lines[v_idx + 3], &ctx->color[0], sizeof(ctx->color[0]) * 4);

        ctx->indices_lines[i_count] = ctx->index_lines;
        ctx->index_lines += 1;

        ctx->line_buffer.v_count += attribute_stride;
        ctx->line_buffer.i_count += 1;

        break;
    }

    return true;      
}

template<usize SD_RENDER_BATCH_SIZE> bool vertex(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, Vec2 v)
{
    return sd::vertex(ctx, Vec3(v, 1.0));
}

template<usize SD_RENDER_BATCH_SIZE> inline void color(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, Vec4 color)
{
    ctx->color = color;
}

template<usize SD_RENDER_BATCH_SIZE> inline void push_context(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx)
{
    // TODO
}


// #define MAX_IMG_SIZE (128 * 128)
// static bool draw_lines_from_image_visited[MAX_IMG_SIZE];

// static void draw_lines_from_image_bfs(sd::Render_Batch* ctx, std::vector<Vec3>& bgr_colors_to_ignore, u8* pixels, u32 w, u32 h, u32 pitch, u32 pixels_per_move)
// {
//     // ((w / pixels_per_move) + 1) * ((h / pixels_per_move) + 1)
//     bool* const visited = draw_lines_from_image_visited;
//     memset(visited, 0x00, MAX_IMG_SIZE * sizeof(bool));

//     const u32 bytes_per_pixel = pitch / w;

//     u32 px = 0;
//     u32 py = 0;

//     typedef std::pair<u32, u32> Point;
//     std::queue<Point> Q;


//     #define IDX(py, px) (((py * (w + pixels_per_move)) / (pixels_per_move * pixels_per_move)) + (px / pixels_per_move))

//     visited[IDX(py, px)] = true;
//     Q.push({py, px});

//     while (!Q.empty()) {
//         Point p = Q.front();

//         std::cout << "VISIT: P:{" << p.first << ", " << p.second << "}" << std::endl;
//         Q.pop();

//         //if () {
//             // TODO
//         //}
//     }

//     //std::cout << w << " : " << h << std::endl;


//     #undef IDX
// }

// static bool draw_lines_from_image(sd::Render_Batch* ctx, std::string path,  std::vector<Vec3> bgr_colors_to_ignore)
// {
//     return false;
//     SDL_RWops *rwop;

//     rwop = SDL_RWFromFile(path.c_str(), "rb");
//     if(!IMG_isBMP(rwop)) {
//         fprintf(stderr, "sample.bmp is not a BMP file, or BMP support is not available.\n");
//         return false;
//     }

//     SDL_Surface* img = NULL; 
//     if (!(img = IMG_Load(path.c_str()))) {
//         fprintf(stderr, "SDL_image could not be loaded %s, SDL_image Error: %s\n", 
//                path.c_str(), IMG_GetError());
//         return false;
//     }

//     u8* pixels = (u8*)img->pixels;

//     draw_lines_from_image_bfs(ctx, bgr_colors_to_ignore, (u8*)img->pixels, img->w, img->h, img->pitch, 64);
//     // printf("|||| BMP (bgr format)\n");
//     // printf("W: %d H: %d P: %d \n", img->w, img->h, img->pitch);
//     // for (usize i = 0; i < img->h; ++i) {
//     //     printf("{");
//     //     for (usize j = 0; j < img->w; ++j) {
//     //         printf("[");
//     //         for (usize k = 0; k < img->pitch / img->w; ++k) {
//     //             printf("%u, ", pixels[(i * img->pitch) + (j * (img->pitch / img->w)) + k]);
//     //         }
//     //         printf("], ");
//     //     }
//     //     printf("}\n");
//     // } 
//     // printf("||||\n");


//     SDL_FreeSurface(img);

//     return true;

// }

// template<usize SD_RENDER_BATCH_SIZE>
// inline void sd::Render_Batch_ERR_LOG_PRINT__(sd::Render_Batch<SD_RENDER_BATCH_SIZE>* ctx, const char *const name, const char *const file, int line)
// {
//     if (ctx->status == false) {
//         fprintf(stderr, "%s, %s, %d\n", name, file, line);
//         ctx->status = true;
//     }
// }
// #ifdef SD_DEBUG_LOG_ON
//     #define sd::Render_Batch_ERR_LOG(ctx__, file__, line__) sd::Render_Batch_ERR_LOG_PRINT__(& ctx__, STRING(ctx__), file__, line__)
//     #define sd::Render_Batch_PTR_ERR_LOG(ctxptr__, file__, line__) sd::Render_Batch_ERR_LOG_PRINT__(ctxptr__, STRING(ctxptr__), file__, line__)
// #else
//     #define sd::Render_Batch_ERR_LOG(ctx__, file__, line__)
//     #define sd::Render_Batch_PTR_ERR_LOG(ctxptr__, file__, line__)
// #endif

}

#endif // SD_OPENGL_HPP
