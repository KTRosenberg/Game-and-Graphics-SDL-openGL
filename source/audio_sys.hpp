#ifndef AUDIO_SYS_HPP
#define AUDIO_SYS_HPP

#include "common_utils_cpp.hpp"

#include <ck_ring.h>
#include <ck_pr.h>
#include "sdl.hpp"

#include "external_libraries/freeverb/freeverb.c"

//#include "external_libraries/mini_al/extras/dr_flac.h"  // Enables FLAC decoding.
//#include "external_libraries/mini_al/extras/dr_mp3.h"   // Enables MP3 decoding.
#include "external_libraries/mini_al/extras/dr_wav.h" // Enables WAV decoding.
#include "external_libraries/mini_al/mini_al.h"

#define SAMPLES_PER_QUARTER_NOTE(sample_rate__, bpm__) ((60.0 / bpm__) * sample_rate__)

#define LOOP_FRAME_4_4(measure_count__, quarter_notes_per_measure__, extra_quarter_notes__, sample_rate__, bpm__) \
    ((measure_count__ * quarter_notes_per_measure__) + extra_quarter_notes__) * SAMPLES_PER_QUARTER_NOTE(sample_rate__, bpm__) 


// audio
typedef mal_uint32 mal_u32;

enum struct AUDIO_COMMAND_TYPE : u8 {
    ADJUST_MASTER_VOLUME,
    DELAY,
    ENUM_COUNT
};


typedef void (*Fn_AudioCallback)(void* args);
struct AudioCallbackObject {
    void* args;
    Fn_AudioCallback callback;

    inline void operator()(void)
    {
        this->callback(this->args);
    }
};

struct AudioCommand {
    AUDIO_COMMAND_TYPE type;

    union {
        struct {
            float32* value;
            float32  duration;
            float32  from;
            float32  to;
            float32  t_delta;
            float32  t_prev;
        } adjust_master_volume;
        struct {
            float32 decay;
            float32 channel_a_offset_percent;
            float32 channel_b_offset_percent;
            bool on;
        } delay;
    };

    Fn_AudioCallback callback;
    void* audio_callback_args;
};

/*

// just for the lerping example (temp, need to rewrite for each command)
// this will be called by the audio thread after dequeuing
void add_interp_command(float64* value, float64 target_value, float64 duration)
{
    foreach(i, 10) {// 10 should instead be the number of active commands
        if ()
    }
}

#define MAX_ANIMATIONS 128

animation animations[MAX_ANIMATIONS];
int active_animations;

void update()
{
    for (int i = 0; i < active_animations; i++)
    {
        *animations[i].target += animations[i].delta;

        if ((animations[i].delta <= 0.0f && *animations[i].target <= animations[i].target_value) || (animations[i].delta > 0.0f && *animations[i].target >= animations[i].target_value))
        {
            *animations[i].target = animations[i].target_value;

            if (i < active_animations - 1)
            {
                animations[i] = animations[active_animations - 1];
                i--;
            }

            active_animations--;
        }
    }
}

void add_animation(float* target, float target_value, float delta)
{
    for (int i = 0; i < active_animations; i++)
    {
        if (animations[i].target == target)
        {
            animations[i].target_value = target_value;
            animations[i].delta = delta;

            return;
        }
    }

    animations[active_animations].target = target;
    animations[active_animations].target_value = target_value;
    animations[active_animations].delta = delta;

    active_animations++;
}
*/



#define MAX_AUDIO_SOURCE_COUNT (16)
struct AudioArgs {
    mal_decoder decoders[MAX_AUDIO_SOURCE_COUNT];
    u8 audio_source_count;


    ConcurrentFIFO_SingleProducerSingleConsumer<128> fifo;

    AudioCommand command_buffer__[128 + 2];
    usize command_buffer_count__;
    usize command_buffer_idx__;
    usize total_frames_read;
};

void AudioArgs_init(AudioArgs* audio_sys, usize audio_source_count); 




#include <iostream>
mal_u32 on_send_frames_to_device(mal_device* p_device, mal_u32 frame_count, void* p_samples);



#endif // AUDIO_SYS_HPP

#ifdef AUDIO_SYS_IMPLEMENTATION
#undef AUDIO_SYS_IMPLEMENTATION

// TODO renaming
void AudioArgs_init(AudioArgs* audio_sys, usize audio_source_count) 
{
    audio_sys->audio_source_count = audio_source_count;
    ck_ring_init(&audio_sys->fifo.ring, audio_sys->fifo.capacity);
    audio_sys->command_buffer_count__ = audio_sys->fifo.capacity;
    audio_sys->command_buffer_idx__ = 0;
    audio_sys->total_frames_read = 0;
}

struct AudioSystem {
    mal_device device;
    float32 master_volume_percentage;
    RingBuffer<AudioCommand, 64> command_queue;
} audio_system;

struct Reverb {
    fv_Context ctx;
} fx_reverb;

#define MAX_DELAY_BUFFER_COUNT (32000)
struct Delay {
    float64 ms;
    float64 sample_count;
    float64 decay;
    float64 buffer[MAX_DELAY_BUFFER_COUNT];
    usize buffer_position;
};

