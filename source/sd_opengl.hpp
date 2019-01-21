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

static constexpr GLenum TRIANGLES = GL_TRIANGLES;
static constexpr GLenum LINES     = GL_LINES;

struct alignas(16) Shader_Vertex {
    Vec3_ua position;
    float32 angle;
    Vec4_ua color;
    Vec2_ua uv;
    float32 rotation_center;
};

#define BUILTIN_SHADER_TYPE_LIST \
    X(GL_SHADER_POLYGON_TEXTURED_2D, _IDX, (1 << 0), 0) \
    X(GL_SHADER_POLYGON_COLORED_2D, _IDX, (1 << 1), 1) \
    X(GL_SHADER_LINE_DEBUG, _IDX, (1 << 2), 2) \


enum BUILTIN_SHADER_TYPE {
    #define X(a, b, c, d) a = c,
    BUILTIN_SHADER_TYPE_LIST
    #undef X
};
constexpr const u32 BUILTIN_GL_SHADER_TYPE_COUNT = 3;

enum BUILTIN_SHADER_TYPE_INDEX {
    #define X(a, b, c, d) a##b = d,
    BUILTIN_SHADER_TYPE_LIST
    #undef X    
};



enum RENDERER_INIT_FLAG {
    RENDERER_INIT_FLAG_DEFAULT = 0,
    RENDERER_INIT_FLAG_CUSTOM_OPENGL_SHADERS_EXTERNAL_STORAGE = (1 << 0),
};
constexpr const u32 RENDERER_INIT_FLAG_COUNT = 2;

struct Shared_Uniforms {
    GLfloat u_time_s;
    GLfloat u_scale;
    Vec2    u_resolution;
    Mat4    u_mv_matrix;
    Mat4    u_projection_matrix;

    Uniform_Block_Index u_time_s_idx;
    Uniform_Block_Index u_scale_idx;
    Uniform_Block_Index u_resolution_idx;
    Uniform_Block_Index u_mv_matrix_idx;
    Uniform_Block_Index u_projection_matrix_idx;

    // TODO probably add texture samplers to the UBO as well

};


#if defined(SD_USE_STATIC_RENDER_BATCH_SIZE) && defined(SD_RENDER_BATCH_SIZE)
    static_assert(SD_STATIC_RENDER_BATCH_SIZE > 0)

    typedef Array<Shader_Vertex, SD_STATIC_RENDER_BATCH_SIZE> VertexData_Array;
    typedef Array<GLuint, SD_STATIC_RENDER_BATCH_SIZE> ElementData_Array;

    #pragma message("using statically allocated arrays")
#else
    #if defined(SD_USE_STATIC_RENDER_BATCH_SIZE)
        #warning "SD_USE_STATIC_RENDER_BATCH_SIZE active, but SD_RENDER_BATCH_SIZE is invalid, falling-back to dynamic batches"
        #undef SD_USE_STATIC_RENDER_BATCH_SIZE
    #endif

    typedef Dynamic_Array<Shader_Vertex> VertexData_Array;
    typedef Dynamic_Array<GLuint>        ElementData_Array;

    #pragma message("using dynamically allocated arrays")
#endif

#ifndef SD_RENDER_BATCH_SIZE
    #define SD_RENDER_BATCH_SIZE (2048)
#endif

#ifndef SD_BUFFERING_COUNT
    #define SD_BUFFERING_COUNT (1)
#endif

#ifndef SD_MAX_RENDER_BATCH_COUNT
    #define SD_MAX_RENDER_BATCH_COUNT (8)
#else
    static_assert(SD_MAX_RENDER_BATCH_COUNT > 1)
#endif

struct GPU_Data_Buffer {
    Vertex_Buffer_Object  vbo;
    Element_Buffer_Object ebo;
    usize vbo_cap;
    usize ebo_cap;
};
struct GPU_Buffer_Object_Set {
    // triple buffering at max
    Vertex_Array_Object vaos[SD_BUFFERING_COUNT];
    GPU_Data_Buffer     buffers[1 + SD_BUFFERING_COUNT]; // UBO, (VBO, EBO) ... (VBO, EBO) ..
    u16 buffering_cursor;
    u16 vertex_cursor; // ?

