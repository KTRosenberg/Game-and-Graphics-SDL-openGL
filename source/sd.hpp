#ifndef SD_HPP
#define SD_HPP

#ifdef SD_DEBUG_LOG_ON

    // from Odin lang, gb library

    #define ASSERT_MSG__INTERNAL(cond, msg, ...) do { \
        if (!(cond)) { \
            gb_assert_handler("Assertion Failure", #cond, __FILE__, (i64)__LINE__, msg, ##__VA_ARGS__); \
            abort(); \
        } \
    } while (0)

    #define ASSERT__INTERNAL(cond) ASSERT_MSG__INTERNAL(cond, NULL)

    //

    #warning "SD DEBUG LOG ON"

    #define SD_ASSERT(condition__) ASSERT__INTERNAL(condition__)
    #define SD_ASSERT_MSG(condition__, msg, ...) ASSERT_MSG__INTERNAL(condition__, msg, __VA_ARGS__)
    #define SD_ASSERT_PROC(proc__) ASSERT__INTERNAL(proc__)
    #define SD_ASSERT_MSG_PROC(proc__, msg, ...) ASSERT_MSG__INTERNAL(proc__, msg, __VA_ARGS__)

    #define SD_LOG(format__, ...) (fprintf(stdout, format__, __VA_ARGS__));
    #define SD_LOG_ERR(format__, ...) (fprintf(stderr, format__, __VA_ARGS__));
#else
    #define SD_ASSERT(condition__)
    #define SD_ASSERT_MSG(condition__, msg, ...)
    #define SD_ASSERT_PROC(proc__) proc__
    #define SD_ASSERT_MSG_PROC(proc__, msg, ...) proc__
    #define SD_LOG(format__, ...)
    #define SD_LOG_ERR(args__, ...)
#endif

namespace Color {
    static const Vec4 RED     = {1.0, 0.0, 0.0, 1.0};
    static const Vec4 MAGENTA = {1.0, 0.0, 1.0, 1.0};
    static const Vec4 YELLOW  = {1.0, 1.0, 0.0, 1.0};
    static const Vec4 WHITE   = {1.0, 1.0, 1.0, 1.0};
    static const Vec4 GREEN   = {0.0, 1.0, 0.0, 1.0};
    static const Vec4 CYAN    = {0.0, 1.0, 1.0, 1.0};
    static const Vec4 BLACK   = {0.0, 0.0, 0.0, 1.0};
    static const Vec4 BLUE    = {0.0, 0.0, 1.0, 1.0};
}

namespace sd {

enum struct BATCH_TYPE {
    POLYGON_TEXTURED = (1 << 0),
    POLYGON_COLORED  = (1 << 1),
    LINE_DEBUG       = (1 << 2),
};

}

#if defined(SD_RENDERER_VULKAN)
    #error VULKAN RENDERER NOT IMPLEMENTED
#elif defined(SD_RENDERER_OPENGL)
    #include "sd_opengl.hpp"
#else
    #error SD RENDERER NOT DEFINED
#endif

#endif // SD_HPP