void Delay_init(Delay* d, float64 ms, float64 decay, float64 sample_rate = 44.1);
void Delay_init(Delay* d, float64 ms, float64 decay, float64 sample_rate)
{
    d->ms = ms;
    d->sample_count = d->ms * sample_rate * 2;
    d->decay = decay;

    memset(d->buffer, 0, sizeof(d->buffer));

    d->buffer_position = 0;
}

void AudioSystem_init(void)
{
    RingBuffer_init(&audio_system.command_queue);

    audio_system.master_volume_percentage = 1.0;

    fv_init(&fx_reverb.ctx);
    fv_set_dry(&fx_reverb.ctx, .8);
    fv_set_wet(&fx_reverb.ctx, .2);
    fv_set_roomsize(&fx_reverb.ctx, 0.5);
    fv_set_damp(&fx_reverb.ctx, 0.2);
}


mal_u32 on_send_frames_to_device(mal_device* p_device, mal_u32 frame_count, void* p_samples)
{
    struct AudioArgs* args = (struct AudioArgs*)p_device->pUserData;
    if (args == NULL) {
        return 0;
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

    void* result;

    static bool delay_is_on = false;

    // (60 / 170) * 1000
    static const f64 delayMilliseconds = 352.94117647 * 1; // milliseconds per beat
    static const f64 delaySamples = (delayMilliseconds * 44.1);

    static f64 reverb_buffer[(usize)(delaySamples * 2)] = {0}; // slot per channel
    static usize reverb_position = 0;

    static f64 master_volume_percentage = 1.0f;


    while (ck_ring_dequeue_spsc(&args->fifo.ring, args->fifo.buffer, &result)) {
        //std::cout << (i64)result << std::endl;
        
        AudioCommand* cmd = (AudioCommand*)result;

        RingBuffer_enqueue(&audio_system.command_queue, *cmd);

        free(cmd); // temporary, will have a static buffer to avoid allocations and frees at runtime

    }




    f64 t_now_s = (f64)SDL_GetPerformanceCounter() / SDL_GetPerformanceFrequency();

    f64 master_pan_value = 0.0f;//glm::sin(t_now_s) * 100.0f;
    static f64 pan_range = 200.0;
    #define PAN_FOR_RIGHT(val) ((val / pan_range) + 0.5)
    #define PAN_FOR_LEFT(val) (1 - PAN_FOR_RIGHT(val))
    f64 pan[2] = {PAN_FOR_LEFT(master_pan_value), PAN_FOR_RIGHT(master_pan_value)};

    for (usize i = 0, size = audio_system.command_queue.size; i < size; ++i) {
        AudioCommand* cmd = RingBuffer_dequeue_pointer(&audio_system.command_queue);

        switch (cmd->type) {
        case AUDIO_COMMAND_TYPE::ADJUST_MASTER_VOLUME: {
            if (cmd->adjust_master_volume.t_prev == 0.0f) {
                cmd->adjust_master_volume.t_prev = t_now_s;
                master_volume_percentage = cmd->adjust_master_volume.from; // TODO back and forth switches, need to adjust duration based on how close actual value is to expected value
                //cmd->adjust_master_volume.from = master_volume_percentage;
                if (cmd->adjust_master_volume.from != cmd->adjust_master_volume.to) {
                    RingBuffer_enqueue(&audio_system.command_queue, *cmd);   
                }

                //printf("%f\n", master_volume_percentage);

                break;
            }


            const f32 elapsed = cmd->adjust_master_volume.t_delta + (t_now_s - cmd->adjust_master_volume.t_prev);
            const f32 t = elapsed / cmd->adjust_master_volume.duration;
            const f32 val = lerp(
                cmd->adjust_master_volume.from,
                cmd->adjust_master_volume.to,
                t
            );

            cmd->adjust_master_volume.t_delta = elapsed;
            cmd->adjust_master_volume.t_prev = t_now_s;

            master_volume_percentage = val;

            if (std::abs(val - cmd->adjust_master_volume.to) > 0.01) {
                RingBuffer_enqueue(&audio_system.command_queue, *cmd);                
            } else {
                master_volume_percentage = cmd->adjust_master_volume.to;
            }
            
            //printf("%f\n", master_volume_percentage);

            
            break;
        }
        case AUDIO_COMMAND_TYPE::DELAY: {
            if ((delay_is_on = !delay_is_on) == false) {
                memset(reverb_buffer, 0, sizeof(reverb_buffer));
            }
            break;
        }
        default: { 
            break; 
        } 
        }
    }

    // handle delay
    if (delay_is_on) {

        f32 decay = 0.2f;
        
        for (usize frame = 0; frame < frames_read; ++frame) {
            for (usize channel = 0; channel < 2; ++channel) {

                f32 val = ((float*)p_samples)[(frame * 2) + channel] + reverb_buffer[reverb_position] * decay * (1 + (channel * 0.05));
                ((float*)p_samples)[(frame * 2) + channel] = val;
                reverb_buffer[reverb_position] = val;
                reverb_position = (reverb_position + 1) % ((usize)(delaySamples * 2));
            }
        }
        //fv_process(&fx_reverb.ctx, (float*)p_samples, frames_read * 2);
    }


    for (usize frame = 0; frame < frames_read; ++frame) {
        for (usize channel = 0; channel < 2; ++channel) {
            ((float*)p_samples)[(frame * 2) + channel] *= pan[channel] * master_volume_percentage;
        }
    }


    args->total_frames_read += frames_read;

    return frames_read;
}

#endif


