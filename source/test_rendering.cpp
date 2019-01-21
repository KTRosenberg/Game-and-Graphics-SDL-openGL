
#include <dlfcn.h>


using sd::layer_index;
#include "playground_common.cpp"

Vec2 from(pg::Vec2 v) 
{
    return {v.x, v.y};
}
Vec3 from(pg::Vec3 v) 
{
    return {v.x, v.y, v.z};
}
Vec4 from(pg::Vec4 v)
{
    return {v.x, v.y, v.z, v.w};
}

bool quad(sd::Renderer* ctx, layer_index L, pg::Vec2 tl, pg::Vec2 bl, pg::Vec2 br, pg::Vec2 tr)
{
    return sd::quad(*ctx, L, from(tl), from(bl), from(br), from(tr));
}
bool quad(sd::Renderer* ctx, layer_index L, pg::Vec2 tl, pg::Vec2 bl, pg::Vec2 br, pg::Vec2 tr, float32 angle)
{
    return sd::quad(*ctx, L, from(tl), from(bl), from(br), from(tr), angle);
}
bool quad(sd::Renderer* ctx, layer_index L, pg::Vec2 tl, pg::Vec2 bl, pg::Vec2 br, pg::Vec2 tr, pg::Vec4 color_tl, pg::Vec4 color_bl, pg::Vec4 color_br, pg::Vec4 color_tr)
{
    return sd::quad(*ctx, L, from(tl), from(bl), from(br), from(tr), from(color_tl), from(color_bl), from(color_br), from(color_tr));
}
bool quad(sd::Renderer* ctx, layer_index L, pg::Vec2 tl, pg::Vec2 bl, pg::Vec2 br, pg::Vec2 tr, pg::Vec4 color_tl, pg::Vec4 color_bl, pg::Vec4 color_br, pg::Vec4 color_tr, float32 angle)
{
    return sd::quad(*ctx, L, from(tl), from(bl), from(br), from(tr), from(color_tl), from(color_bl), from(color_br), from(color_tr), angle);
}

bool triangle(sd::Renderer* ctx, layer_index L, pg::Vec2 a, pg::Vec2 b, pg::Vec2 c)
{
    return sd::triangle(*ctx, L, from(a), from(b), from(c));
}

void clear(u32 mask)
{
    sd::clear(mask);
}
void clear_color(float32 r, float32 g, float32 b, float32 a)
{
    sd::clear_color(r, g, b, a);
}
void clear_color(pg::Vec4 c)
{
    sd::clear_color(from(c));
}
void clear_color(pg::Vec3 c)
{
    sd::clear_color(Vec4(from(c), 1.0));
}

void color(sd::Renderer* ctx, pg::Vec4 c)
{
    sd::color(*ctx, Vec4(from(c)));
}

void pg_init(SD_Data* data, sd::Renderer& ctx, float32 w, float32 h)
{
    data->renderer = &ctx;
    data->screen_width = w;
    data->screen_height = h;
    data->quad_no_angle = quad;
    data->quad_angle = quad;
    data->quad_color_no_angle = quad;
    data->quad_color_angle = quad;
    data->triangle_no_angle = triangle;
    data->clear = clear;
    data->clear_color_rgba = clear_color;
    data->clear_color_vec4 = clear_color;
    data->clear_color_vec3 = clear_color;
    data->color = color;
}


typedef void* (*addr_get_proc)(const char*);
typedef void* (*pg_setup_proc)(SD_Data*);
typedef void* (*pg_draw_proc)(float32, float32);

addr_get_proc get_proc_address;

void* pg_setup_error_state(SD_Data* _)
{
    return NULL;
}
pg_setup_proc pg_setup = pg_setup_error_state;

void* pg_draw_proc_error_state(float32 _, float32 __) 
{
    sd::clear(sd::CLEAR_COLOR | sd::CLEAR_DEPTH);
    sd::clear_color(0.0f, 0.0f, 0.0f, 1.0f); 
    return NULL; 
}

pg_draw_proc pg_draw;
pg_draw_proc pg_draw_error_state = pg_draw_proc_error_state;

void load_playground(void);
void reload_playground(void);


int test_rendering(int argc, char* argv[]);
void render_playground(sd::Renderer& ctx, SDL_Window* window, pg_draw_proc pg_draw);
bool test_rendering_poll_input(void);

float32 w_width;
float32 w_height;

sd::Renderer* R = NULL;



struct Pg_Reload_Time {
    struct stat stat;
    time_t t_prev_modified; 
    struct stat stat_header;
    time_t t_prev_modified_header;
} pg_time = {};