    inline void advance_buffering_cursor(void) 
    {
    #if (SD_BUFFERING_COUNT > 1)
        this->buffering_cursor = (this->buffering_cursor + 1) % SD_BUFFERING_COUNT; 
    #endif
    }
};

enum CLEAR_FLAGS {
    CLEAR_COLOR = GL_COLOR_BUFFER_BIT,
    CLEAR_DEPTH = GL_DEPTH_BUFFER_BIT,
};
constexpr const u32 CLEAR_FLAG_COUNT = 2;

void GPU_Buffer_Object_Set_init(GPU_Buffer_Object_Set& buff_obj, const usize reserved_space);
void GPU_Buffer_Object_Set_deinit(GPU_Buffer_Object_Set& buff_obj);

enum RENDER_STATE_FLAGS {
    STATE_BLEND_MODE  = (1 << 0),
    STATE_DEPTH_WRITE = (1 << 1),
};

struct Render_Batch_State_Data {
    u16 start_idx;
    u16 element_count;

    // TODO possibly combine the program into and state_bitmask into one u64
    Shader_Program program;
    // texture handle or texture index? TODO
    u32 state_bitmask;
};


struct Render_Batch {
    VertexData_Array v_data;
    ElementData_Array e_data;

    GLuint v_idx;
    GLuint next_element;

    Dynamic_Array<Render_Batch_State_Data> state_changes;
};

void Shader_Vertex_print(Shader_Vertex const& v);
void VertexData_Array_print(VertexData_Array const& vda);
void ElementData_Array_print(VertexData_Array const& eda);
void Render_Batch_print(Render_Batch const& batch);




typedef uint16 layer_index;

static constexpr const u16 INDICES_PER_TRIANGLE = 3;
static constexpr const u16 INDICES_PER_QUAD = INDICES_PER_TRIANGLE * 2;
static constexpr const u16 ELEMENTS_PER_QUAD = 4;

struct Renderer {
    // GPU objects
    static constexpr u16 BUFFERING_COUNT = SD_BUFFERING_COUNT;
    GPU_Buffer_Object_Set gpu_buff_objs;

    // shader data
    Shared_Uniforms shared_uniform_data;
    Array<Uniform_Location, 64> uniform_location_backing_array;
    Array<Shader, BUILTIN_GL_SHADER_TYPE_COUNT> shader_backing_array;
    Shader_Catalogue shader_catalogue;

    // render batches for layers
    static constexpr u16 MAX_RENDER_BATCH_COUNT = SD_MAX_RENDER_BATCH_COUNT;
    u16 render_batch_count;
    Array<Render_Batch, SD_MAX_RENDER_BATCH_COUNT> batches;

    // state
    Vec4 active_color;

    u64 vertex_idx;
    u64 element_idx;
};

extern char const * const builtin_shader_paths[3][2];



void render(sd::Renderer& ctx, uint16 layer);
void render(sd::Renderer& ctx);

void present(SDL_Window* window);

inline void present(sd::Renderer& ctx, SDL_Window* window)
{
    ctx.vertex_idx = 0;
    ctx.element_idx = 0;
    SDL_GL_SwapWindow(window);
}

void present(sd::Renderer& renderer);

inline void clear(GLbitfield mask)
{
    glClear(mask);
}
inline void clear_color(float32 r, float32 g, float32 b, float32 a)
{
    glClearColor(r, g, b, a);
}
inline void clear_color(Vec4 c)
{
    glClearColor(c.r, c.g, c.b, c.a);
}
inline void clear_color(Vec3 c)
{
    glClearColor(c.r, c.g, c.b, 1.0f);
}

void begin(sd::Renderer const& ctx);
void render(sd::Renderer& ctx);
void end(sd::Renderer const& ctx);
void end_no_reset(sd::Renderer const& ctx);
void batch_render(sd::Renderer& ctx);

