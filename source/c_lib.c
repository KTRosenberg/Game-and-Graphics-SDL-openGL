// C11

#define COMMON_UTILS_IMPLEMENTATION
#include "common_utils.h"

#define MINI_AL_IMPLEMENTATION
#include "external_libraries/external/mini_al.c"   // Implemented in mini_al.c. Cannot implement this here because it conflicts with Win32 APIs such as CloseWindow(), etc.
#undef MINI_AL_IMPLEMENTATION

#define AUDIO_STANDALONE
#define SUPPORT_FILEFORMAT_WAV
#define SUPPORT_FILEFORMAT_OGG
#define SUPPORT_FILEFORMAT_FLAC

#if defined(SUPPORT_FILEFORMAT_OGG)
    //#define STB_VORBIS_HEADER_ONLY
    #include "external_libraries/external/stb_vorbis.h"    // OGG loading functions
    #include "external_libraries/external/stb_vorbis.c"
#endif

#if defined(SUPPORT_FILEFORMAT_XM)
    #define JAR_XM_IMPLEMENTATION
    #include "external_libraries/external/jar_xm.h"        // XM loading functions
#endif

#if defined(SUPPORT_FILEFORMAT_MOD)
    #define JAR_MOD_IMPLEMENTATION
    #include "external_libraries/external/jar_mod.h"       // MOD loading functions
#endif

#if defined(SUPPORT_FILEFORMAT_FLAC)
    #define DR_FLAC_IMPLEMENTATION
    #define DR_FLAC_NO_WIN32_IO
    #include "external_libraries/external/dr_flac.h"       // FLAC loading functions
#endif

#if defined(SUPPORT_FILEFORMAT_MP3)
    #define DR_MP3_IMPLEMENTATION
    #include "external_libraries/external/dr_mp3.h"       // MP3 loading functions
#endif

#include "external_libraries/audio.c"