bool test_rendering_poll_input(void)
{
    static bool auto_reload_on = true;
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            return false;
        case SDL_WINDOWEVENT:
            switch (event.window.event) {
            case SDL_WINDOWEVENT_MINIMIZED:
                window_state.minimized = true;
                break;
            case SDL_WINDOWEVENT_RESTORED:
                window_state.minimized = false;
                window_state.restored = true;
                break;
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                window_state.focused = true;
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                window_state.focused = false;
                break;
            default:
                break;
            }
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_A:
                auto_reload_on = !auto_reload_on;
                printf("auto reload : %s\n", ((auto_reload_on) ? "true" : "false"));
                break;
            case SDL_SCANCODE_GRAVE:
                if (auto_reload_on) {
                    break;
                }
                reload_playground();
                pg_init(&sd_data, *R, w_width, w_height);
                pg_setup(&sd_data);
                break;
            default:
                break;
            }

        default:
            break;
        }  
    }

    if (window_state.minimized) {
        SDL_Delay(1000);
    } else if (!window_state.focused) {
        if (auto_reload_on) { 
            check_file_status("./playground.cpp", &pg_time.stat);
            check_file_status("./playground.hpp", &pg_time.stat_header);
            bool updated = false;
            if (pg_time.stat.st_mtime != pg_time.t_prev_modified) {
                reload_playground();
                pg_init(&sd_data, *R, w_width, w_height);
                pg_setup(&sd_data);
                pg_time.t_prev_modified = pg_time.stat.st_mtime;
                updated = true;
            }
            if (pg_time.stat_header.st_mtime != pg_time.t_prev_modified_header) {
                if (!updated) {
                    reload_playground();
                    pg_init(&sd_data, *R, w_width, w_height);
                    pg_setup(&sd_data);                    
                }
                pg_time.t_prev_modified_header = pg_time.stat_header.st_mtime;
            }
        } else {
            SDL_Delay(64);
        }
    }

    return true;
}


struct Timer {
    float64 frequency;
    uint64 t_now, t_prev, t_start, t_delta;
    float64 t_now_s, t_prev_s, t_since_start_s, t_delta_s;
    float64 frame_time;
    uint32 frame_count, fps;
} timer;


void* dylib = NULL;

void load_playground(void)
{
    pg::version_number = 0;

    pg_draw = pg_draw_error_state;

    std::string header = "#define PLAYGROUND_VERSION " + std::to_string(pg::version_number) + "\n";
    if (false == write_file_mode("./playground_version.hpp", "w", header.c_str(), strlen(header.c_str()))) {
        fprintf(stderr, "file write unsuccessful");
        abort();
    }

    if (system("clang++ -O3 -dynamiclib -o playground.dylib playground.cpp") != 0) {
        fprintf(stderr, "%s\n", "ERROR: playground compiled unsuccessfully");
        return;
    }

    dylib = dlopen("./playground.dylib", RTLD_LAZY);
    if (dylib == NULL) {
        printf("%s\n", "couldn't load dylib");
        abort();
    }

    get_proc_address = (addr_get_proc)dlsym(dylib, ("get_proc_address" + std::to_string(pg::version_number)).c_str());
    if (get_proc_address == NULL) {
        printf("%s\n", dlerror());
        dlclose(dylib);
        return;
    }


    pg_setup = (pg_setup_proc)get_proc_address("setup");
    if (pg_setup == NULL) {
        printf("%s\n", dlerror());
        dlclose(dylib);
        return;
    }


    pg_draw = (pg_draw_proc)get_proc_address("draw");
    if (pg_draw == NULL) {
        printf("%s\n", dlerror());
        pg_draw = pg_draw_error_state;
        dlclose(dylib);
        return;
    }

    check_file_status("./playground.cpp", &pg_time.stat);
    pg_time.t_prev_modified = pg_time.stat.st_mtime;
    check_file_status("./playground.hpp", &pg_time.stat_header);
    pg_time.t_prev_modified_header = pg_time.stat_header.st_mtime;
}

void reload_playground(void)
{

    static char const* dylib_name_swap[4] = {
        "clang++ -O3 -dynamiclib -o playground.dylib playground.cpp",
        "./playground.dylib",
        "clang++ -O3 -dynamiclib -o playground_swap.dylib playground.cpp",
        "./playground_swap.dylib"
    };
    static u8 dylib_name_idx = 1;

    void* old_dylib = dylib;

    pg_draw = pg_draw_error_state;

    std::string header = "#define PLAYGROUND_VERSION " + std::to_string(pg::version_number + 1) + "\n";
    if (false == write_file_mode("./playground_version.hpp", "w", header.c_str(), strlen(header.c_str()))) {
        fprintf(stderr, "file write unsuccessful");
        abort();
    }

    if (system(dylib_name_swap[dylib_name_idx * 2]) != 0) {
        fprintf(stderr, "%s\n", "ERROR: playground compiled unsuccessfully");
        return;
    }

    dylib = dlopen(dylib_name_swap[(dylib_name_idx * 2) + 1], RTLD_LAZY);
    if (dylib == NULL) {
        printf("%s\n", "couldn't load dylib");
        abort();
    }


    get_proc_address = (addr_get_proc)dlsym(dylib, ("get_proc_address" + std::to_string(pg::version_number + 1)).c_str());
    if (get_proc_address == NULL) {
        printf("%s\n", dlerror());
        dlclose(dylib);
        dylib = old_dylib;
        return;
    }


    pg_setup = (pg_setup_proc)get_proc_address("setup");
    if (pg_setup == NULL) {
        printf("%s\n", dlerror());
        dlclose(dylib);
        dylib = old_dylib;
        return;
    }


    pg_draw = (pg_draw_proc)get_proc_address("draw");
    if (pg_draw == NULL) {
        printf("%s\n", dlerror());
        dlclose(dylib);
        dylib = old_dylib;
        pg_draw = pg_draw_error_state;
        return;
    }

    dlclose(old_dylib);

    pg::version_number += 1;

    dylib_name_idx = 1 - dylib_name_idx;

}