sd::Renderer& init(sd::Renderer& ctx, const usize reserved_space_per_batch = SD_RENDER_BATCH_SIZE);
sd::Renderer& init_options(sd::Renderer& ctx, const u64 bitmask_flags = 0, const usize reserved_space_per_batch = SD_RENDER_BATCH_SIZE);
void Render_Batch_init(Render_Batch& batch, const usize reserved_space_per_batch = SD_RENDER_BATCH_SIZE);
bool shaders_load_builtin(Shader_Catalogue& shader_catalogue, uint64 which = 0xFFFFFFFFFFFFFFFF);
Shader_Catalogue shaders_load_builtin(uint64 which = 0xFFFFFFFFFFFFFFFF);
bool shaders_append_builtin(Shader_Catalogue& shader_catalogue, uint64 which = 0xFFFFFFFFFFFFFFFF);
void shaders_set_uniform_locations_internal(sd::Renderer& ctx);
inline Shader* set_catalogue_uniform_location_ptrs_internal(
    Dynamic_Array<Shader>* const shaders, Uniform_Location* const uniforms,
    const usize shader_i, const usize uniform_i, const usize u_count
)
{
    Shader* shader = &((*shaders)[shader_i]);
    init_from_ptr(&shader->uniform_locations, &uniforms[uniform_i], u_count, u_count);

    return shader;
}


 
void Render_Batch_deinit(Render_Batch& batch);
sd::Renderer Renderer_make(const usize reserved_space_per_batch = SD_RENDER_BATCH_SIZE);
sd::Renderer Renderer_make_options(u64 bitmask_flags = 0, const usize reserved_space_per_batch = SD_RENDER_BATCH_SIZE);
void deinit(sd::Renderer& ctx);
bool line_projected(sd::Renderer const& ctx, layer_index L, Vec3 a, Vec3 b);
bool line(sd::Renderer const& ctx, layer_index L, Vec2 a, Vec2 b);
bool debug_line(sd::Renderer const& ctx, layer_index L, Vec3 a, Vec3 b);
bool debug_line(sd::Renderer const& ctx, layer_index L, Vec2 a, Vec2 b);
bool remove_line_swap_end(sd::Renderer const& ctx, usize idx);
bool quad_projected(sd::Renderer const& ctx, layer_index L, Vec3 tl, Vec3 bl, Vec3 br, Vec3 tr, float32 angle = 0.0);

bool quad(sd::Renderer& ctx, layer_index L, Vec2 tl, Vec2 bl, Vec2 br, Vec2 tr);
bool quad(sd::Renderer& ctx, layer_index L, Vec2 tl, Vec2 bl, Vec2 br, Vec2 tr, float32 angle);
bool quad(sd::Renderer& ctx, layer_index L, Vec2 tl, Vec2 bl, Vec2 br, Vec2 tr, Vec4 color_tl, Vec4 color_bl, Vec4 color_br, Vec4 color_tr);
bool quad(sd::Renderer& ctx, layer_index L, Vec2 tl, Vec2 bl, Vec2 br, Vec2 tr, Vec4 color_tl, Vec4 color_bl, Vec4 color_br, Vec4 color_tr, float32 angle);

bool polygon_convex_regular(sd::Renderer const& ctx, layer_index L, GLfloat radius, Vec2 center, const usize count_sides);
bool circle(sd::Renderer const& ctx, layer_index L, GLfloat radius, Vec2 center, usize detail = 37);
bool vertex(sd::Renderer const& ctx, layer_index L, Vec2 v);
bool polygon(sd::Renderer const& ctx, layer_index L, Vec2* vs, const usize count);
bool polygon(sd::Renderer const& ctx, layer_index L, Array_Slice<Vec3> slice);
bool polygon(sd::Renderer const& ctx, layer_index L, Array_Slice<Vec2>* slice);

inline void color(sd::Renderer & ctx, Vec4 color)
{
    ctx.active_color = color;
}

inline void push_mv_matrix(sd::Renderer const& ctx, Mat4 mv_mat)
{
    // TODO
}



}

#endif

#ifdef SD_IMPLEMENTATION
#undef SD_IMPLEMENTATION

