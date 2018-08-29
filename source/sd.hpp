#ifndef SD_HPP
#define SD_HPP

#ifdef SD_DEBUG_LOG_ON
    // inline void sd__log(FILE * stream, const char * format, ...)
    // {
    //     va_list argptr;
    //     va_start(argptr, format);
    //         vfprintf(stream, format, argptr);
    //     va_end(argptr);  
    // }
    // inline void sd__log(const char * format, ...)
    // {
    // va_list argptr;
    // va_start(argptr, format);
    //     vfprintf(stderr, format, argptr);
    // va_end(argptr);  
    // }

    #define SD_ASSERT(condition__) ASSERT(condition__)
    #define SD_ASSERT_MSG(condition__, ...) ASSERT_MSG(condition__, __VA_ARGS__)
    #define SD_LOG(format__, ...) (fprintf(stdout, format__, __VA_ARGS__));
    #define SD_LOG_ERR(format__, ...) (fprintf(stderr, format__, __VA_ARGS__));
#else
    #define SD_ASSERT(condition__) condition__
    #define SD_ASSERT_MSG(condition__, ...) condition__
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

#if defined(SD_RENDERER_VULKAN)
    #error VULKAN RENDERER NOT IMPLEMENTED
#elif defined(SD_RENDERER_OPENGL)
    #include "sd_opengl.hpp"
#else
    #error SD RENDERER NOT DEFINED
#endif

#endif // SD_HPP
