#ifndef AUDIO_SYS_HPP
#define AUDIO_SYS_HPP

#include <ck_ring.h>
#include <ck_pr.h>
#include "sdl.hpp"

#include "external_libraries/mini_al/extras/dr_flac.h"  // Enables FLAC decoding.
#include "external_libraries/mini_al/extras/dr_mp3.h"   // Enables MP3 decoding.
#include "external_libraries/mini_al/extras/dr_wav.h" // Enables WAV decoding.
#include "external_libraries/mini_al/mini_al.h"

#define SAMPLES_PER_QUARTER_NOTE(sample_rate__, bpm__) ((60.0 / bpm__) * sample_rate__)

#define LOOP_FRAME_4_4(measure_count__, quarter_notes_per_measure__, extra_quarter_notes__, sample_rate__, bpm__) \
    ((measure_count__ * quarter_notes_per_measure__) + extra_quarter_notes__) * SAMPLES_PER_QUARTER_NOTE(sample_rate__, bpm__) 


// audio
typedef mal_uint32 mal_u32;

enum struct AUDIO_COMMAND_TYPE : u8 {
    ADJUST_MASTER_VOLUME,
    TOGGLE_DELAY,
    ENUM_COUNT
};

struct AudioCommand {
    AUDIO_COMMAND_TYPE type;

    union {
        struct {
            float32 percentage;
        } adjust_master_volume;
        struct {
            float32 decay;
            float32 channel_a_offset_percent;
            float32 channel_b_offset_percent;
        } toggle_delay;
    };
};

#define MAX_AUDIO_SOURCE_COUNT (4)
struct AudioArgs {
    mal_decoder decoders[1];
    u8 audio_source_count;


    ConcurrentFIFO_SingleProducerMultiConsumer fifo;
    AudioCommand command_buffer__[CONCURRENT_FIFO_MAX_SIZE];
    usize command_buffer_count__;
    usize command_buffer_idx__;
    usize total_frames_read;
};

void AudioArgs_init(AudioArgs* audio_sys, usize audio_source_count); 

struct AudioSystem {
    mal_device device;
};



#include <iostream>
mal_u32 on_send_frames_to_device(mal_device* p_device, mal_u32 frame_count, void* p_samples);



#endif // AUDIO_SYS_HPP

#ifdef AUDIO_SYS_IMPLEMENTATION
#undef AUDIO_SYS_IMPLEMENTATION

void AudioArgs_init(AudioArgs* audio_sys, usize audio_source_count) 
{
    audio_sys->audio_source_count = audio_source_count;
    ck_ring_init(&audio_sys->fifo.ring, CONCURRENT_FIFO_MAX_SIZE);
    audio_sys->command_buffer_count__ = CONCURRENT_FIFO_MAX_SIZE;
    audio_sys->command_buffer_idx__ = 0;
    audio_sys->total_frames_read = 0;
}


mal_u32 on_send_frames_to_device(mal_device* p_device, mal_u32 frame_count, void* p_samples)
{
    struct AudioArgs* args = (struct AudioArgs*)p_device->pUserData;
    if (args == NULL) {
        return 0;
    }

    void* result;

    static bool delay_is_on = false;

    // (60 / 170) * 1000
    static const f64 delayMilliseconds = 352.94117647 * 1; // milliseconds per beat
    static const f64 delaySamples = (delayMilliseconds * 44.1f);

    static f64 reverb_buffer[(usize)(delaySamples * 2)] = {0}; // slot per channel
    static usize reverb_position = 0;

    while (ck_ring_dequeue_spsc(&args->fifo.ring, args->fifo.buffer, &result)) {
        //std::cout << (i64)result << std::endl;
        if ((delay_is_on = !delay_is_on) == false) {
            memset(reverb_buffer, 0, sizeof(reverb_buffer));
        }
    }

    mal_decoder* decoders = args->decoders;

    mal_u32 frames_read = (mal_u32)mal_decoder_read(&decoders[0], frame_count, p_samples);


    // detect if audio finished playing
    if (frames_read == 0) {

        // loop bgm
        // (44100 = 1 second into the clip, given a sample rate of 44.1 KHz)

        // for the current iteration of "Time Rush" (July 18th, 2018), which plays at 170 bpm,
        // the loop-point is 26 measures in at 4/4 (26 * 4 quarter notes) plus 1 quater note
        // samples per quarter note = (60 / BPM) * SAMPLE_RATE
        // e.g. (60.0 / 170) * 44100 * 26 * 4

        #define SAMPLE_RATE (44100)
        #define BPM (170)
        #define MEASURE_COUNT (26)
        #define QUARTER_NOTES_PER_MEASURE (4)

        mal_decoder_seek_to_frame(&decoders[0], (mal_uint64)roundf(
           LOOP_FRAME_4_4(MEASURE_COUNT, QUARTER_NOTES_PER_MEASURE, 1, SAMPLE_RATE, BPM)
        ));

        // read samples from loop point
        frames_read = (mal_u32)mal_decoder_read(&decoders[0], frame_count, p_samples);
    }

    f32 x = 1.0f;

    for (usize frame = 0; frame < frames_read; ++frame) {
        for (usize channel = 0; channel < 2; ++channel) {
            ((float*)p_samples)[(frame * 2) + channel] *= x;
        }
    }

    // handle delay
    if (delay_is_on) {

        f32 decay = 0.4f;
        
        for (usize frame = 0; frame < frames_read; ++frame) {
            for (usize channel = 0; channel < 2; ++channel) {
                f32 val = ((float*)p_samples)[(frame * 2) + channel] + reverb_buffer[reverb_position] * decay * (1 + (channel * 0.05));
                ((float*)p_samples)[(frame * 2) + channel] = val;
                reverb_buffer[reverb_position] = val;
                reverb_position = (reverb_position + 1) % ((usize)(delaySamples * 2));
            }
        }
    }

    args->total_frames_read += frames_read;

    return frames_read;
}

#endif