namespace sd {



void present(sd::Renderer& renderer)
{
}




void GPU_Buffer_Object_Set_deinit(GPU_Buffer_Object_Set& buff_obj)
{
    glDeleteVertexArrays(sd::Renderer::BUFFERING_COUNT, buff_obj.vaos);
    GL_GET_ERRORS();

    glDeleteBuffers(1 + (sd::Renderer::BUFFERING_COUNT * 2), (GL_Buffer*)buff_obj.buffers);
    GL_GET_ERRORS();
}

sd::Renderer& init(sd::Renderer& ctx, const usize reserved_space_per_batch)
{
    GPU_Buffer_Object_Set_init(ctx.gpu_buff_objs, reserved_space_per_batch * sd::Renderer::MAX_RENDER_BATCH_COUNT);
    init(&ctx.batches);

    const layer_index MAX_BATCH_COUNT = sd::Renderer::MAX_RENDER_BATCH_COUNT;
    for (layer_index l = 0; l < MAX_BATCH_COUNT; l += 1) {
        Render_Batch_init(ctx.batches[l], reserved_space_per_batch);
    }

    // initialize built-in shaders //////////////////////////////////////
    ctx.uniform_location_backing_array.count = 
        ctx.uniform_location_backing_array.cap();

    // shader array
    init_from_ptr(
        &ctx.shader_catalogue.shaders,
        ctx.shader_backing_array.data,
        BUILTIN_GL_SHADER_TYPE_COUNT,
        BUILTIN_GL_SHADER_TYPE_COUNT
    );

    shaders_load_builtin(ctx.shader_catalogue);
    shaders_set_uniform_locations_internal(ctx);

    ctx.vertex_idx = 0;
    ctx.element_idx = 0;
    ctx.active_color = Vec4(0.0);

    return ctx;
}

sd::Renderer& init_options(sd::Renderer& ctx, u64 bitmask_flags, const usize reserved_space_per_batch)
{
    GPU_Buffer_Object_Set_init(ctx.gpu_buff_objs, reserved_space_per_batch * sd::Renderer::MAX_RENDER_BATCH_COUNT);
    init(&ctx.batches);

    const layer_index MAX_BATCH_COUNT = sd::Renderer::MAX_RENDER_BATCH_COUNT;
    for (layer_index l = 0; l < MAX_BATCH_COUNT; l += 1) {
        Render_Batch_init(ctx.batches[l], reserved_space_per_batch);
    }

    if (!BSET_CONTAINS(bitmask_flags, sd::RENDERER_INIT_FLAG_CUSTOM_OPENGL_SHADERS_EXTERNAL_STORAGE)) {

        ctx.uniform_location_backing_array.count = 
            ctx.uniform_location_backing_array.cap();

        init_from_ptr(
            &ctx.shader_catalogue.shaders,
            ctx.shader_backing_array.data,
            BUILTIN_GL_SHADER_TYPE_COUNT,
            BUILTIN_GL_SHADER_TYPE_COUNT
        );

        shaders_load_builtin(ctx.shader_catalogue);
        shaders_set_uniform_locations_internal(ctx);
    }

    ctx.vertex_idx = 0;
    ctx.element_idx = 0;
    ctx.active_color = Vec4(0.0);

    return ctx;
}

void GPU_Buffer_Object_Set_init(GPU_Buffer_Object_Set& buff_obj, const usize reserved_space)
{
    glGenVertexArrays(sd::Renderer::BUFFERING_COUNT, (Vertex_Array_Object*)buff_obj.vaos);
    GL_GET_ERRORS();
    
    glGenBuffers(1 + (sd::Renderer::BUFFERING_COUNT * 2), (GL_Buffer*)buff_obj.buffers); // +1 for the uniform buffer object
    GL_GET_ERRORS();

    for (usize i = 0; i < sd::Renderer::BUFFERING_COUNT; i += 1) {
        glBindVertexArray(buff_obj.vaos[i]);
        printf("VAO in init: %u\n", buff_obj.vaos[i]);
        GL_GET_ERRORS();

        glBindBuffer(GL_ARRAY_BUFFER, buff_obj.buffers[i].vbo);
        buff_obj.buffers[i].vbo_cap = reserved_space * sizeof(Shader_Vertex);
        glBufferData(GL_ARRAY_BUFFER, buff_obj.buffers[i].vbo_cap, nullptr, GL_STREAM_DRAW);

        GL_GET_ERRORS();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buff_obj.buffers[i].ebo);
        buff_obj.buffers[i].ebo_cap = reserved_space * 4 * sizeof(GLuint);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, buff_obj.buffers[i].ebo_cap, nullptr, GL_STREAM_DRAW);

        GL_GET_ERRORS();