int test_rendering(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC) < 0) {
        fprintf(stderr, "%s\n", "SDL could not initialize");
        return EXIT_FAILURE;
    }

    // OpenGL pre-initialization ///////////////////////////////////////////////////
    
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    // create the window
    w_width = 1280.0f;
    w_height = 720.0f;
    SDL_Window* window = nullptr;
    if (nullptr == (window = SDL_CreateWindow(
        WINDOW_HEADER,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        w_width, w_height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN)))
    {
        fprintf(stderr, "Window could not be created\n");
        return EXIT_FAILURE;
    }

    window_state.focused = true;
    window_state.minimized = false;

    // initialize image library
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if(!(IMG_Init(imgFlags) & imgFlags)) {
      fprintf(stderr, "SDL_image could not initialize, SDL_image Error: %s\n", IMG_GetError());
        SDL_DestroyWindow(window);
        return EXIT_FAILURE;
    }

    // OpenGL main initialization
    SDL_GLContext gfx_ctx = SDL_GL_CreateContext(window);

    glewExperimental = GL_TRUE;
    glewInit();

    glViewport(0, 0, w_width, w_height);
    glEnable(GL_MULTISAMPLE);

    // print info
    {
        SDL_version compiled;
        SDL_version linked;

        SDL_VERSION(&compiled);
        SDL_GetVersion(&linked);
        
        printf(
            "COMPILED AGAINST SDL VERSION %d.%d.%d.\n",
            compiled.major, compiled.minor, compiled.patch
        );
        printf(
            "LINKED AGAINST SDL VERSION %d.%d.%d.\n",
            linked.major, linked.minor, linked.patch
        );
        printf("USING GL VERSION: %s\n", glGetString(GL_VERSION));
    }



    timer.frequency = SDL_GetPerformanceFrequency(),
    timer.t_now = SDL_GetPerformanceCounter(),
    timer.t_prev = 0.0,
    timer.t_start = timer.t_now;
    timer.t_now_s = (f64)timer.t_now / timer.frequency;
    timer.t_prev_s = 0.0;
    timer.t_since_start_s = 0.0;
    timer.t_delta_s = 0.0;
    timer.frame_time = 0.0;
    timer.frame_count = 0;
    timer.fps = 0;

    SDL_GL_SetSwapInterval(1);
    //glEnable(GL_DEPTH_TEST);
    //glDepthRange(0, 1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    sd::Renderer renderer = sd::Renderer_make_options();
    R = &renderer;

    // main loop
    bool is_running = true;

    load_playground();
    pg_init(&sd_data, renderer, w_width, w_height);
    pg_setup(&sd_data);

    while (is_running) {
        // update time variables
        timer.t_prev = timer.t_now;
        timer.t_prev_s = timer.t_now_s;

        timer.t_now = SDL_GetPerformanceCounter();
        timer.t_now_s = (f64)timer.t_now / timer.frequency;

        timer.t_delta = (timer.t_now - timer.t_prev);
        timer.t_delta_s = (f64)timer.t_delta / timer.frequency;

        timer.t_since_start_s = ((f64)(timer.t_now - timer.t_start)) / timer.frequency;

        if (!test_rendering_poll_input()) {
            is_running = false;
        } else if (window_state.minimized) {
            continue;
        } else if (window_state.restored) {
            window_state.restored = false;
            SDL_GL_SwapWindow(window);
            continue;
        }

        // rendering

        render_playground(renderer, window, pg_draw);
        
        timer.frame_count += 1;
        if (timer.t_now_s - timer.frame_time > 1.0) {
            timer.fps = timer.frame_count;
            timer.frame_count = 0;
            timer.frame_time = timer.t_now_s;


            //printf("%f\n", (double)timer.fps);
        }
    }

    dlclose(dylib);

    sd::deinit(renderer);
    // de-initialize
    SDL_GL_DeleteContext(gfx_ctx);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();


    return EXIT_SUCCESS;
}



void render_playground(sd::Renderer& ctx, SDL_Window* window, pg_draw_proc pg_draw) // temporarily no parameters for the renderer
{
    sd::clear(sd::CLEAR_COLOR | sd::CLEAR_DEPTH);
    sd::color(ctx, Vec4(0.0, 0.0, 0.0, 1.0));

    pg_draw(timer.t_since_start_s, timer.t_delta_s);

    sd::render(ctx, 0);

    clear(&ctx.batches[0].v_data);
    clear(&ctx.batches[0].e_data);
    ctx.batches[0].next_element = 0;
    // TODO

    sd::present(ctx, window);

}










