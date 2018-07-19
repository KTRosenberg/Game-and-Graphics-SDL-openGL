// C11

#define COMMON_UTILS_IMPLEMENTATION
#include "common_utils.h"

// audio

#define DR_FLAC_IMPLEMENTATION
#include "external_libraries/mini_al/extras/dr_flac.h"  // Enables FLAC decoding.
#define DR_MP3_IMPLEMENTATION
#include "external_libraries/mini_al/extras/dr_mp3.h"   // Enables MP3 decoding.
#define DR_WAV_IMPLEMENTATION
#include "external_libraries/mini_al/extras/dr_wav.h" // Enables WAV decoding.

#define MINI_AL_IMPLEMENTATION
#include "external_libraries/mini_al/mini_al.h"
#undef MINI_AL_IMPLEMENTATION
// #define AUDIO_STANDALONE
// #define SUPPORT_FILEFORMAT_WAV
// #define SUPPORT_FILEFORMAT_OGG
// #define SUPPORT_FILEFORMAT_FLAC
// #include "external_libraries/audio.c"