        /*
        struct alignas(16) Shader_Vertex {
            Vec3_ua position;
            float32 angle;
            Vec4_ua color;
            Vec2_ua uv;
            float32 rotation_center;
        };
        */
        glVertexAttribPointer(0, sizeof(Shader_Vertex::position) / sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(Shader_Vertex), (GLvoid*)offsetof(Shader_Vertex, position));
        glEnableVertexAttribArray(0);

        GL_GET_ERRORS();

        glVertexAttribPointer(1, sizeof(Shader_Vertex::angle) / sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(Shader_Vertex), (GLvoid*)offsetof(Shader_Vertex, angle));
        glEnableVertexAttribArray(1);

        GL_GET_ERRORS();

        glVertexAttribPointer(2, sizeof(Shader_Vertex::color) / sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(Shader_Vertex), (GLvoid*)offsetof(Shader_Vertex, color));
        glEnableVertexAttribArray(2);
        
        GL_GET_ERRORS();

        glVertexAttribPointer(3, sizeof(Shader_Vertex::uv) / sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(Shader_Vertex), (GLvoid*)offsetof(Shader_Vertex, uv));
        glEnableVertexAttribArray(3);

        GL_GET_ERRORS();

        glVertexAttribPointer(4, sizeof(Shader_Vertex::rotation_center) / sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(Shader_Vertex), (GLvoid*)offsetof(Shader_Vertex, rotation_center));
        glEnableVertexAttribArray(4);

        GL_GET_ERRORS();

        GL_GET_ERRORS();

        glBindVertexArray(0);
    }

    glBindVertexArray(buff_obj.vaos[0]);

    GL_GET_ERRORS();
}

void Render_Batch_init(sd::Render_Batch& batch, const usize reserved_space_per_batch)
{
#ifdef SD_USE_STATIC_RENDER_BATCH_SIZE
    // fixed-size arrays
    init(&batch.v_data);
    init(&batch.e_data);
#else
    // dynamic arrays
    init(&batch.v_data, 0, reserved_space_per_batch);
    init(&batch.e_data, 0, reserved_space_per_batch);
#endif
    batch.next_element = 0;
}

char const * const builtin_shader_paths[3][2] = {
    {
        "./shaders/main/polygon_textured_2D/polygon_textured_2D.vrt.glsl",
        "./shaders/main/polygon_textured_2D/polygon_textured_2D.frg.glsl"
    },
    {
        "./shaders/main/polygon_colored_2D/polygon_colored_2D.vrt.glsl",
        "./shaders/main/polygon_colored_2D/polygon_colored_2D.frg.glsl"
    },
    {
        "./shaders/main/line_debug/line_debug.vrt.glsl", 
        "./shaders/main/line_debug/line_debug.frg.glsl"
    },
};

// @internal
bool shaders_load_builtin(Shader_Catalogue& catalogue, uint64 which)
{
    for (usize i = 0; i < BUILTIN_GL_SHADER_TYPE_COUNT; i += 1) {
        if (BSET_CONTAINS(which, i)) {
            SD_LOG("loading {%s, %s} into slot {%llu}\n", builtin_shader_paths[i][0], builtin_shader_paths[i][1], i);
            const bool success = Shader_load_from_file(
                &catalogue.shaders[i],
                builtin_shader_paths[i][0],
                builtin_shader_paths[i][1]
            );
            if (!success) {
                return false;
            }
        }
    }

    return true;
}

bool shaders_append_builtin(Shader_Catalogue& catalogue, uint64 which)
{
    for (usize i = 0; i < BUILTIN_GL_SHADER_TYPE_COUNT; i += 1) {
        if (BSET_CONTAINS(which, i)) {
            catalogue.shaders.count += 1;
            const bool success = Shader_load_from_file(
                &catalogue.shaders[i],
                builtin_shader_paths[i][0],
                builtin_shader_paths[i][1]
            );
            if (!success) {
                catalogue.shaders.count -= 1;
                return false;
            }
        }
    }

    return true;
}

