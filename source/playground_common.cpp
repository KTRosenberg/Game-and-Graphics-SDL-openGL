namespace pg {
    static uint64 version_number;
    struct Vec2 {
        float32 x;
        float32 y;
    };
    struct alignas(16) Vec3 {
        float32 x;
        float32 y;
        float32 z;
    };
    struct alignas(16) Vec4 {
        float32 x;
        float32 y;
        float32 z;
        float32 w;
    };
}

#ifdef __cplusplus
extern "C" 
{
#endif
    struct SD_Data {
        sd::Renderer* renderer;
        float32 screen_width;
        float32 screen_height;
        bool (*quad_no_angle)(sd::Renderer*, layer_index, pg::Vec2, pg::Vec2, pg::Vec2, pg::Vec2);
        bool (*quad_angle)(sd::Renderer*, layer_index, pg::Vec2, pg::Vec2, pg::Vec2, pg::Vec2, float32);
        bool (*quad_color_no_angle)(sd::Renderer* ctx, layer_index, pg::Vec2, pg::Vec2, pg::Vec2, pg::Vec2, pg::Vec4, pg::Vec4, pg::Vec4, pg::Vec4);
        bool (*quad_color_angle)(sd::Renderer* ctx, layer_index, pg::Vec2, pg::Vec2, pg::Vec2, pg::Vec2, pg::Vec4, pg::Vec4, pg::Vec4, pg::Vec4, float32);
        void (*clear)(u32);
        void (*clear_color_rgba)(float32, float32, float32, float32);
        void (*clear_color_vec4)(pg::Vec4);
        void (*clear_color_vec3)(pg::Vec3);
    } sd_data;

#ifdef __cplusplus
}
#endif