void shaders_set_uniform_locations_internal(sd::Renderer& ctx)
{
    Dynamic_Array<Shader>* const shaders = &ctx.shader_catalogue.shaders;
    Uniform_Location* const uniforms = ctx.uniform_location_backing_array.data;

    usize shader_i = 0;
    usize uniform_i = 0;

    // get uniform locations

    // GL_SHADER_POLYGON_TEXTURED_2D
    {
        usize u_count = 1;

        Shader* shader = set_catalogue_uniform_location_ptrs_internal(
            shaders, uniforms, shader_i, uniform_i, u_count
        );

        shader->uniform_locations[0] = glGetUniformLocation(shader->program, "u_matrix");
        GL_GET_ERRORS();
        SD_ASSERT(shader->uniform_locations[0] != -1);

        shader_i += 1;
        uniform_i += 1;
    }

    // GL_SHADER_POLYGON_COLORED_2D
    {
        usize u_count = 1;

        Shader* shader = set_catalogue_uniform_location_ptrs_internal(
            shaders, uniforms, shader_i, uniform_i, u_count
        );

        shader->uniform_locations[0] = glGetUniformLocation(shader->program, "u_matrix");
        GL_GET_ERRORS();
        SD_ASSERT(shader->uniform_locations[0] != -1);

        shader_i += 1;
        uniform_i += 1;
    }

    // GL_SHADER_LINE_DEBUG
    {
        usize u_count = 1;

        Shader* shader = set_catalogue_uniform_location_ptrs_internal(
            shaders, uniforms, shader_i, uniform_i, u_count
        );

        shader->uniform_locations[0] = glGetUniformLocation(shader->program, "u_matrix");
        GL_GET_ERRORS();
        SD_ASSERT(shader->uniform_locations[0] != -1);

        shader_i += 1;
        uniform_i += 1;
    }
}

sd::Renderer Renderer_make(const usize reserved_space_per_batch)
{
    sd::Renderer ctx;
    sd::init(ctx, reserved_space_per_batch);
    return ctx;
}

sd::Renderer Renderer_make_options(const u64 bitmask_flags, const usize reserved_space_per_batch)
{
    sd::Renderer ctx;
    sd::init_options(ctx, bitmask_flags, reserved_space_per_batch);
    return ctx;
}





Shader_Catalogue shaders_load_builtin(uint64 which)
{
    Shader_Catalogue catalogue = {};
    init(&catalogue.shaders);

    SD_ASSERT_MSG_PROC(shaders_load_builtin(catalogue, which), "%s\n", "Shader initialization unsuccessful");

    return catalogue;
}


void deinit(sd::Renderer& ctx)
{
    const layer_index MAX_BATCH_COUNT = sd::Renderer::MAX_RENDER_BATCH_COUNT;
    for (layer_index l = 0; l < MAX_BATCH_COUNT; l += 1) {
        Render_Batch_deinit(ctx.batches[l]);
    }

    GPU_Buffer_Object_Set_deinit(ctx.gpu_buff_objs);
    Shader_Catalogue_deinit_programs(ctx.shader_catalogue);
}







void Render_Batch_deinit(sd::Render_Batch& batch)
{
#ifndef SD_USE_STATIC_RENDER_BATCH_SIZE
    deallocate(&batch.v_data);
    deallocate(&batch.e_data);
#endif
}

void render(sd::Renderer& ctx, layer_index idx)
{
    Render_Batch* const batch = &ctx.batches[idx];

    // TODO
    if (sd::Renderer::BUFFERING_COUNT > 1) {

        // TODO later after no-buffering version works
        clear(&batch->v_data);
        clear(&batch->e_data);


        return;
    }



    GL_GET_ERRORS();
    // vao should never be unbound when buffering count is 1
    glBindVertexArray(ctx.gpu_buff_objs.vaos[0]);
    GL_GET_ERRORS();

    // vertex data ////////
    glBindBuffer(GL_ARRAY_BUFFER, ctx.gpu_buff_objs.buffers[0].vbo);
        GL_GET_ERRORS();

    glBufferSubData(
        GL_ARRAY_BUFFER, 
        ctx.vertex_idx * sizeof(Shader_Vertex),
        batch->v_data.count * sizeof(Shader_Vertex),
        (GLvoid*)batch->v_data.begin_ptr()
    );
        GL_GET_ERRORS();


    // element data ////////
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ctx.gpu_buff_objs.buffers[0].ebo);
        GL_GET_ERRORS();

    glBufferSubData(
        GL_ELEMENT_ARRAY_BUFFER, 
        ctx.element_idx * sizeof(Shader_Vertex),
        batch->e_data.count * sizeof(Shader_Vertex),
        (GLvoid*)batch->e_data.begin_ptr()
    );
        GL_GET_ERRORS();

    { // temporary for testing TODO

        //glUseProgram(shader_catalogue.shaders[1]);


        const f32 w = (1280.0f);
        const f32 h = (720.0f);
        mat4 mat_projection = glm::ortho(
            0.0f, 
            1.0f * w, 
            1.0f * h,
            0.0f,
            0.0f, 
            1.0f * 10.0f
        );

        glUseProgram(ctx.shader_catalogue.shaders[1].program);
        glUniformMatrix4fv(
            ctx.shader_catalogue.shaders[1].uniform_locations[0],
            1,
            GL_FALSE,
            glm::value_ptr(mat_projection)
        );
            GL_GET_ERRORS();

    }

    // draw
    {
        glDrawElements(GL_TRIANGLES, batch->e_data.count, GL_UNSIGNED_INT, (GLvoid*)ctx.element_idx);
            //GL_GET_ERRORS();
    }

    ctx.vertex_idx += batch->v_data.count;
    ctx.element_idx += batch->e_data.count;
    batch->next_element = 0; // will need to do this in present() so I can redraw this batch

    clear(&batch->v_data);
    clear(&batch->e_data);

    // TODO reset vertex idx and element idx after presenting
}

void render(sd::Renderer const& ctx, uint16 layer)
{
    // TODO
}



void batch_render(sd::Renderer const& ctx)
{      
}

bool line_projected(sd::Renderer const& ctx, layer_index L, Vec3 a, Vec3 b)
{
    return true;
}

bool line(sd::Renderer const& ctx, layer_index L, Vec2 a, Vec2 b)
{
    return true;
}

bool remove_line_swap_end(sd::Renderer const& ctx, layer_index L, usize idx)
{
    return true;
}

bool quad_projected(sd::Renderer const& ctx, layer_index L, Vec3 tl, Vec3 bl, Vec3 br, Vec3 tr)
{
    return true;
}

bool quad(sd::Renderer& ctx, layer_index L, Vec2 tl, Vec2 bl, Vec2 br, Vec2 tr)
{
    return quad(ctx, L, tl, bl, br, tr, 0.0);
}
bool quad(sd::Renderer& ctx, layer_index L, Vec2 tl, Vec2 bl, Vec2 br, Vec2 tr, float32 angle)
{
    Render_Batch& layer = ctx.batches[L];
    auto& v_data = layer.v_data;
    auto& e_data = layer.e_data;
    auto& next_element = layer.next_element;
    auto& color = ctx.active_color;

    usize vdata_idx = v_data.count;
    {
        auto *const vertices = v_data.next_free_slot_ptr();

        increment_count(&v_data, ELEMENTS_PER_QUAD);

        vertices[0].position = Vec3(tl, -L);
        vertices[0].angle    = angle;
        vertices[0].color    = color;
        vertices[0].uv       = Vec2(0);
        vertices[0].rotation_center = 0.0;

        vertices[1].position = Vec3(bl, -L);
        vertices[1].angle    = angle;
        vertices[1].color    = color;
        vertices[1].uv       = Vec2(0);
        vertices[1].rotation_center = 0.0;

        vertices[2].position = Vec3(br, -L);
        vertices[2].angle    = angle;
        vertices[2].color    = color;
        vertices[2].uv       = Vec2(0);
        vertices[2].rotation_center = 0.0;

        vertices[3].position = Vec3(tr, -L);
        vertices[3].angle    = angle;
        vertices[3].color    = color;
        vertices[3].uv       = Vec2(0);
        vertices[3].rotation_center = 0.0;
    }

    usize edata_idx = e_data.count;
    {
        auto *const elements = e_data.next_free_slot_ptr();

        increment_count(&e_data, INDICES_PER_QUAD);

        elements[0] = next_element;
        elements[1] = next_element + 1;
        elements[2] = next_element + 2;
        elements[3] = next_element + 2;
        elements[4] = next_element + 3;
        elements[5] = next_element;

        next_element += ELEMENTS_PER_QUAD;
    }

    return true;
}

bool quad(sd::Renderer& ctx, layer_index L, Vec2 tl, Vec2 bl, Vec2 br, Vec2 tr, Vec4 color_tl, Vec4 color_bl, Vec4 color_br, Vec4 color_tr)
{
    return quad(ctx, L, tl, bl, br, tr, color_tl, color_bl, color_br, color_tr, 0.0);
}
bool quad(sd::Renderer& ctx, layer_index L, Vec2 tl, Vec2 bl, Vec2 br, Vec2 tr, Vec4 color_tl, Vec4 color_bl, Vec4 color_br, Vec4 color_tr, float32 angle)
{
    Render_Batch& layer = ctx.batches[L];
    auto& v_data = layer.v_data;
    auto& e_data = layer.e_data;
    auto& next_element = layer.next_element;
    auto& color = ctx.active_color;

    usize vdata_idx = v_data.count;
    {
        auto *const vertices = v_data.next_free_slot_ptr();

        increment_count(&v_data, ELEMENTS_PER_QUAD);

        vertices[0].position = Vec3(tl, -L);
        vertices[0].angle    = angle;
        vertices[0].color    = color_tl;
        vertices[0].uv       = Vec2(0);
        vertices[0].rotation_center = 0.0;

        vertices[1].position = Vec3(bl, -L);
        vertices[1].angle    = angle;
        vertices[1].color    = color_bl;
        vertices[1].uv       = Vec2(0);
        vertices[1].rotation_center = 0.0;

        vertices[2].position = Vec3(br, -L);
        vertices[2].angle    = angle;
        vertices[2].color    = color_br;
        vertices[2].uv       = Vec2(0);
        vertices[2].rotation_center = 0.0;

        vertices[3].position = Vec3(tr, -L);
        vertices[3].angle    = angle;
        vertices[3].color    = color_tr;
        vertices[3].uv       = Vec2(0);
        vertices[3].rotation_center = 0.0;
    }

    usize edata_idx = e_data.count;
    {
        auto *const elements = e_data.next_free_slot_ptr();

        increment_count(&e_data, INDICES_PER_QUAD);

        elements[0] = next_element;
        elements[1] = next_element + 1;
        elements[2] = next_element + 2;
        elements[3] = next_element + 2;
        elements[4] = next_element + 3;
        elements[5] = next_element;

        next_element += ELEMENTS_PER_QUAD;
    }

    return true;
}

bool polygon_convex_regular(sd::Renderer const& ctx, layer_index L, GLfloat radius, Vec2 center, const usize count_sides)
{
    return true;
}

bool circle(sd::Renderer const& ctx, layer_index L, GLfloat radius, Vec2 center, usize detail)
{
    return sd::polygon_convex_regular(ctx, L, radius, center, detail);
}

bool vertex(sd::Renderer const& ctx, layer_index L, Vec2 v)
{
    #ifdef SD_USE_STATIC_RENDER_BATCH_SIZE
    // TODO
    #endif

    return true;
}

void Shader_Vertex_print(Shader_Vertex const& v)
{
    printf("%s(position:v3)", "<");
    vec3_print(Vec3(v.position));
    printf(", (angle:f32)%f, (color)", v.angle);
    vec4_print(Vec4(v.color));
    printf(", (uv:v2)");
    vec2_print(Vec2(v.uv));
    printf(", (rotation_center:f32)%f%s", v.rotation_center, ">");
}
void VertexData_Array_print(VertexData_Array const& vda)
{
    printf("%s\n", "V{");
    for (usize i = 0; i < vda.count; i += 1) {
        Shader_Vertex_print(vda[i]);
        printf("%s\n", ",");
    }
    printf("%s\n\n", "}");
}
void ElementData_Array_print(ElementData_Array const& eda)
{
    printf("%s\n", "E{");
    for (usize i = 0; i < eda.count; i += 1) {
        printf("%u,\n", eda[i]);
    }
    printf("%s\n\n", "}");
}
void Render_Batch_print(Render_Batch const& batch)
{
    VertexData_Array_print(batch.v_data);
    ElementData_Array_print(batch.e_data);
}


}

#endif // SD_OPENGL_HPP
